// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 QuestMovieInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "QuestMovieInfo/importQuestMovieList.inc"
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
        // [laneC] ORIG 0x08917569：pair 先默认构造（C1Ev，槽 -0x40/-0x3c），其后
        // QuestMovieInfo C1Ev（-0x48）⇒ 声明序 key 先 / info 后。
        // ORIG 0x0891756e+0x08917575（初始）与该分支 0x089175c3+0x089175ca 均为
        // 逐成员 `movl $0xffffffff,-0x40; movl $0x3,-0x3c`；原 `key(-1,NONE)` 与
        // `key = std::make_pair(...)` 会多发 pair 转换/赋值调用（P135 反向）。
        std::pair<int, ENUM_QUEST_MOVIE_EVENT> key;
        key.first = -1;
        key.second = QUEST_MOVIE_EVENT_NONE;
        QuestMovieInfo info;
        for (;;)
        {
            if (!ScanType(token, true))
                break;
            if (token == "[/movie play]")
            {
                info.clear();
                key.first = -1;
                key.second = QUEST_MOVIE_EVENT_NONE;
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

#include "QuestMovieInfo/QuestMovieInfo.inc"
    : m_movieName(), m_moviePath()
{
    clear();
}

#include "QuestMovieInfo/QuestMovieInfo_1.inc"
    : m_movieName(other.m_movieName), m_moviePath(other.m_moviePath)
{
}

#include "QuestMovieInfo/dtor_QuestMovieInfo.inc"
{
}

#include "QuestMovieInfo/clear.inc"
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
