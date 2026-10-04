// ============================================================================
// 7. Dispatcher_EnterDisjointStore::check_error
//    报告 docs/class_func_reports/Dispatcher_EnterDisjointStore/check_error.md
// ============================================================================
unsigned int Dispatcher_EnterDisjointStore::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;

    MSG_ENTER_DISJOINT_STORE& req = reinterpret_cast<MSG_ENTER_DISJOINT_STORE&>(msg);
    CUser* target = G_GameWorld()->find_from_world(req.m_field0e);
    if (target == 0)
        return 0x15;

    if (target->GetCurCharacExpertJobType() != 3)          // 0x0822f894
        return 0x13;
    expert_job::CExpertJob* job = 0;                               // ORIG movl $0,-0xc 声明预清
    job = target->GetCurCharacExpertJob();                         // 0x0822f8d4
    if (job == 0)
        return 0x13;
    if (job->GetType() != 3)                               // 0x08234796
        return 0x13;
    return 0;
}