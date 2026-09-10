void FunctionGuildMember::MakePacketGuildMembersInfo(PacketGuard& packet,
                                                     int count,
                                                     ST_Guild_Mem_Info* members)
{
    packet.put_byte(count);
    int nameLen = 0;
    int infoLen = 0;
    const long long curSec = GlobalData::s_systemTime_.getCurSec();
    while (count-- != 0)
    {
        nameLen = (int)strlen(members->f.m_name);
        packet.put_int(nameLen);
        packet.put_str(&members->f.m_name[0], nameLen);
        infoLen = (int)strlen(members->f.m_guildMemDBInfo);
        packet.put_int(infoLen);
        packet.put_str(&members->f.m_guildMemDBInfo[0], infoLen);
        packet.put_short((int)members->f.m_level);
        packet.put_byte((int)members->f.m_job);
        packet.put_byte((int)members->f.m_growthType);
        packet.put_byte((int)(unsigned char)members->f.m_channel);
        packet.put_byte((int)members->f.m_sex);
        packet.put_byte((int)members->f.m_blackUser);
        packet.put_byte((int)(unsigned char)members->f.m_grade);
        // ORIG +0x3b（f.m_pad3b，P1 单 disp）：0 或 curSec<secs → 0，否则差
        if (members->f.m_pad3b == 0 ||
            curSec < members->f.m_pad3b)
            packet.put_int(0);
        else
            packet.put_int((int)(curSec - members->f.m_pad3b));
        members = (members + 1);
    }
