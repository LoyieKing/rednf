// ============================================================================
// CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
// ORIG T _ZN5CUser12quest_actionEiiii @0x0866da8a。
// 报告 docs/class_func_reports/CUser/quest_action.md 全文已读；草稿（本文件
// quest_action W7-BLOCKED 注释区）按报告全文转写，本定义为其真实落地。
// cmd：0x21 accept / 0x22 giveup / 0x23 trigger / 0x24 finish(reward)。
// ============================================================================
// [b28d-22 N82] ORIG 0x0866da8a：p167=M0/E12:_ZN9__gnu_cxx17__normal_iteratorIPKiSt6vectorIiSaIiEEEppEv;_ZNK9__gnu_cxx17__normal_iteratorIPKiSt6vectorIiSaIiEEEdeEv;_ZNSt20_List_const_iteratorIiEC1ERKSt14_List_iteratorIiE，gap=9；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0866da8a：N82 封顶——帧/槽差 + 迭代器族（p167 M0/E12，gap 9）。
// [BigP3C 2026-09-12 语义再核] p167 = ORIG 106 call / 我方 106 call，差集仅 C1↔C2、D1↔D2
// 同址别名（P93 伪差集）⇒ **调用点层无语义缺口**。逐块核 ORIG 后修正 4 处：
//   ① giveup/trigger 的「quest 不存在」分支：ORIG 0x0866dd2e(回 0x28) / 0x0866df93(回 0x18) 都是
//      `je 0x866e38d` —— 0x866e38d 是 PacketGuard::~PacketGuard 出口（即 switch 的 break），
//      **不是** finalize/Send 段 ⇒ 原码是 `if (q == 0) break;`（不发响应包）；
//      旧形 `if (q != 0) { ... }` 在 q==0 时仍会 finalize+Send（真语义差，已修）。
//   ② accept 两个守卫：ORIG 0x0866db37/0x0866db45/0x0866db59 三条件**直跳同一目标** 0x0866dbab
//      （第二守卫起点）；`A && B && C` 写法被 4.4 物化为 setcc 汇聚块 ⇒ 改**嵌套 if**（P225 推广）
//      复现直跳链（三个失败出口各自 je/jne 同址）。
//   ③ finish 段：ORIG `call getCurTickCount; mov 0x8(%ebp),%edx; mov 0x79644(%edx),%edx;
//      sub %edx,%eax` 全程无槽往返 ⇒ 原码无 `now` 具名局部，表达式直用（P0）；已删局部。
//   ④ case 叶块物理序：ORIG 0x21@+0x92 < 0x22@+0x217 < 0x23@+0x448 < 0x24@+0x54b
//      ⇒ 源码 case 书写序为 0x21/0x22/0x23/0x24（P46 叶块序=case 序），已按升序重排。
//   ⑤ giveup 的 if/else 物理布局：ORIG 0x0866dcbe `jle <+0x264>`（失败块在前）⇒ P59 双向布局，
//      改 `if (gResult > 0) {失败} else {成功}` 后我方分支落点 +0x264 与 ORIG 完全一致。
//   ⑥ finish：err>0 只写两个字节后 `goto` 函数尾共享块（ORIG `jmp 0x866e333`）；qi 去内层 `{}`
//      使析构落在 _onQuestClear 之后 ⇒ p167 **调用序号 106/106 完全对齐（mismatch 30→0）**。
//   ⑦ mail 循环三失败条件改独立早退 goto（P223 岛）⇒ 复现 ORIG 0x755/0x758/0x75b 三个
//      `nop; jmp` 岛（探针 /tmp/BigP3C/probe_islands.cpp）。
//   ⑧ accept 的 `int local42;` 去初值（P171：ORIG 0x0866dc2f 无 store）。
//   ⑨ `m_postChecksumData.m_field48` int→unsigned（P217 同型化；CUser.h 单字段，2026-09-12
//      已广播、Main 批准）⇒ `mov %eax,0x79644(%edx)` 直存（消 edx 中转）。
//   ⑩ mail 循环 `*mi` 复具名局部（P0 槽往返；补后帧仍 0xc0、归一化指令数 613→615）。
// 【最终读数 2026-09-12】TU 口径（compare_tu_game_orig）：帧 `sub $0xc0` 与 ORIG 一致；
//   归一化指令 ORIG 615 / 我方 615（delta=0）；函数字节 2335 vs ORIG 2334（仅差 1 字节）；
//   p167 调用点 106/106（差集仅 C1↔C2、D1↔D2 同址别名，P93）；调用**顺序** mismatch=0。
//   残余 DIFF 仅两处 4.4 布局产物（不可控）：① ORIG 0x0866ddc4 的 `mov %eax,-0x24(%ebp)`
//   （delete_event_items 返回值死写槽；补具名局部即帧 0xc0→0xd0、§6.4 反劣实证）⇒ N82 封顶；
//   ② 我方 EH 段尾多 `nop; jmp`（4.4 landing pad 排布）⇒ EH 布局族。语义等价成立。
void CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
{
    Quest* q = G_CDataManager()->find_quest(questId);
    if (q == 0)
        return;
    if (q->m_type == 7)
        return;

    PacketGuard packet;
    packet.put_header(1, cmd);

    // [b28d-22 修法] ORIG 0x0866dc84-0x0866dca1 为 4-case switch 跳转链；if/else 链
    // 使分支体内联主干造成 854 行错位。case 序按 ORIG 叶块物理序（0x21 在前）。
    switch (cmd)
    {
    case 0x21:                                                  // accept
    {
        Quest* qa = G_CDataManager()->find_quest(questId);
        // [BigP3C 2026-09-12 · P133/P225] ORIG 0x0866db39-cmp$1 / 0x0866db45-jne / 0x0866db59-je
        // 三个失败条件全部落 0x0866dbab（第二守卫检查点）⇒ 原码为嵌套 if（非 `&&` 短路链）。
        // 两个守卫**各自内联**一份 `put_byte(0); put_byte(0x15); finalize(true); Send(packet);`
        // 再 jmp 函数尾（0x0866db5b / 0x0866dbd1）。
        if (qa != 0)
        {
            if (qa->m_field134 == 1)
            {
                if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
                {
                    packet.put_byte(0);
                    packet.put_byte(0x15);
                    packet.finalize(true);
                    Send(packet);
                    break;
                }
            }
        }
        if (qa != 0)
        {
            if (qa->m_field134 == 2)
            {
                if (!qa->IsOpenScheduleQuest())
                {
                    packet.put_byte(0);
                    packet.put_byte(0x15);
                    packet.finalize(true);
                    Send(packet);
                    break;
                }
            }
        }
        {
            // [BigP3C 2026-09-12 · P171] ORIG 0x0866dc2f 直接 `lea -0x40(%ebp),%edx` 取址压参，
            // **无** `movl $0x0,-0x40(%ebp)` ⇒ 原码是 `int local42;`（未初始化，accept_quest 写回）。
            int local42;
            const int errCode =
                getCurCharacQuestW()->accept_quest(
                    questId, (char*)&packet, local42);
            if (errCode > 0)
            {
                packet.put_byte(0);
                packet.put_byte(errCode);
            }
            packet.finalize(true);
            Send(packet);
        }
        break;
    }
    case 0x22:                                                  // giveup
    {
        // ORIG：giveup_quest 返回值 local_38 <1（0 成功）→ 成功包；
        // ≥1（0x12/0x13/0x14 失败码）→ put_byte(0)+失败码。
        const int gResult = getCurCharacQuestW()->giveup_quest(questId);
        // [BigP3C 2026-09-12 · P59 双向布局] ORIG 0x0866dcbe `cmpl $0,-0x34(%ebp); jle 0x264`
        // ⇒ 失败块（put_byte(0)+失败码）物理在前、成功块（put_byte(1)+…）在后
        // ⇒ 原码是 `if (gResult > 0) {失败} else {成功}`（if 体在前，4.1/4.4 均生 `jle else`）。
        if (gResult > 0)
        {
            packet.put_byte(0);
            packet.put_byte(gResult);
        }
        else
        {
            packet.put_byte(1);
            packet.put_short(questId);
            Quest* q2 = G_CDataManager()->find_quest(questId);
            // [BigP3C 2026-09-12 真语义修复] ORIG 0x0866dd2e `je 0x866e38d`（PacketGuard
            // 析构出口=break）⇒ quest 不存在时不发包直接跳出；旧 `if (q2 != 0){...}` 会多发包。
            if (q2 == 0)
                break;
            // [M8BigTURest 2026-09-11 04:05 语义修复·字段错位] ORIG 0x0866dd34/0x0866dd5c/
            // 0x0866dd81 三处**都是 Quest+0x70**：`add $0x70,%eax; call
            // vector<pair<int,int>>::empty/size()` 与 `lea 0x70(%eax),%ebx` 作
            // delete_event_items 第一实参；我方 Quest 布局 0x70=m_rewardSelectList、
            // 0xac=m_triggerDataPairs（Quest.h:443，且 quest_reward 侧 ORIG 0xac 与
            // `m_triggerDataPairs.begin/end` 已对齐=布局两侧一致）⇒ 原稿把字段写成
            // m_triggerDataPairs 是**读错列**（运行期清错列表），按 ORIG 直证改回
            // m_rewardSelectList（三处：empty/size/传参）。
            if (!q2->m_rewardSelectList.empty())
            {
                bool touched = false;
                std::vector<int> updates;
                updates.reserve(q2->m_rewardSelectList.size() * 3);
                // [BigP3C 2026-09-12 · N82 封顶] ORIG 0x0866ddc4 有一条 `mov %eax,-0x24(%ebp)`
                // （delete_event_items 返回值死写，P0 意义上原码有具名局部）；但一式实测：
                // 补具名局部后 4.4 不复用槽 ⇒ 帧 0xc0→**0xd0**、函数 2332→2350B（§6.4「加具名局部
                // 反劣」同族）⇒ 判 N82 封顶，不补（保留 ORIG 的调用形态）。
                getCurCharacInvenW()->delete_event_items(
                    q2->m_rewardSelectList, updates, touched,
                    (eMoneySubReason)4, (eItemDelReason)4, false);
                // [b28d-solo-CUserOnlyOrig P96/P270] p167 差集：MISSING
                // `__normal_iterator<int*>::ppEv/deEv` + EXTRA `<const int*>` 族
                // ⇒ ORIG 用非 const iterator（updates 非 const 局部）。
                std::vector<int>::iterator ui = updates.begin();
                while (ui != updates.end())
                {
                    const int spId = *ui; ++ui;
                    // [H6 2026-09-11 单旋钮·类型轴] ORIG 该件 `mov %ax,-0x1c(%ebp)` +
                    // `movswl -0x1c(%ebp),%eax`（16 位具名槽 + 符号扩展为 SendUpdateItem 实参）
                    // ⇒ 原码是 **short** 局部（数量按 16 位截断）。旧 `const int cntS` 给 32 位槽。
                    const short cntS = static_cast<short>(*ui); ++ui;
                    SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)spId, cntS);
                    ++ui;                                   // 第三元素只读丢弃
                }
            }
            SendHasNoGoWithApcQuestInParty(q2);
        }
        packet.finalize(true);
        Send(packet);
        break;
    }
    case 0x23:                                                  // trigger
    {
        const char b = (char)opt;
        const int trigResult =
            getCurCharacQuestW()->set_trigger(
                questId, (char)triggerVal, b != 0);
        if (trigResult == -1)
        {
            packet.put_byte(0);
            packet.put_byte(0x16);
        }
        else
        {
            packet.put_byte(1);
            packet.put_short(questId);
            packet.put_int(trigResult);
            if (trigResult == 0)
            {
                Quest* qt = G_CDataManager()->find_quest(questId);
                // [BigP3C 2026-09-12 真语义修复] ORIG 0x0866df93 `je 0x866e38d`（break 出口）
                // ⇒ qt 不存在时直接跳出（不发包），与 giveup 同型。
                if (qt == 0)
                    break;
                SendHasNoGoWithApcQuestInParty(qt);
            }
        }
        packet.finalize(true);
        Send(packet);
        break;
    }
    case 0x24:                                                  // finish(reward)
    {
        // [BigP3C 2026-09-12 · P0] ORIG 0x0866dfd5-e01x `call getCurTickCount; mov 0x8(%ebp),%edx;
        // mov 0x79644(%edx),%edx; sub %edx,%eax` 无槽往返 ⇒ 原码无 now 具名局部（表达式直用）。
        if ((GlobalData::s_systemTime_.getCurTickCount() -
             m_postChecksumData.m_field48) <= 0x3e8)            // 0x79644 近期已请求
        {
            ++m_postChecksumData.m_field4c;                     // 0x79648
            // [H6 2026-09-11 单旋钮·符号性] ORIG 0x0866e01d `cmp $0x1,%eax; jbe 0x0866e0cf`
            // = **无符号**比较；4.4 对 int 字段产 `jle`（有符号）⇒ 显式 unsigned 视角复现
            // （计数字段，语义等价区间内的负数不可达）。
            if (1 < static_cast<unsigned int>(m_postChecksumData.m_field4c))
            {
                // [H6 2026-09-11 单旋钮·P0 去具名] ORIG 0x0866e029 把 getHackAnalyzer() 结果
                // 直接落 (%esp) 调 addServerHackCnt（**无** ha 具名槽），旧形多一对 store/load。
                getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x192,
                                                    1, 0, 0);
                packet.put_byte(0);
                packet.put_byte(1);
                packet.finalize(true);
                Send(packet);
                return;
            }
        }
        else
        {
            m_postChecksumData.m_field4c = 0;
            m_postChecksumData.m_field48 =
                GlobalData::s_systemTime_.getCurTickCount();
        }

        const int err = quest_reward(questId, triggerVal, opt);
        if (err > 0)
        {
            packet.put_byte(0);
            packet.put_byte(err);
            // [BigP3C 2026-09-12 · 调用序列对齐] ORIG 0x0866e11x `jmp 0x866e333`：错误码包**只写
            // 两个字节**，finalize/Send/WriteFineQuest 是函数尾的共享块 0x866e333（p167 调用
            // 序号：ORIG 的 finalize/Send/WriteFineQuest 在 #100-102，位于全部析构之后）。
            goto L_fine_quest;
        }
        {
            std::list<int> mailQuestIds;
            stSelectQuestParam selParam(this);
            // [H6 2026-09-11 单旋钮·P0 去具名 bool] ORIG 0x0866e166 `test %al,%al; je
            // 0x0866e220` 直判返回值（**无** hasMailQ 槽）；旧形多 `mov %al,-0x11; cmpb $0x0,-0x11`。
            if (getCurCharacQuestR()->get_mail_quest_info(mailQuestIds, selParam))
            {
                // [b28d-solo-CUserOnlyOrig P96/P270] p167：MISSING `_List_iterator<int>`
                // 族 + EXTRA `_List_const_iterator` 转换构造 ⇒ 非 const iterator。
                // [BigP3C 2026-09-12 · P223] ORIG 0x0866e1df/e1e2/e1e5 三个失败条件**各落一个
                // nop 岛**（0x755/0x758/0x75b，各 `nop; jmp 0x75c`）⇒ 原码是三个**独立早退
                // goto**（非 `&&` 短路）。探针 /tmp/BigP3C/probe_islands.cpp 在 4.4.7 下复现
                // `je <nop岛>; …; call; jmp <join>; nop; jmp; nop; jmp; nop; join:` 同构。
                for (std::list<int>::iterator mi = mailQuestIds.begin();
                     mi != mailQuestIds.end(); ++mi)
                {
                    // [BigP3C 2026-09-12 · N82 封顶] ORIG 0x0866e18x 把 `*mi` 存 -0x14 槽再跨
                    // G_CDataManager 调用取用（原码具名局部）；同上一式实测：补具名局部帧
                    // 0xc0→0xd0 ⇒ 维持内联实参（4.4 以 %ebx 跨调用保活，语义等价）。
                    const int mailQuestId = *mi;
                    Quest* mq = G_CDataManager()->find_quest(mailQuestId);
                    if (mq == 0)
                        goto L_mail_next1;
                    if (mq->m_mailFlag != 1)
                        goto L_mail_next2;
                    if (mq->m_index != questId)
                        goto L_mail_next3;
                    _postQuestTriggerItem(mq);
                    goto L_mail_join;
                L_mail_next1:
                    ;
                    goto L_mail_join;
                L_mail_next2:
                    ;
                    goto L_mail_join;
                L_mail_next3:
                    ;
                L_mail_join:
                    ;
                }
            }
            getCurCharacQuestW()->finish_quest(questId);
            // [BigP3C 2026-09-12 · 作用域对齐] ORIG 的 qi 析构在 0x866e304（_onQuestClear 之后，
            // 与 list D1 0x326 相邻、p167 序号 #96/97 位于 #93-95 之后）⇒ 原码 qi 与 qz 同作用域
            // （无内层 `{}`）；我方内层块把 D1 提前到 qz 检查之前，调用序号错位 7 处。
            PacketGuard qi;
            getCurCharacQuestR()->get_quest_info((char*)&qi);
            Send(qi);
            Quest* qz = G_CDataManager()->find_quest(questId);
            if (qz != 0 && qz->m_type == 3)
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    this, (APSystem::_ActionGroupIndex)0xe, 0,
                    (ENUM_PACKETCLASS)0);
            m_historyLog.QuestComplete(questId);
            _onQuestClear(questId, opt);
        }
        // ORIG 主体末尾 `jmp 0x866e304`（qi 析构）→ `jmp 0x866e38d`（0x326 处 list D1 后）：
        // 主体路径**跳过**函数尾 tail 块 ⇒ 显式 goto 到 case 出口（list D1 后的 jmp 目标）。
        goto L_case24_done;
    L_fine_quest:
        packet.finalize(true);
        Send(packet);
        HistoryLog::WriteFineQuest((FILE*)m_historyLogFp, questId);
    L_case24_done:
        break;
    }
    default:
        break;
    }
}
