// ============================================================================
// ArrangeMailAndLetterIndex —— ORIG 0x0841d16d：vecNew 按 letter_id 排序后，从
//   size-count 处起逐项：找 postal 区中 letter_id 更大的最新邮件（FindLast
//   LetterIndexMail）并删除（RemoveMailByLatestLetter），同时从 vecOld 删同
//   letter_id 项。恒返 1。
// ============================================================================
// [b28d-solo-DB_MailBox_Req_List] verdict=DIFF（111 vs 109），N 封顶两项：
// ①ORIG 0x0841d1fc `cmpl $0xffffffff,-0x10; setne %al; test %al,%al; je` 为
//   4.1.2 槽比 setcc 物化，4.4 对槽比恒直跳 `je`（P197④；`bool z=(x!=-1)` 会多槽）
//   ⇒ 不可复现；②letterId 取用 cast 视图（保留 ORIG `add $0x50; mov 0xc(%eax)`
//   两段形）的「基址先」装载序（P30/P235，同 IsContainedLetter②）。
// 已修：start 改 unsigned（复现 `mov count,%edx; sub %edx,%eax` + seta）、
// operator[] 结果内联作实参（去 letterId 具名槽）、拆两条独立 if（消 && 物化岛）、
// begin/end 迭代器循环头缓存（P13）。语义等价。
// [round5-A29 · C05④ 四步流程复核，2026-09-10 定稿] 按 Main 最新四步判定顺序逐关走过：
//   ① **无消费者死比较？** 否 —— ORIG 0x0841d1fc 的 `setne %al; test %al,%al; je +0x159`
//      有真实消费者（je 跳过 if 体）⇒ 配方㉓ 空 switch 不适用。
//   ② **槽值/表达式物化？** 是 —— ORIG 正是「先落槽再比较」（`mov %eax,-0x10(%ebp)` →
//      `cmpl $0xffffffff,-0x10(%ebp); setne …`，无 movzbl/movsbl 归一）。按 D04 反证
//      （`DB_LoadInventory::dispatch` 259=259 AE）应整块换 **P225 原形**；**本件已实测**
//      `while (mailIdx != -1) { …; break; }`（见下方 b28e-L5 记录：clean 差异 24→24、
//      jcc 相对偏移反 +7 字节）——与 D04 的差别在 D04 的 body 以 `throw` 终止（无回边），
//      本件 body 尾部 `++start` 需真实 break ⇒ 得回边 + jmp 残留，故 P225 原形在此不成立；
//      本件亦无嵌套 if 可拆（推广形不适用）。
//   ③ **callee 返回类型与 ORIG 不一致？** 否 —— `FindLastLetterIndexMail` 两侧同为
//      `int`（返回下标、与 -1 有符号比较，`jle/jge` 有符号序与 ORIG 一致）⇒ 归因 1 不成立。
//   ④ ⇒ 按 **N-88 / P197④ 封顶收工**（不动控制流、不动 callee）。letterId 一项已由本轮
//      P235 具名视图修复（见函数体注释）。
bool ArrangeMailAndLetterIndex(SIG_MAILBOX_LIST* list,
                               std::vector<SIG_LETTER_INFO>& vecNew,
                               int count,
                               std::vector<SIG_LETTER_INFO>& vecOld)
{
    // [M5F_CtorDB 2026-09-11] 新范式回扫（旧判词 N-88/P197④）—— 逐条试过：
    // · P333（首建/查表双检锁状态机）：本件无函数局部 static 表、无 SpinLock/flag，
    //   不适用（ORIG 反汇编 0x0841d16d..d2ef 无 `.bss` 取址 + guard 分支）。
    // · P283/P164（循环落点、goto 底条件）：唯一残差 2 条在 `mailIdx != -1`，
    //   ORIG 0x0841d1fc `cmpl $0xffffffff,-0x10(%ebp); setne %al; test %al,%al;
    //   je +0x159`；4.4 对槽比恒直跳 `je`（P197④）。P225 `while(…!= -1){…break;}`
    //   上一轮已实测无收益（jcc 相对偏移反 +7B）；`bool f=(mailIdx!=-1); if(f)`
    //   会在 4.4 引入槽 store/load + `cmpb`，与 ORIG 的 %al 直物化形**不等价于**
    //   任何源形 ⇒ 不落地（避免拿 3 条换 4 条）。
    // · P337（不可达死块）：ORIG 该区间无独立死块（jcc 落点全可达，逐条核过）。
    // 判定：N 封顶（4.1 槽比 setcc 物化，4.4 无对应后端路径），语义等价。
    std::sort(vecNew.begin(), vecNew.end(), CompareLetterIndex);
    // ORIG 0x0841d1c2 `mov count,%edx; sub %edx,%eax` + 尾测 `cmp start,%eax;
    // seta` ⇒ start 为 unsigned（int 形恒折 `sub mem,%eax` + setg，探针
    // /tmp/p2/sub.cpp a1/a2 对照）。
    unsigned int start = vecNew.size() - count;
    while (vecNew.size() > start)
    {
        int mailIdx = -1;
        // [b28d-solo-DB_MailBox_Req_List] ORIG 把 operator[] 结果直接作实参
        // （0x0841d1e3 `call operator[]; mov (%eax),%eax; mov %eax,0x4(%esp)`），
        // 无 letterId 具名局部；具名会多 1 槽 + 2 条 mov。
        mailIdx = FindLastLetterIndexMail(list, *(int*)&vecNew[start]);
        // [b28e-L5] P225 `while (mailIdx != -1) {…break;}` 试验**无收益**（clean 差异
        // 24→24、jcc 相对偏移反 +7 字节）⇒ 回滚维持原 `if`；ORIG 0x0841d1fc
        // `cmpl $0xffffffff,-0x10(%ebp); setne %al; test %al,%al; je +0x159`
        // 属 P197④ 槽比 setcc 物化（4.4 恒直跳）⇒ N，附地址。
        // [P6S6 2026-09-11 负结论·回滚] 为诱导 ORIG 0x0841d1fc 的 `setne %al;
        //   test %al,%al; je` 物化，本波试 `for (; mailIdx != -1; ) {…break;}`
        //   首测形：ours 109→**113**（ORIG 111，多 2 条 jmp）、块内新增
        //   `jmp;jmp` 岛 ⇒ 劣化回滚；连同早先的 `!(mailIdx == -1)` 反相形一并否证
        //   （物化 setne 属 4.1 专有 lowering，4.4 恒直跳）⇒ 维持 N。
        if (mailIdx != -1)
        {
            // [round5-A29] 同 IsContainedLetter：改权威头具名嵌套视图（ORIG
            // 0x0841d234 段 `imul $0x61; lea; add $0x50; mov 0xc(%eax)`）。
            int letterIdx =
                (int)list->m_box.m_postals[mailIdx].m_tail.m_occ.m_letterId;
            // 拆两条独立 if：ORIG 0x0841d234/0x0841d23e 两处直跳统一失败出口；
            // 合并 && 在 4.4 生 mov$1/jmp/mov$0/test 物化岛（P225 推广反向应用）。
            if (RemoveMailByLatestLetter(list, mailIdx))
            {
                if (letterIdx != 0)
                {
                    // ORIG 在循环头缓存 begin/end 两枚迭代器槽（-0x24/-0x28，
                    // 0x0841d251/0x0841d266 两 call 实证），非每轮重调 end()（P13）。
                    std::vector<SIG_LETTER_INFO>::iterator it = vecOld.begin();
                    std::vector<SIG_LETTER_INFO>::iterator e = vecOld.end();
                    for (; it != e; ++it)
                    {
                        if (*(int*)&*it == letterIdx)
                        {
                            vecOld.erase(it);
                            break;
                        }
                    }
                }
            }
        }
        ++start;
    }
    return 1;
}