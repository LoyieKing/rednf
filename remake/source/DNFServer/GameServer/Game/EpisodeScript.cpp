// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EpisodeScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "EpisodeScript.h"

#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "STQuestScript.h"  // g_questScriptBaseDirectory

#include "EpisodeScript/EpisodeScript.inc"
{
    clear();
}

#include "EpisodeScript/dtor_EpisodeScript.inc"
{
}

#include "EpisodeScript/EpisodeScript_1.inc"
    : m_episodeIndex(other.m_episodeIndex),
      m_episodeName(other.m_episodeName),
      m_completeText(other.m_completeText),
      m_quests(other.m_quests)
{
}

#include "EpisodeScript/clear.inc"
{
    m_episodeIndex = -1;
    m_episodeName = "";
    m_completeText = "";
    m_quests.clear();
}

#include "EpisodeScript/setEpisodeIndex.inc"
{
    m_episodeIndex = (index << 16) | questIndex;
}

#include "EpisodeScript/importEpicQuestScript.inc"
{
    if (out == 0)
    {
        return 0;
    }
    if (!loadRDARScriptFile(g_questScriptBaseDirectory, path))
    {
        return 0;
    }

    std::string token;
    std::string token2;
    int local_18 = 0;
    bool ok = false;
    EpisodeScript es;

    while (ScanType(token, true))
    {
        if (token == "[episode]")
        {
            es.clear();
            int a = 0;
            int b = 0;
            a = ScanInt(&ok);
            if (ok != true) break;
            b = ScanInt(&ok);
            if (ok != true) break;
            es.setEpisodeIndex(a, b);
            while (true)
            {
                if (ScanType(token, true) && token == "[/episode]") goto done;
                if (token == "[episode_name]")
                {
                    ScanStr(&es.m_episodeName);
                }
                else if (token == "[epic_index]")
                {
                    while (true)
                    {
                        int idx = ScanInt(&ok);
                        if (!ok) break;
                        std::pair<int, QuestScript*> p;
                        p.first = idx;
                        p.second = 0;
                        es.m_quests.push_back(p);
                    }
                }
                else if (token == "[complete_text]")
                {
                    ScanStr(&es.m_completeText);
                }
                else
                {
                    LogManager::logFormat(0, "../RDARScriptQuest.cpp",
                                          __FUNCTION__, 0x687,
                                          "Unknown Type (%s)", token.c_str());
                    break;
                }
            }
        }
    done:
        out->push_back(es);
    }
    return 1;
}
