// 0x0866c46a:技能信息包(send_skill_info.md)。
// 用具名语义字段访问：玩家技能经 SkillSlot（m_skillSlot 树0 / m_skillSlot2 树1，
// _Mastered_skill{索引,等级}），公会技能经 STGuildDBInfoOnly（m_skillLearnCnt 计数、
// m_skills[16] 5 字节条目 STGuildSkill{技能ID,等级}）。背包命令段经
// findSkillCommandData(索引) 取 STSkillCommandData{count, m_key0..}。
// ORIG 语义要点：公会槽位号 = i + 0x8a；公会技能判定 = 双 grade 非零 且 技能ID > 0xc7。
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

    STGuildDBInfoOnly* guild =
        static_cast<STGuildDBInfoOnly*>(GetGuildDBInfo());
    int guildCount = (unsigned char)guild->m_skillLearnCnt;
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        guildCount = 0;
    }

    for (int i = 0; i < guildCount; ++i)
    {
        if (getGuildMemberGrade() && getGuildMemberGrade() &&
            guild->m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skills[i].m_field4);
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
            guild->m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skills[i].m_field4);
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
}