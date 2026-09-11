// 残余 DIFF 归因（o=306 n=311；CStringMaker for_each 段/const 迭代器形/
// cur-prev 单游标语义已按 ORIG 恢复）：① 70 元 multiset 数组 ORIG 4.1 发
// 显式构造循环 `mov $0x45; 倒计数 call multiset C1`（0x08363d03..0x08363d2a），
// 4.4.7 对 `multiset applyLevels[0x46]` 的构造循环寄存器/步序不可逐项复刻
// （P151 同族形已用，槽 -0x6e4 对齐）；② for_each 段 4.4 引用别名物化槽
// 使 maker 区平移（P112）；③ ORIG EH landing pad（0x08363d2c..0x08363d89
// cleanup 展开）与 4.4 异常表结构差异属跨版本负例。
// [b28d-solo-CDataManager AE 306=306] 本轮翻正（o=306 n=294→306）：
//   ① quest 提升为函数头声明（ORIG 0x08363cec 入口 `movl $0,-0x28`，P212/P309）；
//   ② cur/questIdx 提到 level 循环头（ORIG 0x08363dbe/0x08363dc5 每 level 一次
//      具名初始化）；
//   ③ 去掉 bucket 引用槽：内联 `static_cast<const multiset<int>&>(applyLevels
//      [level-1]).begin()/end()`，复现 ORIG 每轮重算地址 + 重调 end（P183）；
//   ④ m_type 分支改否定式（ORIG 0x08363e6f `je` 跳 push_back 块，P272）；
//   ⑤ 两处空块 nop 中继岛用否定 goto + 尾标签串（ORIG 0x08363e42/0x08363e64，
//      P223）。p167 31/31 CLEAR。
bool CDataManager::reselectDailyTrainingQuest()
{
    // ORIG 0x08363ce0 T。主分支：
    //  1) suffleTrainingQuests() 重排每日训练任务。
    //  2) getApplyLevel 填充 multiset<int>（各档可选等级）。
    //  3) 对每个等级 level(1..0x46)：清空 m_dailyTrainingQuest[level-1]，
    //     遍历 multiset，用 getTrainingQuest(level, prevApply, prevMin) 取任务 idx；
    //     find_quest 命中且 Quest::m_type==1 → push_back 到当日列表；否则 error 日志。
    //  4) reset_same_apply_level_cnt()；非空列表记 TRAINING_QUEST trace。
    // ORIG 0x08363cec 函数入口 `movl $0x0,-0x28` ⇒ quest 为函数头声明
    // （P212/P309 全头部声明），循环内仅赋值。
    Quest* quest = 0;
    m_trainingQuest.suffleTrainingQuests();

    // ORIG 0x08363d03：getApplyLevel 按 level 分桶写入 multiset 数组
    // （out[level-1]，level 1..0x46），主循环对每个 level 遍历对应桶。
    std::multiset<int> applyLevels[0x46];
    m_trainingQuest.getApplyLevel(applyLevels);

    for (int level = 1; level < 0x47; ++level)
    {
        m_dailyTrainingQuest[level - 1].clear();
        int prevApply = 0x7fffffff;
        // ORIG 0x08363dbe/0x08363dc5：cur(-0x1c)/questIdx(-0x50) 亦为每
        // level 一次的具名初始化（cur 初值 0x7fffffff、questIdx 0）。
        int cur = 0x7fffffff;
        int questIdx = 0;
        // ORIG 0x08363dee 调 _ZNKSt8multiset...beginEv（const 版）+
        // _Rb_tree_const_iterator deref/!=，且无 iterator→const_iterator
        // 转换调用 ⇒ 经 const 视图走 const 重载。ORIG 0x08363ee7/0x08363f0e
        // 每次迭代重算 applyLevels[level-1] 地址并重调 end()（无 bucket 引用
        // 槽、无 end 缓存，P183 地址重物化）⇒ 内联 static_cast 视图表达式。
        for (std::multiset<int>::const_iterator it =
                 static_cast<const std::multiset<int>&>(applyLevels[level - 1]).begin();
             it != static_cast<const std::multiset<int>&>(applyLevels[level - 1]).end();
             ++it)
        {
            cur = *it;
            questIdx = m_trainingQuest.getTrainingQuest(level, cur, prevApply);
            prevApply = cur;
            // ORIG 0x08363e42 `je +0x1f8` / 0x08363e64 `je +0x1fb` 两处空块
            // nop 中继岛（P223）：否定 goto + 尾标签串（标签序=守卫序）。
            if (questIdx == 0x7fffffff)
                goto train_skip_quest;
            quest = m_questList->find_quest(questIdx);
            if (quest == 0)
                goto train_skip_branch;
            {
                if (quest->m_type != 1)
                {
                    // ORIG 0x08363e6f `je +0x1cd`（m_type==1 跳 push_back 块，
                    // 日志块直落）⇒ 源形为否定式（P272 德摩根块序交换）。
                    LogManager::logFormat(
                        1, "data_manager.cpp",
                        __PRETTY_FUNCTION__,
                        0x2ac8,
                        "reselectDailyTrainingQuest() ERROR : quest(%d), "
                        "level(%d)",
                        questIdx, level);
                }
                else
                {
                    m_dailyTrainingQuest[level - 1].push_back(questIdx);
                }
            }
        train_skip_branch:;
        train_skip_quest:;
        }
        m_trainingQuest.reset_same_apply_level_cnt();
        if (!m_dailyTrainingQuest[level - 1].empty())
        {
            // ORIG 0x08363f58..0x08364075：仅一次 `CStringMakerC1Ev`
            // （0x08363f6e）+ 4×D1、**零** copy ctor call ⇒ 原码传匿名临时
            // （copy elision 直构形参）；旧具名 maker 形多 1 copy C1ERKS_ + 2 D1
            // （p167 EXTRA）。全表达式临时链复现（P37/P221）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2ad2, 0)(
                "TRAINING_QUEST level(%d), quest(%s)", level,
                std::for_each(m_dailyTrainingQuest[level - 1].begin(),
                              m_dailyTrainingQuest[level - 1].end(),
                              sync_script::CStringMaker()).c_str());
        }
    }
    return 1;
}