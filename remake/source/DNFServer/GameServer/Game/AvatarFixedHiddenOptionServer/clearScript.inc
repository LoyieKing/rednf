void ARAD::SCRIPT::AvatarFixedHiddenOption::clearScript()
{
    // [b28d-56 判词·AE] ORIG 0x088b00e6 循环前先清零两具名 int 槽（-0x14/-0x10，
    // P0 槽往返 ⇒ 原码有具名局部），ctor 实参从槽读（0x8(%esp)=value@-0x10、
    // 0xc(%esp)=rate@-0x14）；循环上界为 `i<=5`（ORIG 尾 setle，P2）。
    int rate = 0;
    int value = 0;
    for (int i = 0; i <= 5; ++i)
    {
        m_abilities[i].clear();
        m_abilities[i].push_back(
            STAvatarHiddenAbilityVariation(AVATAR_HIDDEN_ABILITY_TYPE::NONE, value, rate));
    }
