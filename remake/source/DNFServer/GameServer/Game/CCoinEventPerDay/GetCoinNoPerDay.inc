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