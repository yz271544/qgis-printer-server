//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_JWLAYOUT3D_H
#define CBOOT_JWLAYOUT3D_H


#include <qgis.h>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QScreen>
#include <QSplashScreen>
#include <QStringLiteral>
#include <Qt3DCore>
#include <QtConcurrent/QtConcurrent>

#include <qentity.h>
#include <qgs3dmapcanvas.h>
#include <qgsproject.h>
#include <qgslayout.h>
#include <qgsprintlayout.h>
#include <qgslayoutitemmap.h>
#include <qgslayoutpagecollection.h>
#include <qgslayoutmanager.h>
#include <qgslayoutitemlegend.h>
#include <qgslayoutitemlabel.h>
#include <qgslayoutitempicture.h>
#include <qgslayoutitemscalebar.h>
#include <qgslayoutitempolyline.h>
#include <qgslayoutpoint.h>
#include <qgslayoutsize.h>
#include <qgslayoutmeasurement.h>
#include <qgscoordinatereferencesystem.h>
#include <qgsfillsymbol.h>
#include <qgslinesymbol.h>
#include <qgsarrowsymbollayer.h>
#include <qgsreadwritecontext.h>
#include <qgsmapcanvas.h>
#include <qgs3dmapsettings.h>
#include <qgslayoutitem3dmap.h>
#include <qgscamerapose.h>
#include <qgsvector3d.h>
#include <qgslayoutitemshape.h>
#include <qgssymbollayer.h>
#include <qgssymbollayerwidget.h>
#include <qgsmarkersymbollayer.h>
#include <qgsprojectviewsettings.h>
#include <qgsflatterraingenerator.h>
#include <qgsprojectelevationproperties.h>
#include <qgsterrainprovider.h>
#include <qgspointlightsettings.h>
#include <qgs3dmapscene.h>
#include <qgsmapviewsmanager.h>
#include <qgsconfig.h>
#include <qgs3dutils.h>
#include <qgssettings.h>
#include <qgsdirectionallightsettings.h>
#include <qgslayoutexporter.h>
#include <qgsfillsymbollayer.h>

//#include <qgis/app/3d/qgs3dmapcanvaswidget.h>
#include "JwLegend.h"
#include "core/enums/PaperSpecification.h"
#include "utils/QgsUtil.h"
#include "utils/ColorTransformUtil.h"
#include "utils/ImageUtil.h"
#include "core/qgis/d3/CameraUtil.h"
#include "core/handler/dto/plotting.h"
#include "config.h"



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


class JwLayout3D {

public:
    JwLayout3D(QgsProject *project,
               QgsMapCanvas *canvas2d,
               Qgs3DMapCanvas *canvas3d,
               QString &sceneName,
               const QVariantMap &imageSpec,
               QString &projectDir,
               QString &layoutName,
               QString &qgisPrefixPath);

    ~JwLayout3D() = default;

    void filterMapLayers(const QVector<QString> &removeLayerNames = QVector<QString>(),
                         const QVector<QString> &removeLayerPrefixes = QVector<QString>(),
                         Qgs3DMapSettings *mapSettings3d = nullptr);

    static void setPageOrientation(
            QgsPrintLayout *layout,
            const PaperSpecification &availablePaper,
            int pageNum,
            QgsLayoutItemPage::Orientation orientation = QgsLayoutItemPage::Landscape);

    void setTitle(QgsPrintLayout *layout, const QVariantMap &titleOfLayinfo);

    void setLegend(
            QgsPrintLayout *layout,
            const QVariantMap &imageSpec,
            int legendWidth = 40,
            int legendHeight = 80,
            const QString &borderColor = "#000000",
            const QSet<QString> &filteredLegendItems = QSet<QString>());

    void setRemarks(QgsPrintLayout *layout,
                    const QVariantMap &remarkOfLayinfo,
                    const bool writeQpt);

    void addRightSideLabel(
            QgsPrintLayout *layout,
            const QVariantMap &subTitle,
            int rightSideLabelWidth,
            int rightSideLabelHeight,
            bool mapDoubleFrame,
            float margin_offset_if_double_frame);

    void addSignatureLabel(
            QgsPrintLayout *layout,
            const QString &signatureText,
            bool mapDoubleFrame,
            float margin_offset_if_double_frame);

    static void addArrowToLayout(
            QgsLayout *layout,
            const QVector<QgsPointXY> &points,
            const QColor &color, double width);

    void addArrowBasedOnFrontendParams(
            QgsLayout *layout,
            const QList<QVariant> &position, double rotate);

    void init3DLayout(const QString &layoutName);

    void init3DMapSettings(
            const QVector<QString> &removeLayerNames,
            const QVector<QString> &removeLayerPrefixes
    );

    /**
     * @param camera 设置 3D 地图相机参数
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
     * heading 摄像机偏航角
     * pitch 摄像机俯仰角
     * roll 摄像机翻滚角
     */
    LookAtPoint* set3DCanvasCamera(DTOWRAPPERNS::DTOWrapper<Camera3dPosition>& camera,
            double default_distance,
            double max_pitch_angle=77.0,
            double offset_pull_pitch=16.0);

    double calculateAdjacentSide(double cameraHeight, double pitchDegrees);

    double calculate_opposite_side(double distance, double pitchDegrees);

    void setTest3DCanvas();

    void set3DMap(
            QgsPrintLayout *layout,
            const PaperSpecification &availablePaper,
            DTOWRAPPERNS::DTOWrapper<Camera3dPosition>& camera,
            int mapFrameWidth = 1,
            const QString &mapFrameColor = "#000000",
            bool isDoubleFrame = false,
            double mapRotation = 0.0,
            double max_pitch_angle = 77.0,
            double offset_pull_pitch = 16.0
    );


    void addNorthArrow(
            QgsPrintLayout *layout,
            const QVariantMap &northArrowPath);

    void addPrintLayout(const QString &layoutType, const QString &layoutName,
                        const QVariantMap &plottingWeb, const PaperSpecification &availablePaper,
                        DTOWRAPPERNS::DTOWrapper<Camera3dPosition>& camera,
                        bool writeQpt = false,
                        double max_pitch_angle=77.0,
                        double offset_pull_pitch=16.0);

    void loadQptTemplate(const QString &qptFilePath, const QString &layoutTemplateName);

    void updateLayoutExtent(const QString &layoutName);

    QPair<double, double> getLegendDimensions(const QString &layoutName);

    void exportLayoutToPng(
            const QString &layoutName,
            QString &outputPath);

    void exportLayoutToPdf(
            const QString &layoutName,
            QString &outputPath);

    void exportLayoutToSvg(
            const QString &layoutName,
            QString &outputPath);

    QgsPrintLayout *getLayout(const QString &layoutName);

    void saveQptTemplate(QgsLayout *layout);

    QgsLayoutItem3DMap *getMapItem3d();

    Qgs3DMapSettings *getMapSettings3d();

    QgsPrintLayout *getLayout3D();

    void destroy3DCanvas();

    QgsLayoutItemShape *addRect(
            QString &fillColor,
            const QString &borderColor,
            double borderWidth,
            qreal remarksX,
            qreal remarksY,
            qreal remarksWidth,
            qreal remarksHeight
    );

    static QgsPoint *transformPoint(const QgsPoint &point, const QgsCoordinateTransform &transformer);

    void setTestFrom2dExtent();

private:
    QgsProject *mProject;
    QString &mLayoutName;
    QString &mProjectDir;
    QgsMapCanvas *mCanvas2d;
    Qgs3DMapCanvas *mCanvas3d;
    QString &mSceneName;
    std::unique_ptr<JwLegend> mJwLegend;
    QgsPrintLayout *mLayout;
    QVariantMap mImageSpec;
    QgsLayoutItem3DMap *mMapItem3d;
    Qgs3DMapSettings *mMapSettings3d;
    QString &mQgisPrefixPath;
    double mMapWidth;
    double mMapHeight;
};


#endif //CBOOT_JWLAYOUT3D_H
