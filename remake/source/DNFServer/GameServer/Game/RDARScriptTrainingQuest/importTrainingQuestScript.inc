// [b28d-solo-STQuestScript · P0/P244/P171] ORIG 0x08aaf554（257 条）逐条还原：
// ①具名指针副本 s（ORIG `mov 0x8(%ebp),%eax; mov %eax,-0x1c; mov -0x1c,%eax`
//   槽往返 ⇒ 原码有具名指针局部，P0）；②pair 循环外默认构造 + 成员赋值；
// ③各段尾 `sub` 与闭合标签比较（ORIG 中 sub 全函数无写入点 = 原码遗留恒假
//   死条件，P171 照抄）；④ScanType 失败/[training quest end] 走统一 `return ok`。
// [§6.2 三件套] 2026-09-09 复核：257 vs 253（gap 4）；p167 30/30 CLEAR。
// 逐条归一化 diff 实测 4 处残差全为跳转中继岛/对齐 nop（无语义）：
// ① +0x5e..+0x63 ORIG `jmp;nop;jmp;nop` 双中继岛（[level] else 的 continue
//    落 +0x5b4、[training quest end] 不等落 +0x5b7）vs 我方单 `jmp`（差 2）；
// ② +0x71e nop（`je` 后 `jmp` 前）与 +0x802 nop 两处 4.1 对齐填充（差 2）；
// ③ +0x680/+0x685 两条同目标 jmp（5b8）4.4 合并为一条（差 1），我方
//    +0x2cb/+0x2ee 侧多一条 jmp（+1）⇒ 净 -4。
// 4.4 对同址目标恒 jump-thread（P223 负例），nop 属 §6.1 对齐产物 ⇒ N 封顶。
// 尾部 `movzbl -0x25(%ebp),%ebx` 已与 ORIG 单段形一致（旧 N18a 残差已消）。
// [2026-09-10 SweepSTQuestScript-2] [level] 分支补 2 处显式 continue
// （ORIG 0x8aaf7d6 / 0x8aaf802 两条 `jmp loop_head`）⇒ raw 253→254；
// 残余 gap 3 = ORIG 循环头前 2 组 `jmp loop_head;nop`（0x8aaf5b2 入口 +
// 0x8aaf5b5，后者是 0x8aaf7df `jne` 的落点）被 4.4 jump-thread 合并为 1 组，
// 0x8aaf685 中继同被合并 ⇒ P223 族封顶（无合法 C++ 源形可分裂）。
// 四数 458/917/0/12 不回退；p167 无 ORIG-only 游戏域 callee。
// [P2S2STQuest 2026-09-11 复核] 本轮实测 verdict=DIFF orig_insns=257 our_insns=255
// gap=+2，4 处残差全为 jmp 中继/对齐（ORIG 0x8aaf5b5 中继块被 0x8aaf7df `jne` 引用、
// 0x8aaf680/0x8aaf685 双 `jmp`、我方 +0x2cb/+0x2ee 侧各多一条）⇒ 与 importTownScript
// 残余块同族（4.4 continue-label 布局启发式，本轮已被 /tmp/p2/probe4.cpp 定性）
// ⇒ 封顶维持，无源码层旋钮。
bool importTrainingQuestScript(TrainingQuestScript* script, const char* path)  // [P330] 与 STQuestScript.h 同步
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    TrainingQuestScript* s = script;
    s->clear();
    std::string line;
    int prev = 0;
    std::string sub;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[select prob]")
        {
            s->m_selectProb.m_count = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            std::pair<int, int> p;
            for (;;)
            {
                p.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.second = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                s->m_selectProb.m_pairs.push_back(p);
                // p167：ORIG 用 _ZSteq…（operator==）⇒ 原码正极性 ==；相等→退出循环。
                if (sub == "[/select prob]")
                {
                    break;
                }
            }
            continue;
        }
        if (line == "[rand value]")
        {
            for (int day = 0; day <= 0x16d; ++day)
            {
                int count = 0;
                while (count < s->m_selectProb.m_count)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    s->m_selectProb.m_dayProb[day].push_back(v);
                    ++count;
                }
                if (sub == "[/rand value]")
                {
                    break;
                }
            }
            continue;
        }
        if (line == "[level]")
        {
            int level = ScanInt((bool*)0);
            if (prev + 1 == level)
            {
                // ORIG：递增序号段的数值进 m_questProb[prev]，行首失败仅结束该段。
                for (;;)
                {
                    int value = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    s->m_questProb[prev].push_back(value);
                    if (sub == "[/level]")
                    {
                        break;
                    }
                }
                prev = level;
                continue;
            }
            else if (level == -100)
            {
                // ORIG：-100 段的数值进 m_sameApplyQuest。
                for (;;)
                {
                    int value = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    s->m_sameApplyQuest.push_back(value);
                    if (sub == "[/level]")
                    {
                        break;
                    }
                }
                continue;
            }
            continue;
        }
        if (line == "[training quest end]")
        {
            ok = true;
            break;
        }
        // 未知键：忽略（回循环读下一行）
    }
    return ok;
}