void Dispatcher_GrowthWeaponEventChangeInfinity::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e80be T：param 指针→-0xc 槽；p->m_4==0 → SendCmdOkPacket(0x1f9)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)p->m_4)（字段二次单级读）。
    stGrowthEquipment* p = static_cast<stGrowthEquipment*>(&param);
    if (p->m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1f9);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f9, (unsigned char)p->m_4);
}