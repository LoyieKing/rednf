unsigned int Dispatcher_EnterDisjointStore::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;

    ParamUidView_A2* pUid = reinterpret_cast<ParamUidView_A2*>(&msg);  // ORIG mov 0x10(%ebp) 即 msg
    CUser* target = G_GameWorld()->find_from_world(pUid->m_uid);   // 0x086c4b9c
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
