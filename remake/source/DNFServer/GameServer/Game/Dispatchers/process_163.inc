int Dispatcher_CloseDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stCloseDisjointStoreParam& out = static_cast<stCloseDisjointStoreParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2b1b,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    expert_job::CExpertJob* job = user->GetCurCharacExpertJob();
    job->OnCloseMission(user, (expert_job::ENUM_CLOSE_MISSION_TYPE)0);
    return 0;
