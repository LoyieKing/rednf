bool EventCharacterParameterScript::importEventCharacterParameterScript(const char* file_name)
{
    if (!loadRDARScriptFile(g_eventCharacterScriptBaseDirectory, file_name))
        return false;

    std::string token;
    int job = 0;
    int level = 0;
    STEventCharacterInfo_level level_info;
    bool ok;

    if (!ScanType(token, true)) return false;
    if (token == "[level]") {
        level = ScanInt(&ok);
        if (!ok) {
            LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                __FUNCTION__, 0x5f, "file name(%s) job name [%d]", file_name, job);
            return false;
        }
    }
    for (;;) {
        STEventCharacterInfo current;
        for (;;) {
            if (!ScanType(token, true)) break;
            if (token == "[job const]") {
                job = ScanInt(&ok);
                if (ok) continue;
                break;
            }
            if (token == "[grow type]") {
                current.m_growType = (unsigned char)ScanInt(&ok);
                if (ok) continue;
                LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                    __FUNCTION__, 0x79, "file name(%s) job name [%d]", file_name, job);
                return false;
            }
            if (token == "[awakening type]") {
                current.m_secondGrowType = (unsigned char)ScanInt(&ok);
                if (ok) continue;
                LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                    __FUNCTION__, 0x82, "file name(%s) job name [%d]", file_name, job);
                return false;
            }
            if (token == "[skill]") {
                while (true) {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok) break;
                    p.second = ScanInt(&ok);
                    if (!ok) break;
                    current.m_skillList.push_back(p);
                }
                continue;
            }
            if (token == "[item]") {
                while (true) {
                    STEventCharacterItem item;
                    item.m_itemIdx = ScanInt(&ok);
                    if (!ok) break;
                    item.m_count = ScanInt(&ok);
                    if (!ok) break;
                    item.m_reason = ScanInt(&ok);
                    if (item.m_reason < 0 || item.m_reason > 31) {
                        LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                            __FUNCTION__, 0xa4,
                            "file name(%s) upgrade script error [%d] upgrade < 0 || upgrade > 31",
                            file_name, item.m_reason);
                        return false;
                    }
                    if (!ok) break;
                    current.m_itemList.push_back(item);
                }
                continue;
            }
            if (token == "[/job const]") {
                if (level_info.character_info.insert(std::make_pair(job, current)).second == 0) {
                    LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                        __FUNCTION__, 0xb3, "file name(%s) job name [%d]", file_name, job);
                    return false;
                }
                break;
            }
            if (token == "[/level]") {
                if (m_levels.insert(std::make_pair(level, level_info)).second == 0) {
                    LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                        __FUNCTION__, 0xbd, "file name(%s) same level exist[%d]", file_name, level);
                    return false;
                }
                return true;
            }
        }
    }
    return false;
