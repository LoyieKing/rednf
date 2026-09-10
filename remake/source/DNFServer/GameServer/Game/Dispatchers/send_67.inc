void Dispatcher_Buy_Charac_Status_Using_QP::send(CUser* user, ParamBase& param)
{
    // [b23S3C] ORIG 0x081d65c2：param 槽(-0xc)回读判定、if/else 各持独立 PacketGuard
    // （-0x18/-0x24 双对象+双 EH landing，同 Skill_Command_All_Default::send 孪生形）；
    // 字段直访（m_8/m_4/m_6）替代裸偏移值槽 hoist（P48/P0 律）。
    STQuestShopBuyParam& out = static_cast<STQuestShopBuyParam&>(param);
    if (out.m_8 == 0)
    {
        PacketGuard guard;
        guard.put_header(1, 399);
        guard.put_byte(1);
        guard.put_byte((int)out.m_4);
        guard.put_short((int)out.m_6);
        guard.finalize(true);
        user->Send(guard);
        user->sendCharacQp();
    }
    else
    {
        PacketGuard guard;
        guard.put_header(1, 399);
        guard.put_byte(0);
        guard.put_short(out.m_8);
        guard.finalize(true);
        user->Send(guard);
    }
