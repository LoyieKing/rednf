int CParty::check_dungeon_start(CDungeon const* dungeon)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x859fb16
    // ORIG 0x859fb1f/0x859fbd0：两判各**直接 movzbl 0x89f 字段读 + cmp $N,%al**
    // （无 getter 调用、不复用局部，P48 逐臂重读）。
    if (dungeon->m_bloodDungeonType == 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            // b28d-solo-CParty：ORIG 0x859fb53/0x859fb7c 两次独立完整寻址
            //（P0/P183：m_pUser 无缓存槽，勿具名 CUser* u）。
            int fee = get_blood_dungeon_admission_fee(m_member[i].m_pUser);
            if (m_member[i].m_pUser->getCurCharacInvenR()->get_money() < fee)
                return 10;
        }
    }
    else if (dungeon->m_bloodDungeonType == 2)
    {
        if (checkDugeonInoutTime(dungeon->get_index()) != 1)
            return 0xf4;
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
            return 0xf6;
        if (GlobalData::s_villageMonsterMgr->OnEvent())
            return 0xf7;
        int memberCount = get_member_count();
        if (dungeon->get_limit_party_count() < memberCount)
            return 0xf3;
        for (int i = 0; i <= 3; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            // _Charac_info(CUser.h 冻结)该区为 m_dataEB9/m_dataEC0 占位；
            // 语义名在 CharacInfoFields 镜像（CUserCharacInfo.h，同偏移
            // pack(1)，本次已按 movzbl 证据改 unsigned char）。
            // ORIG 0859fa8f `movzbl 0xec2(%eax)` = m_ultimateInout[2]
            //（unsigned char 宽度修正，原 (char*) 为 movsbl 异形）
            // b28d-solo-CParty：ORIG 0x859fcaf 无 charac 指针槽（call 结果 %eax
            // 直索引，P0 内联）⇒ 去掉具名局部。
            // b28d-solo-CParty：ORIG 0x859fcbb `test %al,%al; jg` 为**有符号**判定
            // （P196 符号性判据）⇒ 字段 unsigned char 按 (signed char) 读，
            // 无符号读会折成 `jne`。
            if ((signed char)m_member[i].m_pUser->getCurCharacR()->m_ultimateInout[2] < 1 &&
                m_battleField.m_bloodState != 1)
                return 0xf5;
        }
    }
    return 0;
}