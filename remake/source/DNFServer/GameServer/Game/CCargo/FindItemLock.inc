bool CCargo::FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space, int& slot) const
{
    space = (ENUM_ITEMSPACE)2;
    slot = 0;
    // ORIG 尾置 while（jmp cond 先行，cond 读 *param_3 后 setle/test），
    // 增量以 *param_3 = *param_3 + 1 两步写回（lea 0x1 形）。
    while (slot <= 0x97) {
        if (m_pItems[slot].m_amp.GetLock() == lock)
            return 1;
        slot = slot + 1;
    }
    return 0;
