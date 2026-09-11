// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 084420ea
// [L4-r1] cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 58（持平；余 push/pop 差来自
//   NumberToString 三实参跨 call 保活，属 P52/N7 族）。
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
        // [M8DBB 2026-09-11 试] P329 具名实参中介内联回实参位（ORIG 0x084421b9
        // `mov %eax,%ebx` / 0x084421c2 `movzbl %al,%edi` / 0x084421cc `movzbl
        // %al,%esi` = 三实参跨 ctor 保活在 callee-saved ⇒ 无具名槽）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa1f5, 5)(
            "upDate charac_view set slot_effect_count(%u), charac_slot_limit(%u) failed at m_id(%u)",
            (unsigned int)sig->m_slotEffect, (unsigned int)sig->m_slotLimit,
            NumberToString(sig->m_mId, 0));
        return 0;
    }
    return 1;
}