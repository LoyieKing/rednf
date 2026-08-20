// ============================================================================
// df_game_r 还原 —— 匹配房间列表（MatchingSystem 链）实现
// 依据 docs/class_func_reports/{CWaitingRoomList,CLeagueMatchRoomList,
// CMultiMatchRoomList}.md 与 ORIG 汇编（df_game_r，见 MatchRoomList.h）。
// ORIG 符号：
//   CWaitingRoomList：C1 0x0856904e / D1 0x08569062 / find 0x08568872 /
//     erase 0x085688be / insert 0x085688d8 / doMatch 0x08566d9c
//   CLeagueMatchRoomList：C1 0x08568fb0 / AddRoom 0x08567e9e /
//     DelRoom 0x08567f72 / Match 0x08567d00
//   CMultiMatchRoomList：C1 0x08566efc / AddRoom 0x08567954 /
//     DelRoom 0x08567bba / Match 0x085675d0 /
//     GetWaitingRoomCombination 0x0856703c
//   GetUserName 0x08563ff7；fair_pvp::CFairMatch::GetPlayCountPerOneTeam
//   0x08562834（恒返 1）
// 日志依赖：CSwitchLog（_ZN10CSwitchLogC1EPKciii / clEPKcz，CSwitchLog.cpp）、
//   LogManager::logFormat（LogManager.h）。
// ============================================================================

#include <cstdio>

#include "MatchRoomList.h"
#include "CUser.h"
#include "LogManager.h"

// ---- CSwitchLog（PvP 日志；与 PvP_Room.cpp 同款局部声明） ----
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);

private:
    char m_pad[0x10];
};

// ============================================================================
// CWaitingRoomList
// ============================================================================

CWaitingRoomList::CWaitingRoomList()
{
}

CWaitingRoomList::~CWaitingRoomList()
{
}

bool CWaitingRoomList::find(IMatch* room)
{
    return m_roomSet.find(room) != m_roomSet.end();
}

void CWaitingRoomList::erase(IMatch* room)
{
    m_roomSet.erase(room);
}

bool CWaitingRoomList::insert(IMatch* room)
{
    return m_roomSet.insert(room).second;
}

int CWaitingRoomList::doMatch(IMatch* me, std::multimap<float, IMatch*>& out)
{
    if (m_roomSet.empty())
        return 0;
    for (std::set<IMatch*>::iterator it = m_roomSet.begin();
         it != m_roomSet.end(); ++it) {
        IMatch* cand = *it;
        if (cand == me)
            continue;
        if (!me->CheckMatchable(cand))
            continue;
        out.insert(std::make_pair(me->GetGap(cand), cand));
    }
    return out.empty() ? 0 : 1;
}

// ============================================================================
// CLeagueMatchRoomList
// ============================================================================

CLeagueMatchRoomList::CLeagueMatchRoomList()
{
}

bool CLeagueMatchRoomList::AddRoom(IMatch* room)
{
    m_roomMap.insert(std::make_pair(room, 1));
    std::pair<std::map<IMatch*, int>::iterator, bool> ins =
        m_roomMap.insert(std::make_pair(room, 1));
    if (!ins.second)
        ++(ins.first->second);
    return true;
}

bool CLeagueMatchRoomList::DelRoom(IMatch* room)
{
    std::map<IMatch*, int>::iterator it = m_roomMap.find(room);
    if (it != m_roomMap.end()) {
        m_roomMap.erase(it);
        return true;
    }
    return false;
}

bool CLeagueMatchRoomList::Match(IMatch* me, IMatch** out)
{
    if (m_roomMap.empty())
        return false;
    bool found = false;
    float bestGap = 0.0f;
    std::map<IMatch*, int>::iterator bestIt = m_roomMap.end();
    for (std::map<IMatch*, int>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it) {
        IMatch* cand = it->first;
        if (cand == me)
            continue;
        if (!me->CheckMatchable(cand))
            continue;
        float gap = me->GetGap(cand);
        if (!found) {
            bestIt = it;
            bestGap = gap;
            found = true;
        }
        if (gap < bestGap) {
            bestIt = it;
            bestGap = gap;
        }
    }
    if (!found)
        return false;
    *out = bestIt->first;
    m_roomMap.erase(bestIt);
    return true;
}

// ============================================================================
// CMultiMatchRoomList
// ============================================================================

CMultiMatchRoomList::CMultiMatchRoomList()
{
    // 按每队人数（1..GetPlayCountPerOneTeam）预建等待列表；
    // GetPlayCountPerOneTeam() 恒返 1，故实际仅插入 key=1。
    for (int i = 0; i < (fair_pvp::CFairMatch::GetPlayCountPerOneTeam() & 0xff); ++i)
        m_roomMap.insert(std::make_pair(i + 1, CWaitingRoomList()));
}

bool CMultiMatchRoomList::AddRoom(IMatch* room)
{
    char name[255] = {0};
    GetUserName(room, name);
    CSwitchLog log("virtual bool CMultiMatchRoomList::AddRoom(IMatch*)", 0x11ea, 0, 0);
    log("pvp@log match(%x), %s ", room, name);

    for (std::map<int, CWaitingRoomList>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it) {
        if (it->second.find(room)) {
            LogManager::logFormat(1, "MatchingSystem.cpp",
                "virtual bool CMultiMatchRoomList::AddRoom(IMatch*)", 0x11f3,
                "error find line(%d)", 0x11f3);
            return false;
        }
    }
    int playCount = room->GetPlayerCount();
    std::map<int, CWaitingRoomList>::iterator it = m_roomMap.find(playCount);
    if (it == m_roomMap.end()) {
        LogManager::logFormat(1, "MatchingSystem.cpp",
            "virtual bool CMultiMatchRoomList::AddRoom(IMatch*)", 0x11ff,
            "error find line(%d), playCount(%d)", 0x11ff,
            room->GetPlayerCount());
    }
    else {
        it->second.insert(room);
    }
    return true;
}

bool CMultiMatchRoomList::DelRoom(IMatch* room)
{
    char name[255] = {0};
    GetUserName(room, name);
    CSwitchLog log("virtual bool CMultiMatchRoomList::DelRoom(IMatch*)", 0x1209, 0, 0);
    log("pvp@log match(%x), %s ", room, name);

    for (std::map<int, CWaitingRoomList>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it)
        it->second.erase(room);
    return true;
}

bool CMultiMatchRoomList::Match(IMatch* me, IMatch** out)
{
    if (m_roomMap.empty())
        return false;
    std::vector<IMatch*> vec;
    int myCnt = me->GetPlayerCount();
    if (myCnt < fair_pvp::CFairMatch::GetPlayCountPerOneTeam()) {
        if (!GetWaitingRoomCombination(me,
                fair_pvp::CFairMatch::GetPlayCountPerOneTeam() - myCnt, vec))
            return false;
        for (std::vector<IMatch*>::iterator it = vec.begin();
             it != vec.end(); ++it)
            DelRoom(*it);
    }
    if (GetWaitingRoomCombination(me,
            fair_pvp::CFairMatch::GetPlayCountPerOneTeam(), vec)) {
        if (!vec.empty()) {
            if (vec.size() == 1) {
                DelRoom(vec[0]);
                *out = vec[0];
                return true;
            }
            IMatch* prev = vec[0];
            *out = prev;
            for (std::vector<IMatch*>::iterator it = vec.begin() + 1;
                 it != vec.end(); ++it) {
                DelRoom(*it);
                prev->SetNextMatch(*it);
                prev = *it;
            }
            return true;
        }
        return false;
    }
    for (std::vector<IMatch*>::iterator it = vec.begin();
         it != vec.end(); ++it)
        AddRoom(*it);
    return false;
}

int CMultiMatchRoomList::GetWaitingRoomCombination(IMatch* me, int playCount,
                                                   std::vector<IMatch*>& out)
{
    // 与 ORIG 同名的函数内静态常量/组合表（数据导出自 0x8ca3200，
    // 0x8ca35d4 起为四个 1 字节常量）。
    static const unsigned char MAX_SAME_COUNT = 2;
    static const unsigned char MAX_WAITING_ROOM_COUNT = 7;
    static const unsigned char MAX_COMBINATION_COUNT = 10;
    static const unsigned char MAX_REMAIN_WAITING_ROOM = 7;
    static const unsigned char CombinationTable[MAX_REMAIN_WAITING_ROOM]
        [MAX_COMBINATION_COUNT][MAX_WAITING_ROOM_COUNT * 2] = {
        /* playCount=0（无效区，全 0） */
        { {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=1：1 队 x 1 人 */
        { {1,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=2：1x2 / 2x1 */
        { {1,2,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=3：1x3 / 2x1+1x1 / 3x1 */
        { {1,3,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=4：1x4 / 2x1+1x2 / 2x1+2x1 / 3x1+1x1 / 4x1 */
        { {1,4,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,2,0,0,0,0,0,0,0,0,0,0},
          {2,1,2,1,0,0,0,0,0,0,0,0,0,0}, {3,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {4,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=5：1x5 / 2x1+1x3 / 2x2+1x1 / 3x1+2x1 / 3x1+1x2 / 4x1+1x1 */
        { {1,5,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,3,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,1,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=6：1x6 / 2x1+1x4 / 2x2+1x2 / 2x3 / 3x1+1x3 /
           3x1+2x1+1x1 / 4x1+1x2 / 4x1+2x1 / 5x1+1x1 */
        { {1,6,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,4,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,2,0,0,0,0,0,0,0,0,0,0}, {2,3,0,0,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,3,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,1,1,0,0,0,0,0,0,0,0},
          {4,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {5,1,1,1,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
    };

    std::vector<IMatch*> combination[MAX_COMBINATION_COUNT];
    float avgScore[MAX_COMBINATION_COUNT] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    std::multimap<float, IMatch*> matched;
    int resultCount = 0;
    int combIdx = 0;

    while (true) {
        // 行标志（CombinationTable[playCount][row][0]）为 0 或行号越界 → 收尾
        if (combIdx > 9 || CombinationTable[playCount][combIdx][0] == 0) {
            if (resultCount == 0)
                return 0;
            bool found = false;
            int bestIdx = 0;
            float bestAvg = 0.0f;
            for (int i = 0; i < MAX_COMBINATION_COUNT; ++i) {
                if (!combination[i].empty()) {
                    if (!found) {
                        bestIdx = i;
                        bestAvg = avgScore[i];
                        found = true;
                    }
                    if (avgScore[i] == bestAvg) {
                        bestIdx = i;
                        bestAvg = avgScore[i];
                    }
                }
            }
            out.insert(out.end(), combination[bestIdx].begin(),
                       combination[bestIdx].end());
            return 1;
        }
        int totalPlayers = 0;
        for (int j = 0; j < MAX_WAITING_ROOM_COUNT; ++j) {
            unsigned char count = CombinationTable[playCount][combIdx][j * 2];
            if (count == 0)
                break;
            unsigned char min = CombinationTable[playCount][combIdx][j * 2 + 1];
            matched.clear();
            // ORIG 对 find 结果不做 end() 检查（GetPlayCountPerOneTeam=1 时
            // 组合表只使用 key=1 的列表），此处忠实还原。
            std::map<int, CWaitingRoomList>::iterator it = m_roomMap.find(count);
            if (!it->second.doMatch(me, matched) ||
                matched.size() < min)
                break;
            int k = 0;
            for (std::multimap<float, IMatch*>::iterator mit = matched.begin();
                 mit != matched.end() && k < min; ++mit, ++k) {
                avgScore[combIdx] = (totalPlayers * avgScore[combIdx] +
                                     mit->second->GetPlayerCount() * mit->first) /
                                    (float)(mit->second->GetPlayerCount() + totalPlayers);
                totalPlayers += mit->second->GetPlayerCount();
                combination[combIdx].push_back(mit->second);
            }
        }
        if (!combination[combIdx].empty()) {
            if (totalPlayers == playCount)
                ++resultCount;
            else
                combination[combIdx].clear();
        }
        ++combIdx;
    }
}

// ============================================================================
// 自由函数 / 跨类符号
// ============================================================================

char* GetUserName(IMatch* match, char* out)
{
    CUser* u3 = match->GetUser(3);
    const char* n3 = u3 ? u3->get_acc_name() : "";
    CUser* u2 = match->GetUser(2);
    const char* n2 = u2 ? u2->get_acc_name() : "";
    CUser* u1 = match->GetUser(1);
    const char* n1 = u1 ? u1->get_acc_name() : "";
    CUser* u0 = match->GetUser(0);
    const char* n0 = u0 ? u0->get_acc_name() : "";
    sprintf(out, "%s,%s,%s,%s", n0, n1, n2, n3);
    return out;
}

namespace fair_pvp
{
int CFairMatch::GetPlayCountPerOneTeam()
{
    return 1;
}
}
