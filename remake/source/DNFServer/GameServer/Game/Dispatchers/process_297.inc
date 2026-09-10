int Dispatcher_ComboSkillExtensionQuickSlotReset::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stComboSkillExtensionQuickSlotReset& out =
        static_cast<stComboSkillExtensionQuickSlotReset&>(param);
    MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET& req =
        reinterpret_cast<MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET&>(msg);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x384,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    SkillSlot* skillSlot = (SkillSlot*)user->getCurCharacSkillW();
    skillSlot->pullComboSkillExtensionQuickSlot((ENUM_SKILL_TREE_KIND)(char)req.m_0d);
    user->send_skill_info();
    return 0;
