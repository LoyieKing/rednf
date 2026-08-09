// Auto-generated header stub from DWARF info
// Original path: output/commsvr/gunnersvr/release/include/gunner_file_scanner.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_FILE_SCANNER_H_H_
#define SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_FILE_SCANNER_H_H_

// sizeof = 1
struct GunnerFileScanner { // line 22
public:
GunnerFileScanner(); // line 24
~GunnerFileScanner(); // line 26
int scan_stat_file(const char *dir, /*anon struct*/ int &all_files); // line 30
};


extern int check_for_stat_file(const dirent *file); // line 31 (inferred)

#endif // SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_FILE_SCANNER_H_H_
