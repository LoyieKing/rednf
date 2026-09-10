bool ArrangeMailAndLetterIndex(SIG_MAILBOX_LIST* list,
                               std::vector<SIG_LETTER_INFO>& vecNew,
                               int count,
                               std::vector<SIG_LETTER_INFO>& vecOld)
{
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
