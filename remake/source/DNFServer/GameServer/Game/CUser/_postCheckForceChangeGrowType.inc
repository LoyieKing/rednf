// ============================================================================
// _postCheckForceChangeGrowType（免费函数，TSV 修复，ORIG 0x08668823 T）。
// ORIG：遍历 QuestList hash_map<int,Quest*>（G_CDataManager()+0x18→getQuestList），
// 逐任务按 CUserCharacInfo::get_charac_job 做 checkPossibleJob，再按 quest 内
// m_fieldB8==1（比对 firstGrow == quest.m_fieldC0）/==2（checkMatchedGrowType +
// secondGrow == quest.m_fieldC0）判定，命中的任务经 _postCheckForceChangeGrowType4AheadQuest
// → CQuestClear::setClearedQuest 清任务并发包。
// 依赖 Quest/QuestList/CQuestClear 等当前未建模类型，提供可链接的签名匹配骨架（推断）。
// ============================================================================
void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow)
{
    // b28d-solo-CUser p167：ORIG 0x08668823 经 `getQuestList()` 一次取得 QuestList
    //   （MISSING ×1），容器按 const 引用访问（`begin() const`/`end() const`；我方非
    //   const 致 2 次转换构造）；命中块由两分支 **goto 共享**（我方重复两份 ⇒
    //   EXTRA 4AheadQuest/getCurCharacQuestW/setClearedQuest 各 ×1，P276）。
    // [H2LaneE2 2026-09-11 · P0/P183 去缓存] ORIG 0x08668830 无 questList 具名局部：
    //   `call G_CDataManager; mov 0x18(%eax),%eax; mov %eax,(%esp); call getQuestList`
    //   （m_questList 结果作 this 直传，无栈槽往返；我方多 `mov %eax,-0x18;
    //   mov -0x18,%eax` 一对待删）⇒ 内联链式表达式，仅保留 map 引用（ORIG -0x10 单槽）。
    const __gnu_cxx::hash_map<int, Quest*>& qmap =
        G_CDataManager()->m_questList->getQuestList()->m_questMap;
    __gnu_cxx::hash_map<int, Quest*>::const_iterator it = qmap.begin();
    // [Main-K4C 2026-09-11 · P13] ORIG 0x8668853 在循环**外**求值一次 end()（槽 -0x2c）：
    //   尾部 `lea -0x2c,%eax; mov %eax,0x4(%esp); lea -0x24,%eax; mov %eax,(%esp);
    //   call _ZNK...neERKS...`；我方原写 `it != qmap.end()` 致每轮重发
    //   `lea -0x18; mov -0x10,%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call end; sub $4`。
    //   A/B 实测（fn_diff_tu，check_tu_routed fresh，指令数两侧均 135/134 norm）：
    //   差异行 ORIG 侧 111 → **49**（保留；残余为 N82/P212 块级对象槽向：ORIG PacketGuard
    //   浅 -0x1c / it -0x24 / end -0x2c，我方 it -0x18 / qend -0x20 / PacketGuard 深 -0x2c）。
    __gnu_cxx::hash_map<int, Quest*>::const_iterator qend = qmap.end();
    for (; it != qend; ++it)
    {
        Quest* quest = it->second;
        // ORIG 0x8668893：checkPossibleJob 取反后为真 → 跳过
        if (!quest->checkPossibleJob((ENUM_CHARACTERJOB)user->get_charac_job()))
            continue;

        if (quest->m_fieldB8 == 1)
        {
            // +0xb8==1：+0xc0 == firstGrow 命中
            if (quest->m_fieldC0 == firstGrow)
                goto HIT;
        }
        else if (quest->m_fieldB8 == 2)
        {
            // +0xb8==2：checkMatchedGrowType(job, firstGrowType, -1) 且 +0xc0 == secondGrow
            // [H2LaneE2 2026-09-11 · P0/P329 删具名局部] ORIG 0x086688e1 `movsbl %al,%ebx`
            //   = getCurCharFirstGrowType() 返回值**直接拓宽进 callee-saved**（无 -0x9 字节槽
            //   store/load；我方旧形多 `mov %al,-0x9; movsbl -0x9,%ebx` 一对）⇒ 表达式内联进
            //   实参位（实参右→左求值：该调用先于 get_charac_job 求值，与 ORIG 顺序一致）。
            if (!quest->checkMatchedGrowType((ENUM_CHARACTERJOB)user->get_charac_job(),
                                             user->getCurCharFirstGrowType(), -1))
                continue;
            if (quest->m_fieldC0 != secondGrow)
                continue;
            goto HIT;
        }
        continue;
HIT:
        _postCheckForceChangeGrowType4AheadQuest(user, quest);
        // ORIG 0x8668939：getCurCharacQuestW()+0x4 → CQuestClear::setClearedQuest(idx)
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest((unsigned int)quest->m_index);
    }

    // ORIG 0x8668984：PacketGuard → getCurCharacQuestR()->get_quest_info(pg) → Send
    PacketGuard packet;
    user->getCurCharacQuestR()->get_quest_info(
        reinterpret_cast<char*>(&packet));                       // ORIG mangled 形参 Pc
    user->Send(packet);
}