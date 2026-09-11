// ORIG 0x084cf546 T dispatch_sig —— 纯 ret 空函数。
// 报告 docs/class_func_reports/Inter_FatigueResetDBReply/dispatch_sig.md 全文转写（汇编 9 字节，mov $0x0,%eax; ret）。
int Inter_FatigueResetDBReply::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}