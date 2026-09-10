int CDungeonClear::AradfromString(CUser* pUser, const char* str)
{
    // ORIG 0x08483fb9：外层 char_separator(",",0,0) → 外层分隔符为 ","；
    // 构造顺序与 ORIG 一致（先 char_separator，后 std::string s）。
    boost::char_separator<char> sep(",");
    std::string s(str);
    boost::tokenizer<boost::char_separator<char>> tokens(s, sep);

    std::vector<std::string> parts;

    for (boost::tokenizer<boost::char_separator<char>>::iterator tit = tokens.begin();
         tit != tokens.end(); ++tit)
    {
        // ORIG 0x084840b6：内层 "|" 以临时 std::string 传给 arad_parsing。
        arad_parsing(*tit, parts, std::string("|"));

        int dungeonIdx = atoi(parts[0].c_str());
        int diff = atoi(parts[1].c_str());

        if (diff == 4)
            diff = 3;

        CDungeon* pDungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        unsigned char questCleared = 0;
        unsigned int questId = 0;
        if (pDungeon)
            questId = pDungeon->getHeroesRequiredQuest();
        if (questId != 0)
        {
            // UserQuest::m_cleared @+0x0004（CUser.h 权威布局）——具名访问，
            // 禁裸偏移（ORIG 0x084841bc `add $0x4,%eax` 即 &m_cleared）。
            questCleared = pUser->getCurCharacQuestR()->m_cleared.isClearedQuest(questId);
            if (questCleared != 0)
                diff = 3;   // ORIG 0x8484169：quest 已清 → 强制 diff=3
        }

        std::map<short, char>::iterator mit = m_map.find((short)dungeonIdx);
        // ORIG 块布局：`je` 跳更新分支、fallthrough 为插入分支 ⇒ 源码形态
        // `if (it == end) 插入 else if (...)`（ORIG 0x08484233）。
        // 比较用 int（ORIG 08484295 `movzbl; movsbl %al,%edx; cmp %eax,%edx`：
        // char 提升为 int 的 32 位有符号比较），不得写 `(char)diff` 收窄成 8 位 cmp。
        if (mit == m_map.end())
        {
            // ORIG 0x0848424f：make_pair<int&,int&>（不做 short/char 收窄转换），
            // 由 pair<short,char> 转换构造收窄。
            m_map.insert(std::make_pair(dungeonIdx, diff));
        }
        else if (mit->second < diff)
        {
            mit->second = (char)diff;
        }
    }
    return 1;  // ORIG 08484351 `mov $0x1,%ebx` + 尾 `mov %ebx,%eax`
