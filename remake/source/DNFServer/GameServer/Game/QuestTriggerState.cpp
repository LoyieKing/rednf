// df_game_r 还原 —— stQuestTriggerState_t 独立 TU（batch 迁移自 GameStubs.cpp）。
// 权威符号（ORIG df_game_r）：
//   stQuestTriggerState_t::stQuestTriggerState_t()             W @0x0836ddac
//   stQuestTriggerState_t::operator=(int const&)               W @0x086ad56c
//   stQuestTriggerState_t::incTirggerState(char)               W @0x086ad602
//   stQuestTriggerState_t::subTriggerState(char)               W @0x086ad6ea
//   stQuestTriggerState_t::getTrigger()                        W @0x0836dd68
//
// 语义：3 个 9 位触发值（mask 0x1ff）打包于 +0x00/+0x02/+0x04（对象 8 字节）。
//   operator= 由 int 解包三字段；getTrigger 打包三字段为 int；
//   incTirggerState 按 v 的位（0x10/0x20/0x40）选字段自增；
//   subTriggerState v==0 对字段0 裸自减，否则按位选字段自减（mod 512）。

class stQuestTriggerState_t
{
public:
    stQuestTriggerState_t();
    stQuestTriggerState_t& operator=(int const& v);
    void incTirggerState(char v);
    void subTriggerState(char v);
    int getTrigger();

    short m_trigger0;  // +0x00
    short m_trigger1;  // +0x02
    short m_trigger2;  // +0x04
};

stQuestTriggerState_t::stQuestTriggerState_t()
{
    m_trigger0 = 0;
    m_trigger1 = 0;
    m_trigger2 = 0;
}

stQuestTriggerState_t& stQuestTriggerState_t::operator=(int const& v)
{
    m_trigger0 = 0;
    m_trigger1 = 0;
    m_trigger2 = 0;
    m_trigger0 = (short)(v & 0x1ff);
    m_trigger1 = (short)((v >> 9) & 0x1ff);
    m_trigger2 = (short)((v >> 18) & 0x1ff);
    return *this;
}

void stQuestTriggerState_t::incTirggerState(char v)
{
    if (v & 0x10)
    {
        short cur = (short)((unsigned short)m_trigger0 & 0x1ff);
        if (cur <= 0x3fe)
            m_trigger0 = (short)((cur + 1) & 0x1ff);
    }
    if (v & 0x20)
    {
        short cur = (short)((unsigned short)m_trigger1 & 0x1ff);
        if (cur <= 0x3fe)
            m_trigger1 = (short)((cur + 1) & 0x1ff);
    }
    if (v & 0x40)
    {
        short cur = (short)((unsigned short)m_trigger2 & 0x1ff);
        if (cur <= 0x3fe)
            m_trigger2 = (short)((cur + 1) & 0x1ff);
    }
}

void stQuestTriggerState_t::subTriggerState(char v)
{
    if (v == 0)
    {
        // ORIG：`mov (%eax),%eax; lea -1(%eax),%edx; mov %edx,(%eax)` —— 对 +0x00 的
        // 4 字节值（m_trigger0/m_trigger1 合并）裸自减 1。
        int* lo = reinterpret_cast<int*>(&m_trigger0);
        *lo = *lo - 1;
        return;
    }
    if (v & 0x10)
    {
        short cur = (short)((unsigned short)m_trigger0 & 0x1ff);
        if (cur > 0)
            m_trigger0 = (short)((cur + 0x1ff) & 0x1ff);
    }
    if (v & 0x20)
    {
        short cur = (short)((unsigned short)m_trigger1 & 0x1ff);
        if (cur > 0)
            m_trigger1 = (short)((cur + 0x1ff) & 0x1ff);
    }
    if (v & 0x40)
    {
        short cur = (short)((unsigned short)m_trigger2 & 0x1ff);
        if (cur > 0)
            m_trigger2 = (short)((cur + 0x1ff) & 0x1ff);
    }
}

int stQuestTriggerState_t::getTrigger()
{
    int r = (int)((unsigned short)m_trigger0 & 0x1ff);
    r |= (((int)((unsigned short)m_trigger1 & 0x1ff)) << 9);
    r |= (((int)((unsigned short)m_trigger2 & 0x1ff)) << 18);
    return r;
}