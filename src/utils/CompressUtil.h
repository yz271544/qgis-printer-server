//
// Created by etl on 2025/2/9.
//

#ifndef JINGWEIPRINTER_COMPRESSUTIL_H
#define JINGWEIPRINTER_COMPRESSUTIL_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <archive.h>
#include <archive_entry.h>
#include <zip.h>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

class CompressUtil {

public:
    static void un_gz(const std::string& file_full_name, const std::string& target_dir);
    static void un_tar(const std::string& file_full_name, const std::string& target_dir);
    static void create_zip(const std::string& source_dir, const std::string& output_filename);
    static void un_zip(const std::string& file_full_name, const std::string& target_dir);
    static void in_tar(const std::string& source_dir, const std::string& target_full_tar);
    static void in_tar_gz(const std::string& source_dir, const std::string& output_filename);

};




#endif //JINGWEIPRINTER_COMPRESSUTIL_H
