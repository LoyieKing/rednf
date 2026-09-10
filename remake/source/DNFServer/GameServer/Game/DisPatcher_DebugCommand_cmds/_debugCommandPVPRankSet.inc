bool DisPatcher_DebugCommand::_debugCommandPVPRankSet(CUser* user,
                                                      MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    // §4.3 具名视图（本 TU 私有，零新符号）：ORIG @0x0858faba 三处均
    // `movzbl 0xe(%eax)` ⇒ +0x0e 按 **字节** 访问（int m_arg 形产 4 字节 mov，
    // 实测异形）。共享头 MSG_DEBUG_COMMAND 的 int 口径由其它命令维持，
    // 此处仅按 ORIG 访存宽度建 pack(1) 字节视图，不动共享布局。
