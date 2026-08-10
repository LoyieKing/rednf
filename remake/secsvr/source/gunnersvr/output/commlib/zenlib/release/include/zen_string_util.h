// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_string_util.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_

#include <string>

// Reconstructed for compilation: the DWARF stub only recorded the explicit
// specialization; the primary template is restored here.
template <typename T> struct zen_char_traits;

template <> struct zen_char_traits<char> { // line 14
public:
static const char * tstrend(const char *arg0); // line 35
};

// Template declaration matching the weak symbol in the binary:
//   replace_all<char, std::char_traits<char>, std::allocator<char>,
//               char const*, char const*, std::equal_to<char> >
template <typename T, typename Traits, typename Alloc,
          typename HeadT, typename TailT, typename CompT>
int replace_all(std::basic_string<T, Traits, Alloc> &s,
                const T *old_str, const T *new_str,
                const HeadT &head, const TailT &tail, const CompT &comp);

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_
