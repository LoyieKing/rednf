// ORIG T 0x0867cd20 sz0x18d：guildkey!=0 时组 header 0x63 包广播队列 2。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x0867cd20 | _ZN5CUser21_UpdateUserInfo4GuildEv
void CUser::_UpdateUserInfo4Guild()
{
    if (get_charac_guildkey() == 0)
        return;

    // b28d-lane5：ORIG 0x0867cd4x 无 Stream* 槽——Acquire 结果经 %eax 直传
    // CStreamGuard ctor 实参（P0 右值内联；具名 s 反产 -0xc 槽 + 回读）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x793f), true);
    // ORIG：每处写经 guard.operator*()->operator<<(…) 显式代理链（operator* 调用点
    // 补齐；SetCharacInfo/SaveInventory 判例）。
    guard.operator*()->operator<<((int)0x63);                       // header
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((int)get_charac_guildkey());
    guard.operator*()->operator<<((unsigned int)get_acc_id());
    guard.operator*()->operator<<((int)getCurCharacNo());
    // b28d-lane5：ORIG 0x0867ce2x `movsbl %al,%ebx` ⇒ 调用点按 signed char 扩展
    // （P11；getter 本体 uchar 直返不变，仅本消费点收窄）。
    guard.operator*()->operator<<((int)(signed char)getCurCharacGrowType());
    guard.operator*()->operator<<((int)get_charac_level());
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}