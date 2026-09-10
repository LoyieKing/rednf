void CLevelRewardTableMgr::SetChallengeInfo(stUnlimitChallengeInfo& info)
{
    // [b28d-sweepC 判词·残余 N82+N7] ORIG 0x08687828：
    // 历史修复：第二循环 `it->m_subIdx` 缓存（p167 CLEAR 50/50）；
    // `m_flag = info.m_field0` 直赋（P136/P292）。
    // 本轮修：第二循环 `int subIdx = it->m_subIdx`（ORIG 0x08687xxx `movzbl
    // <m_subIdx>,%eax; mov %eax,-0x24(%ebp)` ⇒ 局部是 **int** 4 字节槽，调用处
    // 才 movsbl 收窄；旧 `char subIdx` 发单字节 `mov %al,-0x3d`）。
    // 残余：jobIdx/levelIdx ORIG 用 %edi/%esi 跨调用（N7）；三段循环槽号整体
    // 平移（N82：4.1 不复用槽）⇒ 封顶。
    // ORIG 0x08687828 全量转写（报告双段 + objdump 逐条）：
    // 0x08687837 `movzbl (%eax)` + 0x0868783d `mov %dl,(%eax)` ⇒ flag 单字节；
    // 0x08687852 vec 赋值、0x08687868 SetJobAffection(true)；随后三段同构循环，
    // 每段 ORIG 都：① size() **只调一次并存具名槽**（-0x3c / -0x38 / -0x34）；
    // ② begin()/end() 各构造一次成持久迭代器槽（-0x64/-0x68、-0x6c/-0x70、
    //   -0x78/-0x7c）⇒ end() 不能写在循环条件里；③ 条件头 =
    //   `mov -0x30,%eax; cmp -0x3c,%eax; jge out`（0x086878ed）短路后再
    //   `call operator!=`（0x08687902）+ bool 物化岛 ⇒ 原码是 `i < sz && it != last`；
    // ④ 体内 `addl $1,-0x30` 之后 `call operator++(int)`（0x086878e5，带临时槽
    //   -0x58）⇒ **i++ 与 it++ 都是后置**，不是 ++it。
    // [b28d-solo-CtorProducers_1 修复·P136/P292] ORIG 0x08687837..0x0868783d
    // `movzbl (%eax),%edx; mov %dl,(%eax)` = bool→bool 直接字节存储（无 setne）；
    // 旧 `*(const char*)&info.m_field0` 引入 uchar→bool 物化（+2 条 test/setne）。
    m_flag = info.m_field0;
    m_vec8 = info.m_vec4;
    m_table->SetJobAffection(true);

    {
        const int sz = static_cast<int>(info.m_vec10.size());   // 0x0868784e → -0x3c
        std::vector<int>::const_iterator it = info.m_vec10.begin();
        std::vector<int>::const_iterator last = info.m_vec10.end();
        for (int i = 0; i < sz && it != last; i++, it++)
            m_table->m_levelKeys[i] = *it;
    }
    {
        // 0x0868794c size()→-0x38；begin→-0x6c、end→-0x70；i=-0x2c
        const int sz =
            static_cast<int>(info.m_vec1c.size());
        std::vector<stUnlimitChallengeRewardInfo>::const_iterator it =
            info.m_vec1c.begin();
        std::vector<stUnlimitChallengeRewardInfo>::const_iterator last =
            info.m_vec1c.end();
        for (int i = 0; i < sz && it != last; i++, it++)
        {
            unsigned int level = static_cast<unsigned int>(it->m_jobLevel);
            // p167：ORIG 该循环 `operator->` 仅 4 次（m_jobLevel/m_subIdx/m_a/m_b）
            // ⇒ `it->m_subIdx` 必须缓存一次后复用（旧写两遍致 EXTRA×1，Ghidra C 的
            // 第二次 GetProperJobIndex 用具名 local_28）。
            // ORIG 0x08687xxx：`movzbl <m_subIdx>,%eax; mov %eax,-0x24(%ebp)` ⇒
            // 局部是 **int**（4 字节槽），调用处才 `movsbl %al,%edx` 收窄到 char。
            // 旧 `char subIdx` 发单字节 `mov %al,-0x3d`（槽宽/截断序不符）。
            int subIdx = it->m_subIdx;
            int jobIdx = m_table->GetProperJobIndex(subIdx);
            int levelIdx = m_table->GetProperLevelIndex(level);
            m_table->m_rewardPrimary[jobIdx][levelIdx] = it->m_a;
            jobIdx = m_table->GetProperJobIndex(subIdx);
            levelIdx = m_table->GetProperLevelIndex(level);
            m_table->m_rewardSecondary[jobIdx][levelIdx] = it->m_b;
        }
    }
    {
        // 0x08687a30 size()→-0x34；begin→-0x78、end→-0x7c；i=-0x24
        const int sz =
            static_cast<int>(info.m_vec28.size());
        std::vector<stUnlimitChallengeMessageInfo>::const_iterator it =
            info.m_vec28.begin();
        std::vector<stUnlimitChallengeMessageInfo>::const_iterator last =
            info.m_vec28.end();
        for (int i = 0; i < sz && it != last; i++, it++)
        {
            unsigned int level =
                static_cast<unsigned int>(static_cast<unsigned short>(it->m_id));
            // ORIG 0x08687b8x/0x08687b9x：**两个 std::string 拷贝临时**先各自
            // 构造（local_80←it->m_msg1、local_84←it->m_msg2），再取 c_str() 交
            // findString，块尾按构造逆序析构（0x08687c7c 处 ~string×2，带 EH
            // CatchHandler 0x08687c94/0x08687c7f）⇒ 不能直接对 it->m_msgN 调
            // c_str()（P-H：拷贝临时本身就是 ORIG 的指令来源）。
            std::string msg1 = it->m_msg1;
            std::string msg2 = it->m_msg2;
            const char* p1 = g_scriptStringManager_.findString(4, msg1.c_str(), 0);
            int nameIdx = m_table->GetProperLevelIndex(level);
            strncpy(m_table->m_nameStrings + nameIdx * 0x15, p1, 0x14);
            const char* p2 = g_scriptStringManager_.findString(4, msg2.c_str(), 0);
            int msgIdx = m_table->GetProperLevelIndex(level);
            strncpy(m_table->m_messages + msgIdx * 0x100, p2, 0xff);
        }
    }
