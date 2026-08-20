#ifndef GAME_CWORLDMAP_H_
#define GAME_CWORLDMAP_H_
#define GAME_CWORLDMAP_DEFINED

class CWorldMap
{
public:
    int get_index() const { return m_index; }
    bool IsInHellDungeon() const;
    int m_index;
};

#endif
