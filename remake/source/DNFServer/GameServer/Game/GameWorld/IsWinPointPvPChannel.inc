bool GameWorld::IsWinPointPvPChannel() const
{
    // ORIG _ZNK9GameWorld20IsWinPointPvPChannelEv @0x085dfa92（同族，上界 0xf）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x8110)
            return true;
    }
    return false;
