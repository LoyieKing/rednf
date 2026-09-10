int importEpicQuestScript(std::list<EpisodeScript>* out, const char* path)
{
    if (out == 0)
    {
        return 0;
    }
    if (!loadRDARScriptFile(g_questScriptBaseDirectory, path))
    {
        return 0;
    }

    std::string token;
    std::string token2;
    int local_18 = 0;
    bool ok = false;
    EpisodeScript es;

    while (ScanType(token, true))
    {
        if (token == "[episode]")
        {
            es.clear();
            int a = 0;
            int b = 0;
            a = ScanInt(&ok);
            if (ok != true) break;
            b = ScanInt(&ok);
            if (ok != true) break;
            es.setEpisodeIndex(a, b);
            while (true)
            {
                if (ScanType(token, true) && token == "[/episode]") goto done;
                if (token == "[episode_name]")
                {
                    ScanStr(&es.m_episodeName);
                }
                else if (token == "[epic_index]")
                {
                    while (true)
                    {
                        int idx = ScanInt(&ok);
                        if (!ok) break;
                        std::pair<int, QuestScript*> p;
                        p.first = idx;
                        p.second = 0;
                        es.m_quests.push_back(p);
                    }
                }
                else if (token == "[complete_text]")
                {
                    ScanStr(&es.m_completeText);
                }
                else
                {
                    LogManager::logFormat(0, "../RDARScriptQuest.cpp",
                                          __FUNCTION__, 0x687,
                                          "Unknown Type (%s)", token.c_str());
                    break;
                }
            }
        }
    done:
        out->push_back(es);
    }
    return 1;
