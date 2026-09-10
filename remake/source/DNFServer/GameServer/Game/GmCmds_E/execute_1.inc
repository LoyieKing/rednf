void CMonsterDropTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    const char* err = checkCommandExecuteError(party);
    if (err != 0)
    {
        CBoosterTest::SendChatMsg(user, const_cast<char*>(err));
        return;
    }

    CBoosterTest::SendChatMsg(user, const_cast<char*>("Dungeon Monster Drop 결과체크를 시작합니다"));

    if (m_count == 0)
    {
        m_count = 10000;
    }
    char cnt[21];
    sprintf(cnt, "%d번 통계입니다", m_count);
    CBoosterTest::SendChatMsg(user, cnt);

    // ORIG +0x11f：落盘文件名缓冲进入循环前一次性清 255 字节
    //（后续仅由 sprintf 覆盖；非法 mode 无分支写它时仍是空串）。
    // ORIG 0x084acf98 段：fname/fileBuf 头部清零为内联 rep stos（P214③ 大数组
    // 显式零初始化），4.4 对 memset 恒 call（N78）。
    // ORIG 4.1 清零展开计数 ebx=0xff ⇒ fname 为 255 字节（起点 -0x197 非 4 对齐，
    // P312 对齐剥离慢路）。
    char fname[255] = {0};
    char fileBuf[95000] = {0};
    std::map<int, int> drop;
    std::map<int, std::map<int, int> > grade;

    processDungeonDrop(party, drop, grade);

    bool loop = true;
    // ORIG 表头：mode∈{0,2} 走同一段通用表头；mode==3 金币表头；其余无表头。
    // 直证（ORIG 0x084ad0xx）：单次成员读 + 无槽 cmp $2/je、cmp $3/je、test/jne
    // 决策链，0x44 块（case 0）在 0x23 块（case 3）之前 ⇒ 源形是 switch(m_mode)
    // 且 case 按 0、3 书写（P251/P324）。
    switch (m_mode)
    {
    case 2:
    case 0:
        memcpy(fileBuf,
               "item_id\t item_name\t rarity\t drop_cnt\t price\t sell_price\t usable_lv\n",
               0x44);
        break;
    case 3:
        memcpy(fileBuf, "item_name\tgold_quantity\t drop_cnt\n", 0x23);
        break;
    default:
        break;
    }

    while (loop)
    {
        // P251/P0：ORIG 无 mode 具名槽（每轮 `mov [ebp+8]; mov [eax+8]` 单次
        // 直读成员），决策链 cmp $2/je、cmp $3/je、test/jne；叶块物理序 =
        // case 0（fall-through）、case 2、case 3（P324）。
        switch (m_mode)
        {
        case 0:
            sprintf(fname, "single_dungeon_drop_%d.txt", m_count);
            CBoosterTest::makeFileContents(user, fileBuf, drop);
            break;
        case 2:
            sprintf(fname, "passed_dungeon_drop_%d.txt", m_count);
            CBoosterTest::makeFileContents(user, fileBuf, drop);
            break;
        case 3:
            sprintf(fname, "gold_dungeon_drop_%d.txt", m_count);
            makeFileGoldContents(user, fileBuf, drop);
            break;
        default:
            break;
        }

        if (drop.empty())
        {
            loop = false;
        }
        CBoosterTest::printClient(user, loop, fname, fileBuf);
        memset(fileBuf, 0, sizeof(fileBuf));
    }

    if (grade.size() != 0)
    {
        std::string out;
        out.clear();
        for (std::map<int, std::map<int, int> >::iterator it = grade.begin();
             it != grade.end(); it++)   // ORIG ppEi（P28：后置 ++ 带临时槽）
        {
            for (int key = 0; key < 0xb; key++)
            {
                char line[1024] = {0};   // ORIG 内联 rep stos（P214③）
                std::map<int, int>::iterator f = it->second.find(key);
                if (f != it->second.end())
                {
                    sprintf(line, "%5d\t\t%2d\t\t%8d\n", it->first, f->first, f->second);
                    out += line;
                }
            }
            for (int key = 0x3e9; key < 0x3ee; key++)
            {
                char line[1024] = {0};   // ORIG 内联 rep stos（P214③）
                std::map<int, int>::iterator f = it->second.find(key);
                if (f != it->second.end())
                {
                    sprintf(line, "%5d\t\t%2d\t\t%8d\n", it->first, f->first, f->second);
                    out += line;
                }
            }
        }
        CBoosterTest::printClient(user, true, "drop_grade_list.txt", out.c_str());
    }

    CBoosterTest::SendChatMsg(user, const_cast<char*>("Dungeon Monster Drop 결과체크가 완료되었습니다"));
