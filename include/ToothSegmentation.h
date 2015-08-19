#ifndef TOOTHSEGMENTATION_H
#define TOOTHSEGMENTATION_H

#include "Mesh.h"

#include <QProgressDialog>

using namespace SW;
using namespace std;

class ToothSegmentation : public QObject
{
    Q_OBJECT

public:
    enum mBoundaryVertexType
    {
        CENTER_VERTEX = 0, //中心点（边界内部点）
        COMPLEX_VERTEX, //复杂点（单点宽度边界点）
        DISK_VERTEX_GINGIVA, //外围点（边界外围点，与牙龈区域相邻）
        DISK_VERTEX_TOOTH //外围点（边界外围点，与第1颗牙齿区域相邻，后续逐个加1）
    };

    enum mNonBoundaryRegionType
    {
        ERROR_REGION = -2, //被complex vertex包围的center vertex被归为此类
        FILL_BOUNDARY_REGION = -1, //需要填充为边界的区域（用来去除噪声区域，最终mesh中不存在此类点）
        TEMP_REGION = 0, //临时区域（用来计算区域内的定点数，最终mesh中不存在此类点）
        GINGIVA_REGION, //牙龈区域
        TOOTH_REGION //牙齿区域（此为第1颗牙齿区域，后续逐个加1）
    };

    enum mBoundaryType
    {
        TOOTH_GINGIVA_BOUNDARY = 0, //牙齿与牙龈的边界
        TOOTH_TOOTH_BOUNDARY, //牙齿间的边界
        CUTTING_POINT //两种边界的交叉点
    };

private:
    QWidget *mParentWidget;

    Mesh mToothMesh; //牙齿模型网格
    Mesh mTempToothMesh; //用于保存ToothMesh的临时状态
    Mesh mExtraMesh; //附加信息网格，用来显示牙龈分割平面等附加信息

    //Mesh自定义属性
    OpenMesh::VPropHandleT<float> mVPropHandleCurvature; //顶点处曲率
    static const string mVPropHandleCurvatureName; //保存带自定义属性的Mesh到文件时需要设置的对应属性的名称
    OpenMesh::VPropHandleT<bool> mVPropHandleCurvatureComputed; //该顶点处曲率是否被正确计算
    static const string mVPropHandleCurvatureComputedName;
    OpenMesh::VPropHandleT<bool> mVPropHandleIsToothBoundary; //该顶点是否是牙齿边界点
    static const string mVPropHandleIsToothBoundaryName;
    OpenMesh::VPropHandleT<int> mVPropHandleBoundaryVertexType; //该顶点属于的边界点类别（提取单点宽度边界迭代时用，在mBoundaryVertexType中取值）
    static const string mVPropHandleBoundaryVertexTypeName;
    OpenMesh::VPropHandleT<int> mVPropHandleNonBoundaryRegionType; //该顶点属于的非边界区域类别（在mNonBoundaryRegionType中取值）
    static const string mVPropHandleNonBoundaryRegionTypeName;
    OpenMesh::VPropHandleT<bool> mVPropHandleRegionGrowingVisited; //该顶点在区域生长过程中是否已被访问过
    static const string mVPropHandleRegionGrowingVisitedName;
    OpenMesh::VPropHandleT<int> mVPropHandleBoundaryType; //该顶点所在的边界类别（寻找牙齿分割点时用，在mBoundaryType中取值）
    static const string mVPropHandleBoundaryTypeName;
    OpenMesh::VPropHandleT<int> mVPropHandleSearchContourSectionVisited; //该顶点在搜索边界曲线段（根据cutting point将轮廓分成若干段）过程中是否已被访问过
    static const string mVPropHandleSearchContourSectionVisitedName;

    int mBoundaryVertexNum; //牙齿边界点数量

    Mesh::Point mGingivaCuttingPlanePoint; //牙龈分割平面点
    Mesh::Normal mGingivaCuttingPlaneNormal; //牙龈分割平面法向量
    bool mGingivaCuttingPlaneComputed; //牙龈分割平面是否已计算过

    int mToothNum; //牙齿颗数

    //TODO 新建成员变量分别保存边界点handle等中间结果、所有顶点handle和mesh的所有自定义属性
    QVector<Mesh::VertexHandle> mCuttingPointHandles; //cutting point handle

    QVector< QVector<Mesh::VertexHandle> > mContourSections; //所有轮廓曲线段（顶点handle索引）

    QVector<Mesh::Point> mToothMeshVertices;
    QVector<Mesh::VertexHandle> mToothMeshVertexHandles;

public:
    ToothSegmentation(QWidget *parentWidget, const Mesh &toothMesh);

    void setToothMesh(const Mesh &toothMesh);

    Mesh getToothMesh() const;

    Mesh getExtraMesh() const;

    //4.1 Identify potential tooth boundary
    void identifyPotentialToothBoundary(bool loadStateFromFile);

    //4.2 Automatic cutting of gingiva
    void automaticCuttingOfGingiva(bool loadStateFromFile, bool flipCuttingPlane, float moveCuttingPlaneDistance);

    //4.3 Boundary skeleton extraction
    void boundarySkeletonExtraction(bool loadStateFromFile);

    //5.1 Finding cutting points
    void findCuttingPoints(bool loadStateFromFile);

    //6. Refine tooth boundary
    void refineToothBoundary(bool loadStateFromFile);

private:
    //4.1 Identify potential tooth boundary
    void identifyPotentialToothBoundary(QProgressDialog &progress);

    //4.2 Automatic cutting of gingiva
    void automaticCuttingOfGingiva(QProgressDialog &progress);

    //4.3 Boundary skeleton extraction
    void boundarySkeletonExtraction(QProgressDialog &progress);

    //5.1 Finding cutting points
    void findCuttingPoints(QProgressDialog &progress);

    //6. Refine tooth boundary
    void refineToothBoundary(QProgressDialog &progress);

    //计算曲率
    void computeCurvature(QProgressDialog &progress);

    //将曲率转换成灰度，再转换成伪彩色，将伪彩色信息写入到顶点颜色属性
    void curvature2PseudoColor();

    //计算曲率最大值和最小值
    void computeCurvatureMinAndMax(float &curvatureMin, float &curvatureMax);

    //对边界区域进行1邻域腐蚀操作
    void corrodeBoundary(QProgressDialog &progress);

    //对边界区域进行1邻域膨胀操作
    void dilateBoundary(QProgressDialog &progress);

    //边界点着色
    void paintBoundaryVertices(QProgressDialog &progress);

    //边界点分类，classifiedBoundaryVertexNum返回各类边界点数量
    void classifyBoundaryVertex(QProgressDialog &progress, int *classifiedBoundaryVertexNum);

    //分类后的边界点着色
    void paintClassifiedBoundaryVertices(QProgressDialog &progress);

    //根据牙龈分割平面剔除牙龈上的初始边界点
    void removeBoundaryVertexOnGingiva(QProgressDialog &progress);

    //标记牙龈区域
    void markNonBoundaryRegion(QProgressDialog &progress);

    //区域生长，返回该区域的顶点数量。如果regionType为TEMP_REGION，则只计算该区域的顶点数量，visited属性不变为true，也就是说可以再次进行区域生长；如果regionType为FILL_BOUNDARY_REGION，则将此区域填充为边界
    int regionGrowing(Mesh::VertexHandle vertexHandle, int regionType);

    //非边界区域分类着色
    void paintClassifiedNonBoundaryRegions(QProgressDialog &progress);

    //测试，保存牙齿模型（带颜色信息）
    void saveToothMesh(string filename);

    //测试，保存附加模型（带颜色信息）
    void saveExtraMesh(string filename);

    //灰度转伪彩色
    void gray2PseudoColor(float grayValue, Mesh::Color &pseudoColor);

    //将整个模型所有顶点颜色涂成白色
    void paintAllVerticesWhite(QProgressDialog &progress);

    //创建一个平面，存放在mExtraMesh中（point：平面过一点，normal：平面法向量，size：平面正方形边长）
    void createPlaneInExtraMesh(Mesh::Point point, Mesh::Normal normal, float size);

    //保存当前状态（stateSymbol：要保存的状态的标志，返回是否保存成功）
    bool saveState(QProgressDialog &progress, string stateSymbol);

    //读取保存的状态（stateSymbol：要读取的状态的标志，返回是否读取成功）
    bool loadState(QProgressDialog &progress, string stateSymbol);

    //基于PCL的3维点云K近邻搜索（querys中各点到points的最近距离），TODO 将工程中用到的所有STL转换为QTL
    QVector< QVector<int> > kNearestNeighbours(int Knn, const QVector<Mesh::Point> &querys, const QVector<Mesh::Point> &points);

    //分类后的边界着色
    void paintClassifiedBoundary(QProgressDialog &progress);

    //建立单点轮廓点索引
    void indexContourSectionsVertices(QProgressDialog &progress);

    //获取k邻域内所有顶点，包括中心点（中心点在返回列表的首位）
    inline void getKRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &ringVertexHandles);

    //获取k邻域中最外围的顶点，按连接顺序排序
    inline void getKthRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &ringVertexHandles);

    //连接边界（k为连接桥梁半长）
    void connectBoundary(QProgressDialog &progress, const int k);

    //测试，计算曲率直方图
    void computeCurvatureHistogram(QProgressDialog &progress);

    /*//检查mToothMesh中是否存在curvature（顶点处的曲率）属性和curvature_computed（曲率是否被正确计算），如果不存在则报错
    void checkCustomMeshPropertiesExistence();

    //计算两个向量夹角的cos值
    float cos(const Mesh::Point &vector1, const Mesh::Point &vector2) const;

    //计算两个向量夹角的cot值
    float cot(const Mesh::Point &vector1, const Mesh::Point &vector2) const;*/

    //将屏幕2维坐标转换为模型3维坐标
    inline Mesh::Point screenCoordinate2Model3DCoordinate(int screenX, int screenY);

    //计算两点之间距离
    inline float distance(Mesh::Point point1, Mesh::Point point2);

public slots:
    //鼠标右键点击显示顶点属性信息
    void mousePressEventShowVertexAttributes(QMouseEvent *e);

    //鼠标右键点击连接初始边界断开处
    void mousePressEventConnectBoundary(QMouseEvent *e);

};

#endif // TOOTHSEGMENTATION_H
