CAchievement::CAchievement() : m_user(0)
{
    // ORIG 0x828b633：m_user 置零在 map 构造之前（初始化列表，
    // 按声明顺序先于 m_achievements）；m_field24/25 在函数体
    m_field24 = 0;
    m_field25 = 0;
