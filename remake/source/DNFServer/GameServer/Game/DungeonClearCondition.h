#ifndef DNF_GAME_DUNGEON_CLEAR_CONDITION_H_
#define DNF_GAME_DUNGEON_CLEAR_CONDITION_H_

struct DungeonClearCondition
{
    int m_0;
    int m_4;
    int m_8;
    DungeonClearCondition();
    // [b28d-Main 代落·S273 P317] 删用户空析构（ORIG D1/D2 W size=5，同族）。
    void clear();
};

#endif
