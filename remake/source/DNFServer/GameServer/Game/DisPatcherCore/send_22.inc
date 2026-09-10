void DisPatcher_UseEquip::send(CUser* user, ParamBase& param)

{
    stOnUseEquipParam& out = static_cast<stOnUseEquipParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x33);
    if (out.m_errNo != 0)
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
    }
    else
    {
        guard.put_byte(1);
        guard.put_byte(static_cast<char>(out.m_8));
    }
    guard.finalize(true);
    user->Send(guard);
}
