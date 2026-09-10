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
