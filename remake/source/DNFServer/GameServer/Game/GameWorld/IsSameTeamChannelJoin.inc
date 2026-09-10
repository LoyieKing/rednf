bool GameWorld::IsSameTeamChannelJoin()
{
    // ORIG _ZN9GameWorld21IsSameTeamChannelJoinEv @0x082343c8（同族，非 const）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one = 1u << ct;
        if (one & 0xc200)
            return true;
    }
    return false;
