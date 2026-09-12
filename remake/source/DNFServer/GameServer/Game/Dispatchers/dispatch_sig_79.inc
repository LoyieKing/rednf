// [SemA00 2026-09-12] verdict=**IDENTICAL**（compare_tu clean 口径）ORIG 0x08207b52 orig/ours=**125/125**。
//   旧判词「verdict=DIFF / 126-131 / 残余为跨版本寄存器分配·语义已对齐」**作废** ——
//   本件实为**真语义差异（gate 条件反）**：ORIG `test; jne <mov $0>` 证明正常路径 =
//   `state > 2 && getCurCharacR() != 0`，旧源码写 `== 0` ⇒ 两条状态路径整体相反（详见
//   函数体内 [真语义差异·条件反] 注释与 /tmp/lane_diff/SemA00/slice_report.md）。
//   同批修正：头部 user 判空（ORIG 无，P349）、reply 预清（ORIG 无，P348）、
//   characNo 实参内联（ORIG 求值序 reply→characNo→receiver）。
int Dispatcher_ReplyGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [SemA00 2026-09-12] P348：ORIG 全函数无 `movb $0x0` 预清（-0xd(%ebp) 仅 lea/写/读
    //   三处：0x08207bb4 / 0x08207ce7 / 0x08207cea）⇒ 原码是未初始化声明，写 `= 0` 多 1 条。
    unsigned char reply;
    // ORIG 0x0821..：[20] 段 LineFunc(0xa278) 就地早退（`call; mov %eax,%ebx; jmp 尾`），
    //   我方旧形 `if (gate) {...} return LineFunc(...)` 把 LineFunc 沉到函数尾 ⇒ 指令序错位。
    // [SemA00 2026-09-12] P349：ORIG 头部无 user 判空（0x08207b5a 直读 0xc(%ebp) → get_state）
    //   我方多 `cmpl $0x0,0xc(%ebp); je` 两条 ⇒ 按 ORIG 只保留它有的两个条件。
    // [SemA00 2026-09-12] 【真语义差异·条件反】ORIG 0x08207b70-0x08207b85：
    //   test %eax,%eax; jne 8207b80(mov $0) / fall-through mov $1
    //   ⇒ 布尔值 = (curCharacR == 0)；即「state>2 且 curCharacR != 0 才走正常路径」。
    //   我方旧形 `je <mov $0>` 使判据 = (curCharacR == 0)，state>2 时早退条件与 ORIG 相反
    //   （ORIG 在 p==0 时早退，我方在 p!=0 时早退）⇒ 邀请回执路径整体反。
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0xa278,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    {
        if (!packet.get_byte(reply))
        {
            return LineFunc(
                0xa27c,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
        {
            PacketGuard response;
            response.put_header(1, 0x9b);
            response.put_byte(0);
            response.put_int(99);
            response.finalize(true);
            user->Send(response);
            return 0;
        }
        const int secuRet =
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xc);
        if (secuRet != 0)
        {
            reply = static_cast<unsigned char>(
                GlobalData::s_pSecuProtectionField->GetOppositeErr(secuRet));
        }
        // [SemA00 2026-09-12] 实参内联：ORIG 的求值序为 reply(esi) → characNo(ebx) →
        //   GetServerProxy(receiver 最后)。具名 characNo 局部（含 register 形）会使
        //   4.4 提前发射 getCurCharacNo 并把 reply 读回推后（实测 2 条位置差）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendReplyGuildInvite(user->getCurCharacNo(), reply);
        return 0;
    }
}