void DisPatcher_MoveItem::send(CUser* user, ParamBase& param)

{
    stMoveItemParam& p = static_cast<stMoveItemParam&>(param);
    PacketGuard guard;
    if (p.m_4 != 0x7fffffff)
    {
        guard.put_header(1, 0x13);
        // ORIG +0x4d `test; je` ⇒ 非零 4 字节回显块是 fall-through（技17 true 侧）。
        if (p.m_4 != 0)
        {
            guard.put_byte(0);
            guard.put_byte(p.m_4);
            guard.put_byte((char)p.m_8);
            guard.put_byte((char)p.m_10);
            guard.finalize(true);
            user->Send(guard);
        }
        else
        {
            guard.put_byte(1);
            guard.put_byte((char)p.m_8);
            guard.put_short((int)p.m_a);
            guard.put_int(p.m_c);
            guard.put_byte((char)p.m_10);
            guard.put_short(p.m_14);
            guard.finalize(true);
            user->Send(guard);
            ARAD::Singleton<AvatarRechargeServer>::Get()->SendDurability(
                user, (char)p.m_8, (short)p.m_a, (char)p.m_10, p.m_14);
        }
    }
}
