// stLimitItemUsageInfoEx 权威定义在 CDataManager.h（含 ctor/dtor 声明），
// 此处不再重复定义。
// ============================================================================
// _checkTimeoutItem（ORIG 0x867cf8d T，G3 任务域辅助）
// CTitleBook 通过 _checkTimeoutItem 直接调用：检查物品是否过期。
// ORIG 逻辑（逐步核对 0x867cf8d 反汇编）：
//   虚表 +0x10（IsAvatarItem，@4）非 0 → 0；
//   虚表 +0x48（PeriodAfterUnsealing，@18）非 0 且 item.m_seal != 0 → 1；
//   虚表 +0xc（GetItemType，@3）== 0xa 时：expirationDate <= now → 1；
//   否则（其他类型）：item.m_endurance * 86400 + 0x44a54a80 <= now → 1；
//   其余 → 0。
//   注：ORIG vtable（0x8c96da8）实证 @4=IsAvatarItem、@18=PeriodAfterUnsealing、
//   @19=IsRandomOption（NEW 原实现误用 IsCreatureItem/IsRandomOption，已纠正）。
// [b28d-solo-CDataManagerScripts] AE：①P52 链式 find_item（实参先物化 %ebx）；②P226 条件向语句表达式复现短路物化岛（`mov $1/$0 + test/je`，零槽）；③真语义 bug 修正：`PeriodAfterUnsealing() && m_field0!=0` 路径 ORIG 0x0867d03c 走 +0xb1 `mov $0,%eax` = 返回 **0**（原实现误写 return 1）；fresh clean 101=101；ORIG 0x0867cf8d。
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
    if (ci->PeriodAfterUnsealing() && item.m_seal != 0)
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
        int expire = item.m_endurance * 0x15180 + 0x44a54a80;
        if (expire <= now)
        {
            return 1;
        }
    }
    return 0;
}