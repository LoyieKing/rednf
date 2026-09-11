// [N82 封顶] batch-13：三处 `start <= i` 翻 `i >= start` 复现 ORIG setge 向（已消除
// replace 块）；残余 gap=5 = ORIG 跨 MoveItemToEmptySlot 调用以 %esi/%ebx 持两下标
// （push/pop esi,ebx prologue）vs 4.4.7 caller-save 直用形，register-asm 强改有
// qsort 段串扰风险，判 N82 不强求。
int CInventory::SortInvenSlot(Inven_Item::ITEM_TYPE type)
{
    // b28d-solo-CInventory [P212/P332]：ORIG 0x08504a92..0x08504ac7 表明
    // start/end/i 于 StartEndPos 前清零、last 于其后赋值；三个外层循环复用同一 i
    // （4.4 块作用域各自占槽，4.1 复用）；内层两循环各用 j/k。
    // ORIG 0x08504bc4 在 cMyTrace ctor 前把 end/start 载入 esi/ebx（P227 临时链）。
    // ⇒ AE（169/169 norm；TU diff 31→30、ae 260→261）。
    int start = 0;
    int end = 0;
    int i = 0;
    int last;
    StartEndPos(type, start, end);
    last = 0;
    for (i = end; i >= start; i--)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            last = i;
            break;
        }
    }
    if (last == start || last == 0)
    {
        return 0x12;
    }
    bool full = true;
    for (i = start; i <= last; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            full = false;
            break;
        }
    }
    if (full)
    {
        return 0x12;
    }
    int dst = -1;
    for (i = end; i >= start; i--)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            int moveResult = MoveItemToEmptySlotOfInventory(i, start, end);
            if (moveResult != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x194d, 5)(
                    "MoveItemToEmptySlotOfInventory return %d ( index %d,s %d,e %d )",
                    moveResult, i, start, end);
                return moveResult;
            }
            int newLast = 0;
            for (int j = end; j >= start; j--)
            {
                if (m_pEquipSlot[j].m_addInfo != 0)
                {
                    newLast = j;
                    break;
                }
            }
            bool newFull = true;
            for (int k = start; k <= newLast; k++)
            {
                if (m_pEquipSlot[k].m_addInfo == 0)
                {
                    newFull = false;
                    break;
                }
            }
            if (newFull)
            {
                break;
            }
        }
    }
    return 0;
}