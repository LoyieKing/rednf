// ---------------------------------------------------------------------------
// make_blood_dungeon_info —— ORIG T 0x0859e0d2
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info.md
// ---------------------------------------------------------------------------
bool CParty::make_blood_dungeon_info(CUser* user, PacketGuard& packet)
{
    // [b28d-solo-CPartyDepsC2] P167 修复（ORIG 0x0859e0d2）：原 5 处调用点缺口
    // （GetDungeonList×2 / GetRepeatEvent×2 / GetCharacExpandData×1 /
    //  GetEnterCount×1 / GetWeeklyEnterCount×1）系局部缓存与裸字段视图所致，
    // 按 P183 去缓存 + 真实 getter 调用（ORIG 逐点重调）。
    if (m_hellWorldMap == 0)
    {
        // G_GameWorld W 0x080da3a7；GetWorldMapIndex T 0x086cebe0
        int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (worldIdx > 0)
            m_hellWorldMap =   // CWorldMapList::find_world_map T 0x083647a2
                G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
    }
    if (m_hellWorldMap == 0)
        return false;
    if (!m_hellWorldMap->hasDeathTower())      // hasDeathTower W 0x0822b520
        return false;

    packet.put_byte(get_member_count());

    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x0859e197 `test %al,%al; je ++i` = 正向包裹（P59 反形）；
        // P183：无 memberUser 局部，逐点重算 m_member[i].m_pUser。
        if (_checkValidUser(i))
        {
            // put_short W 0x080d9ea4；uid 取低 16 位
            packet.put_short(m_member[i].m_pUser->get_unique_id() & 0xffff);

            // 计数占位字节：get_index W 0x08110b4c / put_byte W 0x080cb920 /
            // put_byte(int&,int) W 0x08110b28；ORIG 0x0859e1e8 压的是
            // dungeonCount 槽（=0）而非立即数。
            int countIdx = packet.get_index();
            int dungeonCount = 0;
            packet.put_byte(dungeonCount);

            // CWorldMap::GetDungeonList W 0x085beda4 = &m_dungeonList(+0x10)：
            // ORIG 0x0859e1f7（begin 段）与 0x0859e344（每轮 end 段）各自独立调用。
            for (std::vector<int>::iterator it =
                     m_hellWorldMap->GetDungeonList().begin();
                 it != m_hellWorldMap->GetDungeonList().end(); ++it)
            {
                CDungeon* dungeon = G_CDataManager()->find_dungeon(*it);  // 0x0835f9f8
                // ORIG 0x0859e244/0x0859e256 两跳同落 ++it（正向包裹）。
                if (dungeon != 0 && (char)dungeon->m_bloodDungeonType == 1)
                {
                    packet.put_short(*it);
                    packet.put_int(get_blood_dungeon_admission_fee(
                        m_member[i].m_pUser));
                    // ORIG 0x0859e2c4/0x0859e302 两次独立 GetCharacExpandData(1) +
                    // 真实 getter（GetEnterCount 0x085bfe88 / Weekly 0x085bfe94）。
                    packet.put_int(reinterpret_cast<CCharacBloodDungeon*>(
                                       m_member[i].m_pUser->GetCharacExpandData(
                                           ENUM_CHARAC_EXPAND_TYPE_1))
                                       ->GetEnterCount());
                    packet.put_int(reinterpret_cast<CCharacBloodDungeon*>(
                                       m_member[i].m_pUser->GetCharacExpandData(
                                           ENUM_CHARAC_EXPAND_TYPE_1))
                                       ->GetWeeklyEnterCount());
                    ++dungeonCount;
                }
            }
            packet.put_byte(countIdx, dungeonCount);
        }
    }

    // 金币奖励：repeat 0x2e(SecondEvent) → 0x2d(FirstEvent) → Jackpot
    // IsEventing 经 vptr+0x34（槽13）虚调用，实参 0；ORIG 每次使用点独立重调
    // GetRepeatEvent（0x0859e3b1/0x0859e3de 与 0x0859e40c/0x0859e439）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x2e)->IsEventing(0))
    {
        packet.put_int(static_cast<CBloodDungeonRewardSecondEvent*>(
                           GlobalData::s_event_manager->GetRepeatEvent(0x2e))
                           ->GetGold());
    }
    else
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x2d)->IsEventing(0))
        {
            packet.put_int(static_cast<CBloodDungeonRewardFirstEvent*>(
                               GlobalData::s_event_manager->GetRepeatEvent(0x2d))
                               ->GetGold());
        }
        else
        {
            packet.put_int(G_CDataManager()->m_serverParameter.GetJackpot());
        }
    }
    return true;
}