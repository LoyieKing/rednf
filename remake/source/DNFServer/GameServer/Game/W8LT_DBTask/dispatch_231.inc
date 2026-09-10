bool DB_SaveCharacSlotExtend::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SaveCharacSlotView* sig = reinterpret_cast<SaveCharacSlotView*>(
        stream->GetOutBuffer<SIG_EXTEND_CHARAC_SLOT>());
    db->set_query(
        "upDate charac_view set slot_effect_count=%u, charac_slot_limit=%u where m_id=%s",
        (unsigned int)sig->m_slotEffect, (unsigned int)sig->m_slotLimit,
        NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        unsigned int slotEffect = sig->m_slotEffect;
        unsigned int slotLimit = sig->m_slotLimit;
        const char* idStr = NumberToString(sig->m_mId, 0);
        // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa1f5, 5)(
            "upDate charac_view set slot_effect_count(%u), charac_slot_limit(%u) failed at m_id(%u)",
            slotEffect, slotLimit, idStr);
        return 0;
    }
    return 1;
