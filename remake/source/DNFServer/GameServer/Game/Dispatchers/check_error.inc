// ---- Wave5 三分解方法（独立 const 成员，指令级转写自报告）----
int Dispatcher_Pause::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return 0;      // [objdump 0x08140b32] mov $0x0,%eax; pop %ebp; ret
}