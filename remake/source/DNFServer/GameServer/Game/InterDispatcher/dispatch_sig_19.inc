// [b28d-lane2] verdict=DIFF ndiff=162（ORIG 0x084c8ec0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=102（ORIG 0x84c8ec0）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorBuddyInOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorBuddyInOutPktView* pPkt = reinterpret_cast<Inter_MonitorBuddyInOutPktView*>(data);
    PacketGuard packet;
    if (pPkt->m_14 == '\x01')
        packet.put_header(0, 0x36);
    else
        packet.put_header(0, 0x37);
    packet.put_byte(
        (unsigned int)pPkt->m_12);
    packet.put_int(pPkt->m_accId);
    const int local_20 = (int)strlen(pPkt->m_name);
    packet.put_int(local_20);
    packet.put_str(pPkt->m_name, local_20);
    packet.put_byte(
        (unsigned int)pPkt->m_13);
    packet.finalize(true);
    user->Send(packet);

    // [b28d-solo-InterDispatcher] P226 语句表达式：ORIG 0x084c8ff2..0x084c900f 短路物化
    // 岛（mov$1/mov$0/test al）零槽直测；具名 bool 初值+置位形 4.4 恒落槽。
    if (({ pPkt->m_33 == '\x01' &&
          user->getCurCharacR() != 0; }))
    {
        // ORIG 0x084c8ff2..0x084c900f：中间量**全部无槽**（`mov <pPkt 槽>,%eax;
        //   lea 0x15(%eax),%esi`、值直落 %ebx/%edi/%esi 跨调用；`mov %esi,0x1c(%esp)` 等）
        //   ⇒ 原码是内联实参（P0/P183），旧具名形每量多一次 `mov %eax,<槽>; mov <槽>,%eax`
        //   往返（我方 OURS[110:150] 段），且基址走 pPkt 槽（非重读 data 形参）。
        Inven_Item item;
        char local_16d[256];
        OS_API::snprintf(local_16d, 0xff,
                         g_scriptStringManager_.findString(4, "return_user_msg_3", 0),
                         user->getCurCharacName(),
                         pPkt->m_name);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0),
            item, 0, user->getCurCharacNo(), local_16d, strlen(local_16d), 0,
            user->GetServerGroup(), 0, 0);
    }
    return 0;

// [G2B-Monitor] 本件已做：删 5 个具名中间量（iVar2/characName/pcVar4/uVar3/sVar5/uVar6/uVar7）
//   全内联，基址改走 pPkt 槽（ORIG `mov <S>,%eax; lea 0x15(%eax),%esi`；我方旧形
//   `mov 0x10(%ebp),%eax; add $0x15,%eax; mov %eax,<S>`）。ORIG `push %edi`+帧 0x19c
//   与我方旧 `sub $0x1a0`（无 push）总量相同 ⇒ 旧具名槽顶替了 callee-saved 寄存器。
//   残余留 N：跨调用中转寄存器 ORIG=%esi/%edi/%ebx 的选择，属 LRA 分配族，无源码旋钮。
}