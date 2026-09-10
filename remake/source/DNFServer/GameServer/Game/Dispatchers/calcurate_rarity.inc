int Dispatcher_MonstercardBind::calcurate_rarity(ENUM_RARITY rarity,
                                                 unsigned short rate)
{
    // ORIG 0x081d842e：range 唯一槽 -0xc；randInt 结果免槽留 %eax，rate 从
    // -0x1c 回读 %edx，`cmp %edx,%eax; setb %al; test; je` 物化形（技巧22）。
    unsigned long range = 10000;
    // P0：ORIG randInt 结果免槽（%eax 直比）→ 源不得用具名局部承接。
    if (m_rand->randInt(range) < rate)
    {
        switch (rarity)
        {
        case RARITY_COMMON:
            return 1;
        case RARITY_UNCOMMON:
            return 2;
        case RARITY_RARE:
        case RARITY_UNIQUE:
            return 3;
        default:
            return 0;
        }
    }
    return rarity;
