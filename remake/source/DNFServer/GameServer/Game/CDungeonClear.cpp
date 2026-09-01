#include "CUserCharacInfo.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "QuestClear.h"  // WongWork::CQuestClear 唯一权威定义
#include "WongWorkEvents_A.h"  // WongWork::arad_parsing（ORIG 0x08483e98）

#include <boost/tokenizer.hpp>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

namespace WongWork
{

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
    // ORIG 0x08483fb9：外层 char_separator(",",0,0) → 外层分隔符为 ","；
    // 构造顺序与 ORIG 一致（先 char_separator，后 std::string s）。
    boost::char_separator<char> sep(",");
    std::string s(str);
    boost::tokenizer<boost::char_separator<char>> tokens(s, sep);

    std::vector<std::string> parts;

    for (boost::tokenizer<boost::char_separator<char>>::iterator tit = tokens.begin();
         tit != tokens.end(); ++tit)
    {
        // ORIG 0x084840b6：内层 "|" 以临时 std::string 传给 arad_parsing。
        arad_parsing(*tit, parts, std::string("|"));

        int dungeonIdx = atoi(parts[0].c_str());
        int diff = atoi(parts[1].c_str());

        if (diff == 4)
            diff = 3;

        CDungeon* pDungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        unsigned char questCleared = 0;
        unsigned int questId = 0;
        if (pDungeon)
            questId = pDungeon->getHeroesRequiredQuest();
        if (questId != 0)
        {
            // CQuestClear 位于 UserQuest 内嵌 +4 处（CListTables.cpp 同型声明）
            questCleared =
                ((WongWork::CQuestClear*)((char*)pUser->getCurCharacQuestR() + 4))
                    ->isClearedQuest(questId);
            if (questCleared != 0)
                diff = 3;   // ORIG 0x8484169：quest 已清 → 强制 diff=3
        }

        std::map<short, char>::iterator mit = m_map.find((short)dungeonIdx);
        if (mit != m_map.end())
        {
            if (mit->second < (char)diff)
                mit->second = (char)diff;
        }
        else
        {
            // ORIG 0x0848424f：make_pair<int&,int&>（不做 short/char 收窄转换），
            // 由 pair<short,char> 转换构造收窄。
            m_map.insert(std::make_pair(dungeonIdx, diff));
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
            written = snprintf(buf + offset, bufSize - offset, ",%d|%d", (int)it->first, (int)it->second);
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
            snprintf(buf, sizeof(buf), ",%d|%d", (int)it->first, (int)it->second);
        }
        out += buf;
    }
}

} // namespace WongWork
