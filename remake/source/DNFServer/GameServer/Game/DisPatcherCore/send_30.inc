void Dispather_RegenerationRandomOption::send(CUser* user, ParamBase& param)
{
    stRegenROIResult& out = static_cast<stRegenROIResult&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x197);
    if (out.m_errNo == 0)
    {
        guard.put_byte(1);
        guard.put_short(out.m_insertedSlot);
    }
    else
    {
        guard.put_byte(0);
        guard.put_int(out.m_errNo);
    }
    guard.finalize(true);
    user->Send(guard);
