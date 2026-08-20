#include "GameRand.h"
// df_game_r Game/ CMonster 实现。
#include <utility>
#include <vector>

#include "STMonsterScript.h"
#include "CMonster.h"


int get_rand_int(int range);

int _drop_item_index(int randomValue,
                     const std::vector<std::pair<int, int> >& entries)
{
    int accumulated = 0;
    for (std::vector<std::pair<int, int> >::const_iterator it = entries.begin();
         it != entries.end(); ++it)
    {
        accumulated += it->second;
        if (randomValue < accumulated)
            return it->first;
    }
    return -2;
}

CMonster::CMonster()
    : m_index(0), m_specialItemWeight(0), m_deathTowerFlag(0),
      m_independentDropCount(0), m_catchItemCount(0), m_weight(0)
{
}

CMonster::~CMonster()
{
}

int CMonster::get_index()
{
    return m_index;
}

void CMonster::set_monster(STMonsterScript& script)
{
    m_index = script.m_field0;
    // 当前 STMonsterScript 只暴露基础字段，不能将 m_vec7c 伪装成掉落表。
    // 四组特殊表、权重、塔表及其它脚本字段待镜像补齐后按报告偏移复制。
    m_script = script;
}

unsigned char CMonster::IsHellMonster() const
{
    return m_deathTowerFlag;
}

unsigned int CMonster::get_specialItem_list_size(unsigned int index) const
{
    if (index >= 4)
        return 0;
    return static_cast<unsigned int>(m_specialItemLists[index].size());
}

long CMonster::deathTowerSpecifyItemDrop() const
{
    if (m_deathTowerItems.empty())
        return -2;
    return _drop_item_index(get_rand_int(10000), m_deathTowerItems);
}

void CMonster::drop_item_index(unsigned int index) const
{
    if (!m_dropItems.empty())
    {
        _drop_item_index(get_rand_int(m_specialItemWeight), m_dropItems);
        return;
    }
    if (index < 4)
        _drop_item_index(get_rand_int(10000), m_specialItemLists[index]);
}

void CMonster::drop_item_index(unsigned int index, int randomValue) const
{
    if (!m_dropItems.empty())
    {
        _drop_item_index(get_rand_int(m_specialItemWeight), m_dropItems);
        return;
    }
    if (index < 4)
        _drop_item_index(randomValue, m_specialItemLists[index]);
}

int CMonster::get_weight() const
{
    return m_weight;
}
