int Dispatcher_UpgradeItemSeparate::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b7, (unsigned char)err);
        return -1;
    }
    // b28d-D0: ORIG 0x081cc152 `mov 0x14(%ebp),%eax; mov %eax,-0x10` = param 死槽
    //   （P198 声明+赋值两句保地址 store）；0x081cc158 `mov 0x10(%ebp),%eax;
    //   mov %eax,-0xc` = msg 具名指针槽（4.4 直传 &msg 会折叠该槽）。
    ParamBase* dp;
    dp = &param;
    (void)dp;
    MSG_ITEM_UPGRADE* pm;
    pm = reinterpret_cast<MSG_ITEM_UPGRADE*>(&msg);
    // ORIG 0x081cc15e 链式直取 m_itemUpgradeSeparate（无 system 具名槽）。
    ((WongWork::IItemUpgrade*)G_CDataManager()->m_itemUpgradeSeparate)
        ->item_upgrade_system(user, pm, 0x1b7);
    return 0;
