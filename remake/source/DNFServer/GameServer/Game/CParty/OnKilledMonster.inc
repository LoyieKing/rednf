// ============================================================================
// CParty::OnKilledMonster —— ORIG 0x85b5a4c：怪物击杀反外挂核查链
// （伤害占比/跨图伤害/秒杀检测）后按 check_grid_clear 收图。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x44/0x74 + 槽；325 vs 331
    // [G7-sub 2026-09-10 复核·帧差归属] ORIG 0x085b5a4c `sub $0x44` vs 我方 `sub $0x64`
    // （+0x20）。逐槽核对：差量全部来自 **.cpp 段（非本 .inc 写域）** 的具名量
    //   · `WongWork::CHackAnalyzer* hack`（ORIG 0x085b5b0a `call getHackAnalyzer` 后
    //     直用，无槽；我方落 -0x34）—— 应改链式 `…->getHackAnalyzer()->addServerHackCnt(…)`
    //   · `member/memberParty/userParty/mpDungeon/upDungeon`（ORIG 0x085b5c30 段各为
    //     表达式重读；我方 -0x28/-0x24/-0x20 三个槽）
    //   · `MapInfo* mapInfo`（ORIG 直用返回值）
    //   本 .inc 仅到 `IsCollectDungeonStatistic();`（ORIG 0x085b5a51 首条 call ✓ 一致），
    //   故本轮在此无可改项；上述具名量请由 CParty.cpp 属主按 P0/P183 处理。
    // p167：CALLSITE CLEAR（ORIG/我方各 34 call）。
void CParty::OnKilledMonster(CUser* user, int monsterIdx, map_monster monster,
                             MSG_MONSTER_DIE const& dieInfo)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS ActiveStaticInfo::getHpMax（1 次）。@0x85b5a4c
    IsCollectDungeonStatistic();
    // P183：ORIG 每个使用点各自 `mov 0x8(%ebp),%eax; add $0xb24,%eax` 重取址
    //（无引用缓存槽）；skip 布尔局部亦无槽 ⇒ 改短路表达式直用。
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    if (!bf.IsClearDungeon() && dieInfo.m_fieldA22 == 0)
    {
        // 实参序按 ORIG 0x85b5a91..0x85b5ac4 取证：
        //   arg2 = (unsigned short)dieInfo->m_fieldF  [movzwl 0xf(%eax),%eax]
        //   arg3 = (unsigned short)monsterIdx         [mov 0x10(%ebp),%eax; movzwl %ax,%edx]
        //   arg4 = dieInfo->m_killValue2              [mov 0xa1a(%eax),%eax]
        // 被调方 addMonsterDamage(nMonsterIndex, monsterId, damage) 以 **monsterId 为
        // map key**（CTraceMobDieHack.cpp:135 m_map1.insert(make_pair(monsterId, log))）
        // ⇒ ORIG 语义 = 按"怪物序号"键控、把 killer uid 记入 m_monsterIndex；
        // 旧码把两实参互换（按 killer uid 键控）为转写错误，已正。
        // ORIG 085b5aa7 段 `call addMonsterDamage; cmp $0x2,%eax; setg %al`（无结果槽）
        // ORIG 085b5ac9 `cmp $0x2,%eax; setg %al; test %al,%al; je`：4.1 bool 物化形，
        // 4.4 复现钥匙 = `while(cond){…;break;}`（P225；裸 if/&& 恒产 jle 直跳）。
        // ORIG 085b5ad3：param_1 != 0 的判空在 addMonsterDamage **之后**（独立守卫，
        // 与 `je` 同落点 0x85b5b13）⇒ 未通过时不再调用 addMonsterDamage 之外的东西；
        // 旧码把 `user != 0` 提前到首条件为语义错误（addMonsterDamage 有副作用）。
        while (2 < m_traceMobDieHack
                        .addMonsterDamage((unsigned short)dieInfo.m_fieldF,
                                           (unsigned short)monsterIdx,
                                           (int)dieInfo.m_killValue2))
        {
            if (user != 0)
            {
                // [P0] ORIG 0x085b5abx：getHackAnalyzer 结果直传（无 hack 槽）。
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xc9, 1, 0, 0);
            }
            break;
        }
    }

    bool doCrossCheck = true;
    int dungIdx = bf.get_dungeon_index();
    if (dungIdx == 0x3e || dungIdx == 0x3f || dungIdx == 0x40)
        doCrossCheck = false;

    if (doCrossCheck && dieInfo.m_fieldA22 == 0 &&
        monster.m_roleType < 4 &&
        // ORIG 085b5b79..085b5b8b：先 call getHpMax（结果入 edx）再读 0xa1a，
        // `cmp %eax,%edx; seta %al; test %al,%al; je` ⇒ 原码书写序 = hpMax > killValue2。
        (unsigned int)monster.m_active.getHpMax() >           // +0x38 子对象
            (unsigned int)dieInfo.m_killValue2 &&
        monsterIdx != 10 && monsterIdx != 0xc351 && monsterIdx != 0x40a)
    {
        for (int i = 0; i < 4; ++i)                     // -0x20
        {
            // ORIG 085b5bd3 `call checkValidUser; xor $0x1,%eax; test %al,%al; jne 085b5e1b`
            // + 尾岛（0x3cf nop / 0x3d0 jmp）⇒ 原码是否定早退守卫（P59/P223），非正向 if。
            if (!checkValidUser(i))
                continue;
            CUser* member = get_user(i);
                // ORIG 0x085b5c07 `call GetParty; mov %eax,-0x10(%ebp)` + 0x085b5c30/3a
                // 两处 `mov -0x10/-0x14(%ebp),%eax; mov 0xcac(%eax),%eax` ⇒ memberParty/
                // userParty **各只调一次**并落槽（-0x14 userParty / -0x10 memberParty）。
                void* memberParty = member->GetParty();
                void* userParty = user->GetParty();
                if (memberParty != 0 && userParty != 0 && member != 0)
                {
                    // [P183] ORIG 085b5c30/0x3a 每次使用都重发 `mov -0x14/-0x10(%ebp),%eax;
                    // mov 0xcac(%eax),%eax`（**无 mpDungeon/upDungeon 具名槽**）⇒ 表达式直用。
                    if (reinterpret_cast<CParty*>(memberParty)->m_battleField.m_pDungeon == 0 ||
                        reinterpret_cast<CParty*>(userParty)->m_battleField.m_pDungeon == 0)
                    {
                        // ORIG 085b5c51 `cmp 0xc(%ebp),%eax; je 085b5c92`：不相等直落 member 臂
                        // ⇒ 原码 `if (member != user) {…0x71} else {…0x70}`（旧码臂序相反）。
                        if (member != user)
                        {
                            member->getHackAnalyzer()->addServerHackCnt(
                                member, (WongWork::ENUM_HACKTYPE)0x71, 1, 0, 0);
                        }
                        else
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x70, 1, 0, 0);
                        }
                    }
                    else
                    {
                        // ORIG 085b5cd1..085b5d0d：两次 `call get_index; cmp $0x2afe` +
                        // `mov $1/jmp/mov $0/test/je` 岛（**无 bool 具名槽**）⇒ 内联条件。
                        if (reinterpret_cast<CParty*>(memberParty)->m_battleField.m_pDungeon->get_index() == 0x2afe ||
                            reinterpret_cast<CParty*>(userParty)->m_battleField.m_pDungeon->get_index() == 0x2afe)
                        {
                            // ORIG 085b5d15/085b5d1b：`cmp 0xc(%ebp),%eax; je 085b5d5c` 同理。
                            if (member != user)
                            {
                                member->getHackAnalyzer()->addServerHackCnt(
                                    member, (WongWork::ENUM_HACKTYPE)0x71, 1, 2, 0);
                            }
                            else
                            {
                                user->getHackAnalyzer()->addServerHackCnt(
                                    user, (WongWork::ENUM_HACKTYPE)0x70, 1, 2, 0);
                            }
                        }
                        else if (member != user)
                        {
                            member->getHackAnalyzer()->addServerHackCnt(
                                member, (WongWork::ENUM_HACKTYPE)0x71, 1, 0, 0);
                        }
                        else
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x70, 1, 0, 0);
                        }
                    }
                }
        }
    }

    // ORIG 0x085b5e38..0x085b5e7c：三项短路条件直接物化在 %eax（`mov $1/jmp/mov $0/test/je` 岛），
    // 无 instantCheck 布尔槽、无 mapInfo 具名局部 ⇒ 内联为单一 if 条件（P0/P214①）。
    if (dieInfo.m_fieldA22 == 0 && monster.m_roleType < 4 &&
        bf.GetCurrentMapInfo()->m_killChecker1.getUseSkillMaterialTime() == 0)
    {
        int level = 0;                                  // -0x10
        // P167（修正版）实证：ORIG 0x85b5e94/e4a/e58 三级判定各调一次 getHpMax
        //（共 3 次），无 hpMax 缓存 —— 原缓存少 2 次 call。
        if ((unsigned int)monster.m_active.getHpMax() * 4 <
            (unsigned int)dieInfo.m_killValue2)
            level = 3;
        else if ((unsigned int)monster.m_active.getHpMax() * 3 <
                 (unsigned int)dieInfo.m_killValue2)
            level = 2;
        else if ((unsigned int)monster.m_active.getHpMax() * 2 <
                 (unsigned int)dieInfo.m_killValue2)
            level = 1;
        // ORIG 085b5f1b `cmpl $0x0,-0xc(%ebp); jle`（有符号 <=0 跳走）⇒ 原码写 `0 < level`。
        if (0 < level)
        {
            // [P0] ORIG 0x085b5e9x：getHackAnalyzer 结果直传（无 hack 槽）。
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x86, level, 0, 0);
        }
    }
    if (bf.check_grid_clear())
        bf.onClearMap(true);
    // [负结论 2026-09-11] ORIG 0x085b5f82/0x540 两出口均汇到 `mov $0x1,%eax`，暗示原码
    // `return 1;`（非 void）；但权威头声明 void（P27：定义必须随头）⇒ 不可复现（差 1 条），
    // 如需闭合须改 .h 返回类型（本泳道禁改头，仅报告）。
    // [2026-09-11 推进] 语义修复 4 处：①`user != 0` 判空下移到 addMonsterDamage 之后
    //（ORIG 085b5ad3 独立守卫；addMonsterDamage 有副作用，旧码提前判空会漏调用）；
    // ②instantCheck 布尔槽改内联条件（ORIG 0x3ec 三项短路 eax 直传岛）；③臂序/极性改
    // `member != user`（ORIG 085b5c51/085b5d15/085b5d9b `je <user臂>`）；④`0 < level`
    //（ORIG 085b5f1b `jle`）。codegen：`while(cond){…break;}` 复现 setg 物化（P225）；
    // 比较书写序 `hpMax > killValue2` 复现 ORIG 085b5b79 的 seta+装载序。
    // 残 335/333：ORIG 085b5c8d..085b5e19 六臂各 `jmp 085b5e25` + 并行 nop 岛，4.4 恒
    // jump-thread（continue 与 goto 尾标签两形各试一次，读数均 333 无变化 ⇒ 归 N(P223 边界)）；
    // seta 段仍差 `seta/test/je`↔`jbe`（跨版本 setcc 物化，同上不可翻）。
}