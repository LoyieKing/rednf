int Dispatcher_EnterDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d01ae：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2adf)；err==0 → 分解机进入商店。
    stEnterDisjointStoreParam& out = static_cast<stEnterDisjointStoreParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x2adf,
                 __PRETTY_FUNCTION__,
                 out.m_4, (unsigned int)user->get_acc_id());
        return 0;
    }

    // msg+0xe：ushort 对象 uid → find_from_world；目标用户分解机 OnEnterDisjointStore。
    unsigned short uid =
        *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&msg) + 0xe);
    CUser* target = G_GameWorld()->find_from_world(uid);

    expert_job::CExpertJob* job = target->GetCurCharacExpertJob();
    expert_job::CDisjointer* disjointer = (expert_job::CDisjointer*)job;
    disjointer->OnEnterDisjointStore(user);
    return 0;
