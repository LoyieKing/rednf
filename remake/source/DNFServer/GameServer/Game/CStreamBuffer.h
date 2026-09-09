#ifndef GAME_CSTREAMBUFFER_H_
#define GAME_CSTREAMBUFFER_H_

// ============================================================================
// df_game_r 还原 —— CStreamBuffer 权威声明（ORIG 0x086269be 域）
// ORIG 符号（nm -S --defined-only df_game_r）：
//   _ZN13CStreamBufferC1Ev / C2Ev            0x086269be (0x23) ctor：memset(this,0,0xa000)
//   _ZN13CStreamBuffer5c_strEv               0x086269e2 (0x08) 返回 m_buf（非 const，mangled 无 K）
//   _ZN13CStreamBuffer12AppendStringEPciS0_  0x086269ea (0x5f) 非静态成员(char*,int,char*)
//        —— this@+0x8 未用，dst@+0xc/maxlen@+0x10/src@+0x14；strncat 容量截断追加
//   _ZN13CStreamBufferlsEPKc                 0x08626a4a (0x25) operator<<(const char*) "%s"
//   _ZN13CStreamBufferlsEi                   0x08626a70 (0x25) operator<<(int) "%d"
//   _ZN13CStreamBufferlsEj                   0x08626a96 (0x25) operator<<(unsigned int) "%u"
//   _ZN13CStreamBufferlsEf                   0x08626abc (0x25) operator<<(float) "%f"
//   _ZN13CStreamBuffer6InsertIPKcEEvPcT_     0x08627cbc (0x8b) 非静态成员模板 Insert<const char*>
//   _ZN13CStreamBuffer6InsertIiEEvPcT_       0x08627d48       Insert<int>
//   _ZN13CStreamBuffer6InsertIjEEvPcT_       0x08627dd4       Insert<unsigned int>
//   _ZN13CStreamBuffer6InsertIfEEvPcT_       0x08627e60       Insert<float>
// Insert 为成员模板，显式参数 2 个：(char* fmt, T val)（mangled 无 this；
// 实参槽 = this, fmt, val；ls 调用 Insert<T>("%s", v)）。
// 布局：单 char m_buf[0xa000]（对象尺寸 0xa000，无 vptr；this == &m_buf[0]）。
// 证据：docs/class_func_reports 无 CStreamBuffer 目录（报告缺失），按指南 §3.2
// 以 nm 定位 + objdump 逐指令转写；rodata 0x8ce2de0 实测 "%s\0%d\0%u\0%f\0"。
// ============================================================================

#include <string.h>

namespace OS_API   // CInventory.h 同款最小声明（snprintf wrapper，mangled _ZN6OS_API8snprintfEPcjPKcz）
{
int snprintf(char* buf, unsigned int n, const char* fmt, ...);
}

class CStreamBuffer
{
public:
    CStreamBuffer();                              // 0x086269be
    CStreamBuffer& operator<<(const char* v);     // 0x08626a4a "%s"
    CStreamBuffer& operator<<(int v);             // 0x08626a70 "%d"
    CStreamBuffer& operator<<(unsigned int v);    // 0x08626a96 "%u"
    CStreamBuffer& operator<<(float v);           // 0x08626abc "%f"
    char* c_str();                                // 0x086269e2

    void AppendString(char* dst, int maxlen, char* src);  // 0x086269ea

    // ORIG：局部 0xa000 缓冲 snprintf 格式化后 AppendString 截断追加到 m_buf。
    // 函数体含 ORIG 同款 strlen(m_buf)/strlen(tmp) 调用（snprintf 后，结果丢弃）。
    //
    // [b22判词·N] 四个实例（IPKc/If/Ii/Ij = ORIG 0x08627cbc / 0x08627e60 /
    //   0x08627d48 / 0x08627dd4，均 size 0x8b、33 insns）残余 7 insn，两处
    //   gcc 4.4.7 -O0 源码层不可复现：
    //   ① memset 参数块（差 1 insn）：ORIG 为
    //      `lea tmp,%eax / mov $0xa000,%edx / mov %edx,0x8(%esp) / movl $0x0,0x4(%esp)
    //       / mov %eax,(%esp)` —— 立即数经 %edx 物化、第 1 参地址提前算入 %eax 保活；
    //      我方恒出 `movl $0xa000,0x8(%esp) / movl $0x0,0x4(%esp) / lea / mov`。
    //      实测 9 种写法（sizeof / 枚举常量 / 宏十进制 / (unsigned long) 转换 /
    //      0xa000UL / 具名 const ul / 局部 ul 变量 / &t[0] / 成员数组）逐条同形，
    //      无一命中 ORIG 形 ⇒ 属编译器立即数物化策略差。
    //   ② 两条结果丢弃的 strlen（差 6 insn，含 2 call）：ORIG 在 snprintf 之后
    //      保留 `strlen(m_buf)` 与 `strlen(tmp)`（返回值立即被覆盖，无 store、
    //      无比较、无跳转 ⇒ 原码就是裸表达式语句）。glibc <string.h> 的 strlen
    //      带 __attribute__((pure))，gcc 4.4.7 的 tree-DCE 即使在 -O0 下也会删除
    //      "无副作用 + 结果未用" 的调用；实测：保留语句→被删，用 volatile/条件
    //      包裹→额外多生 store/cmp 反而更偏离。（②已由 b28d-solo 轮用纯 register
    //      形复现，见下段；本段②判词作废。）
    //   语义等价性已实测闭合：memset→OS_API::snprintf(fmt,val)→AppendString
    //   [b28c-11 复验] N 封顶维持：ORIG 4.1 的 memset 立即数经 %edx 物化 + 两条
    //   `strlen(m_buf)/strlen(tmp)` 死调用（0x08627cbc 起）；4.4 -O0 探针
    //   /tmp/l11p/dce.cpp 实证裸 `strlen(p);` 被 tree-DCE 删除、volatile 接收
    //   反多 2 条 store/load/处（4 处共 +4），不可翻。
    //   (m_buf,0xa000,tmp) 三调用与 ORIG 一一对应；帧尺寸（IPKc/Ii/Ij = 0xa018、
    //   If = 0xa028）、参数槽位（fmt@0xc/val@0x10、tmp@-0xa008）、float 的
    //   `flds 0x10(%ebp)/fstpl 0xc(%esp)` 传参形、AppendString 调用目标全部一致；
    //   AppendString 本体为 AE。⇒ N 族（指令选择/DCE 层），不可复现，语义等价。
    //   [b28d-N] fresh 复验（四实例 33/26 同形）维持 N：①memset 立即数物化
    //   9 形探针全败；②死 strlen 被 4.4 tree-DCE（/tmp/l11p/dce.cpp 实证），
    //   volatile 反劣 ⇒ 封顶。ORIG 0x08627cbc/0x08627e60/0x08627d48/0x08627dd4。
    //   [b28d-solo-CStreamBuffer] 四实例判词（fresh routed 口径，
    //   check_tu_routed.sh: diff=0 / near=0；逐件 AE，ORIG 33 insns = 我方 33
    //   insns 逐条对齐）：
    //     _ZN13CStreamBuffer6InsertIPKcEEvPcT_  0x08627cbc  AE
    //     _ZN13CStreamBuffer6InsertIfEEvPcT_    0x08627e60  AE
    //     _ZN13CStreamBuffer6InsertIiEEvPcT_    0x08627d48  AE
    //     _ZN13CStreamBuffer6InsertIjEEvPcT_    0x08627dd4  AE
    //   修法（本轮翻正）：原码是**数组零初始化** `char tmp[0xa000] = {0};`，
    //   不是独立 `memset(tmp, 0, sizeof(tmp));`。4.4.7 -O0 由初始化列表发射
    //   memset，实参装载恰为 ORIG 形：
    //     lea -0xa008(%ebp),%eax; mov $0xa000,%edx; mov %edx,0x8(%esp);
    //     movl $0x0,0x4(%esp); mov %eax,(%esp); call memset
    //   （首参地址先算入 %eax 保活 + size 经 %edx 物化）；独立 memset 语句恒产
    //   `movl $0xa000,0x8(%esp); movl $0x0,0x4(%esp); lea; mov`（少 1 条）。
    //   探针矩阵 /tmp/b28d_csb2/probe{,2}.cpp 共 15 形，仅 gI（`= {0}`）命中；
    //   A-H/J-O（sizeof/&tmp/&tmp[0]/(char*)/(void*)/(char*)(void*)/0xa000/'\0'/
    //   语句表达式/具名 char* const/__builtin_memset）全败。
    //   ⇒ 旧 b22/b28c-11/b28d-N「memset 参数块 N 封顶」判词**作废**
    //   （P214③/P202① 扩展：大数组 `= {0}` 亦复现 memset 实参装载形）。
    //   ② 两条结果丢弃 strlen（ORIG 0x08627d08..0x08627d1c）：裸 `strlen(p);`
    //   被 4.4 -O0 tree-DCE 删除（N99），用纯 register 局部承接（P231 合规：
    //   禁的是 register asm 绑定，裸 register 存储类提示合法）→ 保留两条调用、
    //   不占槽、不产 store，与 ORIG 逐条同形（帧 0xa018/-0xa008 不变）。
    //   语义闭合：零初始化→snprintf→strlen×2→AppendString；帧 IPKc/Ii/Ij=0xa018、
    //   If=0xa028、参数槽 fmt@0xc/val@0x10、tmp@-0xa008、float 的
    //   flds 0x10(%ebp)/fstpl 0xc(%esp) 全一致；AppendString 本体 AE。
    //   无子子代理。
    template <typename T>
    void Insert(char* fmt, T val);                          // 0x08627cbc..e60

    char m_buf[0xa000];   // +0x00
};

template <typename T>
void CStreamBuffer::Insert(char* fmt, T val)
{
    char tmp[0xa000] = {0};
    OS_API::snprintf(tmp, 0xa000, fmt, val);
    // 以下两条 = ORIG 保留的死代码（结果丢弃，ORIG 0x08627d08..0x08627d1c 段）。
    // 裸 `strlen(p);` 会被 gcc 4.4.7 -O0 tree-DCE 删除（N99）；用纯 register 局部
    // 承接（P231：非 asm 绑定，合法）即可保留两条调用且不占栈槽——实测与 ORIG
    // 逐条同形（帧 0xa018/-0xa008 不变，无 store）。
    register size_t deadLenA = strlen(m_buf);
    register size_t deadLenB = strlen(tmp);
    (void)deadLenA;
    (void)deadLenB;
    AppendString(m_buf, 0xa000, tmp);
}

#endif  // GAME_CSTREAMBUFFER_H_
