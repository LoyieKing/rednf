void CBattle_Field::SetEnableHellDungeon(bool flag)
{
    bool* p = GetEnableHellDungeon();
    // p 槽取址使 4.4 store 前发纯装载 `mov -0xc(%ebp),%eax`（LHS 先，ORIG
    // 0x0822cf44 形）；无取址时 4.4 恒 RHS 值先（probe /tmp/l0p8.cpp SetH）。
    (void)&p;
    *p = flag;
