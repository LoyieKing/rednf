// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StringResourceDelayedItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

// ORIG D1/D2 @0x08ad52d4：仅 call CharStringD1Ev，无异常路径 ⇒ 空体。
#include "StringResourceDelayedItem/dtor_StringResourceDelayedItem.inc"
}
// ORIG @0x08ad5302：m_pRes->get(m_id, out)，返回 bool（test %al）。
#include "StringResourceDelayedItem/get.inc"
}

// ORIG @0x08ad52f6：直接返回 +0x0c。
#include "StringResourceDelayedItem/getId.inc"
}

// ORIG @0x08ad52ea：直接返回 +0x08。
#include "StringResourceDelayedItem/getStringResource.inc"
}

// ORIG @0x08ad538a：get 命中 → m_cacheStr = *out；未命中 →
//   CharString::format("getStr(%d) == none", m_id)（0x8e32c4c）赋 m_cacheStr；
//   最后 m_pCache = m_cacheStr.c_str()。
// [b28d-24] DIFF 残25: P167 MISSING CharString D1Ev + _Unwind_Resume×1——4.4 拒绝 4.1 非平凡 union 成员抑制默认构造形；raw-buffer 引用形下显式 dtor 被折叠。ORIG 0x08ad538a
#include "StringResourceDelayedItem/makeCacheStr.inc"
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
