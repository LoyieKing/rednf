#ifndef QUEST_MOVIE_INFO_H
#define QUEST_MOVIE_INFO_H

#include <string>

class QuestMovieInfo
{
public:
    QuestMovieInfo();
    void clear();
    QuestMovieInfo& operator=(QuestMovieInfo& other);
    bool operator==(const QuestMovieInfo& other);

    std::string m_movieName;
    std::string m_moviePath;
};

#endif
