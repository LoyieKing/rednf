bool CUser::SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET cmd, int param)
{
    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(0, cmd);
    if (cmd == (ENUM_NOTIPACKET)2)
    {
        pkt->put_byte(param);
        pkt->put_short(1);
        make_basic_info((char*)&packet, (char)param);
    }
    else if (cmd == (ENUM_NOTIPACKET)0x25)
    {
        pkt->put_byte(get_charac_level());
        pkt->put_int(get_charac_exp());
        pkt->put_int(get_charac_party_bonus_exp());
        pkt->put_int(get_charac_member_bonus_exp());
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)0));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)1));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)2));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)3));
        pkt->put_int(getPVPResultR()->m_winPoint);
        pkt->put_int(get_charac_fatigue_buf_bonus_exp());
        pkt->put_int(get_charac_seria_buf_bonus_exp());
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x57);
        if (ev != 0 && ev->IsEventing((CUser*)0))
        {
            pkt->put_int(getWeekendBonusExp());
            resetWeekendBonusExp();
        }
        else
        {
            pkt->put_int(0);
        }
        pkt->put_int(getPremiumBonusExp());
        resetPremiumBonusExp();
    }
    else
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x76d1, "User %s - NotifyID = %d", getCurCharacName(), cmd);
        return false;   // b28c-L5 SIGFIX：ORIG 串 bool；失败臂 `mov $0x0,%ebx` 汇合尾
    }
    pkt->finalize(true);
    SendPacket(target, packet);
    return true;        // ORIG 0x0867bd4e `call SendPacket; mov $0x1,%ebx; …mov %ebx,%eax`
