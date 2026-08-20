#include "CUserCharacInfo.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CDungeon.h"

#include <boost/tokenizer.hpp>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

namespace WongWork
{

// CQuestClear 完整定义见 CListTables.cpp；此处仅声明 isClearedQuest 供链接。
class CQuestClear
{
public:
    bool isClearedQuest(unsigned int questIdx) const;
};

CDungeonClear::CDungeonClear()
{
}

CDungeonClear::~CDungeonClear()
{
}

void CDungeonClear::clear()
{
    m_map.clear();
}

char CDungeonClear::getClearedDungeonDiff(short dungeonIdx) const
{
    std::map<short, char>::const_iterator it = m_map.find(dungeonIdx);
    if (it == m_map.end())
        return 0;
    return it->second;
}

void CDungeonClear::addClearedDungeon(short dungeonIdx, char diff)
{
    std::map<short, char>::iterator it = m_map.find(dungeonIdx);
    if (it == m_map.end())
        m_map.insert(std::make_pair(dungeonIdx, diff));
    else
        it->second = diff;
}

unsigned int CDungeonClear::getDungeonClearSize() const
{
    return m_map.size();
}

void CDungeonClear::AradfromString(CUser* pUser, const char* str)
{
    if (!str)
        return;

    std::string s(str);
    boost::char_separator<char> sep(";");
    boost::tokenizer<boost::char_separator<char>> tokens(s, sep);

    for (boost::tokenizer<boost::char_separator<char>>::iterator it = tokens.begin();
         it != tokens.end(); ++it)
    {
        std::string token = *it;
        std::vector<std::string> parts;
        boost::char_separator<char> pipeSep("|");
        boost::tokenizer<boost::char_separator<char>> pipeTokens(token, pipeSep);

        for (boost::tokenizer<boost::char_separator<char>>::iterator pit = pipeTokens.begin();
             pit != pipeTokens.end(); ++pit)
        {
            parts.push_back(*pit);
        }

        if (parts.size() < 2)
            continue;

        int dungeonIdx = atoi(parts[0].c_str());
        int diff = atoi(parts[1].c_str());

        if (diff == 4)
            diff = 3;

        CDataManager* pDataMgr = G_CDataManager();
        if (pDataMgr)
        {
            CDungeon* pDungeon = pDataMgr->find_dungeon(dungeonIdx);
            if (pDungeon)
            {
                unsigned int questId = pDungeon->getHeroesRequiredQuest();
                if (questId != 0)
                {
                    // Check if quest is cleared
                    // CQuestClear is at CUser + offset + 4 from getCurCharacQuestR
                    // This is a simplified implementation
                }
            }
        }

        std::map<short, char>::iterator it = m_map.find((short)dungeonIdx);
        if (it != m_map.end())
        {
            if (it->second < (char)diff)
                it->second = (char)diff;
        }
        else
        {
            m_map.insert(std::make_pair((short)dungeonIdx, (char)diff));
        }
    }
}

void CDungeonClear::enumDungeonClear(CUser* pUser, bool (*fn)(short, char, char*), ...) const
{
    // ORIG 0x084846e2：变参起始指针直接作为回调第三参（char*）。
    va_list ap;
    va_start(ap, fn);
    char* va = (char*)ap;

    for (std::map<short, char>::const_iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        short dungeonIdx = it->first;
        char diff = it->second;

        CDataManager* pDataMgr = G_CDataManager();
        CDungeon* pDungeon = pDataMgr->find_dungeon(dungeonIdx);
        char questCleared = 0;
        unsigned int questId = 0;
        if (pDungeon)
            questId = pDungeon->getHeroesRequiredQuest();
        if (questId != 0)
        {
            // CQuestClear 位于 UserQuest 内嵌 +4 处（CListTables.cpp 同型声明）
            questCleared = ((WongWork::CQuestClear*)((char*)pUser->getCurCharacQuestR() + 4))
                               ->isClearedQuest(questId);
            if (questCleared != 0)
                diff = 4;
        }

        bool bContinue = fn(dungeonIdx, diff, va);
        if (!bContinue)
            break;
    }
    va_end(ap);
}

void CDungeonClear::fromString(CUser* pUser, char* str)
{
    AradfromString(pUser, str);
}

void CDungeonClear::toString(char* buf, unsigned int bufSize) const
{
    if (!buf || bufSize == 0)
        return;

    buf[0] = '\0';

    if (m_map.empty())
        return;

    unsigned int offset = 0;
    bool first = true;

    for (std::map<short, char>::const_iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        if (offset >= bufSize)
            break;

        int written = 0;
        if (first)
        {
            written = snprintf(buf + offset, bufSize - offset, "%d|%d", (int)it->first, (int)it->second);
            first = false;
        }
        else
        {
            written = snprintf(buf + offset, bufSize - offset, ";%d|%d", (int)it->first, (int)it->second);
        }

        if (written < 0)
            break;

        offset += written;
    }
}

void CDungeonClear::toString(std::string& out, int nUnknown) const
{
    out.clear();

    if (m_map.empty())
        return;

    bool first = true;
    char buf[256] = {0};

    for (std::map<short, char>::const_iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        if (first)
        {
            snprintf(buf, sizeof(buf), "%d|%d", (int)it->first, (int)it->second);
            first = false;
        }
        else
        {
            snprintf(buf, sizeof(buf), ";%d|%d", (int)it->first, (int)it->second);
        }
        out += buf;
    }
}

} // namespace WongWork
