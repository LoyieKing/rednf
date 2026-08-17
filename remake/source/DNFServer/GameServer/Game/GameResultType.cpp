// ============================================================================
// df_game_r GameResultType（G2-4 战报类型，0x50 字节）
// 逐函数对照 docs/class_func_reports/GameResultType.md 与 ORIG 反汇编实现；
// 内嵌 4 x DungeonUserShotCount（+0x14，0x20；ctor 084b766a / reset 084b767e）。
// ============================================================================

#include <cstring>

// ---- DungeonUserShotCount（8 字节：ushort + int，ctor 084b766a）----
class DungeonUserShotCount
{
public:
    DungeonUserShotCount();
    void reset();

    unsigned short m_count;  // +0x00
    int m_field4;            // +0x04
};

DungeonUserShotCount::DungeonUserShotCount()
{
    reset();
}

void DungeonUserShotCount::reset()
{
    m_count = 0;
    m_field4 = 0;
}

// ---- GameResultType（0x50） ----
class GameResultType
{
public:
    GameResultType();
    ~GameResultType();
    int Set(const GameResultType& other);
    void Clear();
    int UpdateHighScore(const GameResultType& other);
    int GetUniqueKey();

    unsigned char m_field0;   // +0x00
    int m_field4;             // +0x04
    unsigned char m_field8;   // +0x08
    unsigned char m_field9;   // +0x09
    unsigned char m_fielda;   // +0x0a
    int m_fieldc;             // +0x0c
    unsigned char m_field10;  // +0x10
    DungeonUserShotCount m_shot[4];  // +0x14（0x20）
    unsigned short m_field34; // +0x34
    unsigned short m_field36; // +0x36
    unsigned short m_field38; // +0x38
    unsigned short m_field3a; // +0x3a
    unsigned short m_field3c; // +0x3c
    unsigned short m_field3e; // +0x3e
    unsigned short m_field40; // +0x40
    unsigned short m_field42; // +0x42
    int m_field44;            // +0x44
    unsigned short m_field48; // +0x48
    unsigned short m_field4a; // +0x4a
    int m_field4c;            // +0x4c
};

GameResultType::GameResultType()
{
    Clear();
}

GameResultType::~GameResultType()
{
}

int GameResultType::Set(const GameResultType& other)
{
    *this = other;
    return 1;
}

void GameResultType::Clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field9 = 0;
    m_fielda = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field34 = 0;
    m_field36 = 0;
    m_field38 = 0;
    m_field3a = 0;
    m_field3c = 0;
    m_field3e = 0;
    m_field40 = 0;
    m_field42 = 0;
    m_field48 = 0;
    m_field44 = 0;
    m_field4a = 0;
    m_field4c = 0;
    memset(m_shot, 0, sizeof(m_shot));
}

int GameResultType::UpdateHighScore(const GameResultType& other)
{
    int flags = 0;
    if (other.m_field40 > m_field40)
    {
        m_field40 = other.m_field40;
        flags |= 1;
    }
    if (other.m_field42 > m_field42)
    {
        m_field42 = other.m_field42;
        flags |= 2;
    }
    if (other.m_field48 > m_field48)
    {
        m_field48 = other.m_field48;
        flags |= 8;
    }
    if ((unsigned)other.m_field44 < (unsigned)m_field44)
    {
        m_field44 = other.m_field44;
        flags |= 4;
    }
    return flags;
}

int GameResultType::GetUniqueKey()
{
    return m_field4 * 10 + m_field8;
}
