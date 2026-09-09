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

    // ORIG _ZN8WongWork17CItemGeneratorMgrC1Ev/C2Ev **W** 0x08374ef4 size 0x18
    // （nm 实证弱绑定 ⇒ 源码为类内定义；我方原 .cpp 定义为 T）。类内定义仅对
    // +0x3e0 的内嵌 CGeneratorCommonData 调空体 ctor（ORIG 0x08374ebc
    // push/mov/pop/ret），由编译器按成员初始化自动完成；sizeof/偏移/mangled/
    // vtable 零变化（static_assert 0xa28 不变）。
    // b28d-Main(P216)：throw() 消 CDataManager::init 的 new landing pad。
    CItemGeneratorMgr() throw() {}
    ~CItemGeneratorMgr();            // 定义于 CDataManagerScripts.cpp（仅析构 common）
    void setDropRatio(float ratio);  // 定义于 CDataManagerScripts.cpp（逐生成器虚调用）
    // ORIG T 0x08534322 _ZN8WongWork17CItemGeneratorMgr12onTimerEventEi
    //   timer==0x6b 时：以 G_CEnvironment()+0x1b0 为首参对三组统计区
    //   (dropStat[g][1..4]，g=0/1/2；即 +0x18/+0x30/+0x48/+0x60、
    //    +0x90/+0xa8/+0xc0/+0xd8、+0x108/+0x120/+0x138/+0x150) 各调一次
    //   DB_UpdateItemDropStatistics::makeRequest(int,int,uint*,uint*,uint*,uint*)，
    //   然后 memset(this,0,0x3c0)（计数区整体清零）。定义 W6_ItemGenDeps.cpp。
    void onTimerEvent(int timer);
    bool initialize();               // ORIG T 0x08533bc8（init 调用；定义 CtorProducers_2C.cpp）

    // 掉落生成尝试计数区（ORIG 证据，2026-09-05 BossMission 批次字段化）：
    //   · onTryGenerateRandomItem T 0x085342c6 索引式
    //     (rarity + 30*genType + 6*dropType)*4 ⇒ 行主序 [genType][dropType][rarity]，
    //     genType 步长 30 int = 0x78、dropType 步长 6 int = 0x18；
    //   · onTimerEvent T 0x08534322 对 genType 0/1/2 各取 dropType 1..4 四行
    //     （this+0x18/0x30/0x48/0x60、+0x90/0xa8/0xc0/0xd8、+0x108/0x120/0x138/0x150）
    //     送 makeRequest，每行 6 uint（makeRequest 逐指针写 6 个 uint）；
    //   · eGenerateType_t 共 8 值 ⇒ 8*5*6*4 = 0x3c0，恰等于原 m_head 区尺寸，
    //     与 initialize/ctor 的 memset(…,0,0x3c0) 整体清零口径一致。
    // union 保留旧基名 m_head（CtorProducers_2C.cpp `memset(m_head,0,0x3c0)`
    // 消费者零破坏；dropType 第 5 行与 rarity 第 6 列为 ORIG 计数表预留槽）。
    union
    {
        char m_head[0x3c0];                    // +0x000（旧基名，整体清零口径）
        unsigned int m_dropStat[8][5][6];      // +0x000 [genType][dropType][rarity]
    };
    CItemGenerator* m_generators[8]; // +0x3c0
    CGeneratorCommonData m_common;   // +0x3e0（ctor 仅构造它；D1 仅析构它）
};

}  // namespace WongWork

// 布局自检：ORIG operator new 0xa28（CDataManager::init 835cd13 实证）。
typedef char assert_sizeof_itemgenmgr[sizeof(WongWork::CItemGeneratorMgr) == 0xa28 ? 1 : -1];

#endif  // GAME_CITEMGENERATORMGR_H_
