// df_game_r QuestMovieInfo 还原（G3 任务域）。
// 依据 docs/class_func_reports/QuestMovieInfo.md 的完整函数体与布局：
//   ctor 0x089173f0 / operator= 0x08917458 / operator== 0x0891748e / clear 0x089174d4
// 布局：两个连续 std::string（+0x00 / +0x04），类大小 8 字节。
// 注意 ORIG mangled：operator= 参数为非 const 引用（_aSERS_），
// operator== 为非 const 成员（_ZN14QuestMovieInfoeqERKS_）。
#include "QuestMovieInfo.h"

QuestMovieInfo::QuestMovieInfo()
    : m_movieName(), m_moviePath()
{
    clear();
}

void QuestMovieInfo::clear()
{
    m_movieName.clear();
    m_moviePath.clear();
}

QuestMovieInfo& QuestMovieInfo::operator=(QuestMovieInfo& other)
{
    m_movieName = other.m_movieName;
    m_moviePath = other.m_moviePath;
    return *this;
}

bool QuestMovieInfo::operator==(const QuestMovieInfo& other)
{
    return m_movieName == other.m_movieName &&
           m_moviePath == other.m_moviePath;
}
