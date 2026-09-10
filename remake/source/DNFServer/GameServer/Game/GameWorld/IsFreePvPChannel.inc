bool GameWorld::IsFreePvPChannel() const
{
    // ORIG _ZNK9GameWorld16IsFreePvPChannelEv @0x081a0ab2（同 0x081424e8 族布局）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x166a0)
            return true;
    }
    return false;
