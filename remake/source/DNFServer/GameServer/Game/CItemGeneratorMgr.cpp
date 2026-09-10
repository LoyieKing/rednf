// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemGeneratorMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CItemGeneratorMgr.cpp — WongWork::CItemGeneratorMgr::getGenerator 真实实现
//
// 提供缺失的 ORIG 符号：
//   _ZN8WongWork17CItemGeneratorMgr12getGeneratorENS0_15eGenerateType_tE
//   （WongWork::CItemGeneratorMgr::getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_t)）
//
// ORIG 反汇编 @ 0x0814a7d0：
//   mov 0xc(%ebp),%edx; mov 0x8(%ebp),%eax; add $0xf0,%edx; mov (%eax,%edx,4),%eax
//   => 字节偏移 (type+0xf0)*4 = 0x3c0 + type*4，即 m_generators[type]。
//
// 不在此定义 CDataManager::getItemGenerator()：该符号 _ZN12CDataManager16getItemGeneratorEv
// 已由 CDataManagerScripts.cpp 提供（返回 m_itemGeneratorMgr），避免重复定义。
// ============================================================================
#include "CItemGeneratorMgr.h"

namespace WongWork
{

#include "CItemGeneratorMgr/getGenerator.inc"
}

}  // namespace WongWork
