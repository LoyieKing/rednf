int Inter_PcroomResponse::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2D] verdict=N：残余 24 行分三类——帧差 +0x10（父泳道统一裁决的共享机制）、
    //   `mov %eax,<S>; mov <S>,%eax` 单点往返、以及 G_TimerQueue 段块序互换；
    //   均无单件可判定依据，未改。
    Inter_PcroomResponsePktView* pPkt = reinterpret_cast<Inter_PcroomResponsePktView*>(data);
    if (user->get_state() == 0)
        return 0x4404;

    if (user->getCurCharacR() != 0)
        user->setPcRoomAuth(true);
    user->SetPCRoomNo(pPkt->m_12);

    // ORIG 0x84db4be `movb $0x0,-0x2b(%ebp)`：写后全函数未读的死局部（P171 照抄原则）。
    char deadFlag = 0;
    (void)deadFlag;

    if (pPkt->m_1c == 7 &&
        pPkt->m_1a != 3 &&
        pPkt->m_1d == 1)
    {   // 封禁登录
        user->DisConnSig((DISCONN_SIG)0x3c, true, 0);
        // P328⑩/P227：ORIG 0x84db50e..0x84db546 先求 acc_id→%esi、acc_name→%ebx，
        // 其后才 cMyTrace ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x4479, 0)(
            "SecuritySystem user ban ip login(%s,%u)",
            user->get_acc_name(), user->get_acc_id());
    }

    bool forbidden = false;                            // -0x2a
    bool showMsg = true;                               // -0x29
    // ORIG 084db578..084db592 二分决策树（cmp $1 / jg / cmp $2 / cmp $3）⇒ 原码是 switch（P17/P46）。
    // ★真语义修正：case 3 无条件 forbidden=true（084db5a5 movb $1,-0x2a），
    //   仅 showMsg=false 受 m_1b==1 约束（084db5b3..084db5b8）；
    //   旧写法把 forbidden 也绑在 m_1b==1 上 ⇒ kind==3 且 m_1b!=1 时漏封禁。
    switch (pPkt->m_1a)
    {
    case 0:
        if (pPkt->m_1b == 2)
            forbidden = true;
        break;
    case 1:                                            // ORIG 084db5be/084db5c1 空叶（nop;jmp）
    case 2:
        break;
    case 3:
        forbidden = true;
        if (pPkt->m_1b == 1)
            showMsg = false;
        break;
    }

    if (forbidden)
    {
        // P328⑩/P227：ORIG 0x84db5cf..0x84db601 先求 m_1b→%esi、m_1a→%ebx 再 ctor。
        cMyTrace(__PRETTY_FUNCTION__, 0x44ec, 0)(
            "Inter_PcroomResponse::dispatch_sig() forbidden ip (%d), %d)",
            (int)pPkt->m_1a,
            (int)pPkt->m_1b);

        PacketGuard packet;
        packet.put_header(0, 0xe9);
        packet.put_byte(1);
        packet.put_byte(5);
        const char* msgStr = g_nexonMsgTable.GetMessageString(
            (int)pPkt->m_1b);
        char msg[0x401] = { '\0' };                    // -0x45d（ORIG 内联清零）
        const char* str = g_scriptStringManager_.findString(4, msgStr, 0);
        strncpy(msg, str, 0x400);
        const int msgLen = (int)strlen(msg);
        packet.put_int(msgLen);
        packet.put_str(msg, msgLen);
        packet.put_byte((int)showMsg);
        packet.finalize(true);
        user->Send(packet);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x84, 0x78,
                                    (int)user->getCurCharacNo(), 0);
    }

    if (pPkt->m_12 != 0)
    {
        // ORIG 0x84db826/0x84db82f 两值各落槽（-0x20/-0x1c）后压参 ⇒ 具名局部（P0）。
        long curSec = GlobalData::s_systemTime_.getCurSec();
        long expireTime = pPkt->m_16;
        WongWork::CHandlePremium::handleSetUserPCRoom(user, curSec, expireTime);
    }
    // ORIG 0x84db84b..0x84db862 get_event_script_mng 返回值 eax 直压 (%esp)，无具名槽 ⇒ 单语句链。
    reinterpret_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->send_pc_room_play_init_time(user);
    return 0;
