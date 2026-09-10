// ---- respond_special_item_charac_slot_extend —— ORIG 0x0827c0c2 ----
// 增角色槽限 → 组 SIG_EXTEND_CHARAC_SLOT 包（acc_id @0 / slotEffect @4 /
// slotLimit @5）入队列 2 → _sendCharacSlotExtendPacket。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P82 具名视图 + P211 字段同型化（LHS-first）
void SpecialItemHandler::respond_special_item_charac_slot_extend(
    int type, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    // b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827c0c2 orig/ours=92/92 ndiff=64。
    // 残差为帧/槽号平移 + CStreamGuard/STL 内联差；语义按报告逐条对齐。
    CERA_PARAM_TYPED(param)->m_user->incCharacSlotLimit(
        static_cast<unsigned char>(CERA_PARAM_TYPED(param)->m_value));

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0x8d),
                           true);
    guard.operator*()->operator<<((int)0x28a);
    guard.operator*()->operator<<(-1);
    SlotExtendPayloadView* buf = reinterpret_cast<SlotExtendPayloadView*>(
        guard->GetInBuffer<SIG_EXTEND_CHARAC_SLOT>());
    buf->m_accId =
        static_cast<int>(CERA_PARAM_TYPED(param)->m_user->get_acc_id());
    buf->m_slotEffect = static_cast<char>(
        CERA_PARAM_TYPED(param)->m_user->getSlotEffectCount());
    buf->m_slotLimit = static_cast<char>(
        CERA_PARAM_TYPED(param)->m_user->getCharacSlotLimit());
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    m_owner->_sendCharacSlotExtendPacket(CERA_PARAM_TYPED(param)->m_user, type);
}

