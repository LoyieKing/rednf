// ============================================================================
// df_game_r 还原 —— user_creature::CCreatureMgr（从 GameStubs.cpp 迁移，G2）
// 运行期布局与 ORIG 一致（sizeof 0x4c，m_pad 镜像 +0x00..+0x4b）。
// ORIG 符号（nm -C df_game_r）：
//   _ZN13user_creature12CCreatureMgr5ResetEv                 Reset()
//   _ZN13user_creature12CCreatureMgr7SetUserEP5CUser         SetUser(CUser*)
//   _ZN13user_creature12CCreatureMgr7UseItemEP10Inven_Itemi  UseItem(Inven_Item*, int)
//   _ZN13user_creature12CCreatureMgr11GainExpEi              GainExp(int)
//   _ZNK13user_creature12CCreatureMgr18IsEquippedCreatureEv  IsEquippedCreature() const
//   _ZNK13user_creature12CCreatureMgr32IsGrowCreature_Equipped_CreatureEv
//   _ZN13user_creature12CCreatureMgr18InsertCreatureItemEP10Inven_Itemiiii
// 各方法当前为语义占位（stub）；后续按
//   docs/class_func_reports/user_creature__CCreatureMgr.md 精修。符号 T。
// ============================================================================
#include "CInventory.h"

namespace user_creature
{

void CCreatureMgr::Reset()
{
    // TODO(G2): ORIG 0x83a45a —— 清空 equippedCreature / creatureItems / 依赖刷新。
}

void CCreatureMgr::SetUser(CUser* user)
{
    // TODO(G2): ORIG 0x000000 —— 记录 m_user。
    (void)user;
}

int CCreatureMgr::UseItem(Inven_Item* item, int slot)
{
    // TODO(G2): ORIG 0x83a5bc —— 使用/item 类型派发。
    (void)item;
    (void)slot;
    return -1;
}

bool CCreatureMgr::InsertCreatureItem(Inven_Item* item, int a, int b, int c, int d)
{
    // TODO(G2): ORIG 0x83b204 —— 插入宠物品，返回成功。
    (void)item;
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    return false;
}

void CCreatureMgr::GainExp(int exp)
{
    // TODO(G2): ORIG 0x839b06 —— 当前宠物加经验。
    (void)exp;
}

bool CCreatureMgr::IsEquippedCreature() const
{
    // TODO(G2): ORIG 0x000000 —— 是否有装备宠物。
    return false;
}

bool CCreatureMgr::IsGrowCreature_Equipped_Creature() const
{
    // TODO(G2): ORIG 0x83debc —— 装备的是否成长宠物。
    return false;
}

}  // namespace user_creature