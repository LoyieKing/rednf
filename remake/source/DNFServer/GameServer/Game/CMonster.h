#ifndef DNF_GAME_MONSTER_H_
#define DNF_GAME_MONSTER_H_

#include <utility>
#include <vector>
#include "STMonsterScript.h"

class CMonster
{
public:
    CMonster();
    ~CMonster();
    long deathTowerSpecifyItemDrop() const;
    int get_index();
    int get_index() const;
    static void* getMonsterScript(CMonster* self);
    void* getMonsterScript() const;
    void set_monster(STMonsterScript& script);
    unsigned char IsHellMonster() const;
    int get_weight() const;
    void drop_item_index(unsigned int index) const;
    void drop_item_index(unsigned int index, int randomValue) const;
    unsigned int get_specialItem_list_size(unsigned int index) const;

private:
    char m_pad0[4];
    int m_index;
    std::vector<std::pair<int, int> > m_specialItemLists[4];
    int m_specialItemWeight;
    std::vector<std::pair<int, int> > m_dropItems;
    std::vector<std::pair<int, int> > m_deathTowerItems;
    unsigned char m_deathTowerFlag;
    int m_independentDropCount;
    int m_catchItemCount;
    STMonsterScript m_script;
    char m_scriptReserved[0xc98 - sizeof(STMonsterScript)];
    char m_reservedTail[0x1d24 - 0xd28];
    int m_weight;               // +0x1d24
    char m_tailEnd[4];
};

#endif
