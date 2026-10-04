// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084d24a2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+8。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=102（ORIG 0x84d24a2）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 8→-7（164 vs 157，TU 121/1330/5/134 不回退）：真修四处——
//   ① 删 `const unsigned char type`/`Stream* stream` 具名局部（P0/P259，类型检查改逐次 `pPkt->m_15e` 重读、Acquire 内联进 CStreamGuard 实参）；
//   ② `if (data==0)`→`if (pPkt==0)` 使判空读槽（ORIG `cmpl $0,-0x10(%ebp)`）；③ 'o'/'c' 分支改早返形（P16/P59：`if(local_10==0)return 0; Send...; return 0;`）；
//   ④ put_binary 实参回 data。残余=4.1 EH 状态标志 esi（`mov $0/%esi` + `test %esi;je;mov $0,%ebx`）在 4.4 无对应（N104/N7 族），语义等价。
int Inter_ReportMannerless::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2D] verdict=N：残余为 ORIG 用 %esi 承载跨分支标志/返回（ORIG[141] mov $0x1,%esi 等）
    //   与我方 %eax + 具名 local_10 的寄存器分配差；未定位到对应具名局部，未改。
    Inter_ReportMannerlessPktView* pPkt = reinterpret_cast<Inter_ReportMannerlessPktView*>(data);
    if (pPkt == 0)
        return 0;

    // [H2·2026-09-11 laneC 语义轴②接收者错] ORIG 084d24c0 `movl $0x0,-0xc(%ebp)` ⇒ 初值常量 0；
    //   084d24cb `cmpl $0x0,0xc(%ebp); je 084d24f1`（user==0 直接跳 `mov $0x0,%eax` ⇒
    //   条件为假）+ 084d24ea `mov $0x1,%eax` 仅 'm_e != characNo' 落点 ⇒ 条件 = `user != 0 &&`；
    //   084d2516 `mov 0xc(%ebp),%eax; mov %eax,-0xc(%ebp)` ⇒ **else 分支保留**（local_10 = user）。
    //   旧写 `user == 0 || …` 使 user==0 时走 GetUserByAccId ⇒ 向**错误接收者**发包/写 DB（真缺陷）。
    CUser* local_10 = 0;
    if (user != 0 &&
        pPkt->m_characNo != user->get_charac_no(-1))
    {
        local_10 = G_CGameManager()->GetUserByAccId(
            pPkt->m_accId);
    }
    else
    {
        local_10 = user;
    }

    if (pPkt->m_15e == 'o')
    {
        if (local_10 == 0)
            return 0;
        local_10->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 0x62);
        return 0;
    }
    else if (pPkt->m_15e == 'c')
    {
        if (local_10 == 0)
            return 0;
        local_10->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 0x5f);
        return 0;
    }

    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "InterDispatcher.cpp", 0x326d), true);
        guard.operator*()->operator<<(0xca);
        guard.operator*()->operator<<(-1);
        guard.operator->()->put_binary(data, 0x15f);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

        if (local_10 != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0x8c);
            packet.put_byte(1);
            packet.finalize(true);
            local_10->Send(packet);
        }
    }
    return 0;
}