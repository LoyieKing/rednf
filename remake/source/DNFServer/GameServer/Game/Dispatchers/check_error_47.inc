unsigned int Dispatcher_GuildCargoPushItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    // [G1Frest 2026-09-10] ORIG 0x081e5xxx：`mov 0x10(%ebp),%eax; mov %eax,-0x1c(%ebp)`
    //   —— 0x10(%ebp) 是**第 2 参 msg**（0xc=user / 0x10=msg / 0x14=param）；其后
    //   全部字段读自该局部（+0xd/+0xe/+0x10）。旧码把视图绑在 param ⇒ **参数错位**（真 bug）。
    //   另：itemSpace/slot/reqAddInfo 三个具名局部为发明 —— ORIG 各使用点重读
    //   （`movzwl 0xe(%eax),%eax` / `mov 0x10(%eax),%eax`），P0/P48。
    (void)param;

    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return -1;
    if (user->get_charac_guildkey() == 0)
        return 100;
    if (user->IsExistGuildAgit() != 1)
        return 0xb5;

    // msg +0x0d/0x0e/0x10：冻结头仅 m_4+pad 占位，按 §8.3 用 TU 局部视图具名
    // （偏移证据=本函数 ORIG 读出，宽度对照读指令）。
    struct __attribute__((packed)) PushParamView
    {
        int m_4;                 // +0x04
        char m_pad08[4];         // +0x08..+0x0c
        unsigned char m_itemSpace;   // +0x0d
        unsigned short m_slot;       // +0x0e
        int m_reqAddInfo;            // +0x10
    };
    const PushParamView& pv =
        *reinterpret_cast<const PushParamView*>(&msg);
    const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)pv.m_itemSpace);
    const CInventory* inven = user->getCurCharacInvenR();
    const Inven_Item slotItem = inven->GetInvenSlot(invenType, (int)pv.m_slot);

    const int err =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)8);
    if (err != 0)
        return err;
    if (user->CheckItemLock(invenType, (int)pv.m_slot))
        return 0xd5;
    if (pv.m_reqAddInfo == 0 || slotItem.m_addInfo != pv.m_reqAddInfo)
        return 0xc5;
    if (G_CDataManager()->find_item(slotItem.m_addInfo) == 0)
        return 0xc5;
    return 0;
