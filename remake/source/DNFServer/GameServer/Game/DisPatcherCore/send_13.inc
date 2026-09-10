void DisPatcher_RepairEquip::send(CUser* user, ParamBase& param)

{
    stRepairEquipParam& p = static_cast<stRepairEquipParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x19);
    // ORIG +0x3c：`test; je +0x6b` ⇒ 非零块是 fall-through 块（技17 写在 true 侧）。
    if (p.m_errNo != 0)
    {
        guard.put_byte(0);
        guard.put_byte(p.m_errNo);
    }
    else
    {
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
        guard.put_byte((char)p.m_8);
        guard.put_short((unsigned int)p.m_a);
        guard.put_short((unsigned int)p.m_c);
    }
    guard.finalize(true);
    user->Send(guard);
}
