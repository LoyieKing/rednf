#include "LogManager.h"
// ============================================================================
// df_game_r Village（G2-5 村庄对象，sizeof 0x34）
// 逐函数对照 docs/class_func_reports/Village.md 与 ORIG 反汇编还原。
// 布局（ORIG ctor 086c32e0 / set_village 086c33b8 推导）：
//   +0x00 int    m_villageId      （set_village 写入 town script +0）
//   +0x04 pad（4）
//   +0x08 map<ushort,CUser*> m_users（0x18 字节，ctor 调 _ZNSt3map...C1Ev）
//   +0x20 bool   m_bEnabled
//   +0x24 int    m_areaCount
//   +0x28 Area*  m_pAreas         （new[] 数组，前 4 字节存元素个数）
//   +0x2c int    m_field2c        （set_village 写入 town script +0x3c）
//   +0x30 int    m_questIdx        （set_village 写入 town script +0x40；CheckMoveTown 读作 quest idx）
// 跨类调用一律 include 真实头（CDataManager.h / GameWorld.h / GameTypes.h /
// PacketGuard.h / TownAreaScript.h），std::map/vector 使用真实容器，
// 不再使用 extern asm 桥。
// ============================================================================

#include "GameTypes.h"        // cMyTrace
#include "CDataManager.h"     // G_CDataManager / CDataManager::get_dungeon
#include "PacketGuard.h"      // PacketGuard
#include "TownAreaScript.h"   // TownAreaScript
#include "CSchoolMgr.h"       // CSchoolMgr::SetSchoolArea / g_schoolMgr
#include "CGuildAgitManager.h" // CGuildAgitManager::SetGuildAgitAreaDomain / g_guildAgitMgr
#include "GameWorld.h"        // GameWorld::IsSchoolPvPChannel / G_GameWorld
#include "Village.h"          // Village（本 TU 实现）
#include "Area.h"             // Area（0x98 权威头）
#include "STQuestScript.h"    // TownScript（0x44 权威头）
#include "CMap.h"             // STMapScript（0x37c 权威头）

#include <map>
#include <new>
#include <string>
#include <vector>

// ImportMapScript（ORIG 0x089dcf54，自由函数；定义于 STMapScript.cpp，无权威头）
bool ImportMapScript(STMapScript* script, const char* path);

Village::Village()
{
    // m_users（map）由编译器隐式构造（ORIG 086c32e0：map C1 于 +0x08）
    m_pAreas = 0;
}

Village::~Village()
{
    destroy();
    // m_users（map）由编译器隐式析构（ORIG：destroy() 后 map D1）
}

void Village::destroy()
{
    if (m_pAreas)
    {
        if (m_pAreas)
        {
            int count = ((int*)m_pAreas)[-1];
            Area* pEnd = (Area*)((char*)m_pAreas + count * 0x98);
            while (pEnd != m_pAreas)
            {
                pEnd = (Area*)((char*)pEnd - 0x98);
                pEnd->~Area();
            }
            ::operator delete[]((char*)m_pAreas - 4);
        }
        m_pAreas = 0;
    }
}

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
        std::vector<int>* v = (std::vector<int>*)((char*)dungeon + 0x6d4);
        v->push_back(m_villageId);
    }

    m_areaCount = (int)script.m_areas.size();
    if (m_areaCount == 0)
    {
        LogManager::logFormat(
            1, "world.cpp", "bool Village::set_village(TownScript&)", 0x2d0,
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
            pArea = (Area*)((char*)pArea + 0x98);
        }
        m_pAreas = (Area*)(raw + 1);
        g_schoolMgr->SetSchoolArea(m_areaCount, m_areaCount + 600);
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
            pArea = (Area*)((char*)pArea + 0x98);
        }
        m_pAreas = (Area*)(raw + 1);
        g_guildAgitMgr->SetGuildAgitAreaDomain(m_areaCount, total);
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
            pArea = (Area*)((char*)pArea + 0x98);
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
            cMyTrace trace("bool Village::set_village(TownScript&)",
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
}

int Village::get_gate_area()
{
    for (int i = 0; i < m_areaCount; ++i)
    {
        if (m_pAreas[i].m_areaType == 1)
            return i;
    }
    return 1;
}

Area* Village::getArea(int area)
{
    if (area < 0 || area >= m_areaCount)
    {
        cMyTrace trace("Area* Village::getArea(int)", 0x356, 5);
        trace("area(%d) < 0 || area >= m_iAreaCount(%d)",
              area, m_areaCount);
        return 0;
    }
    return &m_pAreas[area];
}

void Village::get_user_id_list(
    int area,
    std::vector<unsigned short, std::allocator<unsigned short> >& list)
{
    if (area < 0 || area >= m_areaCount)
    {
        LogManager::logFormat(
            1, "world.cpp",
            "void Village::get_user_id_list(int, std::vector<short unsigned "
            "int, std::allocator<short unsigned int> >&)",
            0x36a, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        m_pAreas[area].get_user_id_list(list);
    }
}

void Village::send_to_area(int area, PacketGuard& pkt)
{
    if (area < 0 || area >= m_areaCount)
    {
        LogManager::logFormat(
            1, "world.cpp", "void Village::send_to_area(int, PacketGuard&)",
            0x360, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        m_pAreas[area].send_to_all(pkt);
    }
}

int Village::GetUserCount(int area) const
{
    if (area < 0 || area >= m_areaCount)
        return 0;
    return m_pAreas[area].GetUserCount();
}
