#ifndef QUEST_MOVIE_INFO_H
#define QUEST_MOVIE_INFO_H

#include <string>
#include <map>
#include <utility>

// ORIG importQuestMovieList（0x089174f5）脚本条件枚举：0=接取 1=通关 2=完成 3=初始。
enum ENUM_QUEST_MOVIE_EVENT
{
    QUEST_MOVIE_EVENT_ON_ACCEPT = 0,
    QUEST_MOVIE_EVENT_CLEAR = 1,
    QUEST_MOVIE_EVENT_ON_COMPLETE = 2,
    QUEST_MOVIE_EVENT_NONE = 3
};

class QuestMovieInfo
{
public:
    QuestMovieInfo();
    QuestMovieInfo(const QuestMovieInfo& other);
    ~QuestMovieInfo();
    void clear();
    QuestMovieInfo& operator=(QuestMovieInfo& other);
    bool operator==(const QuestMovieInfo& other);

    std::string m_movieName;
    std::string m_moviePath;
};

#endif
