// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821d9ee orig/ours=345/329 ndiff-blocks=61
//   帧差 sub $b0 vs $d0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0xb0,%esp | mov    %eax,%ebx
int Dispatcher_UpdateServerMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821d9ee..0x0821df69 (0x57c 字节)。

    if (user->get_state() != 3)
    {
        // [b28d-solo-Dispatchers] P227/P0：ORIG 0x0821d9ee 段在 cMyTrace ctor 前
        //   求值 varargs 并跨 ctor 以 %ebx/%esi 保活（`call get_acc_id;
        //   movl $0,0x4(%esp); mov %eax,(%esp); call NumberToString; mov %eax,%ebx;
        //   call get_state; mov %eax,%esi; call cMyTraceC1; …`），**无** accId/idStr
        //   具名槽 ⇒ 全表达式临时链（旧码两局部使帧 +8）。
        cMyTrace(__PRETTY_FUNCTION__, 0xd1c6, 0)(
            "ServerMessage : Dispatcher_UpdateServerMessage => User State : %d, m_id : %s",
            user->get_state(), NumberToString(user->get_acc_id(), 0));
        return LineFunc(0xd1c7, __PRETTY_FUNCTION__, 0, 0);
    }

    PacketGuard guard;
    guard.put_header(1, 0xdd);

    unsigned char msgIdx = 0;
    if (packet.get_byte(msgIdx) != 1)
    {
        return LineFunc(0xd1d0, __PRETTY_FUNCTION__, 0, 0);
    }
    int titleLen = 0;
    if (packet.get_int(titleLen) != 1)
    {
        return LineFunc(0xd1d5, __PRETTY_FUNCTION__, 0, 0);
    }

    if (titleLen < 0 || 0x31 < titleLen)
    {
        guard.put_byte(0);
        guard.put_byte(1);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }

    char title[50];   // -0x8e，0x32
    if (packet.get_str(title, 0x32, titleLen) != 1)
    {
        return LineFunc(0xd1e4, __PRETTY_FUNCTION__, 0, 0);
    }
    if (G_CDataManager()->hasPreventString(title))
    {
        guard.put_byte(0);
        guard.put_byte(0x17);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }

    int contentLen = 0;
    if (packet.get_int(contentLen) != 1)
    {
        return LineFunc(0xd1f4, __PRETTY_FUNCTION__, 0, 0);
    }

    // [Lane1Dispatchers 2026-09-10] N78/P214③：ORIG 0x0821dcd3-0x0821dd22 是
    //   `memset(content,0,0x1e)` 的 4.1 **内联**对齐剥离慢路（起始 `and $0x2` =
    //   地址 ≡2 mod 4，与 -0x56 相符）；我方 `memset` 在 4.4 -O0 恒退化为
    //   `call memset`（多 3 条 + 丢 30 条形）。改聚合初始化 `= {0}` 即复现慢路。
    char content[30] = {0};   // -0x56，0x1e
    // [b28d-solo-Dispatchers] 语义修正（Main 新判据②回边核对命中）：ORIG
    //   0x0821dcd3-0x0821dd22 是 `memset(content,0,0x1e)` 的 4.1 **内联慢路**展开
    //   （`lea -0x56(%ebp),%eax; mov $0x1e,%ebx; …and $2…; mov %edx,(%eax,%ecx,1);
    //   add $0x4,%ecx; cmp %esi,%ecx; jb`+尾 2/1 字节），即 ORIG **有**清零。
    //   第一波据 p167 EXTRA `memset`+1 误判「ORIG 无清零」而删除 ⇒ 丢失语义
    //   （get_str 短串时 strcmp 读未初始化尾部）⇒ 恢复 memset。
    if (packet.get_str(content, 0x1e, contentLen) != 1)
    {
        return LineFunc(0xd1f9, __PRETTY_FUNCTION__, 0, 0);
    }

    if (strcmp(content, user->getCurCharacName()) != 0)
        return LineFunc(0xd1fd, __PRETTY_FUNCTION__, 0, 0);

    const int curSec = GlobalData::s_systemTime_.getCurSec();
    CSharedServerMessageManager* mgr = G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        int condResult = 0;
        if (!mgr->CheckUserCondition(user, (int)msgIdx, condResult))
        {
            guard.put_byte(0);
            if (condResult == 0)
                guard.put_byte(3);
            else
                guard.put_byte(condResult);
        }
        else if (!mgr->IsChangableMessage((int)msgIdx, curSec))
        {
            guard.put_byte(0);
            guard.put_byte(1);
        }
        else
        {
            mgr->UpdateServerMessage(user, (int)msgIdx, title, content, curSec);
            guard.put_byte(1);
        }
    }
    guard.put_byte((int)msgIdx);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}