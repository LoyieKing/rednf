// Reconstructed from gunnersvr disassembly.
#include "src/commsvr/gunnersvr/gunner_file_scanner.h"

#include <stdlib.h>
#include <string.h>

// zen_os_adapt_dirent.h 未收录 scandir，此处按二进制符号补声明
namespace ZEN_OS {
int scandir(const char *dir, struct dirent ***name_list,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **));
}

GunnerFileScanner::GunnerFileScanner() {
}

GunnerFileScanner::~GunnerFileScanner() {
}

int check_for_stat_file(const struct dirent *file) {
    if (file->d_reclen <= 4) {
        return 0;
    }

    const char *begin_pos = file->d_name;
    if (strncmp(begin_pos, "ZERG", 4) != 0 && strncmp(begin_pos, "TSS", 3) != 0) {
        return 0;
    }

    const char *end_pos = begin_pos + strlen(begin_pos) - 5;
    if (strncmp(end_pos, ".STAT", 5) != 0) {
        return 0;
    }

    int cnt = 0;
    for (const char *p = begin_pos; *p; ++p) {
        if (*p == '_') {
            ++cnt;
        }
    }
    return cnt == 2;
}

int GunnerFileScanner::scan_stat_file(const char *dir, std::vector<std::string> &all_files) {
    int file_num = 0;
    struct dirent **name_list = NULL;

    file_num = ZEN_OS::scandir(dir, &name_list, check_for_stat_file, NULL);
    if (file_num < 0) {
        return -1;
    }

    for (int i = file_num - 1; i >= 0; --i) {
        std::string tmp(name_list[i]->d_name);
        all_files.push_back(tmp);
        free(name_list[i]);
    }
    free(name_list);
    return 0;
}
