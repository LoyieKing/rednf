// CReliablePerson — 交易金币可信度统计（ORIG 0x86453b8..0x86454a8；
// 权威参照 docs/class_func_reports/CReliablePerson.md）
// 布局（+0x00..+0x0f）：
//   +0x00 total（累计交易金；GetTradeGoldTotal；>1e9 时溢出到 billion）
//   +0x04 billion（累计溢出的十亿位；GetTradeGoldTotalBillion）
//   +0x08 daily（日交易金；GetTradeGoldDaily / SetTradeGoldDaily）
//   +0x0c schoolPoint（CUserCharacInfo.h 侧声明）
// 原始工程中 3 个 Get* + SetTradeGoldDaily 为头内联（ORIG W），
// 此处统一落地为本 TU 定义（符号体逐字节一致）。

class CReliablePerson
{
public:
    void InitTradeGold(unsigned int daily, unsigned int billion,
                       unsigned int total);
    unsigned int GetTradeGoldDaily() const;
    unsigned int GetTradeGoldTotal() const;
    unsigned int GetTradeGoldTotalBillion() const;
    void SetTradeGoldDaily(unsigned int gold);
    void IncrementTradeGold(unsigned int gold);

    unsigned int m_totalTradeGold;   // +0x00
    unsigned int m_billionTradeGold; // +0x04
    unsigned int m_dailyTradeGold;   // +0x08
    int m_schoolPoint;              // +0x0c
};

void CReliablePerson::InitTradeGold(unsigned int total, unsigned int billion,
                                    unsigned int daily)
{
    m_totalTradeGold = total;
    m_billionTradeGold = billion;
    m_dailyTradeGold = daily;
}

unsigned int CReliablePerson::GetTradeGoldDaily() const
{
    return m_dailyTradeGold;
}

unsigned int CReliablePerson::GetTradeGoldTotal() const
{
    return m_totalTradeGold;
}

unsigned int CReliablePerson::GetTradeGoldTotalBillion() const
{
    return m_billionTradeGold;
}

void CReliablePerson::SetTradeGoldDaily(unsigned int gold)
{
    m_dailyTradeGold = gold;
}

void CReliablePerson::IncrementTradeGold(unsigned int gold)
{
    m_dailyTradeGold += gold;
    m_totalTradeGold += gold;
    if (m_totalTradeGold > 999999999)
    {
        unsigned int total = m_totalTradeGold;
        // 累计交易额超过 10 亿：十亿位进位到 billion，total 保留余数
        m_totalTradeGold = total - total / 1000000000 * 1000000000;
        m_billionTradeGold += total / 1000000000;
    }
}
