// CDungeonGainedGold — 副本内金币累计（ORIG 0x850d4b6 / 0x84ec5d8 / 0x8697044..
// 权威参照 docs/class_func_reports/_lt_global_gt_.md 对应段）
// 布局：+0x00 unsigned int（ORIG 类尺寸 4；CUser.h 内嵌于 +0x8cf64）

class CDungeonGainedGold
{
public:
    unsigned int incGainedGold(unsigned int amount);
    unsigned int getGainedGold();
    void setGainedGold(unsigned int value);
    void reset();

    unsigned int m_gainedGold;  // +0x00
};

unsigned int CDungeonGainedGold::incGainedGold(unsigned int amount)
{
    // ORIG 语义：返回「跨越的 10 万分位差」（newGold/100000 - oldGold/100000），
    // 并累计 m_gainedGold。
    unsigned int inc = (m_gainedGold + amount) / 100000 - m_gainedGold / 100000;
    m_gainedGold = m_gainedGold + amount;
    if (inc != 0)
        return inc;
    return 0;
}

unsigned int CDungeonGainedGold::getGainedGold()
{
    return m_gainedGold;
}

void CDungeonGainedGold::setGainedGold(unsigned int value)
{
    m_gainedGold = value;
}

void CDungeonGainedGold::reset()
{
    m_gainedGold = 0;
}
