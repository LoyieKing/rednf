#ifndef DNF_GAME_RESULT_TYPE_H_
#define DNF_GAME_RESULT_TYPE_H_

struct DungeonUserShotCount
{
    DungeonUserShotCount();
    void reset();
    unsigned short m_count;
    int m_field4;
};

class GameResultType
{
public:
    GameResultType();
    ~GameResultType();
    int Set(const GameResultType& other);
    void Clear();
    int UpdateHighScore(const GameResultType& other);
    int GetUniqueKey();

    unsigned char m_field0;
    int m_field4;
    unsigned char m_field8;
    unsigned char m_field9;
    unsigned char m_fielda;
    int m_fieldc;
    unsigned char m_field10;
    DungeonUserShotCount m_shot[4];
    unsigned short m_field34;
    unsigned short m_field36;
    unsigned short m_field38;
    unsigned short m_field3a;
    unsigned short m_field3c;
    unsigned short m_field3e;
    unsigned short m_field40;
    unsigned short m_field42;
    int m_field44;
    unsigned short m_field48;
    unsigned short m_field4a;
    int m_field4c;
};

#endif
