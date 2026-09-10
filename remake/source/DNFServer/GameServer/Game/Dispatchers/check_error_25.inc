unsigned int Dispatcher_HatchCreature::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->getCurCharacR() == 0)
    {
        return -1;
    }

    // [b28d-solo-S206] P0/N98：ORIG 0x081cd5cc 在 secu 检查**之前**物化 msg 指针槽
    //   （-0x10，首次使用序决定槽深），secuErr 落 -0xc（后声明→浅槽）。
    char* pMsg = (char*)&msg;
    int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x14);
    if (secuErr != 0)
    {
        return secuErr;
    }

    // [b28d-solo-S206] P200 + disp 折叠：ORIG `movzwl 0xe(%eax),%eax; movswl %ax,%ebx`
    //   （slot 表达式直传跨 call 保活）+ `movzbl 0xd(%eax),%eax`；具名成员视图
    //   令 4.4 折叠 disp（裸 pMsg+0xe 恒 add+mov）。
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)
                reinterpret_cast<const MSG_HATCH_CREATURE*>(pMsg)->m_itemSpace),
            (int)reinterpret_cast<const MSG_HATCH_CREATURE*>(pMsg)->m_slotIdx))
    {
        return 0xd5;
    }

    return 0;
