// df_game_r 每日币事件 CCoinEventPerDay 实现。
// 报告：docs/class_func_reports/CCoinEventPerDay/（7 函数全读）。
#include "CCoinEventPerDay.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0810ab10：基类构造 + vptr 置位 + 4 个档位字节（+0x09..+0x0c）置 1。
CCoinEventPerDay::CCoinEventPerDay()
{
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
}

// ORIG 0x0810ab48 / 0x0810ab78：D1 重置 vptr 后调 CEventBase::~CEventBase（末尾
// __in_chrg 常量折叠不 delete）；D0 调 D1 后 operator delete。空体 + 虚析构由
// 编译器生成同形。
CCoinEventPerDay::~CCoinEventPerDay()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0810ab96：SetEventFlag(true) + 4 个档位字节置 1 +
// cMyTrace(__FUNCTION__,0x38,0) ("Coin Event Per Day Start!")。
void CCoinEventPerDay::StartEvent()
{
    SetEventFlag(true);
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
    cMyTrace tr(__FUNCTION__, 0x38, 0);
    tr("Coin Event Per Day Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0810ac04：SetEventFlag(true) + 整 4 字节写 param 到 +0x09 +
// cMyTrace(__FUNCTION__,0x41,0) 输出 4 个档位字节
// ("Coin Event Per Day Start! coin(%d) coin(%d) coin(%d) coin(%d) ")。
// b28d-76（N76 封顶，语义等价）：ORIG `lea 0xc(%ebp),%edx; mov 0x8(%ebp),%eax;
// mov (%edx),%edx; mov %edx,0x9(%eax)` = 4.1 求值序「RHS 地址 → LHS 地址 → RHS 值」；
// 4.4-O0 对 `m_coinPack = *(unsigned int*)&param;` 恒 RHS 完整求值先
// （`lea ...,%eax; mov (%eax),%edx; mov this,%eax`）。20 形探针（volatile 读/写、
// 视图/引用解引用、字段 int↔unsigned 同异型化、数组下标、语句表达式、具名中转）
// 只能产「this 先」或「RHS 值先」两族，无一复现 ORIG 前两条顺序 ⇒ N76。
void CCoinEventPerDay::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_coinPack = *(const unsigned int*)&param;
    cMyTrace(__FUNCTION__, 0x41, 0)(
        "Coin Event Per Day Start! coin(%d) coin(%d) coin(%d) coin(%d) ",
        m_coin01, m_coin02, m_coin03, m_coin04);
}

// ---------------------------------------------------------------------------
// ORIG 0x0810aca8：SetEventFlag(false) + 4 个档位字节置 1 +
// cMyTrace(__FUNCTION__,0x63,0) ("Coin Event Per Day End")。
void CCoinEventPerDay::EndEvent()
{
    SetEventFlag(false);
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
    cMyTrace tr(__FUNCTION__, 0x63, 0);
    tr("Coin Event Per Day End");
}

// ---------------------------------------------------------------------------
// ORIG 0x0810ad16：level < 0x12 → +0x09；< 0x1b → +0x0a；否则 +0x0b（movzbl 字节读）。
int CCoinEventPerDay::GetCoinNoPerDay(int level)
{
    if (level < 0x12)
        return m_coin01;
    if (level < 0x1b)
        return m_coin02;
    return m_coin03;
}
