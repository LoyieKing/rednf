int Dispatcher_ComboSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stComboSkill& out = static_cast<stComboSkill&>(param);
    MSG_COMBO_SKILL& req = reinterpret_cast<MSG_COMBO_SKILL&>(msg);
    out.m_result = check_error(user, msg);
    if (out.m_result > 0)
    {
        return 0;
    }
    if (out.m_result < 0)
    {
        return LineFunc(0x33f,
                        __PRETTY_FUNCTION__,
                        out.m_result, 0);
    }
    SkillSlot* skillW =
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
    skillW->setComboSkillInfo((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d,
                              req.m_comboMap);
    skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
    skillW->verifyComboSkillTree((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d);
    return 0;
