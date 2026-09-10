void CUser::_updateSaveOption()
{
    // ORIG 0x08652dde：Acquire 结果 %eax 直落 guard ctor（技巧36 删中转，
    // guard 槽 -0x18→-0x14 对齐）
    // [b28d-22 N104] 残差=EH 尾块相对序：ORIG 正常路径尾（dtor+epilogue）居函数最末、
    // unwind 块（0x08652ef0）居中；4.4 把 unwind 压最末、正常尾就地内联（块内容逐条
    // 全等，仅两尾块互换）。gap=1、槽位集相同（-0x14/-0xc）、P167 CLEAR ⇒ 封顶。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1d0d), true);
    guard.operator*()->operator<<(0x2b5);
    guard.operator*()->operator<<((int)GetUID());
    // SIG_CHARACTER_OPTION 协议视图：+0x00 characNo + 0x04 CharacterOption(0x200)。
    struct SIG_CHARACTER_OPTION_VIEW
    {
        // [b28d-solo-CUserOnlyOrig P211 档③] getCurCharacNo 返 unsigned int ⇒ 字段同型化
        // 复现 ORIG 0x08652dde LHS-first 直存（`mov -0xc(%ebp),%edx; mov %eax,(%edx)`；
        // int 字段异型产 RHS-first + `mov %eax,%edx` 中转）。
        unsigned int m_characNo;
        CharacterOption m_option;
    };
    // ORIG 0x08652dde：GetInBuffer<SIG_CHARACTER_OPTION>（0x204 占位协议视图）
    SIG_CHARACTER_OPTION_VIEW* body =
        reinterpret_cast<SIG_CHARACTER_OPTION_VIEW*>(
            guard.operator->()->GetInBuffer<SIG_CHARACTER_OPTION>());
    memset(body, 0, 0x204);
    body->m_characNo = getCurCharacNo();
    GetCurCharacOption(body->m_option);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    setSaveCharacOption(false);
    // [G6Sub3 2026-09-10 复核] 80/81、帧 0x20、槽位集(-0x14/-0xc)全等；除上方 N104 的 EH
    // 尾块相对序（4.4 把 unwind 块中置 ⇒ 正常路径多发 1 条 `jmp`，= 全部 gap）外，其余残差
    // **全部**是数据地址差：`_ZN10GlobalData13s_stream_poolE` @0x940bd6c、
    // `_ZN10GlobalData10s_msgq_mgrE` @0x940bd68、"user.cpp" 字符串（.o 段内显示为
    // `mov 0x0,%eax` / `$0x253`）⇒ N132。无其它真实结构差异（指令/助记符序列逐条同构）。
