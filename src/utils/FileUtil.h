﻿//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_FILEUTIL_H
#define CBOOT_FILEUTIL_H


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <iostream>

#include "core/error/exceptions.h"

#if defined(_WIN32)
#include <windows.h>
#endif

class FileUtil
{
public:
    static void create_directory(const std::string& directory_path);


    static void delete_directory(const std::string& directory_path);


    static std::vector<std::string> list_files(const std::string& directory_path);


    static void copy_file(const std::string& source_path, const std::string& destination_path);


    static void move_file(const std::string& source_path, const std::string& destination_path);


    static void delete_file(const QString& file_path);


    static bool delete_file_with_status(const QString& file_path);
};



#endif //CBOOT_FILEUTIL_H
