unsigned int Dispatcher_RenameCreature::check_error(CUser* user, MSG_BASE& msg)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return -1;
    }

    // ORIG 0x81cd295：msg 绑定(pMsg@-0xc)晚到 name 构造点才物化（技巧13 声明位置）。
    char* pMsg = (char*)&msg;
    std::string name(pMsg + 0x14);

    // ORIG 0x081cf7xx: !isUtf8StrInUnicodeRange(name.c_str()) → 错误 0x9f
    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(name.c_str()))
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlangName(name) ||
        G_CDataManager()->hasSpecialCharacter(pMsg + 0x14))
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlang(name))
    {
        return 0x9f;
    }

    return 0;
}