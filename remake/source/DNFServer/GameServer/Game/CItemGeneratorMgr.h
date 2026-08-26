// ============================================================================
// CItemGeneratorMgr.h — WongWork::CItemGeneratorMgr 权威声明（唯一声明点）
//
// ORIG 证据（df_game_r, elf32-i386）：
//   getGenerator(eGenerateType_t) @ 0x0814a7d0
//       symbol: _ZN8WongWork17CItemGeneratorMgr12getGeneratorENS0_15eGenerateType_tE
//       mov 0xc(%ebp),%edx; add $0xf0,%edx; mov (%eax,%edx,4),%eax
//       => 数组字节偏移 0x3c0 = (0xf0)*4，元素为 4 字节指针，索引 = type。
//   D1 @ 0x08374f0c：仅析构 +0x3e0 处的 CGeneratorCommonData（不 delete 生成器，
//       与 ORIG 一致；生成器由 initialize() @0x08533bc8 new 出并写入 +0x3c0..+0x3dc）。
//
// 布局：
//   +0x000 char[0x3c0]        头部区域（ORIG 其余成员未建模；initialize 尾部
//                             memset(this,0,0x3c0) 实证该区为可清零数据区）
//   +0x3c0 CItemGenerator* m_generators[8]
//   +0x3e0 CGeneratorCommonData m_common （ctor 0x08374ef4 仅构造它；D1 0x08374f0c
//                             仅析构它。ORIG operator new 尺寸 0xa28 =
//                             0x3e0 + sizeof(CGeneratorCommonData)=0x648 ✓）
//
// 注：ORIG 类位于 namespace WongWork。本头为权威声明；GameStubs.cpp /
// CDataManager.h 中既存的【全局】CItemGeneratorMgr 是不同类型（不同 N 空间）。
// ============================================================================
#ifndef GAME_CITEMGENERATORMGR_H_
#define GAME_CITEMGENERATORMGR_H_

#include "CItemGenerator.h"

namespace WongWork
{

// 前置声明（循环包含保护：CItemGenerator.h → PvPTypes.h → … →
// CDataManager.h → 本头时，CItemGenerator.h 因 include guard 尚未完成）。
class CItemGenerator;
class CItemGeneratorMgr
{
public:
    enum eGenerateType_t
    {
        eGenerateType_0 = 0,
        eGenerateType_1 = 1,
        eGenerateType_2 = 2,
        eGenerateType_3 = 3,
        eGenerateType_4 = 4,
        eGenerateType_5 = 5,
        eGenerateType_6 = 6,
        eGenerateType_7 = 7
    };

    // ORIG 0x0814a7d0：返回 m_generators[type]（this + 0x3c0 + type*4）
    CItemGenerator* getGenerator(eGenerateType_t type);

    // ORIG T 0x085342c6：++计数器[rarity + 30*genType + 6*dropType]（无符号溢出回绕，
    // 计数区位于对象头 0x3c0 字节前的统一统计数组）。
    void onTryGenerateRandomItem(eGenerateType_t genType, eItemDropType_t dropType,
                                 ENUM_RARITY rarity);

    CItemGeneratorMgr();             // ORIG _ZN8WongWork17CItemGeneratorMgrC1Ev W 0x08374ef4
    ~CItemGeneratorMgr();            // 定义于 CDataManagerScripts.cpp（仅析构 common）
    void setDropRatio(float ratio);  // 定义于 CDataManagerScripts.cpp（逐生成器虚调用）
    bool initialize();               // ORIG T 0x08533bc8（init 调用；定义 CtorProducers_2C.cpp）

    char m_head[0x3c0];              // +0x000（头部区域未建模）
    CItemGenerator* m_generators[8]; // +0x3c0
    CGeneratorCommonData m_common;   // +0x3e0（ctor 仅构造它；D1 仅析构它）
};

}  // namespace WongWork

// 布局自检：ORIG operator new 0xa28（CDataManager::init 835cd13 实证）。
typedef char assert_sizeof_itemgenmgr[sizeof(WongWork::CItemGeneratorMgr) == 0xa28 ? 1 : -1];

#endif  // GAME_CITEMGENERATORMGR_H_
