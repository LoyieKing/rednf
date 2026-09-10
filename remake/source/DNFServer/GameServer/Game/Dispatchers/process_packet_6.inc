int Dispatcher_ExchangeSlot::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                            CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081408a4] pReq→-0x10 / pRes→-0xc；field08 回显 → check_error
    // early-return → exchangeSlot(&slot1,&slot2) → 两段 memcpy 回显。
    CMDPacketStruct::_ReqAdvanceAltarExchangeSlot* pReq =
        reinterpret_cast<CMDPacketStruct::_ReqAdvanceAltarExchangeSlot*>(&req);
    CMDPacketStruct::_ResAdvanceAltarExchangeSlot* pRes =
        reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarExchangeSlot*>(&resp);
    pRes->m_field08 = pReq->m_field10;                     // field08 回显（int）
    // b28c-L5：清 2 处 `register asm` 存量绑定（P0/N96）。ORIG 0x081408a4 尾形
    //   `call check_error; mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,0x4(%eax)`
    //   由 unsigned 字段 ← int 返回值的异型转换自然产生（P211）。
    pRes->m_nErrorCode = check_error(user, req);
    if (pRes->m_nErrorCode != 0)
        return 0;
    // b28c-L5：清 3 处 `register asm`（同上 P0）。ORIG 三实参地址各占独立寄存器
    //   （slot2(+0x24)→%ecx、slot1(+0x18)→%edx、&user->m_advanceAltar→%eax）且均为
    //   单条 `lea disp(%base),%reg`；改回实参位内联取址（P23/P134 内联实参形），
    //   让 4.4 自行分配 —— 绑定期反而被拆成 `add $imm,%eax; mov %eax,%ecx` 两段。
    pRes->m_nErrorCode = user->m_advanceAltar.exchangeSlot(
        *reinterpret_cast<CMDPacketStruct::_SetSlotData*>(pReq->m_slot1),
        *reinterpret_cast<CMDPacketStruct::_SetSlotData*>(pReq->m_slot2));
    std::memcpy(pRes->m_slot1, pReq->m_slot1, 0xc);        // slotA 回显
    std::memcpy(pRes->m_slot2, pReq->m_slot2, 0xc);        // slotB 回显
    return 0;
