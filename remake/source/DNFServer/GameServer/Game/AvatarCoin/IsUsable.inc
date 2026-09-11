// ORIG 0x0817ff44（IsUsable.md）：mov (%eax),%eax / test / setne %al / ret ——
// 直返 m_coin != 0，无 movzbl 规范化（P9/P22：char 返回只需 %al）。
bool AvatarCoin::IsUsable() const
{
    return m_coin != 0;
}