bool CTitleBook::findItemLock(unsigned char lock, ENUM_ITEMSPACE& space,
                              int& slot) const
{
    // b28d-solo-CTitleBook（P2/P25）：ORIG 0x08641124 为双层标准 for 布局
    // （外层 init → jmp 条件 → 内层 init → jmp 条件；外层 `cmpl $3; setle`，
    // 内层 `mov TITLE_BOOK_MAX(,%eax,4); cmp i; setg`）。原 `for(;;)+continue`
    // 形在 4.4 多一条 nop 中继（ndiff 5 NEAR）。
    for (int category = 0; category <= 3; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            if (m_category[category][i].m_amp.GetLock() == lock)
            {
                space = (ENUM_ITEMSPACE)(category + 0x13);
                slot = i;
                return 1;
            }
        }
    }
    return 0;
