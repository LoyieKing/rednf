// ============================================================================
// 14. Dispatcher_Request_Seria_Buff::check_error
//     报告 docs/class_func_reports/Dispatcher_Request_Seria_Buff/check_error.md
// ============================================================================
int Dispatcher_Request_Seria_Buff::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if (user->getCurCharacR() == 0)
        return -1;

    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x5d);  // 0x08115998
    if (event->IsEventing(0))                        // vptr+0x34（slot 13）
    {
        if (!G_CDataManager()->m_seriaBlessing.is_possible_lv(user->get_charac_level()))
            return -1;
    }
    else
    {
        if (G_CDataManager()->m_growthPower.get_idx_break_away_section(
                user->get_charac_level()) < 0)
            return -1;
    }
    return 0;
}