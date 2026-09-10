int Dispatcher_ResetStar::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                         CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081410aa] pReq→-0x14 / pRes→-0x10 / data→-0xc；check_error
    // early-return；resetStar 回写后短路判定 `err==0 && (data=get(), data!=0)`
    // 进入 noti+guard 发送块。
    CMDPacketStruct::_ReqAdvanceAltarResetStar* pReq =
        reinterpret_cast<CMDPacketStruct::_ReqAdvanceAltarResetStar*>(&req);
    CMDPacketStruct::_ResAdvanceAltarResetStar* pRes =
        reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarResetStar*>(&resp);
    // ORIG 0x081410aa：value→%edx 先，槽地址→%eax 后。
    pRes->m_nErrorCode = check_error(user, req);
    if (pRes->m_nErrorCode != 0)
        return 0;
    // [b28d-solo-S206] N-P211（跨 TU 依赖）：ORIG 该 store LHS-first ⇒ ORIG resetStar
    //   返回与 unsigned 字段同型；我方声明 int（advancealtar.h:444，定义在非本泳道
    //   .cpp）⇒ 调用点 cast 实测无效（P211 复证）。
    pRes->m_nErrorCode = user->m_advanceAltar.resetStar(   // ORIG T 0x081333b2
        user, static_cast<CMDPacketStruct::ResetStarType::T>(pReq->m_field10));
    AdvanceAltarData* data;                                // ORIG 槽 -0xc（条件内写）
    if (pRes->m_nErrorCode == 0)
    {
        // ORIG 0x8141?: 赋值落槽后 `cmpl $0,-0xc(%ebp); je` 直折比较（无 bool 岛）。
        data = user->m_advanceAltar.getCurrentAdvanceAltarData();  // T 0x081348c4
        if (data != 0)
        {
            NOTIPacketStruct::_NotiAdvanceAltarUpdateRidable noti;   // ORIG ctor 0x08141ff8
            PacketGuard guard;
            guard.put_header(0, 0x210);                          // [objdump 0x08141172]
            guard.put_binary((char*)&noti, 1);                   // put_binary(&noti,1)
            advancealtar::putRidableInfoToPacket(guard, *data);  // ORIG T 0x08141781
            guard.finalize(true);
            user->Send(guard);                                   // ORIG T 0x086485ba
        }
    }
    return 0;
