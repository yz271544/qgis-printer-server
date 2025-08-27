//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_JWLAYOUT_H
#define CBOOT_JWLAYOUT_H

#include <spdlog/spdlog.h>
#include <qgis.h>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QScreen>
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
#include <qgslayoutexporter.h>
#include <qgsfillsymbollayer.h>
#include <utility>
#include <QtConcurrent/QtConcurrent>


#include "JwLegend.h"
#include "core/enums/PaperSpecification.h"
#include "utils/QgsUtil.h"
#include "utils/ColorTransformUtil.h"
#include "utils/ImageUtil.h"

class JwLayout {
public:

    JwLayout(QgsProject *project,
             QgsMapCanvas *canvas,
             QString &sceneName,
             const QVariantMap &imageSpec,
             QString &projectDir,
             QString &layoutName,
             QString &qgisPrefixPath);

    ~JwLayout() = default;

    void filterMapLayers(
            QgsLayoutItemMap *mapItem,
            const QVector<QString> &removeLayerNames = QVector<QString>(),
            const QVector<QString> &removeLayerPrefixes = QVector<QString>());

    static void setPageOrientation(QgsPrintLayout *layout, const PaperSpecification &availablePaper, int pageNum,
                                   QgsLayoutItemPage::Orientation orientation = QgsLayoutItemPage::Landscape);

    void setTitle(QgsPrintLayout *layout, const QVariantMap &titleOfLayinfo);

    void setLegend(QgsPrintLayout *layout, const QVariantMap &imageSpec, int legendWidth = 40, int legendHeight = 80,
                   const QString &borderColor = "#000000", const QSet<QString> &filteredLegendItems = QSet<QString>());

    void setRemarks(QgsPrintLayout *layout, const QVariantMap &remarkOfLayinfo, const bool writeQpt);

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

    void addScaleBar(QgsPrintLayout *layout);

    static void
    addArrowToLayout(QgsLayout *layout, const QVector<QgsPointXY> &points, const QColor &color, double width);

    void addArrowBasedOnFrontendParams(QgsPrintLayout *layout, const QList<QVariant> &position, double rotate);

    void init2DLayout(const QString &layoutName);

    void setMap(
            QgsPrintLayout *layout,
            const PaperSpecification &availablePaper,
            int mapFrameWidth = 1,
            const QString &mapFrameColor = "#000000",
            bool isDoubleFrame = false,
            const QVector<QString> &removeLayerNames = QVector<QString>(),
            const QVector<QString> &removeLayerPrefixes = QVector<QString>(),
            double mapRotation = 0.0);

    void addNorthArrow(
            QgsPrintLayout *layout,
            const QVariantMap &northArrowPath);

    void addPrintLayout(const QString &layoutType, const QString &layoutName,
                        const QVariantMap &plottingWeb, const PaperSpecification &availablePaper,
                        bool writeQpt = false, const QVector<QString> &removeLayerNames = QVector<QString>(),
                        const QVector<QString> &removeLayerPrefixes = QVector<QString>());

    void loadQptTemplate(const QString &qptFilePath, const QString &layoutTemplateName);

    void updateLayoutExtent(const QString &layoutName);

    QPair<double, double> getLegendDimensions(const QString &layoutName);

    QgsPrintLayout *getLayout(const QString &layoutName);

    QgsLayoutItemMap *getMapItem();

    void saveQptTemplate(QgsPrintLayout *layout);

    QgsLayoutItemShape *addRect(
            QString &fillColor,
            const QString &borderColor,
            double borderWidth,
            qreal remarksX,
            qreal remarksY,
            qreal remarksWidth,
            qreal remarksHeight
    );

    void exportLayoutAsPng(const QString &layoutName,
                           const QString &outputPath,
                           bool forceEvent);

    void exportLayoutAsPdf(const QString &layoutName,
                           const QString &outputPath,
                           bool forceEvent);

    void exportLayoutAsSvg(const QString &layoutName,
                           const QString &outputPath,
                           bool forceEvent);

private:
    QString &mLayoutName;
    QgsProject *mProject;
    QString &mProjectDir;
    QgsMapCanvas *mCanvas;
    QString &mSceneName;
    std::unique_ptr<JwLegend> mJwLegend;
    QgsPrintLayout *mLayout;
    QString &mQgisPrefixPath;
    QVariantMap mImageSpec;
    double mMapWidth;
    double mMapHeight;
};

#endif //CBOOT_JWLAYOUT_H
