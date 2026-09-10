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
        QuestMovieInfo info;
        std::pair<int, ENUM_QUEST_MOVIE_EVENT> key(-1, QUEST_MOVIE_EVENT_NONE);
        for (;;)
        {
            if (!ScanType(token, true))
                break;
            if (token == "[/movie play]")
            {
                info.clear();
                key = std::make_pair(-1, QUEST_MOVIE_EVENT_NONE);
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
