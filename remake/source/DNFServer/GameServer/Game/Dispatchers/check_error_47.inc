unsigned int Dispatcher_GuildCargoPushItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    (void)msg;  // ORIG 不读 MSG_BASE，字段全部来自 param

    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return -1;
    if (user->get_charac_guildkey() == 0)
        return 100;
    if (user->IsExistGuildAgit() != 1)
        return 0xb5;

    // param +0x0d/0x0e/0x10：stGuildCargoPushItemParam 冻结头仅 m_4+pad 占位，
    // 按 §8.3 用 TU 局部视图具名（偏移证据=本函数 ORIG 读出，宽度对照读指令）。
    struct __attribute__((packed)) PushParamView
    {
        int m_4;                 // +0x04
        char m_pad08[4];         // +0x08..+0x0c
        unsigned char m_itemSpace;   // +0x0d
        unsigned short m_slot;       // +0x0e
        int m_reqAddInfo;            // +0x10
    };
    const PushParamView& pv =
        *reinterpret_cast<const PushParamView*>(&param);
    const unsigned char itemSpace = pv.m_itemSpace;
    const unsigned short slot = pv.m_slot;
    const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)itemSpace);
    const CInventory* inven = user->getCurCharacInvenR();
    const Inven_Item slotItem = inven->GetInvenSlot(invenType, (int)slot);

    const int err =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)8);
    if (err != 0)
        return err;
    if (user->CheckItemLock(invenType, (int)slot))
        return 0xd5;
    const int reqAddInfo = pv.m_reqAddInfo;
    if (reqAddInfo == 0 || slotItem.m_addInfo != reqAddInfo)
        return 0xc5;
    if (G_CDataManager()->find_item(slotItem.m_addInfo) == 0)
        return 0xc5;
    return 0;
