// ORIG T 0x089174f5 importQuestMovieList(map<pair<int,ENUM_QUEST_MOVIE_EVENT>,QuestMovieInfo>&)：
// loadRDARScriptFile("", "Etc/QuestMovieList.etc")；"[movie play]" 块内按
// [quest index]/[play info]/[/play info]/[condition]/[file path]/[title] 填充，
// "[/play info]" 时 insert；[condition] 子串映射事件枚举；成功返 0。
// [b28d-19] N：N82/N93 —— 帧 sub $0x60 vs $0x70 + 局部槽序；ORIG 0x089174f5。
int importQuestMovieList(
    std::map<std::pair<int, ENUM_QUEST_MOVIE_EVENT>, QuestMovieInfo>& out)
{
    if (!loadRDARScriptFile("", "Etc/QuestMovieList.etc"))
        return 0;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return 0;
        if (token != "[movie play]")
            continue;
        // [laneC] ORIG 0x08917569：pair 先默认构造（C1Ev，槽 -0x40/-0x3c），其后
        // QuestMovieInfo C1Ev（-0x48）⇒ 声明序 key 先 / info 后。
        // ORIG 0x0891756e+0x08917575（初始）与该分支 0x089175c3+0x089175ca 均为
        // 逐成员 `movl $0xffffffff,-0x40; movl $0x3,-0x3c`；原 `key(-1,NONE)` 与
        // `key = std::make_pair(...)` 会多发 pair 转换/赋值调用（P135 反向）。
        std::pair<int, ENUM_QUEST_MOVIE_EVENT> key;
        key.first = -1;
        key.second = QUEST_MOVIE_EVENT_NONE;
        QuestMovieInfo info;
        for (;;)
        {
            if (!ScanType(token, true))
                break;
            if (token == "[/movie play]")
            {
                info.clear();
                key.first = -1;
                key.second = QUEST_MOVIE_EVENT_NONE;
            }
            else if (token == "[quest index]")
            {
                ScanInt(&key.first);
            }
            else if (token == "[play info]")
            {
                key.second = QUEST_MOVIE_EVENT_NONE;
                info.clear();
            }
            else if (token == "[/play info]")
            {
                out.insert(std::make_pair(key, info));
            }
            else if (token == "[condition]")
            {
                std::string cond;
                ScanStr(&cond);
                if (cond == "[on accept]")
                    key.second = QUEST_MOVIE_EVENT_ON_ACCEPT;
                else if (cond == "[clear]")
                    key.second = QUEST_MOVIE_EVENT_CLEAR;
                else if (cond == "[on complete]")
                    key.second = QUEST_MOVIE_EVENT_ON_COMPLETE;
            }
            else if (token == "[file path]")
            {
                ScanStr(&info.m_moviePath);
            }
            else if (token == "[title]")
            {
                ScanStr(&info.m_movieName);
            }
        }
    }
}