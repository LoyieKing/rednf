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
