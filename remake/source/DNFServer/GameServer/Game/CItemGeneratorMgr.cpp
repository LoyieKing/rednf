// ============================================================================
// CItemGeneratorMgr.cpp — WongWork::CItemGeneratorMgr::getGenerator 真实实现
//
// 提供缺失的 ORIG 符号：
//   _ZN8WongWork17CItemGeneratorMgr12getGeneratorENS0_15eGenerateType_tE
//   （WongWork::CItemGeneratorMgr::getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_t)）
//
// ORIG 反汇编 @ 0x0814a7d0：
//   mov 0xc(%ebp),%edx; mov 0x8(%ebp),%eax; add $0xf0,%edx; mov (%eax,%edx,4),%eax
//   => 字节偏移 (type+0xf0)*4 = 0x3c0 + type*4，即 m_generators[type]（基址 +0x3c0）。
//   注：ORIG 类位于 namespace WongWork；CDataManager.h/GameStubs.cpp 既存的【全局】
//   CItemGeneratorMgr 为不同类型。
//
// 不在此定义 CDataManager::getItemGenerator()：该符号 _ZN12CDataManager16getItemGeneratorEv
// 已由 GameStubs.cpp（CDataManager::getItemGenerator() 返回 m_itemGeneratorMgr）提供，避免重复定义。
// ============================================================================
#include "CItemGeneratorMgr.h"

namespace WongWork
{

CItemGenerator* CItemGeneratorMgr::getGenerator(eGenerateType_t type)
{
    int idx = static_cast<int>(type);
    if (idx < 0 || idx >= 8)
        return 0;
    return m_generators[idx];
}

}  // namespace WongWork