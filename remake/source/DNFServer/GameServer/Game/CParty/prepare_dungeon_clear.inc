// ============================================================================
// CParty::prepare_dungeon_clear —— ORIG 0x85aa39a：清图前处理（追踪器、任务
// 认证、宠物经验）并视条件触发 ClearDungeon。
// ============================================================================
void CParty::prepare_dungeon_clear()
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85aa39a
    // b28d-solo-CParty2：ndiff 9→1 —— ①grid_clear 守卫改正向包裹（消 xor 段）；
    // ②循环内 `_checkValidUser` 改正负极性早退 goto L_next（P168a 裸形）复现 ORIG
    // `xor $1;test;jne`。余 1 行 = ORIG 体尾第二个 `jmp+nop` 岛（4.4 epilogue-threading
    // 恒合并，N121 封顶；多源 goto 尾标签串实测仍合并）。
    m_battleField.m_dungeonClearTracer.SetClearedDungeon(m_tournamentVictory != 0);
    // ORIG 0x85aa39a：bf 地址每次现取（mov 0x8 + add $0xb24），不建局部缓存槽。
    m_battleField.m_dungeonClearTracer.SetEndPoint(
        reinterpret_cast<CBattle_Field*>(&m_battleField)->check_end_point());
    // b28d-solo-CParty2：ORIG 0x85aa3xx `call check_grid_clear; test %al,%al; je <跳过主流程>`
    // （2 条、无 xor ⇒ 正极性 if 包裹，P214①/P59）。早返形 `if(!f()) return;` 实测产
    // `xor $1;test;jne`（+1 条且分支反）⇒ 改正向包裹，语义等价（void 函数）。
    if (reinterpret_cast<CBattle_Field*>(&m_battleField)->check_grid_clear())
    {
    for (int i = 0; i < 4; ++i)
    {
        // b28d-solo-CParty2：ORIG 0x85aa3f6 `call; xor $0x1,%eax; test %al,%al; jne <尾>`
        // = 负极性早退（bool callee 取反物化）。正向嵌套 `if (f()) { … }` 实测产
        // `test %al,%al; je`（缺 xor 段）⇒ 改写为 `if (!f()) goto L_next;` 复现。
        // [判据③/P223 多 continue 岛] ORIG 0x85aa557..0x85aa55c 为「body 尾 jmp T /
        //   G: nop;jmp T / S: nop / T: i++」三出口两岛形 —— 同层两个 `continue`
        //   时 4.4 才逐岛复现（旧单 goto 尾标签形被 jump-thread 成单岛，少 1 jmp）。
        if (!_checkValidUser(i))
            continue;
        if (m_battleField.m_bloodState == 1)     // +0xcd8
            continue;
        {
            // P183 反向（ORIG 0x85aa42e/457/46b/49a/4c8/4f6/524 实证）：ORIG 无
            // user/quest/mgr 具名缓存，每个使用点各自 9 条重算 m_member[i].m_pUser。
            m_member[i].m_pUser->getCurCharacQuestW()->set_authen_data(
                (QUEST_CONDITION)1,
                reinterpret_cast<CBattle_Field*>(&m_battleField)->get_cur_map(),
                0);
            if (0 < m_member[i].m_pUser->m_fatigueGain)
            {
                if (!reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                        ->getCurCharacInvenR()
                        ->GetCreatureMgrR()
                        ->IsGrowCreature_Equipped_Creature())
                {
                    reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                        ->getCurCharacInvenW()
                        ->GetCreatureMgrW()
                        ->GainExp(m_member[i].m_pUser->m_fatigueGain);
                }
            }
            m_member[i].m_pUser->m_fatigueGain = 0;
        }
    }

    if (reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->ClearCondition(
                (ENUM_DUNGEON_CLEAR_CONDITION_TYPE)1,
                reinterpret_cast<CBattle_Field*>(&m_battleField)
                    ->GetCurrentMapIndex()))
        ClearDungeon();
    else if (reinterpret_cast<CBattle_Field*>(&m_battleField)->check_end_point())
        ClearDungeon();
    }
}