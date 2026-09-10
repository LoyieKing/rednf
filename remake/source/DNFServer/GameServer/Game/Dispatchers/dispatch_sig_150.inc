int Dispatcher_CompoundItemByExpertJob::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821eb66: 角色非空门
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xd73c, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x04d: 有专家职业门
    if (user->GetCurCharacExpertJobType() == 0)
    {
        return LineFunc(0xd73f, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x0ab: state==3 且交易中直接放弃
    if (user->get_state() != 3)
        return 0;
    if (user->CheckInTrade())
        return 0;

    // +0x0dd: isRestricted(user,1,0x13) → error 0xf2,0xd1
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0x13))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf2), 0xd1);
        return 0;
    }

    // +0x122: Secu Check(0x1c) → error 0xf2,ret
    int secuCheck = GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x1c));
    if (secuCheck != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf2),
                                 static_cast<unsigned char>(secuCheck));
        return 0;
    }

    // +0x167: 读 ulong itemIdx / ushort count
    unsigned long itemIdx = 0;
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xd762, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned short count = 0;
    if (!packet.get_short(count))
    {
        return LineFunc(0xd764, __PRETTY_FUNCTION__, 0, 0);
    }
    if (count == 0)
    {
        return LineFunc(0xd76a, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x241: compound_item(itemIdx, true, count, 0xf2)
    int compoundResult = user->compound_item(static_cast<int>(itemIdx), true, count, 0xf2);
    if (compoundResult < 1)
    {
        // +0x2f3: 失败且专家职业 2/4 → 清对应行动组
        if (user != 0 && compoundResult == 0)
        {
            int expertJobType = user->GetCurCharacExpertJobType();
            if (expertJobType == 2)
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    user, static_cast<APSystem::_ActionGroupIndex>(0x1d), 0, static_cast<ENUM_PACKETCLASS>(0));
            }
            else if (expertJobType == 4)
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    user, static_cast<APSystem::_ActionGroupIndex>(0x1e), 0, static_cast<ENUM_PACKETCLASS>(0));
            }
        }
        return 0;
    }

    // +0x259: 成功 → 0xf2 响应包（byte 0, byte result）
    PacketGuard pg;
    pg.put_header(1, 0xf2);
    pg.put_byte(0);
    pg.put_byte(compoundResult);
    pg.finalize(true);
    user->Send(pg);
    return 0;
