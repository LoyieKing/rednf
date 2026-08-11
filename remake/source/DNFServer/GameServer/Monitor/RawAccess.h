#ifndef MONITOR_RAWACCESS_H_
#define MONITOR_RAWACCESS_H_
// 逐字节布局访问助手（monitor 无 DWARF，原版为命名成员源码；为让 GCC -O0
// 生成与 ORIG 相同的位移寻址 `mov 0xNN(%eax),%eax` / `movl $0x0,0xNN(%eax)`，
// 用 packed 模板结构按固定偏移做成员访问，避免 `(char*)p + off` 触发
// `add $0xNN,%eax` + 间接寻址形态）。
template <int OFF> struct RA_INT   { char p[OFF]; int v; }           __attribute__((packed));
template <int OFF> struct RA_UINT  { char p[OFF]; unsigned int v; }  __attribute__((packed));
template <int OFF> struct RA_LONG  { char p[OFF]; long v; }          __attribute__((packed));
template <int OFF> struct RA_ULONG { char p[OFF]; unsigned long v; } __attribute__((packed));
template <int OFF> struct RA_S8    { char p[OFF]; signed char v; }   __attribute__((packed));
template <int OFF> struct RA_U8    { char p[OFF]; unsigned char v; } __attribute__((packed));
template <int OFF> struct RA_S16   { char p[OFF]; short v; }         __attribute__((packed));
template <int OFF> struct RA_U16   { char p[OFF]; unsigned short v; } __attribute__((packed));
#endif  // MONITOR_RAWACCESS_H_
