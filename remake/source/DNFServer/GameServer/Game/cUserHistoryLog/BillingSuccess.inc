// ============================================================================
// cUserHistoryLog::BillingSuccess @ 0x086858a0
//   BillingSuccess(ENUM_BILLING_TYPE, unsigned long long, int)
//   type==3 走 "BILLING, Success,%d,%s"；否则 "BILLING, Success,%d,%d,%d"
//  （ORIG 第二分支对 NumberToString 结果用 %d 打印——忠实转写）。
// [B24D判词 DONE_AE] 语义正确：两分支各独立 NumberToString(tranNo,0)（P48）+LogHistory
//   "BILLING, Success,%d,%s"（type==3）/ "%d,%d,%d"（type,str,amount）。已修第一分支
//   字面量 3→(int)type（ORIG 读参数槽 0xc(%ebp)），45/45 指令对齐；仅剩 call/字符串
//   地址 AE 占位差异。
// ============================================================================
void cUserHistoryLog::BillingSuccess(ENUM_BILLING_TYPE type,
                                     unsigned long long tranNo, int amount)
{
    // ORIG 0x086858a0：两分支各独立调 NumberToString（P48 去跨臂缓存，
    // `push %ebx`（第二臂 %ebx=amount）正由此双调用引入）。
    if (type == 3)
        m_user->LogHistory("BILLING, Success,%d,%s", (int)type,
                           NumberToString(tranNo, 0));
    else
        m_user->LogHistory("BILLING, Success,%d,%d,%d", (int)type,
                           NumberToString(tranNo, 0), amount);
}