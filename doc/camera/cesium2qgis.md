# 从 Cesium 转换到 QGIS

提供了方向向量 dir、上向量 up、右向量 right，这三个通常是正交的单位向量，构成相机的坐标系。同时还有 heading、pitch、roll 这三个欧拉角，可能在 Cesium 中用来表示相机的朝向。不过，通常情况下，方向向量、上向量、右向量已经足够构建相机的旋转矩阵，所以可能欧拉角在这里是冗余的信息，或者可能用于不同的表示方法。

接下来，观察点的位置通常指的是相机正在观察的点的位置，也就是视线的焦点。在三维图形学中，相机的视点位置（position）和观察方向（dir）可以确定视线，但观察点可能是在远裁剪面上的某个点，或者用户所关心的特定目标点。

用户提到的远裁剪面可能是一个线索。在视锥体中，远裁剪面是一个平面，距离相机的位置是 farPlane 的距离。假设观察点位于远裁剪面中心，那么可以沿着相机的视线方向（dir 向量）移动 farPlane 的距离，从而得到观察点的位置。也就是说，观察点位置 = 相机位置 + dir \* farPlane。

不过，这可能有问题，因为 farPlane 是沿着相机的视线方向的距离，但相机的方向向量 dir 通常是单位向量，所以移动 farPlane 长度会到达远裁剪面中心的位置。这个点是否就是用户所指的观察点？或者观察点是否指的是视线的焦点，可能是在某个特定的距离，比如在 Cesium 中，相机可能会看向某个具体的位置，而不仅仅是远裁剪面中心？

另外，用户提到的参数中还有 fov 和 aspectRatio，这些可能影响视锥体的形状，但计算观察点的位置是否需要这些参数？如果观察点确实是远裁剪面中心，那么只需要方向向量和 farPlane 距离即可，可能不需要 fov 和 aspectRatio。但需要确认用户的需求。

此外，需要考虑到 Cesium 中相机的参数是否以某种方式确定观察点。例如，Cesium 的 Camera 类可能有方法直接获取或计算视线方向的目标点。例如，camera.direction 向量是单位向量，而相机的 position 加上 direction 乘以某个距离可能得到目标点。这个距离可能由用户设定的参数决定，比如 farPlane，或者可能是地球表面的某个点？

另外，用户给出的参数中包括经纬度和高度，可能需要将这些转换为笛卡尔坐标，也就是 ECEF（地心地固坐标系）或 ENU（东北天坐标系）中的坐标。例如，Cesium 中的相机位置通常用 Cartesian3 表示，即三维笛卡尔坐标。因此，可能需要先将经度、纬度、高度转换为三维坐标，然后根据方向向量计算目标点。

假设用户希望得到的是远裁剪面中心点的位置，那么步骤如下：

1. 将相机的经纬高转换为三维笛卡尔坐标（使用 Cesium 的 API，例如 Cartesian3.fromDegrees）。

2. 获取相机的方向向量 dir（已经给出分量 cameraDirX, Y, Z），假设是单位向量。

3. 观察点位置 = 相机位置 + dir \* farPlane。

但需要检查 dir 向量是否指向远裁剪面的方向。在相机的视锥体中，远裁剪面中心确实是在相机的前方向（即 dir）乘以 farPlane 的距离处。因此，这个计算是正确的。

但可能存在其他情况，例如用户可能认为观察点是近裁剪面和远裁剪面之间的某个点，或者视线与地球表面的交点。不过根据常规的 3D 图形学，远裁剪面中心点通常是视线方向上的最远可见点，所以可能是用户所指的观察点。

另外，如果用户希望计算的是视锥体的中心线（即视线方向）上的某个特定点，比如在远裁剪面上的点，那么确实可以通过位置和方向向量加上距离得到。而这里的距离是 farPlane，但需要注意的是，在透视投影中，farPlane 是沿着视线方向的直线距离，而不是欧氏距离。

但需要确认是否应该使用 farPlane 作为距离。因为在透视投影中，视锥体的远裁剪面距离是沿着视线方向的距离，所以是的，应该用 farPlane 乘以 dir 向量加到相机位置上得到远裁剪面中心点。

不过，可能需要考虑相机的坐标系是否已经考虑了方向向量。例如，方向向量是否已经是在世界坐标系中的表示，或者在相机本地坐标系中的。例如，在相机本地坐标系中，前方向是 Z 轴，但这里用户给出的 dir 向量可能已经是世界坐标系中的方向，因此可以直接使用。

总结：在已知相机的位置（经纬高转换为三维坐标）、方向向量 dir（世界坐标系中的单位向量）、以及 farPlane 的情况下，观察点的位置可以计算为相机位置加上 dir 向量乘以 farPlane 的距离。因此，答案应该是可以计算的，方法是使用这些参数进行坐标转换。

但需要验证这一点是否正确。例如，在 Cesium 中，相机的方向向量是否确实指向远裁剪面中心。另外，可能需要考虑相机的视锥体的构造，但通常远裁剪面中心确实在方向向量乘以 farPlane 的位置。

另外，如果用户提供的参数中的方向向量已经是归一化的，那么直接相乘即可。否则需要先归一化 dir 向量，但根据参数描述，cameraDirX 等是方向向量的分量，可能已经是单位向量，或者可能需要归一化处理。不过，通常在 3D 引擎中，方向向量是单位向量，所以可以直接使用。

因此，结论是，可以计算出观察点的位置，即相机位置沿着方向向量移动 farPlane 的距离后的点。步骤包括将经纬高转换为笛卡尔坐标，然后加上方向向量乘以 farPlane。

## 1. 获取 Cesium 的相机参数

### 关键参数分析

相机位置：由 cameraLongitude（经度）、cameraLatitude（纬度）、cameraHeight（高度）可转换为三维笛卡尔坐标（例如通过 Cesium.Cartesian3.fromDegrees）。

方向向量：cameraDirX/Y/Z 表示相机的视线方向（单位向量），直接指向观察方向。

远裁剪面距离：farPlane 定义了视线方向的最大可见距离。

### 验证与注意事项

- 方向向量的正交性：

若 cameraDir, cameraUp, cameraRight 是正交单位向量，可直接使用；否则需归一化。

- 远裁剪面的几何意义：

远裁剪面是视锥体的最大可见平面，其中心点沿视线方向距离为 farPlane。

-地球曲率的影响：

若观察点在地表附近，需计算视线与地形的交点（例如通过 Cesium.Scene.pickPosition），而非直接使用远裁剪面中心。

### 总结

可以计算观察点位置。核心公式为：
`观察点 = 相机位置 + 方向向量 × 远裁剪面距离`
通过提供的参数（位置、方向向量、远裁剪面距离）即可完成计算。

```cpp
/**
 * 设置 3D 地图相机参数
 * @param camera 地图相机参数
 * cameraLongitude 摄像机经度
 * cameraLatitude 摄像机纬度
 * cameraHeight 摄像机高度
 * cameraDirX 摄像机方向向量 X 分量
 * cameraDirY 摄像机方向向量 Y 分量
 * cameraDirZ 摄像机方向向量 Z 分量
 * cameraUpX 摄像机上方向向量 X 分量
 * cameraUpY 摄像机上方向向量 Y 分量
 * cameraUpZ 摄像机上方向向量 Z 分量
 * cameraRightX 摄像机右方向向量 X 分量
 * cameraRightY 摄像机右方向向量 Y 分量
 * cameraRightZ 摄像机右方向向量 Z 分量
 * fov 垂直视场角
 * aspectRatio 长宽比
 * nearPlane 近裁剪面
 * farPlane 远裁剪面
 * centerLatitude 锁定中心点纬度
 * centerLongitude 锁定中心点经度
 * centerHeight 锁定中心点高度
 * heading 摄像机偏航角
 * pitch 摄像机俯仰角
 * roll 摄像机翻滚角
 */
LookAtPoint *JwLayout3D::set3DCanvasCamera(
    DTOWRAPPERNS::DTOWrapper<Camera3dPosition> &camera, double default_distance,
    double max_pitch_angle, double offset_pull_pitch) {

    }

/**
 * QGIS 3D 地图相机参数
 */
class LookAtPoint
{
private:
        QgsVector3D mLookingAtPoint;
        float mDistance;
        float mPitch = 0;
        float mYaw = 0;

public:
        LookAtPoint(QgsVector3D lookingAtPoint,
           float distance,
           float pitch,
           float yaw)
        {
                mLookingAtPoint = lookingAtPoint;
                mDistance = distance;
                mPitch = pitch;
                mYaw = yaw;
        };

        ~LookAtPoint() = default;

        QgsVector3D lookingAtPoint()
        {
                return mLookingAtPoint;
        }

        void setLookingAtPoint( QgsVector3D lookingAtPoint )
        {
                mLookingAtPoint = lookingAtPoint;
        }

        float distance() const
        {
                return mDistance;
        }

        void setDistance( float distance )
        {
                mDistance = distance;
        }

        float pitch() const
        {
                return mPitch;
        }

        void setPitch( float pitch )
        {
                mPitch = pitch;
        }

        float yaw() const
        {
                return mYaw;
        }

        void setYaw(float yaw)
        {
                mYaw = yaw;
        }
};
```

## 2. 将 Cesium 的相机参数转换为 QGIS 的相机参数

以下是使用 C++（基于 Cesium Native 或类似地理空间计算库）计算观察点位置的代码示例。假设你已经有类似 Cartesian3 的 3D 向量类，并支持基本的向量运算（如加法、标量乘法等）。

```cpp
#include <cmath>

// 假设有一个简单的 3D 向量类（类似 Cesium 的 Cartesian3）
struct Cartesian3 {
    double x, y, z;

    Cartesian3(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}

    // 向量加法
    Cartesian3 operator+(const Cartesian3& other) const {
        return Cartesian3(x + other.x, y + other.y, z + other.z);
    }

    // 向量乘以标量
    Cartesian3 operator*(double scalar) const {
        return Cartesian3(x * scalar, y * scalar, z * scalar);
    }
};

// 从经纬度（WGS84 椭球体）计算笛卡尔坐标（简化版，实际需用椭球体模型）
Cartesian3 fromDegrees(double longitude, double latitude, double height) {
    // 简化计算：假设地球是完美球体（实际需用 WGS84 参数）
    const double earthRadius = 6378137.0; // WGS84 椭球体长半轴
    double lonRad = longitude * (M_PI / 180.0);
    double latRad = latitude * (M_PI / 180.0);

    double cosLat = cos(latRad);
    return Cartesian3(
        (earthRadius + height) * cosLat * cos(lonRad),
        (earthRadius + height) * cosLat * sin(lonRad),
        (earthRadius + height) * sin(latRad)
    );
}

// 计算观察点位置
Cartesian3 calculateViewPoint(
    double cameraLongitude, double cameraLatitude, double cameraHeight,
    double cameraDirX, double cameraDirY, double cameraDirZ,
    double farPlane
) {
    // 1. 将相机位置转换为笛卡尔坐标
    Cartesian3 cameraPosition = fromDegrees(cameraLongitude, cameraLatitude, cameraHeight);

    // 2. 归一化方向向量（假设输入已是单位向量，否则需归一化）
    Cartesian3 direction(cameraDirX, cameraDirY, cameraDirZ);

    // 3. 沿方向向量延伸 farPlane 距离
    Cartesian3 farPlaneCenter = cameraPosition + direction * farPlane;

    return farPlaneCenter;
}

// 示例调用
int main() {
    // 假设相机参数（需替换为实际值）
    double cameraLongitude = 116.4;  // 经度（度）
    double cameraLatitude = 39.9;    // 纬度（度）
    double cameraHeight = 1000.0;    // 高度（米）
    double cameraDirX = 0.0, cameraDirY = 0.0, cameraDirZ = -1.0; // 指向地心（示例）
    double farPlane = 5000.0;       // 远裁剪面距离（米）

    // 计算观察点
    Cartesian3 viewPoint = calculateViewPoint(
        cameraLongitude, cameraLatitude, cameraHeight,
        cameraDirX, cameraDirY, cameraDirZ,
        farPlane
    );

    // 输出结果（世界坐标）
    printf("观察点坐标: (%.2f, %.2f, %.2f)\n", viewPoint.x, viewPoint.y, viewPoint.z);

    return 0;
}

```

## 3. cesium 的 camera 参数

```json
{
  "camera": {
    "cameraLongitude": 112.71667544112256,
    "cameraLatitude": 37.780424411199846,
    "cameraHeight": 918.4969723235808,
    "cameraDirX": -0.283375634175063,
    "cameraDirY": -0.7575647571660277,
    "cameraDirZ": 0.588042420796203,
    "cameraUpX": -0.3370122347070477,
    "cameraUpY": 0.6527332870401279,
    "cameraUpZ": 0.6784998228795288,
    "cameraRightX": -0.8978424158022624,
    "cameraRightY": -0.005907172738916054,
    "cameraRightZ": -0.4402773009099945,
    "fov": 1.0471975511965976,
    "aspectRatio": 1.6087912087912088,
    "nearPlane": 0.1,
    "farPlane": 10000000000,
    "centerLongitude": 112.77179476854889,
    "centerLatitude": 37.845574072997444,
    "centerHeight": 1176.1596527697093,
    "heading": "33.85",
    "pitch": "-6.06",
    "roll": "360.00"
  }
}
```
