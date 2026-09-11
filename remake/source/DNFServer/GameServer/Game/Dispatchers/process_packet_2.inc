int Dispatcher_SetSlot::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                       CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0814006e] pReq→-0x10 / pRes→-0xc；回显 3 条 → check_error →
    // !=0 提前 return；slotType!=1/2 → err=2 提前 return；否则 setSlot(bool,slot)。
    AltarSetSlotReqView* pReq =
        reinterpret_cast<AltarSetSlotReqView*>(&req);
    CMDPacketStruct::_ResAdvanceAltarSetSlot* pRes =
        reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarSetSlot*>(&resp);
    pRes->m_bSet = pReq->m_bSet;                           // isSet 回显（bool 字节写）
    pRes->m_field08 = pReq->m_field10;                     // field08 回显（int）
    std::memcpy(pRes->m_slot, pReq->m_slot.m_raw, 0xc);    // _SetSlotData 整块回显
    pRes->m_nErrorCode = check_error(user, req);
    if (pRes->m_nErrorCode != 0)
        return 0;
    if (pReq->m_slot.m_data.m_type != 2 &&
        pReq->m_slot.m_data.m_type != 1)
    {
        pRes->m_nErrorCode = 2;                            // [objdump 0x08140104]
        return 0;
    }
    pRes->m_nErrorCode = user->m_advanceAltar.setSlot(     // ORIG T 0x08131b58
        pReq->m_bSet,
        pReq->m_slot.m_data);
    return 0;
}