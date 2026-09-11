// ============================================================================
// AvatarRoulette::isAvatarCoin —— ORIG T 0x08911da0（range 0x10）
// 汇编：mov 0x8(%ebp),%eax / mov (%eax),%eax（读 +0x00 m_avatarCoinInfo）/
//       cmp 0xc(%ebp),%eax / sete %al / pop / ret
// 即 bool 直返 m_avatarCoinInfo == avatarCoin（sete %al 后无 movzbl 规范化，
// bool 返回形态；Ghidra 反编译的 *(ulong*)this 为 8 字节读伪影，真实只读 4 字节）。
// ============================================================================
bool AvatarRoulette::isAvatarCoin(unsigned long avatarCoin)
{
    return m_avatarCoinInfo == avatarCoin;
}