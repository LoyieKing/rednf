CTraceMobDieHack::CTraceMobDieHack()
{
    // m_map1/m_map2（map）与 m_mobKillers/m_hackLogs（vector）由编译器
    // 按成员声明序隐式构造，随后执行本函数体（ORIG 086c32e0 同序）。
    reset();
