unsigned int Dispatcher_RenameCreature::check_error(CUser* user, MSG_BASE& msg)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return -1;
    }

    // ORIG 0x81cd295：msg 绑定(pMsg@-0xc)晚到 name 构造点才物化（技巧13 声明位置）。
    const MSG_RENAME_CREATURE& req = *reinterpret_cast<const MSG_RENAME_CREATURE*>(&msg);
    std::string name(req.m_name);

    // ORIG 0x081cf7xx: !isUtf8StrInUnicodeRange(name.c_str()) → 错误 0x9f
    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(name.c_str()))
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlangName(name) ||
        G_CDataManager()->hasSpecialCharacter(req.m_name))
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlang(name))
    {
        return 0x9f;
    }

    return 0;
}