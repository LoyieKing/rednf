// ---------------------------------------------------------------------------
// check_dungeon_start(CDungeon const*, int&) —— ORIG T 0x0859f7fa
// 报告：docs/class_func_reports/CParty/check_dungeon_start.md
// 返回错误码并写出违规成员槽：血图 10/0x5f/0xf5；m_bloodDungeonType==2 图 0xf4~0xf8；
// 其余 0xf3（人数超限）；通过返回 0。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（242=242）：P183 去 user 指针缓存（逐点
// m_member[i].m_pUser 寻址）、CCharacBloodDungeon::GetEnterCount（include
// W9_EventSmall.h）、(signed char)limit/party_count、三路共享尾 return 0 ⇒
// 槽 -0x24/-0x20/-0x1c/-0x18/-0x14/-0x10/-0xc 与帧 0x34 全等。
int CParty::check_dungeon_start(CDungeon const* dungeon, int& param)
{
    if (dungeon->m_bloodDungeonType == 1)
    {   // 血之副本
        for (int i = 0; i < 4; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            // [b28d-solo-CPartyDepsD2] P183：ORIG 逐点重算 m_member[i].m_pUser
            // 寻址（0x042/0x06b/0x0ae/0x106 四段完整链），无 user 指针槽。
            // 入场费 vs 现金（thunk 0x080da27e=getCurCharacInvenR）
            const int fee = get_blood_dungeon_admission_fee(m_member[i].m_pUser);
            if (m_member[i].m_pUser->getCurCharacInvenR()->get_money() < fee)
            {
                param = i;
                return 10;
            }
            // 等级下限
            if (m_member[i].m_pUser->get_charac_level() < dungeon->get_min_level())
            {
                param = i;
                return 0x5f;
            }
            // 进入次数上限（ORIG 0x0859f8f5 movsbl get_limit_Inout_count；
            // 0x0859f92a call CCharacBloodDungeon::GetEnterCount）
            const int limit = (signed char)dungeon->get_limit_Inout_count();
            const int enterCount = reinterpret_cast<CCharacBloodDungeon*>(
                m_member[i].m_pUser->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1))
                ->GetEnterCount();
            if (limit <= enterCount)
                return 0xf5;
        }
    }
    else if (dungeon->m_bloodDungeonType == 2)
    {   // 特殊权限图（PowerWar/VillageAttack 等）
        // ORIG 0859f97a：get_index 结果作为实参传入 checkDugeonInoutTime
        if (!checkDugeonInoutTime(dungeon->get_index()))  // T 0x0859f6e2
            return 0xf4;
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
            return 0xf6;
        if (GlobalData::s_villageMonsterMgr->OnEvent())
            return 0xf7;

        const int members = get_member_count();
        if ((signed char)dungeon->get_limit_party_count() < members)
            return 0xf3;

        for (int i = 0; i < 4; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            if (m_member[i].m_pUser->get_charac_level() < dungeon->get_min_level())
            {
                param = i;
                return 0xf8;
            }
            // 入场券计数 = _Charac_info.m_ultimateInout[2]（+0xec2，CUserCharacInfo.h:441）
            // ORIG 0859fa8f `movzbl 0xec2(%eax); test %al,%al; jg` 无 ticket 槽
            // ⇒ 内联 (signed char) 表达式。
            if ((signed char)m_member[i].m_pUser->getCurCharacR()->m_ultimateInout[2] < 1 &&
                m_battleField.m_bloodState != 1)
            {
                param = i;
                return 0xf5;
            }
        }
    }
    else
    {
        // 普通副本：仅人数上限
        const int members = get_member_count();
        if ((signed char)dungeon->get_limit_party_count() < members)
            return 0xf3;
    }
    // ORIG 0x0859fb0a 共享尾 `mov $0x0,%eax`（血图/==2/else 三路汇入）
    return 0;
}