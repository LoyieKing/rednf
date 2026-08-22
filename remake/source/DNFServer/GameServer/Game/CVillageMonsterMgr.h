// df_game_r 还原 —— village_attacked::CVillageMonsterMgr
// 独立头文件（从 GameStubs.cpp 拆出）。只声明本 batch 负责的三个方法；
// OnKillVillageMonster 由 VillageMonster_helpers.cpp 并发 TU 提供（不入本批）。
// 布局存根：方法当前为空实现，不触碰成员，m_pad 仅占位保 ABI。

#ifndef VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H
#define VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H

#include <vector>

class CUser;

// ORIG 反汇编关联：STAttackedMonster 0x20 字节。
// 注意：必须在全局作用域声明（ORIG mangled InsertVillageMonster 使用非限定的
// 17STAttackedMonsterE / 7MapAreaE），不能放进 village_attacked 命名空间，
// 否则产生的符号为 _ZN...NS_17STAttackedMonsterE... 与 ORIG 不符。
struct STAttackedMonster
{
    char m_pad[0x20];
};

// ORIG 反汇编关联：MapArea 0x10 字节
struct MapArea
{
    char m_pad[0x10];
};

namespace village_attacked
{

class CVillageMonsterMgr
{
public:
    void OnMoveArea(CUser* user);                    // _ZN16village_attacked18CVillageMonsterMgr10OnMoveAreaEP5CUser (0x86b45bc)
    void OnEvent();                                   // _ZN16village_attacked18CVillageMonsterMgr7OnEventEv (0x82f0f44)
    void InsertVillageMonster(int a, int b, int c, int d,
                              std::vector<STAttackedMonster>* monsters,
                              std::vector<MapArea>* areas);  // _ZN16village_attacked18CVillageMonsterMgr20InsertVillageMonsterEiiiiPSt6vectorI17STAttackedMonsterSaIS2_EEPS1_I7MapAreaSaIS6_EE (0x86b4144)

private:
    char m_pad[0x10];
};

}  // namespace village_attacked

#endif  // VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H