int DisPatcher_UseSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseSkillParam& out = static_cast<stUseSkillParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG 0x081cadaa+0x35
    // `mov %eax,%edx; mov -0x24(%ebp),%eax; mov %edx,0x4(%eax)`（RHS-first+edx 中转）
    // 由「unsigned 返回 + 无具名 err/dst 局部」复现。
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1ff2,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }
    // ORIG 0x081c...：msg→-0x20 spill 位于 LineFunc(0x1ff2) 岛之后（技巧13）
    MSG_USE_SKILL& req = *reinterpret_cast<MSG_USE_SKILL*>(&msg);

    SKILL_USING_VALIDATE_INFO info(user->get_charac_job(), req.m_slot,
                                   user->getCurCharFirstGrowType(),
                                   user->getCurCharSecondGrowType());
    if (!CHackLog_InvalidUsingSkill::IsValidUsingSkill(&info))
    {
        user->getHackAnalyzer()->addServerHackCnt(user,
                                                  (WongWork::ENUM_HACKTYPE)CHackLog_InvalidUsingSkill::GetHackType(),
                                                  1, (unsigned int)req.m_slot, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party != 0)
    {
        party->use_skill(user, (int)req.m_slot, (int)req.m_targetId);
    }
    return 0;
