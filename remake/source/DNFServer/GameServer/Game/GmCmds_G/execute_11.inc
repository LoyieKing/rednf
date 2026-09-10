void CUpgradeCardDebug::execute()
{
    CUser* user = GetUser();
    if (user == 0)
        return;

    // P0/P183：ORIG 0x084b1bba `mov 0x8(%ebp),%eax; mov 0x8(%eax),%ebx` 把 m_slot
    // 装 callee-saved %ebx 直传 find_item（无具名槽）⇒ 内联表达式，不设具名局部。
    CItem* item = G_CDataManager()->find_item(
        reinterpret_cast<const IntSlotView*>(this)->m_slot);
    if (item == 0)
        return;

    // ORIG：call is_stackable 后 xor $0x1/test 判 _Bool 假值即出函数。
    if (item->is_stackable() == false)
        return;

    // ORIG 入口对三块栈缓冲整块清零（0x400/0x1e/0x400 内联展开 +
    // 95000 大缓冲 memset call）；除 sprintf 目标外均为死存储。
    // ORIG 物理顺序 0x084b1bf4(0x400) → 0x084b1c3e(memset) → 0x084b1c5d(0x1e)
    // → 0x084b1caf(0x400) ⇒ dead400 必须声明在 dead1e 之后。
    char msg[1024] = { 0 };
    unsigned char deadLarge[95000];
    // P68：ORIG 0x084b1c3e 先 `lea` 落 %eax、size 经 %edx 中转再压栈
    // （`memset((char*)p,0,N)` 形）；裸 memset(p,0,N) 直接 movl 立即数。
    memset((char*)deadLarge, 0, sizeof(deadLarge));
    unsigned char dead1e[30] = { 0 };
    unsigned short dead400[512] = { 0 };

    int rate = 0;                                   // -0x34
    switch (item->get_index())
    {
    case 0x1d29:
    case 0x1c69:
    case 0x1c6a:
    case 0x1c6b:
    case 0x1e3f:
    case 0x1e40:
    case 0x1e86:
        rate = 100;
        break;
    default:
        break;
    }

    if (static_cast<CStackableItem*>(item)->IsReinforceTicket())
    {
        signed char value =
            (signed char)static_cast<CStackableItem*>(item)->GetReinforceRate();
        if (value < 0 || value > 100)
            rate = 0;
        else
            rate = (int)value;
    }
    else if (static_cast<CStackableItem*>(item)->IsAmplifyReinforceTicket())
    {
        signed char value =
            (signed char)static_cast<CStackableItem*>(item)->GetAmplifyReinforceRate();
        if (value < 0 || value > 100)
            rate = 0;
        else
            rate = (int)value;
    }

    int success = 0;                                // -0x30
    int failure = 0;                                // -0x2c
    for (int i = 0; i <= 9999; ++i)                 // -0x1c
    {
        if (get_rand_int(100) < rate)
            ++success;
        else
            ++failure;
    }

    sprintf(msg,
            "아이템:%s(%d) 성공확률:%d  (%d번 성공, %d번 실패)\n",
            item->GetItemName(), item->get_index(), rate, success, failure);
    SendChatMsg(GetUser(), msg);
