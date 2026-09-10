bool CDataManager::load_map_files(std::vector<PVPMAP_DATA>& pvpMaps)
{
    const char* filename = 0;
    STMapScript script;
    CDungeon* dungeon = 0;
    CMap* map = 0;
    std::map<int, std::string>::iterator it;
    std::list<int>::iterator stageIt;
    it = g_stMapScriptFileList.begin();
    std::pair<int, int> pos;
    while (it != g_stMapScriptFileList.end())
    {
        filename = it->second.c_str();
        if (filename == 0)
        {
            break;
        }
        if (it->first == 50000)
        {
            script.Clear();
        }
        else if (!ImportMapScript(&script, filename))
        {
            printf("Error in script('%s')\n", filename);
            return 0;
        }

        // ORIG 同样不释放错误路径上的 CMap（忠实转写）。
        map = new CMap();
        map->set_map(script);

        if (script.m_list4.size() == 0)
        {
            m_mapList->insert_map(15, 15, map, pvpMaps);
            it++;
            continue;
        }

        stageIt = script.m_list4.begin();
        while (stageIt != script.m_list4.end())
        {
            int stageIdx = *stageIt;
            dungeon = m_dungeonList->find_dungeon(stageIdx);
            if (dungeon == 0)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x254b, 5);
                trace("[Load Map] Not exist dungeon. [Index:%d] filename : [%s]",
                      stageIdx, filename);
                return 0;
            }

            if (script.m_recoveryObjects.size() == 0)
            {
                dungeon->InsertMap(15, 15, map);
                m_mapList->insert_map(15, 15, map, pvpMaps);
                if (script.m_dungeonStartArea.m_w > 0)  /* +0x148 = m_dungeonStartArea.m_w（新 0x1c MapArea 吸收旧独立 int）*/
                {
                    dungeon->insertStart(15, map);
                }
            }

            for (int i = 0; i < static_cast<int>(script.m_recoveryObjects.size()); ++i)
            {
                pos = script.m_recoveryObjects[i];
                dungeon->InsertMap(pos.first, pos.second, map);
                m_mapList->insert_map(pos.first, pos.second, map, pvpMaps);
                if (script.m_dungeonStartArea.m_w > 0)  /* +0x148 = m_dungeonStartArea.m_w（新 0x1c MapArea 吸收旧独立 int）*/
                {
                    dungeon->insertStart(pos.first, map);
                }
            }
            stageIt++;
        }
        it++;
    }
    return 1;
