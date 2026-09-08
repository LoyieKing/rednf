// df_game_r QuestMovieInfo 还原（G3 任务域）。
// 依据 docs/class_func_reports/QuestMovieInfo.md 的完整函数体与布局：
//   ctor 0x089173f0 / operator= 0x08917458 / operator== 0x0891748e / clear 0x089174d4
// 布局：两个连续 std::string（+0x00 / +0x04），类大小 8 字节。
// 注意 ORIG mangled：operator= 参数为非 const 引用（_aSERS_），
// operator== 为非 const 成员（_ZN14QuestMovieInfoeqERKS_）。
// C1ERKS 0x08921fbc / D1 0x0891cbd0 为 ORIG 模板实例化弱符号（隐式生成形态）：
//   拷贝构造=初始化列表逐成员 string 拷贝（P29），析构=成员逆序 + EH 清理。
#include "QuestMovieInfo.h"
#include "DNFLexWrapper.h"   // ScanType / ScanInt / ScanStr
#include "NpcScript.h"       // loadRDARScriptFile

// ORIG T 0x089174f5 importQuestMovieList(map<pair<int,ENUM_QUEST_MOVIE_EVENT>,QuestMovieInfo>&)：
// loadRDARScriptFile("", "Etc/QuestMovieList.etc")；"[movie play]" 块内按
// [quest index]/[play info]/[/play info]/[condition]/[file path]/[title] 填充，
// "[/play info]" 时 insert；[condition] 子串映射事件枚举；成功返 0。
// [b28d-19] N：N82/N93 —— 帧 sub $0x60 vs $0x70 + 局部槽序；ORIG 0x089174f5。
int importQuestMovieList(
    std::map<std::pair<int, ENUM_QUEST_MOVIE_EVENT>, QuestMovieInfo>& out)
{
    if (!loadRDARScriptFile("", "Etc/QuestMovieList.etc"))
        return 0;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return 0;
        if (token != "[movie play]")
            continue;
        QuestMovieInfo info;
        std::pair<int, ENUM_QUEST_MOVIE_EVENT> key(-1, QUEST_MOVIE_EVENT_NONE);
        for (;;)
        {
            if (!ScanType(token, true))
                break;
            if (token == "[/movie play]")
            {
                info.clear();
                key = std::make_pair(-1, QUEST_MOVIE_EVENT_NONE);
            }
            else if (token == "[quest index]")
            {
                ScanInt(&key.first);
            }
            else if (token == "[play info]")
            {
                key.second = QUEST_MOVIE_EVENT_NONE;
                info.clear();
            }
            else if (token == "[/play info]")
            {
                out.insert(std::make_pair(key, info));
            }
            else if (token == "[condition]")
            {
                std::string cond;
                ScanStr(&cond);
                if (cond == "[on accept]")
                    key.second = QUEST_MOVIE_EVENT_ON_ACCEPT;
                else if (cond == "[clear]")
                    key.second = QUEST_MOVIE_EVENT_CLEAR;
                else if (cond == "[on complete]")
                    key.second = QUEST_MOVIE_EVENT_ON_COMPLETE;
            }
            else if (token == "[file path]")
            {
                ScanStr(&info.m_moviePath);
            }
            else if (token == "[title]")
            {
                ScanStr(&info.m_movieName);
            }
        }
    }
}

QuestMovieInfo::QuestMovieInfo()
    : m_movieName(), m_moviePath()
{
    clear();
}

QuestMovieInfo::QuestMovieInfo(const QuestMovieInfo& other)
    : m_movieName(other.m_movieName), m_moviePath(other.m_moviePath)
{
}

QuestMovieInfo::~QuestMovieInfo()
{
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
