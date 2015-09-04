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
    enum BoundaryVertexType
    {
        CENTER_VERTEX = 0, //中心点（边界内部点）
        COMPLEX_VERTEX, //复杂点（单点宽度边界点）
        DISK_VERTEX_GINGIVA, //外围点（边界外围点，与牙龈区域相邻）
        DISK_VERTEX_TOOTH //外围点（边界外围点，与第1颗牙齿区域相邻，后续逐个加1）
    };

    enum NonBoundaryRegionType
    {
        ERROR_REGION = -2, //被complex vertex包围的center vertex被归为此类
        FILL_BOUNDARY_REGION = -1, //需要填充为边界的区域（用来去除噪声区域，最终mesh中不存在此类点）
        TEMP_REGION = 0, //临时区域（用来计算区域内的定点数，最终mesh中不存在此类点）
        GINGIVA_REGION, //牙龈区域
        TOOTH_REGION //牙齿区域（此为第1颗牙齿区域，后续逐个加1）
    };

    enum BoundaryType
    {
        TOOTH_GINGIVA_BOUNDARY = 0, //牙齿与牙龈的边界
        TOOTH_TOOTH_BOUNDARY, //牙齿间的边界
        CUTTING_POINT, //两种边界的交叉点
        JOINT_POINT //牙齿间边界与牙齿间边界的交叉点
    };

    enum ProgramScheduleValues
    {
        SCHEDULE_START = 0,
        SCHEDULE_IdentifyPotentialToothBoundary_STARTED,
        SCHEDULE_IdentifyPotentialToothBoundary_FINISHED,
        SCHEDULE_AutomaticCuttingOfGingiva_STARTED,
        SCHEDULE_AutomaticCuttingOfGingiva_FINISHED,
        SCHEDULE_BoundarySkeletonExtraction_STARTED,
        SCHEDULE_BoundarySkeletonExtraction_FINISHED,
        SCHEDULE_FindCuttingPoints_STARTED,
        SCHEDULE_FindCuttingPoints_FINISHED,
        SCHEDULE_RefineToothBoundary_STARTED,
        SCHEDULE_RefineToothBoundary_FINISHED
    };

    enum CursorType
    {
        CURSOR_DEFAULT = 0,
        CURSOR_ADD_BOUNDARY_VERTEX,
        CURSOR_DELETE_BOUNDARY_VERTEX,
        CURSOR_DELETE_ERROR_CONTOUR_SECTION
    };

private:
    QWidget *mParentWidget;
    QProgressDialog *mProgress;

    Mesh mToothMesh; //牙齿模型网格
    Mesh mTempToothMesh; //用于保存ToothMesh的临时状态
    Mesh mExtraMesh; //附加信息网格，用来显示牙龈分割平面等附加信息
    //bool mShouldShowExtraMesh; //是否要显示ExtraMesh

    ProgramScheduleValues mProgramSchedule; //记录程序运行进度

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
    QVector<Mesh::VertexHandle> mJointPointHandles; //joint point handle
    QVector< QVector<Mesh::VertexHandle> > mContourSections; //所有轮廓曲线段（顶点handle索引）

    QVector<Mesh::Point> mToothMeshVertices;
    QVector<Mesh::VertexHandle> mToothMeshVertexHandles;

    QVector<Mesh::VertexHandle> mErrorRegionVertexHandles; //记录属于ERROR_REGION的顶点

    CursorType mCursorType; //鼠标形状类型（在不同鼠标操作时显示不同形状的鼠标）
    int mCircleCursorRadius; //鼠标为圆形时圆形的半径

    QVector<QPoint> mMouseTrack; //鼠标拖动轨迹

public:
    ToothSegmentation(QWidget *parentWidget, const Mesh &toothMesh);

    ToothSegmentation();

    ToothSegmentation(const ToothSegmentation &toothSegmentation);

    ToothSegmentation& operator=(const ToothSegmentation &toothSegmentation);

    void copyFrom(const ToothSegmentation &toothSegmentation);

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

    //设置自定义鼠标形状
    void setCustomCursor(CursorType cursorType);

    //获取程序当前运行进度
    ProgramScheduleValues getProgramSchedule();

    //是否显示ExtraMesh
    bool shouldShowExtraMesh();

private:
    void setToothMesh(const Mesh &toothMesh);

    //4.1 Identify potential tooth boundary
    void identifyPotentialToothBoundary();

    //4.2 Automatic cutting of gingiva
    void automaticCuttingOfGingiva();

    //4.3 Boundary skeleton extraction
    void boundarySkeletonExtraction();

    //5.1 Finding cutting points
    void findCuttingPoints();

    //6. Refine tooth boundary
    void refineToothBoundary();

    //计算曲率
    void computeCurvature();

    //将曲率转换成灰度，再转换成伪彩色，将伪彩色信息写入到顶点颜色属性
    void curvature2PseudoColor();

    //计算曲率最大值和最小值
    void computeCurvatureMinAndMax(float &curvatureMin, float &curvatureMax);

    //对边界区域进行1邻域腐蚀操作
    void corrodeBoundary();

    //对边界区域进行1邻域膨胀操作
    void dilateBoundary();

    //边界点着色
    void paintBoundaryVertices();

    //边界点分类，classifiedBoundaryVertexNum返回各类边界点数量
    void classifyBoundaryVertex(int *classifiedBoundaryVertexNum);

    //分类后的边界点着色
    void paintClassifiedBoundaryVertices();

    //根据牙龈分割平面剔除牙龈上的初始边界点
    void removeBoundaryVertexOnGingiva();

    //标记牙龈区域，返回牙龈区域个数，可据此判断是否需要对牙龈分割平面进行翻转
    int markNonBoundaryRegion();

    //区域生长，返回该区域的顶点数量。如果regionType为TEMP_REGION，则只计算该区域的顶点数量，visited属性不变为true，也就是说可以再次进行区域生长；如果regionType为FILL_BOUNDARY_REGION，则将此区域填充为边界
    int regionGrowing(Mesh::VertexHandle vertexHandle, int regionType);

    //非边界区域分类着色
    void paintClassifiedNonBoundaryRegions();

    //测试，保存牙齿模型（带颜色信息）
    void saveToothMesh(string filename);

    //测试，保存附加模型（带颜色信息）
    void saveExtraMesh(string filename);

    //灰度转伪彩色
    void gray2PseudoColor(float grayValue, Mesh::Color &pseudoColor);

    //将整个模型所有顶点颜色涂成白色
    void paintAllVerticesWhite();

    //创建一个平面，存放在mExtraMesh中（point：平面过一点，normal：平面法向量，size：平面正方形边长）
    void createPlaneInExtraMesh(Mesh::Point point, Mesh::Normal normal, float size);

    //保存当前状态（stateSymbol：要保存的状态的标志，返回是否保存成功）
    bool saveState(string stateSymbol);

    //读取保存的状态（stateSymbol：要读取的状态的标志，返回是否读取成功）
    bool loadState(string stateSymbol);

    //基于PCL的3维点云K近邻搜索（querys中各点到points的最近距离），TODO 将工程中用到的所有STL转换为QTL
    QVector< QVector<int> > kNearestNeighbours(int Knn, const QVector<Mesh::Point> &querys, const QVector<Mesh::Point> &points);

    //基于PCL的2维点云K近邻搜索（querys中各点到points的最近距离）
    QVector< QVector<int> > kNearestNeighbours(int Knn, const QVector<QPoint> &querys, const QVector<QPoint> &points);

    //分类后的边界着色
    void paintClassifiedBoundary();

    //建立单点轮廓点索引
    void indexContourSectionsVertices();

    //获取k邻域内所有顶点，包括中心点（中心点在返回列表的首位）
    inline void getKRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &ringVertexHandles);

    //获取k邻域中最外围的顶点，按连接顺序排序
    inline void getKthRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &ringVertexHandles);

    //连接边界（k为连接桥梁半长）
    //void connectBoundary(, const int k);

    //测试，计算曲率直方图
    void computeCurvatureHistogram();

    /*//检查mToothMesh中是否存在curvature（顶点处的曲率）属性和curvature_computed（曲率是否被正确计算），如果不存在则报错
    void checkCustomMeshPropertiesExistence();

    //计算两个向量夹角的cos值
    float cos(const Mesh::Point &vector1, const Mesh::Point &vector2) const;

    //计算两个向量夹角的cot值
    float cot(const Mesh::Point &vector1, const Mesh::Point &vector2) const;*/

    //将屏幕2维坐标转换为模型3维坐标
    inline Mesh::Point screenCoordinate2Model3DCoordinate(const int screenX, const int screenY);

    //将模型3维坐标转换为屏幕2维坐标
    inline void model3DCoordinate2ScreenCoordinate(const Mesh::Point modelPoint, int &screenX, int &screenY, float &depth);

    //计算两点之间距离
    inline float distance(Mesh::Point point1, Mesh::Point point2);

    //判断某顶点是否可见
    inline bool isVisiable(Mesh::VertexHandle vertexHandle);

    //根据mMouseTrack获取鼠标拖动时选中的所有可见的顶点
    QVector<Mesh::VertexHandle> getSelectedVertices();

    //更新程序运行状态
    void updateProgramSchedule(ProgramScheduleValues programSchedule);

public slots:
    //鼠标右键点击显示顶点属性信息
    void mousePressEventShowVertexAttributes(QMouseEvent *e);

    //鼠标右键点击开始记录鼠标轨迹（用于添加或去除边界点）（在automaticCuttingOfGingiva之后使用）
    void mousePressEventStartRecordMouseTrack(QMouseEvent *e);

    //鼠标右键拖动记录鼠标轨迹（用于添加或去除边界点）（在automaticCuttingOfGingiva之后使用）
    void mouseMoveEventRecordMouseTrack(QMouseEvent *e);

    //将鼠标右键拖动选中的点添加为边界点（在automaticCuttingOfGingiva之后使用）
    void mouseReleaseEventAddSelectedBoundaryVertex(QMouseEvent *e);

    //将鼠标右键拖动选中的点剔除为非边界点（在automaticCuttingOfGingiva之后使用）
    void mouseReleaseEventDeleteSelectedBoundaryVertex(QMouseEvent *e);

    //鼠标右键点击删除错误被判为牙齿的区域，将其填充为边界（比较大的错误区域最好手动将其和牙龈区域相连）（在automaticCuttingOfGingiva之后使用）
    void mousePressEventDeleteErrorToothRegion(QMouseEvent *e);

    //鼠标右键点击删除错误的轮廓段（在findCuttingPoints之后使用）
    void mousePressEventDeleteErrorContourSection(QMouseEvent *e);

    //按键“Ctrl” + “+/-”调整鼠标(画笔)半径（用于添加或去除边界点）（在automaticCuttingOfGingiva之后使用）
    void keyPressEventChangeMouseRadius(QKeyEvent *e);

signals:
    //通知主窗口类保存历史状态，以便撤销操作
    void onSaveHistory();

    //通知主窗口程序运行状态改变
    void onProgramScheduleChanged(int programSchedule);

};

#endif // TOOTHSEGMENTATION_H
