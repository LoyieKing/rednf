// Reconstructed from gunnersvr DWARF + disassembly.
// sizeof = 1：无成员。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_FILE_SCANNER_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_FILE_SCANNER_H_H_

#include <dirent.h>
#include <string>
#include <vector>

// sizeof = 1
struct GunnerFileScanner { // line 22
public:
    GunnerFileScanner(); // line 24
    ~GunnerFileScanner(); // line 26
    int scan_stat_file(const char *dir, std::vector<std::string> &all_files); // line 30
};

extern int check_for_stat_file(const struct dirent *file); // line 31

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_FILE_SCANNER_H_H_
