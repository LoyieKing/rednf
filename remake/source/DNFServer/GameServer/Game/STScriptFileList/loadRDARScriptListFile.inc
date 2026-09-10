bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* /*unused*/)
{
    if (!loadRDARScriptFile(dir, listPath))
        return 0;
    list->Clear();                 // ORIG 0x088bbda8 call STScriptFileList::Clear（非内联 m_fileMap.clear）

    // [round4-L19 2026-09-09] 判据⑫ widthscan 实测 `clr_b orig=0 our=1` ⇒
    // ORIG 无该 bool 预清（ScanInt 为出参，总会写）⇒ 按 P171「ORIG 不写即删」
    // 去掉 `= false`。
    bool ok;
    std::string line;
    for (;;)
    {
        int idx = ScanInt(&ok);
        if (!ok)
            return 1;
        if (!ScanStr(&line))
            return 1;

        bool isNew = (list->m_fileMap.find(idx) == list->m_fileMap.end());
        if (!isNew && !g_isAllowScriptDuplication)
        {
            // ORIG 0x088bf09：logFormat(0,"../RDARScript.cpp",
            // __FUNCTION__,0x323,"SCRIPT FILE LIST LOAD FAIL!
            // Index Duplicated [%s%s][%d]", dir, listPath, idx)
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 0x323,
                "SCRIPT FILE LIST LOAD FAIL! Index Duplicated [%s%s][%d]",
                dir, listPath, idx);
            return 0;
        }
        int sign = isNew ? 1 : -1;
        list->m_fileMap.insert(std::pair<const int, std::string>(idx * sign, line));
    }
