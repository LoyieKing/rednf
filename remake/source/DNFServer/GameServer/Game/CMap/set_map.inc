// ORIG 0x0834e18a。P167 调用点差集（修复 2026-09-09）：ORIG 有
// STAssignMonster::operator=(aSERKS_) ×2 与 STAssignMonster::Clear() ×1，重建曾全缺。
// 证据（报告 docs/class_func_reports/CMap/set_map.md）：
//   ① -0x2b4 的 monster 只在两循环之前构造一次（0834e281 STAssignMonster::C1），
//      两循环内均为 `monster = *it`（0834e2fc / 0834e488 aSERKS_），迭代器槽 -0x240 复用
//      ⇒ 源码是循环外声明 monster/it，non-const iterator（P96/P220）。
//   ② boss 判定读写 monster+0x24（-0x290）而非 +0x14：0834e337 `mov -0x290; cmp $3`
//      → 0834e342 `movl $1,-0x290` + 0834e34c `movb $1,-0x260`（+0x54）。
//   ③ SPOBJ 分支 0834e658 Clear()（call 0836c4fa）→ m_0=action->m_4、m_8=action->m_8
//      （0834e678 `mov %eax,-0x2ac` = +0x8）、m_4=0（0834e67e `movb $0,-0x2b0`）。
//   ④ 内层 action 用索引 + 缓存指针（0834e5db operator[] 一次，结果 -0xc 复用），
//      外层两处循环有 list::empty() 预检查（0834e57e / 0834e82f）。
// [L6S6b 2026-09-11 残差] 523/522 gap+1：① 全函数匿名 sret/迭代器槽整体比 ORIG
//   深 4B（ORIG it -0x240 / 临时 -0x238 / end -0x23c / insert 实参 -0x1d0/-0x160/
//   -0xf8/-0x88/-0x90 区；我方各 +4）⇒ 疑似我方多一枚 4B 具名局部，属 N82/N95
//   槽族；② 首段 `m_8 = script.m_c`：ORIG 0x0834e195 `mov 0xc(%eax),%eax;
//   mov %eax,%edx` 两段（P254/P265 信号 ⇒ ORIG 侧字段与 RHS 异型），我方单段
//   `mov 0xc(%eax),%edx` ⇒ 需 STMapScript::m_c 或 CMap::m_8 之一 enum 化才能翻，
//   二者均在 .h（只报告，未改）；③ SPOBJ 条件链已按 P223 改为独立守卫+尾岛。
// [M4S2 R2 三件套复核 · 未动码] ① §6.5 串：set_map 体内无 cMyTrace/日志串
//   （ORIG 0x0834e18a..0x0834e9a1 段无 .rodata 字面量引用）⇒ 无串盲区；
//   ② §6.5.1 编码：enc_kr_scan 全库 total mismatch = 0；
//   ③ P167 调用点：ORIG 67 = OUR 67 **CLEAR**，仅 STAssignMonster 与
//   __normal_iterator 的 C1/C2、D1/D2 变体名差（全库共性）；
//   ④ 字段偏移：逐条一致，唯一指令数差仍是①的 `m_8 = script.m_c`（P211/P248
//   档③ 异型转换中转）。探针 /tmp/m4s2_probe/pr_field{,2}.cpp 直证该两段形来源：
//   f2 `t->m8=(int)*(E*)&s->c`、h3 `v->m8=(E)s->c`、h5 `u->m8=s->c`(u.m8 unsigned)
//   三者均产 `mov (%eax),%eax; mov %eax,%edx`（= ORIG），h1/h6（两侧同型）产单段。
//   ⇒ .h 建议（他泳道落）：STMapScript::m_c(+0x00c) 或 CMap::m_8(+0x08) 之一
//   改枚举/unsigned；本件整体残差（槽整体深 4 + 迭代器槽组）属 N82/N95 槽族。
// [P6AuditOldN.P6S2 2026-09-11 · 本波复核] n1_v DIFF 527/527 gap=0（blocks=49，全为
//   4.1 两段搬运/槽平移：ORIG 0x0834e18a `mov 0xc(%eax),%eax; mov %eax,%edx` vs 我方
//   `mov 0xc(%eax),%edx`；ORIG `lea -0x240(%ebp)` vs 我方 `lea -0x244(%ebp)`）。
//   新钥匙判定：P339 声明序（无入口可定序）/P0 删具名局部（无多余槽证据）/类型轴三式
//   （纯装载形态）均不适用；未落地，维持 N。
int CMap::set_map(STMapScript script)
{
    m_4 = script.m_index;
    m_8 = script.m_c;
    m_80 = script.m_championRate;
    m_entranceVec = script.m_recoveryObjects;
    m_pathObjectList = script.m_list4;
    m_bloodSystem = script.m_vecBloodSystem;
    m_bloodPhaseTime = script.m_vecBloodPhaseTime;
    m_b4 = script.m_maxMonsterDropException;
    std::memcpy(m_pathArea1, script.m_pathgatePos, sizeof(m_pathArea1));
    std::memcpy(m_pathArea2, script.m_pathgateRange, sizeof(m_pathArea2));

    STAssignMonster monster;
    std::vector<STAssignMonster>::iterator it;
    for (it = script.m_vecMonster.begin(); it != script.m_vecMonster.end(); it++)
    {
        monster = STAssignMonster(*it);
        if (m_8 == 2 && monster.m_24 == 3)
        {
            monster.m_24 = 1;
            monster.m_54 = 1;
        }
        if (monster.m_24 == 3)
            ++m_bossCount;
        insert_monster(monster, -1);
    }

    for (it = script.m_vecConditionMonster.begin(); it != script.m_vecConditionMonster.end(); it++)
    {
        monster = STAssignMonster(*it);
        insert_condition_monster(monster);
    }

    int area = 0;
    if (!script.m_listSpecialPassive.empty())
    {
        for (std::list<STAssignSpecialPassiveObj>::iterator obj = script.m_listSpecialPassive.begin();
             obj != script.m_listSpecialPassive.end(); ++area, obj++)
        {
            // ORIG 0834e5b7 只调一次 _List_iterator::operator*（const）并把引用存 -0x14，
            // 之后全部经该引用访问（0834e5cb `mov -0x14; lea 0x10(%eax)`）。
            STAssignSpecialPassiveObj& spObj = *obj;
            for (unsigned int i = 0; i < spObj.m_vec10.size(); ++i)
            {
                SPOBJ_ACTION_DATAS* action = &spObj.m_vec10[i];
                if (action->m_type == 0)
                {
                    // [L6S6b 2026-09-11] ORIG 0834e5f0..0834e72e 是**四个独立否定
                    //   守卫各自 goto 到物理连续的尾中继岛**（落点 0x59b/0x59e/0x5a1/
                    //   0x5a4 递增，岛序=守卫序；尾部 4×`jmp;nop`）⇒ 复合 && 会被
                    //   4.4 折成 bool 汇聚岛（我方多 `mov$1;jmp;mov$0;test;je`），
                    //   必须写 P223 多岛形：裸条件 goto + 尾标签串。
                    if (action->m_10 == 0)
                        goto SPOBJ_SKIP1;
                    if (get_rand_int(100) > (action->m_c * 100) / action->m_10)
                        goto SPOBJ_SKIP2;
                    if (action->m_8 > 0xc8)
                        goto SPOBJ_SKIP3;
                    if (action->m_8 <= 0)
                        goto SPOBJ_SKIP4;
                    {
                        monster.Clear();
                        monster.m_0 = action->m_4;
                        monster.m_8 = action->m_8;
                        monster.m_4 = 0;
                        insert_monster(monster, area);
                    }
                    SPOBJ_SKIP1:;
                    SPOBJ_SKIP2:;
                    SPOBJ_SKIP3:;
                    SPOBJ_SKIP4:;
                }
                else if (action->m_type == 2)
                {
                    insert_item(action->m_4, action->m_8, action->m_c, spObj.m_1c);
                }
            }
        }
    }

    m_rivalData1 = script.m_vecMonsterSpawnPos;
    m_rivalData2 = script.m_vecEventMonsterPos;
    m_78 = script.m_apcRandomPoint;
    m_aiCharacters = script.m_vecAICharacter;
    m_assignedNpcCount = static_cast<int>(script.m_vecNpc.size());
    if (!script.m_listPassiveObject.empty())
    {
        // ORIG 0834e86d/0834e872：end() 在循环**外**只调一次（落具名迭代器 -0x24c）；
        // 循环条件 0834e8bb 只发 operator!=，体内不重算 end()。
        std::list<STAssignPassiveObject>::iterator obj = script.m_listPassiveObject.begin();
        std::list<STAssignPassiveObject>::iterator objEnd = script.m_listPassiveObject.end();
        for (; obj != objEnd; ++obj)
        {
            // ORIG 0834e880 用 _List_iterator::operator*（0838b542），非 operator->。
            m_passiveObjectList.push_back((*obj).m_0);
        }
    }
    m_ec = script.m_maxMonsterCountException;
    m_f0 = script.m_328;
    m_timeLine = script.m_timeLine;
    m_110 = script.m_178;
    // ORIG 0834e917..0834e931：G_CDataManager() 只调一次（eax→+0x62e8=&m_actionMap），
    // checkAndSetActionDefine 为 static 成员（P245/P281），参数即 m_timeLine/&m_actionMap；
    // 返回值 0834e936 `mov %al,-0x15(%ebp)` 单字节落槽后未使用（P0 具名局部）。
    char checkRet = advancealtar::StageTimeLineParameter::checkAndSetActionDefine(
        m_timeLine, G_CDataManager()->m_stageTimeLine.m_actionMap);
    (void)checkRet;
    m_tournamentMap = script.m_tournamentMonsters;
    m_12c = script.m_fixChampion;
    return 1;
}