// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_string_util.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_STRING_UTIL_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_STRING_UTIL_H_H_

#include <bits/stl_iterator.h>

// sizeof = 1
struct zen_char_traits<char> { // line 14
public:
static int tisspace(int arg0); // line 26
static const char * tstrend(const char *arg0); // line 35
};
extern int str_to_hex(const char *str, unsigned char *buff, unsigned int *buf_len); // line 412
extern int replace_all(char *s, const char *old_chars, const char new_char); // line 602
extern int replace_all<char, std::char_traits<char>, std::allocator<char> >(/*anon struct*/ int &const s, const char *old_str, const char *new_str); // line 576
extern short int idx; // line 422
extern short unsigned int i; // line 423
extern char temp_char; // line 424
extern int temp; // line 425
extern short unsigned int str_len; // line 426
extern char *p; // line 610
extern /*anon struct*/ int pos; // line 529
extern /*anon struct*/ int write_it; // line 538

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_STRING_UTIL_H_H_
