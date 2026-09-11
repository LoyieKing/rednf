// ORIG T 0x08911e4e：前 5 个 dword 自 other 单向拷贝（+0x00..+0x10），
// 随后 i=0..10 对 m_avatars[i]（步长 0xc @+0x14）调 vector::swap（双向），
// 恒返回 1。stiker 数组不参与。
int AvatarRoulette::swap(AvatarRoulette* other)
{
    m_avatarCoinInfo = other->m_avatarCoinInfo;
    m_rareCloneAvatarRate = other->m_rareCloneAvatarRate;
    m_rareAvatarRate = other->m_rareAvatarRate;
    m_upperAvatarRate = other->m_upperAvatarRate;
    m_normalAvatarRate = other->m_normalAvatarRate;
    for (int i = 0; i <= 10; ++i)
    {
        m_avatars[i].swap(other->m_avatars[i]);
    }
    return 1;
}