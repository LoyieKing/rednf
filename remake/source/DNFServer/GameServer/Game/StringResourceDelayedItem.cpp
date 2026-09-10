// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StringResourceDelayedItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// StringResourceDelayedItem 实现。
// 证据：docs/class_func_reports/StringResourceDelayedItem/StringResourceDelayedItem.md
//   （仅覆盖 ctor @0x08ad52a2）；其余 7 函数报告缺失，按 nm 定址 +
//   objdump -d [0x08ad52d4, 0x08ad5439] 兜底转写（§3.2）。
// 布局：+0x00 m_pCache(char const*)、+0x04 m_cacheStr(CharString)、
//   +0x08 m_pRes(StringResource const*)、+0x0c m_id(int) → sizeof 0x10。
#include "StringResourceDelayedItem.h"

#include "StringResource.h"
#include "StringTypes.h"

//   m_pCache=0 在 CharString 构造之前 ⇒ 须走初始化列表（声明序先于子对象构造）。
#include "StringResourceDelayedItem/StringResourceDelayedItem.inc"
    : m_pCache(0)
{
    m_pRes = &res;
    m_id = id;
}

// ORIG D1/D2 @0x08ad52d4：仅 call CharStringD1Ev，无异常路径 ⇒ 空体。
#include "StringResourceDelayedItem/dtor_StringResourceDelayedItem.inc"
{
}
// ORIG @0x08ad5302：m_pRes->get(m_id, out)，返回 bool（test %al）。
#include "StringResourceDelayedItem/get.inc"
{
    return m_pRes->get(m_id, out);
}

// ORIG @0x08ad52f6：直接返回 +0x0c。
#include "StringResourceDelayedItem/getId.inc"
{
    return m_id;
}

// ORIG @0x08ad52ea：直接返回 +0x08。
#include "StringResourceDelayedItem/getStringResource.inc"
{
    return m_pRes;
}

// ORIG @0x08ad538a：get 命中 → m_cacheStr = *out；未命中 →
//   CharString::format("getStr(%d) == none", m_id)（0x8e32c4c）赋 m_cacheStr；
//   最后 m_pCache = m_cacheStr.c_str()。
// [P2-S4 2026-09-11 已翻] 原 raw-buffer + 引用 + 显式 dtor 形（68 条，DIFF gap=-10）里
//   `CharString& msg = *reinterpret_cast<CharString*>(msgRaw);` 的**引用槽**（-0xc）
//   使 4.4 在 get 前多发 `lea -0x18(%ebp),%eax; mov %eax,-0xc(%ebp); lea -0x14(%ebp),%eax`
//   且 else 路径多一次整体赋值搬运。ORIG 0x08ad538a 实测：out 单槽 -0x10，
//   0x08ad53d7 `call CharString::format` 的 sret 直指匿名临时 -0xc（无独立 C1）、
//   0x08ad53ef `aSER` 拷贝赋值、0x08ad5417 尾 D1 仅在 else 路径 —— 即源形就是
//   **直接 `m_cacheStr = CharString::format(...)`**。改回该形后 58/59，verdict=AE gap=-1，
//   TU diff 归零（D1Ev/_Unwind_Resume 亦随 format 内部 EH 正常发射）。
#include "StringResourceDelayedItem/makeCacheStr.inc"
{
    CharString const* out;
    if (get(&out)) {
        m_cacheStr = *out;
    } else {
        // ORIG 0x08ad53d7 `call format` 的 sret 目标 = 匿名临时 -0xc（format 内
        // placement C1 完成构造）→ 0x08ad53ef `aSER` 拷贝赋值 → 0x08ad5417 尾 D1
        // （仅 else 路径；hit 路径 0x08ad53bd 直落 c_str）。即源形就是**直接赋值
        // format 返回值**，4.4 同样以匿名临时承载返回值。
        m_cacheStr = CharString::format("getStr(%d) == none", m_id);
    }
    m_pCache = m_cacheStr.c_str();
}

// ORIG @0x08ad532a（const，Itanium sret：ret@0x8(%ebp)、this@0xc(%ebp)）：
//   m_pCache==0 时先 makeCacheStr（ORIG const 方法调非 const 成员，
//   汇编无多余指令 ⇒ const_cast 还原，无运行时开销）；返回 m_cacheStr 拷贝。
StringResourceDelayedItem::operator CharString() const
{
    if (m_pCache == 0)
        const_cast<StringResourceDelayedItem*>(this)->makeCacheStr();
    return m_cacheStr;
}

// ORIG @0x08ad5368：同惰性缓存判断，直接返回 m_pCache。
StringResourceDelayedItem::operator char const*() const
{
    if (m_pCache == 0)
        const_cast<StringResourceDelayedItem*>(this)->makeCacheStr();
    return m_pCache;
}
