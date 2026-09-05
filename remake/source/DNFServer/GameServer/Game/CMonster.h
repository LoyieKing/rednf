#ifndef DNF_GAME_MONSTER_H_
#define DNF_GAME_MONSTER_H_

#include <utility>
#include <vector>
#include "STMonsterScript.h"

class Inven_Item;  // 方法参数按引用传递（ORIG 0x0834f876）
namespace WongWork { struct stGenerateRefData_t; }

class CMonster
{
public:
    CMonster();
    ~CMonster();
    long deathTowerSpecifyItemDrop() const;
    int get_index();   // ORIG 仅非 const 版本（W 0x083748ce）
    void* getMonsterScript() const;
    void generateIndependentItem(const WongWork::stGenerateRefData_t& ref,
                                 std::vector<Inven_Item>& out) const;  // ORIG 0x0834f876
    // ORIG T 0x0834f8a6 _ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
    // 转发 ::generateDropAvatarItem(m_script.m_dropAvatarInfo(this+0xcf8), a, b, out)。
    void generateDropAvatarItem(unsigned char rarity, unsigned char grade,
                                std::vector<Inven_Item>& out) const;
    // ORIG T 0x0834f8e8 _ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE：
    // 转发 ::generateCatchMonsterItem(getMonsterScript()->m_catchItems(+0x70), out)。
    void generateCatchMonsterItem(std::vector<Inven_Item>& out) const;
    void set_monster(STMonsterScript& script);
    unsigned char IsHellMonster() const;   // ORIG W 0x08539df4（m_deathTowerFlag@+0x54）
    void drop_item_index(unsigned int index) const;
    void drop_item_index(unsigned int index, int randomValue) const;
    unsigned int get_specialItem_list_size(unsigned int index) const;
    // ORIG ConsistMap 0x086b8d97 以 `mov 0x1d24(%edx),%edx` 直读权重（成员偏移
    // 访问而非 getter），故 m_weight 公开；仅访问权限变化，布局/sizeof 不变。
    int m_weight;               // +0x1d24（ORIG new CMonster sizeof 0x1d28，无尾部成员）

private:
    char m_pad0[4];
    int m_index;
    std::vector<std::pair<int, int> > m_specialItemLists[4];
    int m_specialItemWeight;
    std::vector<std::pair<int, int> > m_dropItems;
    std::vector<std::pair<int, int> > m_deathTowerItems;
    bool m_deathTowerFlag;            // +0x54（ORIG set_monster 0x0834f650 movzbl 0x3a8 → %dl 直存）
    int m_independentDropCount;        // +0x58（ORIG set_monster ← script+0x3ec）
    int m_catchItemCount;              // +0x5c（ORIG set_monster ← script+0x3f0）
    STMonsterScript m_script;          // 真实布局（sizeof 0x1cc4，本批次重建）
    // 脚本区之后到权重表的保留字节：脚本区起点 0x60、m_weight 固定 0x1d24，
    // 保留量随真实 STMonsterScript 尺寸收缩（原两段 reserved 合并）。
    char m_scriptReserved[0x1d24 - 0x60 - sizeof(STMonsterScript)];
};

#endif
