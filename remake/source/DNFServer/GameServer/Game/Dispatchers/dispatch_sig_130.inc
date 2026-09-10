int Dispatcher_OperateRidableObject::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08226d34：仅检查 get_state()==5（无 user==0 防御分支）。
    if (user->get_state() != 5)
    {
        return 0;
    }
    char operateType[2];
    operateType[0] = 0;
    operateType[1] = 0;
    int ridableIdx = 0;
    if (!packet.get_byte(operateType[0]))
    {
        return LineFunc(0xe2a4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(ridableIdx))
    {
        return LineFunc(0xe2a7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        return 0;
    }
    // ORIG 0x08226ed4：SetRidableObjectState 实参 -0xc 槽为枚举型，调用点无显式 cast 转换；
    // int 槽+调用点 cast 会翻转 4.4 spill 装载序（-0xc 先于 -0x18）。
    eRidableObjectState type = (eRidableObjectState)(int)(signed char)operateType[0];
    if (type == 2)
    {
        user->m_historyLog.StartRidable((cUserHistoryLog::eRidableType)0, ridableIdx);
    }
    else if (type == 1 || type == 3)
    {
        user->m_historyLog.EndRidable((cUserHistoryLog::eRidableType)0, ridableIdx);
    }
    // ORIG xor $1 形：!Set(...)（bool 返回，广播待落）→ Error 早退（mov $0,jmp），成功走 OK 尾
    if (!party->SetRidableObjectState(ridableIdx, type))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13d, 0x13);
        return 0;
    }
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x13d);
    return 0;
