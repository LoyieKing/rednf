bool Village::set_village(TownScript& script)
{
    bool result = false;

    m_villageId = script.m_field0;
    if (script.m_18 == -1)
    {
        m_bEnabled = false;
    }
    else
    {
        m_bEnabled = true;
        void* dungeon = G_CDataManager()->get_dungeon(script.m_18);
        // §4.3 具名化：get_dungeon 的 ORIG 实体是 find_dungeon 转发
        // （CDataManager.cpp:300 返 CDungeon*），+0x6d4 具名成员 = CDungeon::m_vec6d4
        // （CDungeon.h:749，std::vector<int>，public 区）。
        std::vector<int>* v = &static_cast<CDungeon*>(dungeon)->m_vec6d4;
        v->push_back(m_villageId);
    }

    m_areaCount = (int)script.m_areas.size();
    if (m_areaCount == 0)
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__, 0x2d0,
            "town.townAreaScripts_.size() == 0");
        return false;
    }

    bool bSchool = G_GameWorld()->IsSchoolPvPChannel() != 0 &&
                   script.m_field0 == 2;
    if (bSchool)
    {
        int count = m_areaCount;
        int total = count + 600;
        int* raw = (int*)::operator new[]((total * 0x26 + 1) * 4);
        raw[0] = total;
        Area* pArea = (Area*)(raw + 1);
        for (int i = total - 1; i != -1; --i)
        {
            ::new (pArea) Area();
            pArea = pArea + 1;   // §4.3：步长由 Area 真实尺寸给出（sizeof==0x98 探针实证）
        }
        m_pAreas = (Area*)(raw + 1);
        g_schoolMgr.SetSchoolArea(m_areaCount, m_areaCount + 600);
    }
    else if (script.m_field0 == 8)
    {
        int count = m_areaCount;
        int total = count + 0x960;
        int* raw = (int*)::operator new[]((total * 0x26 + 1) * 4);
        raw[0] = total;
        Area* pArea = (Area*)(raw + 1);
        for (int i = total - 1; i != -1; --i)
        {
            ::new (pArea) Area();
            pArea = pArea + 1;   // §4.3（同上）
        }
        m_pAreas = (Area*)(raw + 1);
        // [b28d-sweep] 对象语义（ORIG 0x86c3685 `movl $0x943e0e0,(%esp)`，符号 B 0x58）
        g_guildAgitMgr.SetGuildAgitAreaDomain(m_areaCount, total);
    }
    else
    {
        int count = m_areaCount;
        int* raw = (int*)::operator new[]((count * 0x26 + 1) * 4);
        raw[0] = count;
        Area* pArea = (Area*)(raw + 1);
        for (int i = count - 1; i != -1; --i)
        {
            ::new (pArea) Area();
            pArea = pArea + 1;   // §4.3（同上）
        }
        m_pAreas = (Area*)(raw + 1);
    }

    m_field2c = script.m_3c;
    m_questIdx = script.m_40;

    TownAreaScript temp;
    bool bSuccess = false;

    std::map<int, TownAreaScript>::iterator it = script.m_areas.begin();
    for (;;)
    {
        if (it == script.m_areas.end())
            break;

        {
            TownAreaScript copy(it->second);
            temp = copy;
        }

        int areaIndex = temp.m_areaIndex;
        if (areaIndex < 0 || areaIndex >= m_areaCount)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x32e, 5);
            trace("temp_area.areaIndex_ %d", areaIndex);
        }
        else
        {
            STMapScript mapScript;
            const char* mapName = temp.m_mapName.c_str();
            if (ImportMapScript(&mapScript, mapName) == 1)
            {
                m_pAreas[areaIndex].set_area(m_villageId, temp, mapScript);
                if (G_GameWorld()->IsSchoolPvPChannel() != 0 &&
                    script.m_field0 == 2 && areaIndex == 4)
                {
                    for (int i = m_areaCount; i < m_areaCount + 600; ++i)
                    {
                        temp.m_areaIndex = i;
                        m_pAreas[i].set_area(m_villageId, temp, mapScript);
                    }
                    m_areaCount += 600;
                }
                if (script.m_field0 == 8 && areaIndex < 4)
                {
                    int nEnd = m_areaCount + 0x960;
                    int nStart = 0;
                    switch (areaIndex)
                    {
                    case 0:
                        nStart = m_areaCount;
                        break;
                    case 1:
                        nStart = m_areaCount + 1;
                        break;
                    case 2:
                        nStart = m_areaCount + 2;
                        break;
                    case 3:
                        nStart = m_areaCount + 3;
                        m_areaCount += 0x960;
                        break;
                    }
                    for (int i = nStart; i < nEnd; i += 4)
                    {
                        temp.m_areaIndex = i;
                        m_pAreas[i].set_area(m_villageId, temp, mapScript);
                    }
                }
                bSuccess = true;
            }
            else
            {
                result = false;
                bSuccess = false;
            }
            if (!bSuccess)
                goto cleanup;
        }

        ++it;
    }
    result = true;

cleanup:
    return result;
