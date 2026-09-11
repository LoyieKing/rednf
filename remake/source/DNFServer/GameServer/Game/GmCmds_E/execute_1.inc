// ORIG execute 0x084acf98：主流程（校验 → 分派击杀 → 分模式落盘 → 升级分表输出）。
// b28d-solo-GmCmds_E：AE 达成（identical=123/ae=251 中本件计入 ae；p167 CLEAR
// 59/59）。本次三项直证修正：
// ① fname 改 255 字节（ORIG 4.1 清零展开 `mov $0xff,%ebx` + 起点 -0x197 非 4
//    对齐 ⇒ P312 慢路；原 256 使 fileBuf 起点 -0x178b4 漂 5）；
// ② 表头改 `switch(m_mode)` 且 case 2/0 共用 0x44 块（ORIG 0x084ad116 段
//    `cmp $2; je 0x84ad128(0x44 块)`、`cmp $3; je 0x84ad14c`、`test;jne done`
//    ⇒ 单次成员读 + case 2 与 case 0 同块；原 if 链重读成员且 mode==2 漏表头）；
// ③ while 内分派改 `switch(m_mode)`，case 0/2/3 书写序复现叶块物理序
//    （ORIG 0x084ad178/0x84ad17d 目标 0x84ad1c9/0x84ad207/0x84ad243）。
// 残留仅重定位显示（A vs 段内偏移）与 `@plt` 后缀，compare_tu AE 口径已归零。
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

    CBoosterTest::SendChatMsg(user, const_cast<char*>("Dungeon Monster Drop \xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb8\xa6 \xbd\xc3\xc0\xdb\xc7\xd5\xb4\xcf\xb4\xd9"));

    if (m_count == 0)
    {
        m_count = 10000;
    }
    char cnt[21];
    sprintf(cnt, "%d\xb9\xf8 \xc5\xeb\xb0\xe8\xc0\xd4\xb4\xcf\xb4\xd9", m_count);
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

    CBoosterTest::SendChatMsg(user, const_cast<char*>("Dungeon Monster Drop \xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb0\xa1 \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9"));
}