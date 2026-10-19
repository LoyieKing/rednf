// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_string_util.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_

#include <bits/stl_iterator.h>
#include <stddef.h>

// sizeof = 1
struct zen_char_traits<char> { // line 14
public:
static const char * tstrend(const char *arg0); // line 35
};
extern int hex_to_str(const unsigned char *buf, size_t buf_len, char *dest, size_t dest_len); // line 481
extern int replace_all<char, std::char_traits<char>, std::allocator<char> >(/*anon struct*/ int &const s, const char *old_str, const char *new_str); // line 576
extern unsigned int i; // line 494
extern char *p; // line 489
extern unsigned char c; // line 492
extern /*anon struct*/ int pos; // line 529
extern /*anon struct*/ int write_it; // line 538

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_
