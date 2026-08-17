// ============================================================================
// df_game_r 还原 —— Statistics（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/Statistics.md + ORIG 反汇编逐函数转录。
// 本类只做消息打包并转发给 SendSignal（ORIG 中 SendSignal 为空实现，
// 实际投递由后续层完成）。
// ============================================================================

#include <string.h>

// ---- 外部符号声明（对应 TU 翻译后移除） ----
class StatisticsShusiaService
{
public:
    void incDungeonFailure(int value);
    void incServiceUsage(int value);
};

class StatisticsPvP
{
public:
    void incPvPRoomResult(int a, int b, int c);
};

class GameWorld
{
public:
    StatisticsShusiaService* getStatisticsShusiaService();
    StatisticsPvP* getStatisticsPvP();
};

GameWorld* G_GameWorld();

// MSG_STAT_RANKRES：+0x00..+0x18 七个整型 + +0x1c GameResultType(0x50)，
// 总尺寸 0x6c。ctor/dtor 属其它 TU（ORIG 0x8610798/0x86107ae）。
class GameResultType
{
public:
    GameResultType();
    ~GameResultType();

    char m_data[0x50];  // +0x00
};

class MSG_STAT_RANKRES
{
public:
    MSG_STAT_RANKRES();
    ~MSG_STAT_RANKRES();

    unsigned int m_a;      // +0x00
    unsigned int m_b;      // +0x04
    int m_c;               // +0x08
    int m_d;               // +0x0c
    int m_e;               // +0x10
    int m_f;               // +0x14
    int m_g;               // +0x18
    GameResultType m_result;  // +0x1c
};

// ----------------------------------------------------------------------------
// Statistics
// ----------------------------------------------------------------------------
class Statistics
{
public:
    Statistics();
    ~Statistics();

    static void SendSignal(int signal, void* data, int len);

    void BreakJar(unsigned int a, unsigned int b, int c);
    void Composition(unsigned int a, unsigned int b, int c);
    void Decomposition(unsigned int a, unsigned int b, int c);
    void UseCoin(unsigned int a, unsigned int b, int c);
    void PvpPlay(unsigned int a, unsigned int b, int c, int d);
    void RankResult(unsigned int a, unsigned int b, int c, int d, int e,
                    int f, int g, GameResultType& result);
    void DungeonClear(unsigned int a, unsigned int b, int c, int d, int e, int f);
    void DungeonGiveUp(unsigned int a, unsigned int b, int c, int d, int e, int f);
    void DungeonFail(int dungeonIdx);
    void UseShusiaService(int value);
    void PVPPlay2(int a, int b, int c);
};

Statistics::Statistics() {}
Statistics::~Statistics() {}

void Statistics::SendSignal(int signal, void* data, int len) {}

void Statistics::BreakJar(unsigned int a, unsigned int b, int c)
{
    unsigned int localA;
    unsigned int localB;
    int localC;

    localA = a;
    localB = b;
    localC = c;
    SendSignal(0x37, &localA, 0xc);
}

void Statistics::Composition(unsigned int a, unsigned int b, int c)
{
    unsigned int localA;
    unsigned int localB;
    char localType;
    int localC;

    localA = a;
    localB = b;
    localType = 0x63;  // 'c'
    localC = c;
    SendSignal(0x38, &localA, 0xd);
}

void Statistics::Decomposition(unsigned int a, unsigned int b, int c)
{
    unsigned int localA;
    unsigned int localB;
    char localType;
    int localC;

    localA = a;
    localB = b;
    localType = 0x64;  // 'd'
    localC = c;
    SendSignal(0x38, &localA, 0xd);
}

void Statistics::UseCoin(unsigned int a, unsigned int b, int c)
{
    unsigned int localA;
    unsigned int localB;
    int localC;

    localA = a;
    localB = b;
    localC = c;
    SendSignal(0x39, &localA, 0xc);
}

void Statistics::PvpPlay(unsigned int a, unsigned int b, int c, int d)
{
    unsigned int localA;
    unsigned int localB;
    int localC;
    int localD;

    localA = a;
    localB = b;
    localC = c;
    localD = d;
    SendSignal(0x3a, &localA, 0x10);
}

void Statistics::RankResult(unsigned int a, unsigned int b, int c, int d, int e,
                            int f, int g, GameResultType& result)
{
    MSG_STAT_RANKRES msg;

    msg.m_a = a;
    msg.m_b = b;
    msg.m_c = c;
    msg.m_d = d;
    msg.m_e = e;
    msg.m_f = g;   // param_7 -> +0x14
    msg.m_g = f;   // param_6 -> +0x18
    msg.m_result = result;
    SendSignal(0x3b, &msg, 0x6c);
}

void Statistics::DungeonClear(unsigned int a, unsigned int b, int c, int d, int e,
                              int f)
{
    unsigned int localA;
    unsigned int localB;
    int localC;
    int localD;
    int localE;
    int localF;
    char localType;

    localA = a;
    localB = b;
    localC = c;
    localD = d;
    localType = 0x63;  // 'c'
    localE = e;
    localF = f;
    SendSignal(0x3c, &localA, 0x19);
}

void Statistics::DungeonGiveUp(unsigned int a, unsigned int b, int c, int d, int e,
                               int f)
{
    unsigned int localA;
    unsigned int localB;
    int localC;
    int localD;
    int localE;
    int localF;
    char localType;

    localA = a;
    localB = b;
    localC = c;
    localD = d;
    localType = 0x67;  // 'g'
    localE = e;
    localF = f;
    SendSignal(0x3c, &localA, 0x19);
}

void Statistics::DungeonFail(int dungeonIdx)
{
    G_GameWorld()->getStatisticsShusiaService()->incDungeonFailure(dungeonIdx);
}

void Statistics::UseShusiaService(int value)
{
    G_GameWorld()->getStatisticsShusiaService()->incServiceUsage(value);
}

void Statistics::PVPPlay2(int a, int b, int c)
{
    G_GameWorld()->getStatisticsPvP()->incPvPRoomResult(a, b, c);
}
