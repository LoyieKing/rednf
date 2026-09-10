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
    // [FL1 2026-09-10] ORIG 0x081d03e4 直证（P0）：`call GetCurCharacExpertJob;
    //   mov (%eax),%edx; add $0x4,%edx; mov (%edx),%ecx; …; mov %eax,(%esp);
    //   call *%ecx` —— 返回值全程留 %eax、**无具名槽**（旧具名 `job` 局部使
    //   4.4 落槽 -0xc 并三次回读，+3 条；且挤得 param 槽由 -0xc 变 -0x10）。
    user->GetCurCharacExpertJob()->OnCloseMission(
        user, (expert_job::ENUM_CLOSE_MISSION_TYPE)0);
    return 0;
