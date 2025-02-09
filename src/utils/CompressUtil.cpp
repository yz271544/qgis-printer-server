//
// Created by etl on 2025/2/9.
//


#include "CompressUtil.h"

void CompressUtil::un_gz(const std::string& file_full_name, const std::string& target_dir) {

    std::string f_name = file_full_name.substr(0, file_full_name.find_last_of('.'));
    std::string output_path = fs::path(target_dir) / fs::path(f_name).filename();

    struct archive *a = archive_read_new();
    archive_read_support_filter_gzip(a);
    archive_read_support_format_raw(a);

    if (archive_read_open_filename(a, file_full_name.c_str(), 10240) != ARCHIVE_OK) {
        std::cerr << "Error opening gz file: " << archive_error_string(a) << std::endl;
        return;
    }

    struct archive_entry *entry;
    if (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        std::ofstream out(output_path, std::ios::binary);
        const void *buff;
        size_t size;
        int64_t offset;

        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            out.write(static_cast<const char*>(buff), size);
        }
        out.close();
    }

    archive_read_close(a);
    archive_read_free(a);

}


void CompressUtil::un_tar(const std::string& file_full_name, const std::string& target_dir) {
    struct archive *a = archive_read_new();
    archive_read_support_format_all(a);

    if (archive_read_open_filename(a, file_full_name.c_str(), 10240) != ARCHIVE_OK) {
        std::cerr << "Error opening tar file: " << archive_error_string(a) << std::endl;
        return;
    }

    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        std::string path = fs::path(target_dir) / archive_entry_pathname(entry);
        fs::create_directories(fs::path(path).parent_path());

        std::ofstream out(path, std::ios::binary);
        const void *buff;
        size_t size;
        int64_t offset;

        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            out.write(static_cast<const char*>(buff), size);
        }
        out.close();
    }

    archive_read_close(a);
    archive_read_free(a);
}



void CompressUtil::create_zip(const std::string& source_dir, const std::string& output_filename) {
    int err = 0;
    zip_t *zip = zip_open(output_filename.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
    if (!zip) {
        std::cerr << "Error creating zip file" << std::endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(source_dir)) {
        if (entry.is_regular_file()) {
            std::string file_path = entry.path().string();
            std::string arcname = fs::relative(entry.path(), source_dir).string();

            zip_source_t *source = zip_source_file(zip, file_path.c_str(), 0, 0);
            if (!source) {
                std::cerr << "Error adding file to zip: " << file_path << std::endl;
                continue;
            }

            zip_int64_t index = zip_file_add(zip, arcname.c_str(), source, ZIP_FL_ENC_UTF_8);
            if (index < 0) {
                zip_source_free(source);
                std::cerr << "Error adding file to zip: " << file_path << std::endl;
            }
        }
    }

    zip_close(zip);
}

void CompressUtil::un_zip(const std::string& file_full_name, const std::string& target_dir) {
    int err = 0;
    zip_t *zip = zip_open(file_full_name.c_str(), 0, &err);
    if (!zip) {
        std::cerr << "Error opening zip file" << std::endl;
        return;
    }

    zip_int64_t num_entries = zip_get_num_entries(zip, 0);
    for (zip_int64_t i = 0; i < num_entries; ++i) {
        struct zip_stat st;
        if (zip_stat_index(zip, i, 0, &st) == 0) {
            std::string path = fs::path(target_dir) / st.name;
            fs::create_directories(fs::path(path).parent_path());

            zip_file_t *file = zip_fopen_index(zip, i, 0);
            if (!file) {
                std::cerr << "Error opening file in zip: " << st.name << std::endl;
                continue;
            }

            std::ofstream out(path, std::ios::binary);
            char buf[1024];
            zip_int64_t n;
            while ((n = zip_fread(file, buf, sizeof(buf))) > 0) {
                out.write(buf, n);
            }
            out.close();
            zip_fclose(file);
        }
    }

    zip_close(zip);
}

void CompressUtil::in_tar(const std::string& source_dir, const std::string& target_full_tar) {
    struct archive *a = archive_write_new();
    archive_write_set_format_ustar(a);
    archive_write_open_filename(a, target_full_tar.c_str());

    for (const auto& entry : fs::recursive_directory_iterator(source_dir)) {
        if (entry.is_regular_file()) {
            struct archive_entry *e = archive_entry_new();
            archive_entry_set_pathname(e, fs::relative(entry.path(), source_dir).string().c_str());
            archive_entry_set_size(e, entry.file_size());
            archive_entry_set_filetype(e, AE_IFREG);
            archive_entry_set_perm(e, 0644);

            archive_write_header(a, e);

            std::ifstream in(entry.path(), std::ios::binary);
            char buf[1024];
            while (in.read(buf, sizeof(buf))) {
                archive_write_data(a, buf, in.gcount());
            }
            archive_write_data(a, buf, in.gcount());

            archive_entry_free(e);
        }
    }

    archive_write_close(a);
    archive_write_free(a);
}

void CompressUtil::in_tar_gz(const std::string& source_dir, const std::string& target_full_tar) {
    struct archive *a = archive_write_new();
    archive_write_set_format_ustar(a);
    archive_write_add_filter_gzip(a);
    archive_write_open_filename(a, target_full_tar.c_str());

    for (const auto& entry : fs::recursive_directory_iterator(source_dir)) {
        if (entry.is_regular_file()) {
            struct archive_entry *e = archive_entry_new();
            archive_entry_set_pathname(e, fs::relative(entry.path(), source_dir).string().c_str());
            archive_entry_set_size(e, entry.file_size());
            archive_entry_set_filetype(e, AE_IFREG);
            archive_entry_set_perm(e, 0644);

            archive_write_header(a, e);

            std::ifstream in(entry.path(), std::ios::binary);
            char buf[1024];
            while (in.read(buf, sizeof(buf))) {
                archive_write_data(a, buf, in.gcount());
            }
            archive_write_data(a, buf, in.gcount());

            archive_entry_free(e);
        }
    }

    archive_write_close(a);
    archive_write_free(a);
}