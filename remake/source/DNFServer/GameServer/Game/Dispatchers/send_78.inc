void Dispatcher_ComboSkillExtensionQuickSlotReset::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825f60a T：param 指针→-0xc 槽；p->m_4==0 → SendCmdOkPacket(0x1ff)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)p->m_4)（字段二次单级读）。
    stComboSkillExtensionQuickSlotReset* p = static_cast<stComboSkillExtensionQuickSlotReset*>(&param);
    if (p->m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1ff);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1ff, (unsigned char)p->m_4);
}