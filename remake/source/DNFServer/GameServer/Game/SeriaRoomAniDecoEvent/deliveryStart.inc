void OneADayItemShopEvent::deliveryStart(CDelivery* d)  // 0x081ba156
{
    // b28c-L19：ORIG 早返形（cmpl/je→尾 nop 岛 + body 尾 jmp 跳过），非正向 if 包裹。
    if (d == 0)
        return;
    d->attach(1, this);                                // type1: questClear
    d->attach(2, this);                                // type2: buyItem
}
