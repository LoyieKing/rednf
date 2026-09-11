// ORIG 0x081c1182 T _ZN23DisPatcher_DeleteCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0xa41,pretty,err,acc_id)。
// RemoveCharacMercenaryInfo → StreamPool 取流构造 SIG_DELETE_CHARAC（memset 0x14）→
// characNo==0 则 m_errNo=0x15 并 Free 回流 → 填公会 id → lockDeleteCharac 成功投递
// [b28d-29] N82/P211：帧 0x5c vs 0x50 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c1182 insn 216->210)
// DB_Q 并打 cMyTrace（accid 十进制串 / m_characNo / m_name），失败 Free + LineFunc(0xa68)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 134（ORIG 216 / ours 210），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_DeleteCharac::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stDeleteCharac& out = static_cast<stDeleteCharac&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xa41,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }

    // [b28d] P28：ORIG 0x081c1204 才 `mov 0x10(%ebp),%eax; mov %eax,-0x20(%ebp)`
    // （msg 引用槽在 check_error/两级 errNo 检查之后初始化），非函数头。
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);
    // [b28d] P0/P183：ORIG 无 `slot` 具名局部——每个使用点重新发
    // `mov req,%eax; movzbl 0xd(%eax),%eax; movsbl %al,%eax`（4 处，共 +8 条）。
    user->RemoveCharacMercenaryInfo(
        static_cast<unsigned int>(user->get_charac_no(static_cast<char>(req.m_slot))));

    // [G5-ProcB] ORIG 0x081c124a Acquire → 0x081c124f `movl $0x1,0x8(%esp)` /
    // 0x081c1257 `mov %eax,0x4(%esp)` / 0x081c125b `lea -0x3c(%ebp),%eax;(%esp)`：
    // Acquire 返回值经 %eax 直传 CStreamGuard 构造实参，ORIG 无 `stream` 具名槽
    // （旧文 `Stream* stream = ...` 多 store/load 两条且槽基下移 4）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xa4d), true);
    // ORIG（Ghidra 0x81c13xx 序列 de→op<<→de→op<<→pt→GetInBuffer）：operator* 显式链形。
    guard.operator*()->operator<<(5);                  // SIG 类型 5
    guard.operator*()->operator<<(static_cast<int>(user->GetUID()));
    SIG_DELETE_CHARAC* sig = guard->GetInBuffer<SIG_DELETE_CHARAC>();
    memset(sig, 0, 0x14);
    sig->m_accId = user->get_acc_id();
    sig->m_characNo =
        static_cast<unsigned int>(user->get_charac_no(static_cast<char>(req.m_slot)));
    if (sig->m_characNo == 0)
    {
        out.m_errNo = 0x15;                       // 槽位无角色
        GlobalData::s_stream_pool->Free(guard);
        return 0;
    }
    sig->m_guildId = static_cast<unsigned int>(
        user->GetGuildIdBeforeDeleteCharacter(static_cast<char>(req.m_slot)));

    if (!user->lockDeleteCharac())
    {
        GlobalData::s_stream_pool->Free(guard);
        return LineFunc(0xa68,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    // [J5-ProcD 2026-09-11 核账] P328⑩ 单语句链式试形**已实测劣化 + 回滚**：
    // `cMyTrace(...)(...)` 使 ours 216→**219（+3）**。ORIG 0x081c12xx 段求值序虽为
    // 「先算 3 实参入 callee-saved（%edi/%esi/%ebx）→ 再 call ctor → 最后压栈」，
    // 但 4.4 -O0 对本处单语句链式反而多出 3 条（实参求值序旋钮不成立），
    // **勿再试**；恢复两语句形（本地 216 = ORIG 216 已持平）。
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xa6e, 0);
    trace("[%s][%d] accid(%s) delete charac(%d) %s",
          __PRETTY_FUNCTION__,
          0xa6e,
          NumberToString(user->get_acc_id(), 0),
          // P48：m_characNo/m_name 两字段各触发一次 vector::operator[]（禁缓存 info）。
          static_cast<int>(
              user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_characNo),
          reinterpret_cast<const char*>(
              user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_name));
    return 0;
}
