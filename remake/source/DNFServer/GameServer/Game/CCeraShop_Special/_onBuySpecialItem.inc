// ============================================================================
// _onBuySpecialItem —— ORIG 0x08327454 (T)
// itemIdx ∈ 特殊集（0x3f..0x49 除 0x44，掩码 0x7df）时，投递
// SIG_SAVE_CRAZY_LEVELUP_EVENT 流（"0707" 头 + accId + characNo(-1)）至 LOGDB_Q；
// 再按 Is2010ChuseokPacket(itemIdx) 分支调 DB_UpdateChuseokPackagePurchase。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08327454 orig/ours=97/102 ndiff=79。
// 槽/寄存器分配差（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N6/N99] 97/99；(1u<<k)&0x7df 被 4.4 恒折
// (0x7df>>k)&1 + k 重算。探针 14 形（内联/具名/register/volatile/双 register/bit 拆分）
// 全产 shr，shl 形不可择 ⇒ N6/N99 封顶。P167 CLEAR 18/18。
void CCeraShop::_onBuySpecialItem(CUser* user, ENUM_CERA_SPECIALITEM,
                                  unsigned long itemIdx)
{
    // ORIG: (param_4 - 0x3f < 0xb) && ((1 << ((param_4 - 0x3f) & 0x1f)) & 0x7df) != 0
    if ((unsigned int)(itemIdx - 0x3f) < 0xb &&
        ((1u << (itemIdx - 0x3f)) & 0x7dfu) != 0)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x1971),
                               true);
        guard.operator*()->operator<<((int)0xb4);
        guard.operator*()->operator<<(-1);
        SaveCrazyLevelupPayloadView* buf =
            reinterpret_cast<SaveCrazyLevelupPayloadView*>(
                guard->GetInBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>());
        std::memcpy(buf->m_tag, "0707", 5);  // DAT_08c25e65 = "0707\0"
        buf->m_accId = user->get_acc_id();
        buf->m_characNo = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    if (Is2010ChuseokPacket(itemIdx))
        DB_UpdateChuseokPackagePurchase::makeRequest(user->get_acc_id());
}

