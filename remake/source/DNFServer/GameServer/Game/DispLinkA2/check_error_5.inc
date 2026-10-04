// ============================================================================
// 13. Dispatcher_RequestDisjointItem::check_error
//     报告 docs/class_func_reports/Dispatcher_RequestDisjointItem/check_error.md
// ============================================================================
unsigned int Dispatcher_RequestDisjointItem::check_error(CUser* user, MSG_BASE& msg,
                                                ParamBase& param)
{
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()     // 0x081625e6（结果直传）
            ->isRestricted(user, RestrictCategory::Enum(1), 0x13))
        return 0xd1;

    MSG_REQUEST_DISJOINT_ITEM& req = reinterpret_cast<MSG_REQUEST_DISJOINT_ITEM&>(msg);
    CUser* target = G_GameWorld()->find_from_world(req.m_field0e);
    if (target == 0)
        return 3;
    return 0;
}