// ORIG 0x8543044：m_count = 0。
// [P338/㉓ 符号绑定] ORIG 该符号为 W（弱）⇒ 原码头内 inline 定义；显式 inline
// 复现弱绑定（原为 T 强符号，与 ORIG 绑定不符）。
inline CItemLockTimer::CItemLockTimer()
    : m_count(0)
{
}