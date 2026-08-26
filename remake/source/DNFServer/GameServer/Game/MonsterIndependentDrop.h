#ifndef GAME_MONSTER_INDEPENDENT_DROP_H_
#define GAME_MONSTER_INDEPENDENT_DROP_H_

// ============================================================================
// MonsterIndependentDrop.h — 独立掉落生成域（df_game_r 忠实转写）
//
// 覆盖符号（全部 T，地址经 nm/objdump 核实）：
//   ::generateIndependentItem(vector<stMonsterIndependentDrop_t> const&,
//                             WongWork::stGenerateRefData_t const&,
//                             vector<Inven_Item>&)          0x0834989a
//   ::independentItemRateControl(stMonsterIndependentDrop_t&,
//                             WongWork::stGenerateRefData_t const&) 0x0834972f
//   CAICharacter::generateIndependentItem(...) const        0x0834a13a
//   CMonster::generateIndependentItem(...) const            0x0834f876
//
// stMonsterIndependentDrop_t 权威布局见 CDataManager.h（sizeof 0x48）。
// WongWork::stGenerateRefData_t 权威定义见 CItemGenerator.h（勿在此重复）。
// ============================================================================

#include <utility>
#include <vector>

#include "CDataManager.h"    // stMonsterIndependentDrop_t / stIndependentDropRateControl
#include "CItemGenerator.h"  // WongWork::stGenerateRefData_t
#include "GameRand.h"        // get_rand_int（deathTowerSpecifyItemDrop 内联用）
#include "Inven_Item.h"

void generateIndependentItem(
    const std::vector<stMonsterIndependentDrop_t>& drops,
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out);                                   // ORIG 0x0834989a

void independentItemRateControl(
    stMonsterIndependentDrop_t& drop,
    const WongWork::stGenerateRefData_t& ref);                       // ORIG 0x0834972f

// ---- CAICharacter（权威镜像类）----
// ORIG 布局依据 set 0x0834a066 / ctor 0x08373a9a / 各访问器与
// generateIndependentItem 0x0834a13a（容器 lea 0x24(%eax) 实证）：
//   +0x00 int idx；+0x04 int level；+0x08 int field8；
//   +0x0c int appearancePoint；+0x10 vector<pair<int,int>> dropItems；
//   +0x1c/+0x20 PvP 胜点上下界；+0x24 vector<stMonsterIndependentDrop_t>。
// 与 CDeathTower.cpp 的本地定义保持同布局；本头文件是唯一包含点，
// 方法实现见 MonsterIndependentDrop.cpp。后续成员（+0x30 头像掉落、
// +0x6c 名字、+0x70 rarity）本 TU 未用，不建模。
#ifndef DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED
#define DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED
class CAICharacter
{
public:
    // ORIG W 访问器（同 CDeathTower.cpp 本地镜像）：
    int getIdx() const { return m_idx; }               // 0x08150850：读 +0x00
    int getLevel() const { return m_level; }           // 0x0815085a：读 +0x04
    int getAppearancePoint() const { return m_appearancePoint; }  // 0x08150866：读 +0x0c

    // ORIG 0x0834a19c _ZNK12CAICharacter25deathTowerSpecifyItemDropERSt6vectorImSaImEE：
    // 遍历 m_dropItems，pair.second > get_rand_int(10000) 时收 pair.first。
    void deathTowerSpecifyItemDrop(std::vector<unsigned long>& dropIds) const
    {
        // ORIG 0x834a19c：begin/end 迭代器均在初始化区求值（缓存 end）
        std::vector<std::pair<int, int> >::const_iterator it = m_dropItems.begin();
        std::vector<std::pair<int, int> >::const_iterator endIter = m_dropItems.end();
        for (; it != endIter; ++it)
        {
            int r = get_rand_int(10000);
            if (it->second > r)
                dropIds.push_back((unsigned long)it->first);
        }
    }

    void generateIndependentItem(const WongWork::stGenerateRefData_t& ref,
                                 std::vector<Inven_Item>& out) const;  // ORIG 0x0834a13a
private:
    int m_idx;                                       // +0x00
    int m_level;                                     // +0x04
    int m_field8;                                    // +0x08
    int m_appearancePoint;                           // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;   // +0x10
    int m_pvpWinLow;                                 // +0x1c
    int m_pvpWinHigh;                                // +0x20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops;  // +0x24
};
#endif  // DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED

#endif  // GAME_MONSTER_INDEPENDENT_DROP_H_

// 布局自检（ORIG __copy_m 元素步长 0x48 实证）。
typedef char assert_sizeof_monster_independent_drop[sizeof(stMonsterIndependentDrop_t) == 0x48 ? 1 : -1];
typedef char assert_sizeof_independent_drop_rate_control[sizeof(stIndependentDropRateControl) == 0xc ? 1 : -1];
