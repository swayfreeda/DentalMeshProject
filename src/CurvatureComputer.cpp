#include "CurvatureComputer.h"

#include <iostream>
#include <cmath>
#include <queue>

#include <QVector>
#include <QTime>

#include <pthread.h>
#include <omp.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

using namespace std;

CurvatureComputer::CurvatureComputer(Mesh &mesh)
{
    mMesh = mesh;
    if(!mMesh.has_face_normals())
    {
        mMesh.request_face_normals();
    }
    if(!mMesh.has_vertex_normals())
    {
        mMesh.request_vertex_normals();
    }

    mMesh.update_normals();
    mLocalMode = true;
    mProjectionPlaneCheck = true;
    mKRing = MAX(ceil((float)mMesh.mVertexNum / 50000), 2);
    mMesh.computeBoundingBox();
    mSphere = (mMesh.BBox.size.x + mMesh.BBox.size.y + mMesh.BBox.size.z) / 3 * 0.02;
}

void CurvatureComputer::computeCurvature(QProgressDialog &progress)
{
    QTime time;

    int vertexNum = mMesh.mVertexNum;

    if(vertexNum <= 0)
    {
        return;
    }

    mCurvature.resize(vertexNum);
    mCurvatureComputed.resize(vertexNum);

    time.start();
    QVector<Mesh::VertexHandle> vertices;
    vertices.reserve(vertexNum);
    for(Mesh::VertexIter vertexIter = mMesh.vertices_begin(); vertexIter != mMesh.vertices_end(); vertexIter++)
    {
        vertices.push_back(*vertexIter);
    }
    cout << "创建所有顶点的线性索引 用时：" << time.elapsed() << "ms." << endl;

    int completedVertexNum = 0; //已计算完的顶点数目
    progress.setLabelText("Computing curvature...");
    progress.setMinimum(0);
    progress.setMaximum(vertexNum);
    progress.setValue(0);
    connect(this, SIGNAL(progressValueChanged(int)), &progress, SLOT(setValue(int)));

    time.start();

    /*//并行计算
    int procNum = omp_get_num_procs(); //处理器数量
    pthread_t *threadIDs = new pthread_t[procNum];
    int sectionLength = ceil((float)vertexNum / procNum); //各线程负责计算的点的数量
//#pragma omp parallel for num_threads(procNum)
    for(int i = 0; i < procNum; i++)
    {
        ThreadArguments arguments;
        arguments.curvatureComputer = this;
        arguments.vertices = &vertices;
        arguments.startVertexIndex = sectionLength * i;
        arguments.endVertexIndex = MIN(sectionLength * (i + 1), vertexNum);
        arguments.completedVertexNum = &completedVertexNum;

        if(pthread_create(&threadIDs[i], NULL, threadFun, &arguments))
        {
            cerr << "Error to create thread." << endl;
        }
    }
    sleep(5);
    for(int i = 0; i < procNum; i++)
    {
        pthread_join(threadIDs[i], NULL);
    }
    delete[] threadIDs;*/

    //串行计算
    computeCurvature(vertices, 0, vertexNum, &completedVertexNum);

    cout << "计算所有顶点的曲率 用时：" << time.elapsed() << "ms." << endl;
}

void* CurvatureComputer::threadFun(void *arg)
{
    ThreadArguments arguments = *((ThreadArguments *)arg);
    arguments.curvatureComputer->computeCurvature(*(arguments.vertices), arguments.startVertexIndex, arguments.endVertexIndex, arguments.completedVertexNum);
}

void CurvatureComputer::computeCurvature(QVector<Mesh::VertexHandle> &vertices, int startVertexIndex, int endVertexIndex, int *completedVertexNum)
{
    for(int vertexIndex = startVertexIndex; vertexIndex < endVertexIndex; vertexIndex++)
    {
        Mesh::Point tempVertex = mMesh.point(vertices[vertexIndex]);

        QVector<Mesh::VertexHandle> vv;
        QVector<Mesh::VertexHandle> vvtmp;

#ifdef KRING
        getKRing(vertices[vertexIndex], mKRing, vv);
#else
        getSphere(vertices[vertexIndex], mSphere, 6, vv);
#endif

        if(vv.size() < 6)
        {
            cerr << "Could not compute curvature of vertex No." << vertexIndex << " . coordinate: " << tempVertex << endl;
            mCurvatureComputed[vertexIndex] = false;
            continue;
        }

        if(mProjectionPlaneCheck)
        {
            applyProjOnPlane(mMesh.normal(vertices[vertexIndex]), vv, vvtmp);
            if(vvtmp.size() >= 6 && vvtmp.size() < vv.size())
            {
                vv = vvtmp;
                if(vv.size() < 6)
                {
                    cerr << "Could not compute curvature of vertex No." << vertexIndex << " . coordinate: " << tempVertex << endl;
                    mCurvatureComputed[vertexIndex] = false;
                    continue;
                }
            }
        }

        Mesh::Normal normal;
        getAverageNormal(vertices[vertexIndex], vv, normal);

        QVector<Mesh::Point> ref(3);
        computeReferenceFrame(vertices[vertexIndex], normal, ref);

        Quadric q;
        fitQuadric(tempVertex, ref, vv, &q);
        mCurvature[vertexIndex] = finalEigenStuff(q);
        mCurvatureComputed[vertexIndex] = true;

//#pragma omp critical //TODO 不知道这句放在这里管不管用
        (*completedVertexNum)++;

        emit progressValueChanged(*completedVertexNum);
    }
}

void CurvatureComputer::getResult(QVector<float> &curvature, QVector<bool> &computed)
{
    curvature.resize(mCurvature.size());
    curvature.swap(mCurvature);
    computed.resize(mCurvatureComputed.size());
    computed.swap(mCurvatureComputed);
}

inline void CurvatureComputer::getKRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &vv)
{
    int bufSize = mMesh.mVertexNum;
    vv.reserve(bufSize);
    bool* visited = (bool*)calloc(bufSize, sizeof(bool));

    QVector< pair<Mesh::VertexHandle, int> > queue;
    queue.reserve(bufSize);
    queue.push_back(pair<Mesh::VertexHandle, int>(centerVertexHandle, 0));
    visited[centerVertexHandle.idx()] = true;

    Mesh::VertexHandle tempVertexHandle;
    int tempDistance;
    while(!queue.empty())
    {
        tempVertexHandle = queue.front().first;
        tempDistance = queue.front().second;
        queue.pop_front();
        vv.push_back(tempVertexHandle);
        if(tempDistance < k)
        {
            for(Mesh::VertexVertexIter vertexVertexIter = mMesh.vv_iter(tempVertexHandle); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(!visited[vertexVertexIter->idx()])
                {
                    queue.push_back(pair<Mesh::VertexHandle, int>(*vertexVertexIter, tempDistance + 1));
                    visited[vertexVertexIter->idx()] = true;
                }
            }
        }
    }

    free(visited);
}

inline void CurvatureComputer::getSphere(const Mesh::VertexHandle &centerVertexHandle, const float r, const int min, QVector<Mesh::VertexHandle> &vv)
{
    int bufSize = mMesh.mVertexNum;
    vv.reserve(bufSize);
    bool* visited = (bool*)calloc(bufSize, sizeof(bool));

    QVector<Mesh::VertexHandle> queue;
    queue.reserve(bufSize);
    queue.push_back(centerVertexHandle);
    visited[centerVertexHandle.idx()] = true;

    Mesh::Point me = mMesh.point(centerVertexHandle);
    priority_queue< pair<Mesh::VertexHandle, float>, vector< pair<Mesh::VertexHandle, float> >, comparer > extraCandidates;
    Mesh::VertexHandle toVisit;
    Mesh::VertexHandle neighbor;
    Mesh::Point neigh;
    float distance;
    while(!queue.empty())
    {
        toVisit=queue.front();
        queue.pop_front();
        vv.push_back(toVisit);
        for(Mesh::VertexVertexIter vertexVertexIter = mMesh.vv_iter(toVisit); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            neighbor = *vertexVertexIter;
            if(!visited[neighbor.idx()])
            {
                neigh = mMesh.point(neighbor);
                distance = (me - neigh).norm();
                if(distance < r)
                {
                    queue.push_back(neighbor);
                }
                else if(vv.size() < min)
                {
                    extraCandidates.push(pair<Mesh::VertexHandle, float>(neighbor, distance));
                }
                visited[neighbor.idx()] = true;
            }
        }
    }

    while (!extraCandidates.empty() && vv.size() < min)
    {
        pair<Mesh::VertexHandle, float> cand = extraCandidates.top();
        extraCandidates.pop();
        vv.push_back(cand.first);
        for(Mesh::VertexVertexIter vertexVertexIter = mMesh.vv_iter(cand.first); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            neighbor = *vertexVertexIter;
            if(!visited[neighbor.idx()])
            {
                neigh = mMesh.point(neighbor);
                distance = (me - neigh).norm();
                extraCandidates.push(pair<Mesh::VertexHandle, float>(neighbor, distance));
                visited[neighbor.idx()] = true;
            }
        }
    }

    free(visited);
}

inline float CurvatureComputer::getAverageEdge(QProgressDialog &progress)
{
    float edgeLengthSum = 0;
    int edgeIndex = 0;
    Mesh::HalfedgeHandle hh1, hh2;
    Mesh::VertexHandle vh1, vh2;

    progress.setLabelText("Computing curvature(compute average edge)...");
    progress.setMinimum(0);
    progress.setMaximum(mMesh.mFaceNum);
    progress.setValue(0);
    for(Mesh::EdgeIter edgeIter = mMesh.edges_begin(); edgeIter != mMesh.edges_end(); edgeIter++)
    {
        progress.setValue(edgeIndex);
        hh1 = mMesh.halfedge_handle(edgeIter, 0);
        hh2 = mMesh.halfedge_handle(edgeIter, 1);
        vh1 = mMesh.to_vertex_handle(hh1);
        vh2 = mMesh.to_vertex_handle(hh2);
        edgeLengthSum += (mMesh.point(vh1) - mMesh.point(vh2)).length();
        edgeIndex++;
    }

    return (edgeLengthSum / mMesh.mFaceNum);
}

inline void CurvatureComputer::applyProjOnPlane(const Mesh::Normal &ppn, const QVector<Mesh::VertexHandle> &vin, QVector<Mesh::VertexHandle> &vout)
{
    int vinSize = vin.size();
    vout.reserve(vinSize);
    Mesh::Normal tempNormal;
    for(QVector<Mesh::VertexHandle>::const_iterator vpi = vin.begin(); vpi != vin.end(); vpi++)
    {
        tempNormal = mMesh.normal(*vpi);
        if((tempNormal | ppn) > 0.0f)
        {
            vout.push_back(*vpi);
        }
    }
}

inline void CurvatureComputer::getAverageNormal(const Mesh::VertexHandle &centerVertexHandle, const QVector<Mesh::VertexHandle> &vv, Mesh::Normal &normal)
{
    if(mLocalMode)
    {
        normal = mMesh.normal(centerVertexHandle);
    }
    else
    {
        for(QVector<Mesh::VertexHandle>::const_iterator vpi = vv.begin(); vpi != vv.end(); vpi++)
        {
            normal += mMesh.normal(*vpi);
        }
    }

    normal.normalize();
}

inline void CurvatureComputer::computeReferenceFrame(const Mesh::VertexHandle &centerVertexHandle, const Mesh::Normal &normal, QVector<Mesh::Point> &ref)
{
    Mesh::Point longest_v = mMesh.point(mMesh.vv_iter(centerVertexHandle));

    Mesh::Point centerVertex = mMesh.point(centerVertexHandle);
    longest_v = (project(centerVertex, longest_v, normal) - centerVertex).normalized();

    Mesh::Point y_axis = (normal % longest_v).normalized();
    ref[0] = longest_v;
    ref[1] = y_axis;
    ref[2] = normal;
}

inline Mesh::Point CurvatureComputer::project(const Mesh::Point &v, const Mesh::Point &vp, const Mesh::Normal &ppn)
{
    return (vp - (ppn * ((vp - v) | (ppn))));
}

inline void CurvatureComputer::fitQuadric(const Mesh::Point &v, const QVector<Mesh::Point> &ref, const QVector<Mesh::VertexHandle> &vv, Quadric *q)
{
    QVector<Mesh::Point> points;
    points.reserve(vv.size());

    Mesh::Point vTang;
    for(QVector<Mesh::VertexHandle>::const_iterator vpi = vv.begin(); vpi != vv.end(); vpi++)
    {
        vTang = mMesh.point(*vpi) - v;

        float x = vTang | ref[0];
        float y = vTang | ref[1];
        float z = vTang | ref[2];
        points.push_back(Mesh::Point(x, y, z));
    }
    *q = Quadric::fit(points);
}

inline float CurvatureComputer::finalEigenStuff(Quadric &q)
{
    float a = q.a();
    float b = q.b();
    float c = q.c();
    float d = q.d();
    float e = q.e();

    float E = 1.0 + d*d;
    float F = d*e;
    float G = 1.0 + e*e;

    Mesh::Point n = Mesh::Point(-d, -e, 1.0).normalized();

    float L = 2.0 * a * n[2];
    float M = b * n[2];
    float N = 2 * c * n[2];

    //Eigen stuff
    Eigen::Matrix2d m;
    m << L*G - M*F, M*E-L*F, M*E-L*F, N*E-M*F;
    m = m / (E*G-F*F);
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> eig(m);

    Eigen::Vector2d c_val = eig.eigenvalues();

    c_val = -c_val;

    return (c_val(0) + c_val(1)) * 0.5;
}
