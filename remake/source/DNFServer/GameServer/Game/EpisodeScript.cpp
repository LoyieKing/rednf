// ============================================================================
// df_game_r 还原 —— EpisodeScript（史诗任务脚本条目）+
// importEpicQuestScript（RDARScriptQuest.cpp 解析器）。
// 对照 ORIG 反汇编：
//   EpisodeScript C1 0x8a66028 / D1 0x8a66154 / C1ERKS_ 0x8a6a830
//                clear 0x8a660bc / setEpisodeIndex 0x8a66108
//   importEpicQuestScript 0x8a65ad0（报告
//   docs/class_func_reports/_lt_global_gt_/importEpicQuestScript.md）
// 解析辅助经 DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include "EpisodeScript.h"

#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "STQuestScript.h"  // g_questScriptBaseDirectory

EpisodeScript::EpisodeScript()
{
    clear();
}

EpisodeScript::~EpisodeScript()
{
}

EpisodeScript::EpisodeScript(EpisodeScript const& other)
    : m_episodeIndex(other.m_episodeIndex),
      m_episodeName(other.m_episodeName),
      m_completeText(other.m_completeText),
      m_quests(other.m_quests)
{
}

void EpisodeScript::clear()
{
    m_episodeIndex = -1;
    m_episodeName = "";
    m_completeText = "";
    m_quests.clear();
}

void EpisodeScript::setEpisodeIndex(int index, int questIndex)
{
    m_episodeIndex = (index << 16) | questIndex;
}

int importEpicQuestScript(std::list<EpisodeScript>* out, const char* path)
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
