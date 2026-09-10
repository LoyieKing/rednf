bool _checkTimeoutItem(const Inven_Item& item)   // b28d-Main(P292/P248档①)：ORIG 0x086412ba 调用点 `call; test %al,%al; jne` ⇒ bool
{
    // ORIG 0x0867cf8f：实参 item.m_addInfo 先物化 %ebx 再 call G_CDataManager()
    // ⇒ 单条链式表达式（P52），勿拆具名 dm 局部（会生 -0x18 槽 + 换序）。
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    // [b28d-solo-CDataManagerScripts] P226 条件向：ORIG 0x0867cfb4/0x0867cfd4/0x0867cfdb
    // 短路链物化为 `mov $1/%eax … mov $0/%eax; test %al; je`（无槽）⇒ 语句表达式
    // 承载整条条件（内不含具名变量）。
    if (({ ci == 0 || (ci->getUsablePeriod() == 0 && ci->getExpirationDate() == 0); }))
    {
        return 0;
    }
    if (ci->IsAvatarItem())
    {
        return 0;
    }
    // [b28d-solo-CDataManagerScripts] 真语义 bug 修正：ORIG 0x0867d03c `test %al; je +0xb8`
    // ⇒ 该条件为真时走 +0xb1 `mov $0,%eax; jmp 尾` = **return 0**（原实现写 return 1）。
    if (ci->PeriodAfterUnsealing() && item.m_field0 != 0)
    {
        return 0;
    }
    int now = GlobalData::s_systemTime_.getCurSec();
    if (ci->GetItemType() == 0xa)
    {
        if (ci->getExpirationDate() <= now)
        {
            return 1;
        }
    }
    else
    {
        int expire = item.m_fieldb * 0x15180 + 0x44a54a80;
        if (expire <= now)
        {
            return 1;
        }
    }
    return 0;
