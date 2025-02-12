//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include "utils/QgsUtil.h"

TEST(QgsUtilTest, font1) {

    auto font = new QFont("黑体");
    GTEST_LOG_(INFO) << "Font family: " << font->family().toStdString();
//    GTEST_LOG_(INFO) << "default family: " << font->defaultFamily().toStdString();
    GTEST_LOG_(INFO) << "Font size: " << font->pointSize();
    GTEST_LOG_(INFO) << "Font style: " << font->styleName().toStdString();
    GTEST_LOG_(INFO) << "Font weight: " << font->weight();
    GTEST_LOG_(INFO) << "Font pixel size: " << font->pixelSize();
    GTEST_LOG_(INFO) << "Font bold: " << font->bold();
    GTEST_LOG_(INFO) << "Font italic: " << font->italic();
    GTEST_LOG_(INFO) << "Font underline: " << font->underline();
    GTEST_LOG_(INFO) << "Font overline: " << font->overline();
    GTEST_LOG_(INFO) << "Font strikeout: " << font->strikeOut();
    GTEST_LOG_(INFO) << "Font fixed pitch: " << font->fixedPitch();
    GTEST_LOG_(INFO) << "Font kerning: " << font->kerning();
    delete font;
}



TEST(QgsUtilTest, qgsTextFormat1) {
    auto text_format = std::make_shared<QgsTextFormat>();
    //auto font = std::make_unique<QFont>(font_family);
    auto font = text_format->font();
    font.setFamily("黑体");
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0.0);
    text_format->setFont(font);
    text_format->setColor(QColor("#000000"));
    text_format->setForcedBold(true);
    text_format->setForcedItalic(true);
    text_format->setOrientation(Qgis::TextOrientation::Horizontal);
    text_format->setSizeUnit(Qgis::RenderUnit::Points);
    text_format->setSize(12);


    GTEST_LOG_(INFO) << "Font family: " << text_format->font().family().toStdString();
    GTEST_LOG_(INFO) << "Font size: " << text_format->size();
    GTEST_LOG_(INFO) << "Font color: " << text_format->color().name().toStdString();
    GTEST_LOG_(INFO) << "Font bold: " << text_format->forcedBold();
    GTEST_LOG_(INFO) << "Font italic: " << text_format->forcedItalic();
    GTEST_LOG_(INFO) << "Font orientation: " << qt_getEnumName(text_format->orientation());
    //delete text_format;
}

TEST(QgsUtilTest, QtFontUtil_CreateFont) {
    auto text_format = QtFontUtil::createFont("黑体", 12, "#000000", false, false, Qgis::TextOrientation::Horizontal,
                                              0.0);

    GTEST_LOG_(INFO) << "Font family: " << text_format->font().family().toStdString();
    GTEST_LOG_(INFO) << "Font size: " << text_format->size();
    GTEST_LOG_(INFO) << "Font color: " << text_format->color().name().toStdString();
    GTEST_LOG_(INFO) << "Font bold: " << text_format->forcedBold();
    GTEST_LOG_(INFO) << "Font italic: " << text_format->forcedItalic();
    GTEST_LOG_(INFO) << "Font orientation: " << qt_getEnumName(text_format->orientation());
    //delete text_format;
}