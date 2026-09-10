bool CAICharacterList::init()
{
    if (!sync_script::GetInstanceSyncScript()->truncate_aicharacter_info_table())
    {
        std::puts("Fail truncate_aicharacter_info_table");
        return false;
    }

    m_maxAP = static_cast<int>(0x7fffffff);   // +0x4bc

    // ORIG 0x0834a2cf：fileName 在循环外置 0（P28 声明序 = 槽序）。
    const char* fileName = 0;
    // [b28d-L14 实证] ORIG 0x0834a28c：非 const _Rb_tree_iterator + 默认构造
    // （C1Ev）+ 后置 it++（ppEi）——P50 复活 / P28 增量侧判据。
    std::map<int, std::string>::iterator it;
    STAICharacterCommonScript script;
    it = g_aiCharacterScriptFileList.m_fileMap.begin();
    while (it != g_aiCharacterScriptFileList.m_fileMap.end())
    {
        fileName = it->second.c_str();
        if (fileName == 0)
            break;

        script.clear();

        CAICharacter* aiChar = new CAICharacter;
        if (!importAICharacterCommonScript(&script, fileName, false))
        {
            std::printf("AI charac Script Error: %s\n", fileName);
            delete aiChar;                    // ORIG 0x0834a3b3 test/je + D1 + _ZdlPv
            return false;
        }

        const int fileIdx =
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName);
        if (!sync_script::GetInstanceSyncScript()->insert_aicharacter_info_to_db(fileIdx, script))
        {
            std::printf("insert_aicharacter_category Error: %s\n", fileName);
            return false;                     // ORIG 此路径不释放 aiChar（忠实保留）
        }

        // 独立掉落收集（G_CDataManager()+0xa890 = m_independentDrop.m_vecC）。
        // ORIG 0x0834a4a9：push_back 目标是 script+0xf2c（m_f2c），非局部 vector；
        // 0x0834a4b2 cmp/setg 为有符号比较 ⇒ 下标 int；每字段独立 operator[]
        // （P157，不缓存元素引用）。
        std::vector<stIndependentDropInfo>& drops =
            G_CDataManager()->m_independentDrop.m_vecC;
        for (int i = 0; i < (int)drops.size(); ++i)
        {
            if (drops[i].m_field0 == 1 && drops[i].m_monsterIndex == fileIdx)
                script.m_f2c.push_back(drops[i].m_drop);
        }

        aiChar->set(
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName), script);
        _add(aiChar);
        it++;
    }
    return true;
