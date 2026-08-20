#ifndef DNF_GAME_BLUE_MARBLE_POD_H_
#define DNF_GAME_BLUE_MARBLE_POD_H_

class BuffInfo
{
public:
    BuffInfo();
    void reset();
    int m_exp;
    int m_gold;
    int m_drop;
};

class TileIndexInfo
{
public:
    TileIndexInfo();
    void reset();
    int m_dungeon;
    unsigned int m_item;
};

#endif
