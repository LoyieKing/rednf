// ORIG 0x084d008a T _ZN31Inter_MailBox_Req_CheckReceiver15SendErrorPacketEP5CUser14ENUM_CMDPACKETt
// 报告 docs/class_func_reports/Inter_MailBox_Req_CheckReceiver/SendErrorPacket.md：
// 记日志后按命令 0x43 分流 2 字节/1 字节错误包。
void Inter_MailBox_Req_CheckReceiver::SendErrorPacket(
    CUser* user, ENUM_CMDPACKET cmd, unsigned short err)
{
    // ORIG 0x084d008a：err 先 2B spill（+0x09 mov %ax,-0xc）供两支 reload，再
    // movzwl -0xc → %ebx 跨 logFormat 缓存（+0x0f，push ebx/sub 0x34）；
    // get_charac_no 结果免槽直用（技巧37）。
    register unsigned int regCode = err;
    LogManager::logFormat(
        1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x2e12,
        "[ITEM_GIFT] Fail Command[%d] charac_no[%u] error_code[%d]",
        cmd,
        user->get_charac_no(-1),
        regCode);
    if (cmd == 0x43)
        user->SendCmdErrorPacket2Byte(cmd, err);
    else
        user->SendCmdErrorPacket(cmd, static_cast<unsigned char>(err));
}