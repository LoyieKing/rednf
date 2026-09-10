void CUser::send_skill_info()
{
    // ORIG 0x0866c46a: send_skill_info.md
    PacketGuard packet;
    packet.put_header(0, 0x13);

    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0));
    int countIdx0 = packet.get_index();
    int totalCount0 = 0;
    packet.put_byte(0);

    for (int i = 0; i < 0xcc; ++i)
    {
        if (reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot[i].m_index != 0)
        {
            packet.put_byte(i);
            packet.put_byte(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot[i].m_index);
            packet.put_byte((signed char)reinterpret_cast<const SkillSlot*>(
                getCurCharacSkillR())->m_skillSlot[i].m_level);
            totalCount0++;
            STSkillCommandData cmd0;
            if (findSkillCommandData(reinterpret_cast<const SkillSlot*>(
                    getCurCharacSkillR())->m_skillSlot[i].m_index, cmd0))
            {
                packet.put_byte((signed char)cmd0.m_count);
                for (int k = 0; k < (signed char)cmd0.m_count; ++k)
                {
                    packet.put_byte((signed char)cmd0.m_keyBytes[k]);
                }
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }

    STGuildDBInfoPacketView* guild =
        reinterpret_cast<STGuildDBInfoPacketView*>(GetGuildDBInfo());
    int guildCount = (unsigned char)guild->m_skillSection.m_skillLearnCnt;
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        guildCount = 0;
    }

    for (int i = 0; i < guildCount; ++i)
    {
        if (getGuildMemberGrade() && getGuildMemberGrade() &&
            guild->m_skillSection.m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skillSection.m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skillSection.m_skills[i].m_field4);
            totalCount0++;
            packet.put_byte(0);
        }
    }
    packet.put_byte(countIdx0, totalCount0);
    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1));
    countIdx0 = packet.get_index();
    totalCount0 = 0;
    packet.put_byte(0);

    for (int i = 0; i < 0xcc; ++i)
    {
        if (reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot2[i].m_index != 0)
        {
            packet.put_byte(i);
            packet.put_byte(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot2[i].m_index);
            packet.put_byte((signed char)reinterpret_cast<const SkillSlot*>(
                getCurCharacSkillR())->m_skillSlot2[i].m_level);
            totalCount0++;
            STSkillCommandData cmd1;
            if (findSkillCommandData(reinterpret_cast<const SkillSlot*>(
                    getCurCharacSkillR())->m_skillSlot2[i].m_index, cmd1))
            {
                packet.put_byte((signed char)cmd1.m_count);
                for (int k = 0; k < (signed char)cmd1.m_count; ++k)
                {
                    packet.put_byte((signed char)cmd1.m_keyBytes[k]);
                }
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }

    for (int i = 0; i < guildCount; ++i)
    {
        if (getGuildMemberGrade() && getGuildMemberGrade() &&
            guild->m_skillSection.m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skillSection.m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skillSection.m_skills[i].m_field4);
            totalCount0++;
            packet.put_byte(0);
        }
    }
    packet.put_byte(countIdx0, totalCount0);

    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2));
    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3));
    packet.finalize(true);
    Send(packet);
    sendComboSkillList();
