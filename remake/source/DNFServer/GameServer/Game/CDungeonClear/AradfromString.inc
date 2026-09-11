// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
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
        // [L6S3] questId 须为 int（与 CDungeon::getHeroesRequiredQuest 返回同型，
        // 赋值处无转换）；isClearedQuest 形参是 unsigned ⇒ 调用点留一次
        // int→unsigned 转换。此异型是 ORIG `084841ae mov -0xc(%ebp),%ebx`
        //（实参升格 callee-saved 跨 receiver call 保活）+ `084841bc add $0x4,%eax`
        // 的成因；4.4 用 P274/P290 同族「实参侧转换」旋钮复现：探针
        // /tmp/probe_ebx.cpp f6（int + 隐式）与 f2（int + 显式 cast）逐条同形。
        int questId = 0;
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
}