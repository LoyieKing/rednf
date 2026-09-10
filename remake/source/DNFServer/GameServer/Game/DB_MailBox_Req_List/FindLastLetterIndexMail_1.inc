int FindLastLetterIndexMail(SIG_MAILBOX_LIST* list, int letterId)
{
    // [b28d-solo-DB_MailBox_Req_List] 两循环各自 **for 内声明** i/v（ORIG 槽
    // -0x10/-0xc 与 -0x8/-0x4 两套独立槽，同一声明复用会并槽）、命中写 ret 后
    // break（ORIG 尾 `mov ret,%eax; leave; ret` 单出口）。
    // [round5-A29] postal 取值改**权威头具名嵌套视图**（同 IsContainedLetter 依据，
    // ORIG 0x0841d080 `mov -0x10(i),%eax; mov 0x8(list),%edx; imul $0x61; lea;
    // add $0x50; mov 0xc(%eax)`「下标先、基址后」形）。
    int maxIdx = 0;
    for (int i = 0; i < list->m_box.m_count; ++i)
    {
        int v = (int)list->m_box.m_postals[i].m_tail.m_occ.m_letterId;
        // ORIG 0x0841d0ae `mov v,%eax; cmp letterId,%eax; jle` ⇒ 写法取
        // `v > letterId`（4.4 对 `letterId < v` 恒折 `mov letterId,%eax;
        // cmp v,%eax; jge`，jcc 方向相反，探针 /tmp/p2/cmp.cpp c1/c2）。
        if (v > letterId && maxIdx < v)
            maxIdx = v;
    }
    int ret = -1;
    if (maxIdx != 0)
    {
        for (int i = 0; i < list->m_box.m_count; ++i)
        {
            int v = (int)list->m_box.m_postals[i].m_tail.m_occ.m_letterId;
            if (v == maxIdx)
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
