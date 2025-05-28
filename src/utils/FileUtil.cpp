//
// Created by etl on 2/3/25.
//

#include "spdlog/fmt/bundled/format.h"
#include "FileUtil.h"

void FileUtil::create_directory(const std::string& directory_path)
{
    QDir dir(QString::fromStdString(directory_path));
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            //throw FileOperationError(QString("创建目录{%1}时出错").arg(QString::fromStdString(directory_path)).toStdString());
            throw FileOperationError(fmt::format("创建目录{}时出错",directory_path));
        }
    }
}

void FileUtil::delete_directory(const std::string& directory_path)
{
    QDir dir(QString::fromStdString(directory_path));
    if (dir.exists())
    {
        if (!dir.removeRecursively())
        {
            throw FileOperationError(fmt::format("删除目录{}时出错", directory_path));
        }
    }
}

std::vector<std::string> FileUtil::list_files(const std::string& directory_path)
{
    std::vector<std::string> file_list;
    QDir dir(QString::fromStdString(directory_path));
    if (dir.exists())
    {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
        for (const QFileInfo& entry : entries)
        {
            file_list.push_back(entry.filePath().toStdString());
        }
    }
    return file_list;
}

void FileUtil::copy_file(const std::string& source_path, const std::string& destination_path)
{
    if (!QFile::copy(QString::fromStdString(source_path), QString::fromStdString(destination_path)))
    {
        throw FileOperationError(fmt::format("复制文件{} -> {}时出错", source_path, destination_path));
    }
}

void FileUtil::move_file(const std::string& source_path, const std::string& destination_path)
{
    if (!QFile::rename(QString::fromStdString(source_path), QString::fromStdString(destination_path)))
    {
        throw FileOperationError(fmt::format("移动文件{} -> {}时出错", source_path, destination_path));
    }
}

void FileUtil::delete_file(const QString& file_path)
{
    QFile file(file_path);
    if (file.exists())
    {
        if (!file.remove())
        {
            throw FileOperationError(fmt::format("删除文件{}时出错", file_path.toStdString()));
        }
    }
}

bool FileUtil::delete_file_with_status(const QString& file_path)
{
    QFile file(file_path);
    if (file.exists())
    {
        return file.remove();
    } else {
        return true;
    }
}
