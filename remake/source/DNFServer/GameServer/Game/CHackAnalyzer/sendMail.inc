void Secu_AccountHacking::sendMail(char const* name, MSG_CHECK_MAIL_RECEIVER* msg)
{
    // §4.3 裸偏移清零 —— 载荷具名视图（本 TU 私有，不动冻结头）：
    // MSG_CHECK_MAIL_RECEIVER（CGameManager.h:103）的 m_pad 是 set<TYPE>() 于
    // +4 memcpy 写入的字节数据区，对应消息体（MSG_MAILBOX_SEND 等）在本仓仅有
    // 前置声明、权威定义属冻结 PacketTypes.h ⇒ 按 ORIG T 0x08278b48
    // （docs/class_func_reports/Secu_AccountHacking/sendMail.md 汇编段）实证
    // 偏移建 pack(1) 视图：
    //   set<0> 分支：@+0x049 `mov 0x32(%eax),%eax`（金额）/
    //                @+0x081 `mov 0x3d(%eax),%eax`（avatar 物品号）
    //   set<2> 分支：@+0x0e3 `mov 0x33(%eax),%eax`（金额）/
    //                @+0x16f `movzbl 0x37(%eax),%eax`（条目数）/
    //                条目物品号 = base + i*0xb + 0x3b（ Ghidra C 段同形）
