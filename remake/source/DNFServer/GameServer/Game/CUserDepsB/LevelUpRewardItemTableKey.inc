LevelUpRewardItemTableKey::LevelUpRewardItemTableKey(int level, int job,
                                                     int grow)
{
    m_lv = static_cast<short>(level);   // b28c-L20: m_lv signed short（ORIG 16 位有符号比较）
    m_job = static_cast<unsigned char>(job);
    m_grow = static_cast<unsigned char>(grow);
