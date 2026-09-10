void Dispatcher_Clear_Used_QP::send(CUser* user, ParamBase& param)
{
    STQuestShopParam& req = static_cast<STQuestShopParam&>(param);   // -0xc（ORIG 缓存 param 地址）
    if (req.m_4 == 0)
    {
        PacketGuard guard;                                            // -0x18（仅 err==0 分支构造）
        guard.put_header(1, 400);
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
        guard.finalize(true);
        user->Send(guard);
        user->sendCharacQp();
        user->sendCharacQuestPiece();
    }
    else
    {
        // ORIG 0x081d69ca：err!=0 时发送独立的第二个包（新 PacketGuard + put_header(1,400)），
        // 原实现复用 guard 导致第二包缺失 header（真语义差，已修）。
        PacketGuard guard2;                                           // -0x24
        guard2.put_header(1, 400);
        guard2.put_byte(0);
        guard2.put_short(req.m_4);
        guard2.finalize(true);
        user->Send(guard2);
    }
