#include "CUserCharacInfo.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "QuestClear.h"  // WongWork::CQuestClear 唯一权威定义
#include "WongWorkEvents_A.h"  // WongWork::arad_parsing（ORIG 0x08483e98）
#include "OS_API.h"

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
    // ORIG 帧 0x38（仅一次 12 字节出参调用，其余为死局部）：P31 dummy local
    // 撑帧对齐（ORIG 0x084843fa，ndiff=2 残余即 sub $0x38 vs $0x18）。
    char framePad[0x20];
    (void)framePad;
    AradfromString(pUser, str);
}

int CDungeonClear::toString(char* buf, unsigned int bufSize) const
{
    if (m_map.empty())
    {
        return 0;
    }

    // b28d-60：P50 直构——原「先声明后赋值」触发两个 iterator 默认 ctor
    // C1Ev（帧 0x44 vs ORIG 0x34、+20 insn）。ORIG 顺序：empty → offset/written
    // 清零 → begin 直构 it → 首段 snprintf → ++it → end 直构 → while。
    int offset = 0;
    int written = 0;
    std::map<short, char>::const_iterator it = m_map.begin();
    // ORIG 首段 snprintf 结果直存 -0x10(offset)，无 written 中转（written 仅
    // 在循环内使用，函数头仍需 `int written = 0;` 的 -0xc 清零）。
    offset = OS_API::snprintf(buf, bufSize, "%d|%d",
                              (int)it->first, (int)it->second);
    ++it;
    std::map<short, char>::const_iterator end = m_map.end();
    while (it != end)
    {
        written = OS_API::snprintf(buf + offset, bufSize - offset,
                                   ",%d|%d", (int)it->first,
                                   (int)it->second);
        if (written < 0)
        {
            return -1;
        }
        offset += written;
        ++it;
    }
    return offset;
}

int CDungeonClear::toString(std::string& out, int nUnknown) const
{
    if (m_map.empty())
        return 0;

    // b28d-60：按 ORIG 0x08484558 逐条重写——原实现（clear + resize(+64) 固定
    // 缓冲 + first 标志）与 ORIG 完全不同。ORIG：resize(nUnknown) 一次，
    // curSize(-0x24)=nUnknown、written(-0x20)=0，首段 snprintf("%d|%d") 用
    // (c_str, curSize)，循环内 snprintf(c_str+written, curSize-written, ",%d|%d")，
    // 返回负则 resize(curSize+nUnknown)、curSize+=nUnknown 后重试同元素。
    int curSize = nUnknown;
    out.resize((unsigned int)nUnknown);
    int written = 0;
    int w = 0;
    std::map<short, char>::const_iterator it = m_map.begin();
    written = OS_API::snprintf(const_cast<char*>(out.c_str()),
                               (unsigned int)curSize, "%d|%d",
                               (int)it->first, (int)it->second);
    ++it;
    std::map<short, char>::const_iterator end = m_map.end();
    while (it != end)
    {
        w = OS_API::snprintf(const_cast<char*>(out.c_str()) + written,
                             (unsigned int)(curSize - written), ",%d|%d",
                             (int)it->first, (int)it->second);
        if (w < 0)
        {
            out.resize((unsigned int)(nUnknown + curSize));
            curSize += nUnknown;
            continue;
        }
        written += w;
        ++it;
    }
    return written;
}

} // namespace WongWork
