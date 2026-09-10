// ============================================================================
// df_game_r 还原 —— Statistics（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/Statistics.md + ORIG 反汇编逐函数转录。
// 本类只做消息打包并转发给 SendSignal（ORIG 中 SendSignal 为空实现，
// 实际投递由后续层完成）。
// [b26-NL28] 8 个上报方法 ORIG 为 static（参数槽 a@0x8 起无 this，mangled 同形），
//   头已同步加 static（Main 批准）。RankResult 由此落 AE。
//   其余 7 件 NEAR 残差=帧 0xc + char 槽对齐差（4.1 紧密槽 vs 4.4 八对齐/出参区
//   复用策略，N82 族，源形不可消除），已封顶勿再耗轮次。
// ============================================================================

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN10StatisticsC2Ev`（ORIG 0x860ff82）。
#include <iostream>

#include <string.h>

#include "Statistics.h"
#include "MSG_STAT_RANKRES.h"   // 权威 MSG_STAT_RANKRES/GameResultType（唯一声明点）

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

// [b28c-4] ORIG 紧凑槽（Composition 0x0860b6a2：-0x15/-0x11/-0xd/-0xc，len 0xd）
// ⇒ 原码是 pack(1) 载荷结构体：4.1 对 pack(1) 紧凑排布，4.4 对独立标量按 4 对齐
// 分配槽（探针 /tmp/l4/pk.cpp C1 与 ORIG 逐条全等）。
struct StatCompositionData
{
    unsigned int a;
    unsigned int b;
    char type;
    int c;
} __attribute__((packed));

// DungeonClear/GiveUp 共用布局（ORIG 0x0860b81c/0x0860b868：-0x21/-0x1d/-0x19/
// -0x15/-0x11/-0xd/-0x9，len 0x19）——成员序 a,b,c,d,e,f,type，char 在末。
struct StatDungeonData
{
    unsigned int a;
    unsigned int b;
    int c;
    int d;
    int e;
    int f;
    char type;
} __attribute__((packed));

// ----------------------------------------------------------------------------
// Statistics（声明见 Statistics.h）
// ----------------------------------------------------------------------------
Statistics::Statistics() {}
Statistics::~Statistics() {}

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
    StatCompositionData data;

    data.a = a;
    data.b = b;
    data.type = 0x63;  // 'c'
    data.c = c;
    SendSignal(0x38, &data, 0xd);
}

void Statistics::Decomposition(unsigned int a, unsigned int b, int c)
{
    StatCompositionData data;

    data.a = a;
    data.b = b;
    data.type = 0x64;  // 'd'
    data.c = c;
    SendSignal(0x38, &data, 0xd);
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
    StatDungeonData data;

    data.a = a;
    data.b = b;
    data.c = c;
    data.d = d;
    data.type = 0x63;  // 'c'
    data.e = e;
    data.f = f;
    SendSignal(0x3c, &data, 0x19);
}

void Statistics::DungeonGiveUp(unsigned int a, unsigned int b, int c, int d, int e,
                               int f)
{
    StatDungeonData data;

    data.a = a;
    data.b = b;
    data.c = c;
    data.d = d;
    data.type = 0x67;  // 'g'
    data.e = e;
    data.f = f;
    SendSignal(0x3c, &data, 0x19);
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

// [b28c-4 P210] SendSignal 定义必须晚于全部 caller：4.4 对"已见定义"的 callee 按
// 其帧精确继承 outgoing（BreakJar 帧塌缩 0x1c、槽基 -0xc），ORIG 4.1 为保守帧
// 0x28/槽基 -0x14。定义后置即复现（探针 /tmp/l4/order.cpp T1 与 ORIG 0x0860b66c
// 逐条全等；同 TU 的 RankResult 等消费函数随之复查）。
void Statistics::SendSignal(int signal, void* data, int len) {}
