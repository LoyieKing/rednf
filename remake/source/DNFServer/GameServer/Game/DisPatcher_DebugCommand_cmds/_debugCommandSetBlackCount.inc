bool DisPatcher_DebugCommand::_debugCommandSetBlackCount(CUser* user,
                                                         MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    // §4.3 本 TU 私有 pack(1) 视图（零新符号、不动共享头）：ORIG @0x0859058a
    // 段为 `mov 0xe(%eax),%edx` 单段直落传 setCurrCharacBlackCount(unsigned int)
    // ⇒ 该处按 **unsigned int** 访问（P211 同型化；共享头 int 口径由
    // DisPatcherCore.cpp:13237/13252 的 `m_arg < 1`/`0 < m_arg` 有符号判定锁定，
    // 换头会把 jl/jle 翻成 jb/jbe，故在消费点侧建视图）。
    // pack(1) 必需：非 pack 时 m_argU 被自然对齐到 +0x10（实测读到 +0x10 而非
    // ORIG `mov 0xe(%eax),%edx` 的 +0x0e —— 真语义 bug）。
