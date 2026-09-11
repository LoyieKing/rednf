void Secu_GoldControl::resetRangeMoney(user_gold_info& cur, user_gold_info& copy)
{
    // ORIG 0x08287bdc 逐条：memcpy(copy, cur, 0x3c)；6 组 64 位
    // if (cur > 0x00000000fffffffe) { copy = -1; cur -= 0xffffffff; }
    // else { cur = 0; }（ORIG add $1/adc $-1 = 64 位减 0xffffffff，手动展开）
    memcpy(&copy, &cur, 0x3c);
    if (cur.m_gold > 0x00000000fffffffeULL)
    {
        copy.m_gold = 0x00000000ffffffffULL;
        cur.m_gold -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_gold = 0;
    }
    if (cur.m_import > 0x00000000fffffffeULL)
    {
        copy.m_import = 0x00000000ffffffffULL;
        cur.m_import -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_import = 0;
    }
    if (cur.m_sub > 0x00000000fffffffeULL)
    {
        copy.m_sub = 0x00000000ffffffffULL;
        cur.m_sub -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_sub = 0;
    }
    if (cur.m_importSub > 0x00000000fffffffeULL)
    {
        copy.m_importSub = 0x00000000ffffffffULL;
        cur.m_importSub -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_importSub = 0;
    }
    if (cur.m_auctionAdd > 0x00000000fffffffeULL)
    {
        copy.m_auctionAdd = 0x00000000ffffffffULL;
        cur.m_auctionAdd -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_auctionAdd = 0;
    }
    if (cur.m_auctionSub > 0x00000000fffffffeULL)
    {
        copy.m_auctionSub = 0x00000000ffffffffULL;
        cur.m_auctionSub -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_auctionSub = 0;
    }
}