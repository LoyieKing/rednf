// ORIG fatigueMatchGift C1 0x0896251c：成员默认构造后 +0x18=0、map/string clear。
useCharacterJobFatigue::fatigueMatchGift::fatigueMatchGift() throw()
    : m_field18(0)   // 声明先于 m_msg1(+1c)/m_msg2(+20) string 自动构造，故置 init-list(ORIG 序)
{
    m_rewardMap.clear();
    m_msg1.clear();
    m_msg2.clear();
}