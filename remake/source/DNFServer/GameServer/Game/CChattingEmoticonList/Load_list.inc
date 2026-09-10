bool CChattingEmoticonList::Load_list()
{
    // [b26L17 判词 N7] 唯一残差：1 处虚调用 callee 装载 %eax↔%edx 互换
    //   （244/244 助记符全等）。
    const char* path = 0;                                  // ORIG -0xc
    std::map<int, std::string>::iterator it =
        g_ChattingEmoticonScriptFileList.m_fileMap.begin();  // ORIG -0x68
    EmoticonType::EmoticonScript script;                   // ORIG -0x7c 环外构造一次

    while (it != g_ChattingEmoticonScriptFileList.m_fileMap.end())
    {
        path = it->second.c_str();          // +0x38：operator->+4 → c_str → -0xc
        if (path == 0)
            break;
        if (!ImportChattingEmoticon(script, path))
        {
            printf("Mission Script Error: %s\n", path);
            return false;
        }
        ChattingEmoticonBase* emo = Factory(script);
        if (emo == 0)
            return false;
        if (m_map.find(emo->GetIndex()) != m_map.end())
        {
            delete emo;
            return false;
        }
        m_map.insert(std::make_pair(emo->GetIndex(), emo));
        std::map<int, std::vector<ChattingEmoticonBase*> >::iterator vit =
            m_mapVec.find(script.m_type);
        if (vit == m_mapVec.end())
        {
            std::vector<ChattingEmoticonBase*> vec;
            m_mapVec.insert(std::make_pair(script.m_type, vec));
        }
        m_mapVec[script.m_type].push_back(emo);
        ++it;
    }
    return true;
