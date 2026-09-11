bool CUser::CheckItemLock(int a, int b) const
{
    if (getCurCharacR() == 0)
        return 0;
    // ORIG 0x86469??：call getCurCharacInvenR 结果留 %eax（this 最后入栈），实参经 %edx
    // 中转——无 pInven 具名槽（直调链式形）。
    const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(a, b);
    if (item == 0)
        return 1;
    unsigned char lock =
        item->m_amp.GetLock();   // Inven_Item::m_amp @+0x11
    if (lock == 0)
        return 0;
    // ORIG 0x86469d4：test;je → mov $0 尾岛 / fall mov $1（正向 if；`!()` 反产 xor 物化）。
    if (((item_lock::CItemLock*)GetCharacExpandDataR(
              (ENUM_CHARAC_EXPAND_TYPE)2))
            ->CheckItemLock(lock))
        return 1;
    return 0;
}