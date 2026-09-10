void CUserCharacInfo::calcHelpAbuseRatioBeforeLevelUp(int param)
{
    char unused[0x20];
    (void)unused;
    (void)param;
    // [b28d-solo-CUserCharacInfo 复核] ORIG 该空体函数仍发射局部静态
    // _ZZN15CUserCharacInfo31calcHelpAbuseRatioBeforeLevelUpEiE19__PRETTY_FUNCTION__
    // （r 段 0x3b，全仓仅此一件 ORIG-only 数据符号；ORIG 体 0x7 条无引用）
    // ⇒ 原码有对该串的死引用，照抄（P171 死写入族；不产生指令）。
    (void)__PRETTY_FUNCTION__;
