#ifndef GAME_EPISODESCRIPT_H_
#define GAME_EPISODESCRIPT_H_

// df_game_r 还原 —— EpisodeScript（史诗任务脚本条目）+ importEpicQuestScript。
// 布局依据 ORIG 反汇编：
//   clear 0x8a660bc（+0x00 置 -1；+0x04/+0x08 string 置 ""；+0x0c vector clear）
//   C1 0x8a66028（string@+4、string@+8、vector@+0xc 构造后调 clear）
//   D1 0x8a66154（vector@+0xc、string@+8、string@+4 析构）
//   setEpisodeIndex 0x8a66108（打包 (a<<16)|b 写 +0x00）
//   C1ERKS_ 0x8a6a830（逐成员拷贝：int/string/string/vector）
//   sizeof = 0x18。

#include <list>
#include <string>
#include <utility>
#include <vector>

class QuestScript;

class EpisodeScript
{
public:
    EpisodeScript(EpisodeScript const&);  // ORIG 0x8a6a830
    EpisodeScript();                      // ORIG 0x8a66028
    ~EpisodeScript();                     // ORIG 0x8a66154
    void setEpisodeIndex(int, int);       // ORIG 0x8a66108
    void clear();                         // ORIG 0x8a660bc

    int m_episodeIndex;                                  // +0x00 打包索引 ((a<<16)|b)
    std::string m_episodeName;                           // +0x04 [episode_name]
    std::string m_completeText;                          // +0x08 [complete_text]
    std::vector<std::pair<int, QuestScript*> > m_quests; // +0x0c [epic_index] 列表
};

// ORIG 0x8a65ad0（RDARScriptQuest.cpp）：解析史诗任务脚本，逐条目 push_back。
int importEpicQuestScript(std::list<EpisodeScript>* out, const char* path);

#endif  // GAME_EPISODESCRIPT_H_
