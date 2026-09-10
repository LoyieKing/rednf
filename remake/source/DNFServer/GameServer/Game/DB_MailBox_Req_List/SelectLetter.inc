bool DB_MailBox_Req_List::SelectLetter(MySQL* db, int slot, int characNo,
                                       unsigned int letterIdx,
                                       SIG_MAILBOX_LIST* out)
{
    (void)slot;
    memset(SIG_PTR(out, 0x101c), 0, 0x230a);
    // [b28d-solo-DB_MailBox_Req_List] ORIG 0x0841d5a8 三处 exec 校验为
    // `call exec; movzbl %al,%eax; mov %eax,-0x40(%ebp); cmpl $0x0,-0x40(%ebp);
    // jne` ⇒ 源形是「int 局部接收 + `== 0` 早返」（不是 `!(unsigned char)exec()`
    // 的 xor 形，探针 /tmp/p2/ex.cpp g1/g2）。
    int bExec;   // ORIG 无前置清零（首次写入在 exec 调用后）

    if (letterIdx == 0)
    {
        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 2);
        bExec = db->exec(true);
        if (bExec == 0)
            return 0;

        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 1);
        bExec = db->exec(true);
        if (bExec == 0)
            return 0;

        db->set_query(
            "seLect letter_id,send_charac_no,send_charac_name,letter_text,unix_timestamp(reg_date),stat from letter where (charac_no=%d and stat = 3) or (charac_no=%d and reg_date>subdate(now(),interval %d day)) order by letter_id",
            characNo, characNo, 0xf);
    }
    else
    {
        db->set_query(
            "seLect letter_id,send_charac_no,send_charac_name,letter_text,unix_timestamp(reg_date),stat from letter where (charac_no=%d and stat = 3) or (charac_no=%d and letter_id > %d and reg_date>subdate(now(),interval %d day)) order by letter_id",
            characNo, characNo, letterIdx, 0xf);
    }

    bExec = db->exec(true);
    if (bExec == 0)
        return 0;

    int oldCount = out->m_box.m_count;   // +0x794
    int oldLetter = SIG_I32(out, SIG_OFF_LETTER_COUNT);  // +0x1018
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = 0;

    int letterCount = db->get_n_rows();
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = letterCount;

    // 三个分流 vector：vec[0] 已含（IsContainedLetter）、vec[1] 新（stat 1/2）、
    // vec[2] stat==3；局部 SIG_LETTER_INFO 缓冲按 ORIG 0x12b。
    std::vector<SIG_LETTER_INFO> vec[3];
    int groupSize[3] = { 0, 0, 0 };

    int i;
    for (i = 0; i < letterCount; ++i)
    {
        if (letterIdx != 0 && oldCount >= oldLetter &&
            out->m_box.m_remain != 0 &&
            (int)vec[0].size() >= oldLetter)
            break;

        if (!db->fetch())
            return 0;


        SIG_LETTER_INFO infoRaw;
        unsigned int letterId = 0;
        int sendCharacNo = 0;
        int regDate = 0;
        short stat = 0;
        memset(&infoRaw, 0, 0x12b);

        if (!db->get_uint(0, letterId)) return 0;
        if (!db->get_int(1, sendCharacNo)) return 0;
        if (!db->get_str(2, SIG_PTR(&infoRaw, 0x08), 0x1d)) return 0;
        if (!db->get_str(3, SIG_PTR(&infoRaw, 0x25), 0x100)) return 0;
        if (!db->get_int(4, regDate)) return 0;
        if (!db->get_short(5, stat)) return 0;


        SIG_U32(&infoRaw, 0x00) = letterId;
        SIG_I32(&infoRaw, 0x04) = sendCharacNo;
        SIG_I32(&infoRaw, 0x125) = regDate;
        *(short*)SIG_PTR(&infoRaw, 0x129) = stat;

        // ORIG 0x0841d900 `movzwl -0x7a(%ebp),%eax; cwtl; cmp $0x1,%eax; jl`
        // vs 4.4 `test %ax,%ax` / `cmp $0x2,%ax`（16 位直比）——`(int)stat`
        // 显式提升实测无效（cwtl 仍 0，指令数不变）⇒ N 封顶。
        if (stat > 0)
        {
            if (stat < 3)
            {
                if (!IsContainedLetter(out, (int)letterId))
                    vec[1].push_back(infoRaw);
                else
                    vec[0].push_back(infoRaw);
            }
            else if (stat == 3)
            {
                vec[2].push_back(infoRaw);
            }
        }
    }

    int n = (int)vec[1].size() + (int)vec[0].size() - SIG_I32(out, SIG_OFF_LETTER_USED);

    if (n > 0)
    {
        ArrangeMailAndLetterIndex(out, vec[1], n, vec[0]);
        if (out->m_box.m_count != 0)
        {
            out->m_box.m_maxPostalId =
                SIG_U32(out, (out->m_box.m_count - 1) * 0x61);
        }
    }

    int adjust = 0;
    if (SIG_I32(out, SIG_OFF_LETTER_COUNT) != 0)
    {
        int k;
        for (k = 0; k < 3; ++k)
            groupSize[k] = (int)vec[k].size();

        if (SIG_I32(out, SIG_OFF_LETTER_USED) < groupSize[0])
            groupSize[0] = SIG_I32(out, SIG_OFF_LETTER_USED);
        if (SIG_I32(out, SIG_OFF_LETTER_USED) <
            out->m_box.m_count + groupSize[1])
        {
            adjust = (out->m_box.m_count + groupSize[1]) -
                     SIG_I32(out, SIG_OFF_LETTER_USED);
            groupSize[1] -= adjust;
        }
        if (groupSize[2] > 10)
            groupSize[2] = 10;
    }

    int outIdx = 0;
    int k;
    for (k = 0; k < 3; ++k)
    {
        unsigned int j;
        for (j = 0; (int)j < groupSize[k]; ++j)
        {
            *(SIG_LETTER_INFO*)SIG_PTR(out, 0x101c + outIdx * 0x12b) = vec[k].at(j);
            ++outIdx;
        }
    }

    SIG_I32(out, SIG_OFF_LETTER_NEW) = groupSize[1];
    int remain = out->m_box.m_remain +
                 std::max(adjust - SIG_I32(out, SIG_OFF_LETTER_ADD), 0);
    out->m_box.m_remain = remain;
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = outIdx;
    return 1;
