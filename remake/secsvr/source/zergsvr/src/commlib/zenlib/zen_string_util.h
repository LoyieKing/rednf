// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_string_util.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_

#include <string>
#include <algorithm>

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

// split 模板（二进制弱符号：zen_string_util.h 内联定义，zerg_ip_restrict 使用）
template <typename T, typename T0, typename T1, typename T2, typename T3>
void split(T first, T0 last, T0 delim_first, T0 delim_last, T1 equal, T2 out,
           const T3 *sep) {
    for (;;) {
        T0 where = std::search(first, last, delim_first, delim_last, equal);
        *out++ = T3(first, where);
        if (where == last) {
            return;
        }
        if (sep != NULL) {
            *out++ = *sep;
        }
        first = where + (delim_last - delim_first);
    }
}

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_STRING_UTIL_H_H_
