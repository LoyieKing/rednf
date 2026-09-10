void cUserHistoryLog::BillingFail(ENUM_BILLING_TYPE type,
                                  unsigned long long tranNo, int amount,
                                  int code, char* reason)
{
    // b28d-50：ORIG 0x08685838 的 NumberToString 结果直留 %eax 落 0xc(%esp)（无
    // str 槽）；具名局部多一槽且使 ull spill 由 -0x10/-0xc 漂到 -0x20/-0x1c、
    // 帧 0x34→0x48、丢 push ebx。内联实参即复现（P0 表达式直传）。
    m_user->LogHistory("BILLING, Fail,%d,%s,%d,%d,%s",
                       (int)type, NumberToString(tranNo, 0), amount, code, reason);
