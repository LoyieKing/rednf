// dispatch —— ORIG 0x0841a7e4（size 0x34b）
// 活动类型 1..7 走对应 Check/Update 对（type 6 用 m_field00），失败返 false；
// 然后 _HardCodeEventItems；未命中硬编码且无事件标记时插 charac_event_items 表。
// [b28d-solo-2R] NEAR(5)：ORIG 先求前两实参（sig+4/sig+0）再载 loadId 入 %ecx；
// 4.4 恒先载 loadId（`mov -0x10,%ecx`）；语句表达式 `({loadId;})` 同形 ⇒ N18a/P75 装载调度封顶。
// [b28d-solo-4] P211/P315 同型化旋钮：loadId 由 int 改 unsigned int（与
//   LoadCharacEventItem 第 3 形参 unsigned 同型）⇒ 4.4 把该实参装载延后到
//   前两实参之后，复现 ORIG `mov -0x10,%ecx` 末位装载（探针 h2 实证）。
// ORIG 0x841a7e4
bool DB_SaveEventItem::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    SIG_SAVE_EVENTITEM* sig = stream->GetOutBuffer<SIG_SAVE_EVENTITEM>();
    unsigned int loadId;
    bool done;
    if (sig->m_field14 != 0)
    {
        done = false;
        if (sig->m_field08 == 1)
        {
            if (_CheckAlreadyGetYegamItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = _UpdateEventYegamInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 2)
        {
            if (WongWork::EventATI::CheckAlreadyGetATIItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventATI::UpdateEventATIInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 3)
        {
            if (WongWork::EventSigong::CheckAlreadyGetSigongItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventSigong::UpdateEventSigongInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 4)
        {
            if (WongWork::EventEverland::CheckAlreadyGetEverlandItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventEverland::UpdateEventEverlandInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 5)
        {
            if (WongWork::EventMuz::CheckAlreadyGetMuzItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventMuz::UpdateEventMuzInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 6)
        {
            if (WongWork::EventAlttle::CheckAlreadyGetAlttleItem(sig->m_field00, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventAlttle::UpdateEventAlttleInfo(sig->m_field00, sig->m_field0c);
        }
        else if (sig->m_field08 == 7)
        {
            if (WongWork::EventFCCombo::CheckAlreadyFCComboEventItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventFCCombo::UpdateFCComboEventInfo(sig->m_field14, sig->m_field0c);
        }
        if (!done)
        {
            return false;
        }
    }
    if (_HardCodeEventItems(sig))
    {
        return true;
    }
    if (sig->m_field18 != 0)
    {
        return true;
    }
    loadId = 0;
    if ((loadId = WongWork::EventCommon::InsertEventItem2EventTable(
             sig->m_field04, sig->m_field0c, sig->m_field10, sig->m_field08)) != 0)
    {
        // 残留（blocked）：ORIG `mov -0x10,%ecx` 单条提前提载先于 vtable 装载，
        // 4.4 恒在组栈位时才装载（或 register asm 反产三重拷贝，实测）——
        // N18a 类跨版本装载调度。
        WongWork::EventCommon::LoadCharacEventItem(sig->m_field00, sig->m_field04, loadId);
    }
    return true;
}