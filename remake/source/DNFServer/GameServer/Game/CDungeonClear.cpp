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

// ----------------------------------------------------------------------------
// arad_parsing —— ORIG T 0x08483e98 (size 0x115)
// b28d-60 跨 TU 迁移（P210②，Main 批准）：ORIG TU = DungeonClear.cpp
//（orig_tu_names.tsv:429 GI 键名 `_GLOBAL__I__ZN8WongWork12arad_parsing...`；
//  且 ORIG 中本函数 0x08483e98 结束 0x08483fad 紧接
//  CDungeonClear::AradfromString T 0x08483fae）。原定义在 WongWorkEvents_A.cpp。
// start=find_first_not_of(delims,pos)/end=find_first_of(...)，非全 npos 则循环
// substr 入 vector（move push_back）；尾段 end==npos 但 start!=npos 也输出。
// ----------------------------------------------------------------------------
void arad_parsing(const std::string& src,
                  std::vector<std::string>& out,
                  const std::string& delims)
{
    std::string::size_type start = src.find_first_not_of(delims, 0);
    std::string::size_type end = src.find_first_of(delims, start);
    out.clear();

    while (end != std::string::npos || start != std::string::npos)
    {
        out.push_back(src.substr(start, end - start));
        start = src.find_first_not_of(delims, end);
        end = src.find_first_of(delims, start);
    }
}

int CDungeonClear::AradfromString(CUser* pUser, const char* str)
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
            // UserQuest::m_cleared @+0x0004（CUser.h 权威布局）——具名访问，
            // 禁裸偏移（ORIG 0x084841bc `add $0x4,%eax` 即 &m_cleared）。
            questCleared = pUser->getCurCharacQuestR()->m_cleared.isClearedQuest(questId);
            if (questCleared != 0)
                diff = 3;   // ORIG 0x8484169：quest 已清 → 强制 diff=3
        }

        std::map<short, char>::iterator mit = m_map.find((short)dungeonIdx);
        // ORIG 块布局：`je` 跳更新分支、fallthrough 为插入分支 ⇒ 源码形态
        // `if (it == end) 插入 else if (...)`（ORIG 0x08484233）。
        // 比较用 int（ORIG 08484295 `movzbl; movsbl %al,%edx; cmp %eax,%edx`：
        // char 提升为 int 的 32 位有符号比较），不得写 `(char)diff` 收窄成 8 位 cmp。
        if (mit == m_map.end())
        {
            // ORIG 0x0848424f：make_pair<int&,int&>（不做 short/char 收窄转换），
            // 由 pair<short,char> 转换构造收窄。
            m_map.insert(std::make_pair(dungeonIdx, diff));
        }
        else if (mit->second < diff)
        {
            mit->second = (char)diff;
        }
    }
    return 1;  // ORIG 08484351 `mov $0x1,%ebx` + 尾 `mov %ebx,%eax`
}

void CDungeonClear::enumDungeonClear(CUser* pUser, bool (*fn)(short, char, char*), ...) const
{
    // ORIG 0x084846e2：变参起始指针直接作为回调第三参（char*）。
    // b28d-60：去 `char* va` 二级槽（ORIG 仅 -0x20 一槽）、dungeonIdx 改 int
    //（ORIG `movzwl; cwtl; mov %eax,-0x1c` 四字节存 + 回调处 cwtl 截断）、
    // 去 pDataMgr 局部（ORIG 嵌套实参直传）、回调结果不落 bool 槽（ORIG 直接
    // `call *%eax; xor $1,%eax; test`）、end 缓存进 for 头（P13）。
    va_list ap;
    va_start(ap, fn);

    for (std::map<short, char>::const_iterator it = m_map.begin(),
                                               e = m_map.end();
         it != e; ++it)
    {
        int dungeonIdx = it->first;
        char diff = it->second;

        CDungeon* pDungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        char questCleared = 0;
        unsigned int questId = 0;
        if (pDungeon)
            questId = pDungeon->getHeroesRequiredQuest();
        if (questId != 0)
        {
            // UserQuest::m_cleared @+0x0004（CUser.h 权威布局）——具名访问，
            // 禁裸偏移（ORIG 0x0848478c `add $0x4,%eax` 即 &m_cleared）。
            questCleared = pUser->getCurCharacQuestR()->m_cleared.isClearedQuest(questId);
            if (questCleared != 0)
                diff = 4;
        }

        if (!fn((short)dungeonIdx, diff, ap))
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
