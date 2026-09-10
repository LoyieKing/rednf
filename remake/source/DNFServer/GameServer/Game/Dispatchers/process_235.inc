int Dispatcher_ChangeAnotherSkillTree::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d1fca
    stChangeAnotherSkillTree& p = static_cast<stChangeAnotherSkillTree&>(param);
    p.m_result = check_error(user, msg);
    if (p.m_result >= 1)
    {
        return 0;
    }
    if (p.m_result < 0)
    {
        return LineFunc(0x2ecf,
                        __PRETTY_FUNCTION__,
                        p.m_result, 0);
    }
    MSG_CHANGE_ANOTHER_SKILL_TREE& req = reinterpret_cast<MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    // b28d-D0: ORIG 0x081d2009 `movzbl 0xd(%eax),%eax; test %al,%al` 无 treeIdx 槽
    //   （P0 表达式直读）；具名 uchar treeIdx 落槽 -0x9 并使 req 深移 4（-0x14 vs ORIG -0x10）。
    if ((unsigned char)req.m_0d == 0)
    {
        user->SetCurCharacSkillTreeIndex((char)1);
        p.m_skillTreeIndex = 1;
    }
    else if ((unsigned char)req.m_0d == 1)
    {
        user->SetCurCharacSkillTreeIndex((char)0);
        p.m_skillTreeIndex = 0;
    }
    return 0;
