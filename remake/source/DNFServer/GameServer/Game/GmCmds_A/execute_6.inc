void CBoosterTest::execute()
{
    CUser* user = GetUser();

    // [b28d-solo-GmCmds_A] P312：ORIG 对 fileName[0xff](-0x153) 发内联对齐剥离
    // 清零（无 memset call，p167 EXTRA memset）。
    char fileName[0xff] = {0};
    char fileBuf[95000] = {0};

    static char sMsgStart[] = "LOTTERY 아이템 결과체크를 시작합니다";
    CBoosterTest::SendChatMsg(user, sMsgStart);

    if (m_mode == -1)
    {
        static char sNeedIdx[] = "아이템 인덱스를 입력하세요";
        CBoosterTest::SendChatMsg(user, sNeedIdx);
        return;
    }

    CItem* item = G_CDataManager()->find_item(m_mode);
    if (item == 0)
    {
        static char sNoItem[] = "존재하지 않는 아이템입니다";
        CBoosterTest::SendChatMsg(user, sNoItem);
        return;
    }

    if (m_count > 1000000 || m_count < 0)
    {
        static char sRange[] = "0 ~ 1000000번 통계까지만 지원됩니다";
        CBoosterTest::SendChatMsg(user, sRange);
        return;
    }
    if (m_count == 0)
    {
        m_count = 10000;
    }

    char countMsg[21];
    sprintf(countMsg, "%d번 통계입니다", m_count);
    CBoosterTest::SendChatMsg(user, countMsg);

    bool goldLottery = this->is_gold_lottery_item(item->get_index());

    std::map<int, int> result;
    switch (item->GetItemType())                       // ORIG vtbl[3]
    {
    // [b28e-solo-2] P324：ORIG 叶块物理序 = 0xf/0x10 → 6/7/0xd → 0x1a/0x1b
    // → 0x14 → default（ORIG 84aeedb/84aeee5/84aef38/84aef53/84aef82）⇒ 按
    // ORIG 块地址序重排 case（case 顺序不改语义，仅决定 .text 块序）。
    case 0xf:
    case 0x10:
        this->getBoosterResult(item, result);
        break;
    case 6:
    case 7:
    case 0xd:
        // [b28e-solo-2] ORIG 正向 `cmpb $0,-0x1a; je <getJar>`（4.4 对
        // `if (bool)` 直发 cmpb 2 条；`if (!bool)` 恒 xor 物化 3 条，N122）+
        // get_gold 块在前 ⇒ 写正极性。
        if (goldLottery)
        {
            this->get_gold_lottery_result(item, result);
        }
        else
        {
            this->getJarResult(item, result);
        }
        break;
    case 0x1a:
    case 0x1b:
        this->getRandomboxResult(item, result);
        break;
    case 0x14:
        if (!this->getCompoundEmblemResult(item, result))
        {
            static char sNotEmblem[] = "엠블렘 합성 대상이 아닙니다.";
            CBoosterTest::SendChatMsg(GetUser(), sNotEmblem);
        }
        break;
    default:
    {
        static char sNotLottery[] = "LOTTERY 아이템이 아닙니다";
        CBoosterTest::SendChatMsg(GetUser(), sNotLottery);
        return;
    }
    }

    bool more = true;
    memcpy(fileBuf, "item_id\titem_name\trarity\titem_cnt\tprice\tsell_price\tusable_lv\n",
           0x3e);
    while (more)
    {
        // [b28e-solo-2] 同 switch：ORIG `cmpb $0,-0x1a; je <makeFileContents>`
        // 且 make_file_gold_contents 块在前 ⇒ 正极性。
        if (goldLottery)
        {
            CBoosterTest::make_file_gold_contents(user, fileBuf, result);
        }
        else
        {
            CBoosterTest::makeFileContents(user, fileBuf, result);
        }
        sprintf(fileName, "lottery_%d_%d.txt", m_mode, m_count);
        if (result.empty())
        {
            more = false;
        }
        CBoosterTest::printClient(user, more, fileName, fileBuf);
        memset(fileBuf, 0, sizeof(fileBuf));
    }

    static char sDone[] = "LOTTERY 아이템 결과체크가 완료되었습니다";
    CBoosterTest::SendChatMsg(user, sDone);
