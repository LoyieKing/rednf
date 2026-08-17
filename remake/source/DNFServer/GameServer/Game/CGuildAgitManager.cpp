// df_game_r 还原 —— CGuildAgitManager（公会驻地分区管理器，G2-4 批次）。
// 无独立类报告：逐函数对照 ORIG 反汇编（ctor 0x856f2c6 / dtor 0x856f146 /
// AllocGuildAgitArea 0x82aa41c / SetGuildAgitAreaDomain 0x86d2642 /
// GetGuildAgitAreaIndex 0x86d2666 / ReleaseGuildAgitArea 0x86d268a）实现，
// 数据流参照 Inter_Notice*GuildAgit 系列报告。
// 布局（与 CSchoolMgr/CSchoolArea 同构）：
//   CGuildAgitManager：+0x00 std::map<uint,uint> m_guildCountMap
//                      +0x18 CGuildAgitArea m_area（sizeof 0x28）
//   CGuildAgitArea：+0x00 std::map<uint,int> m_areaMap
//                   +0x18 std::stack<int> m_areaStack
// 每个驻地占用 4 个连续区号（GetArea 一次弹 4 个，Release 回填 4 个）。
#include <map>
#include <stack>
#include <deque>

// ---------------------------------------------------------------------------
// CGuildAgitArea
// ---------------------------------------------------------------------------
class CGuildAgitArea
{
public:
    CGuildAgitArea();                 // 0x0856f24c W
    ~CGuildAgitArea();                // 0x0856f100 W

    void AllocGuildAgitArea(unsigned int guildKey);   // 0x082aa324 W
    int GetArea();                                    // 0x082aa35a W
    void SetAreaMapping(unsigned int guildKey, int areaIdx);  // 0x082aa3ca W
    void SetGuildAgitAreaDomain(int start, int end);  // 0x086d249e W
    int GetGuildAgitAreaIndex(unsigned int guildKey, int start);  // 0x086d24da W
    void ReleaseGuildAgitArea(unsigned int guildKey);  // 0x086d2542 W

    std::map<unsigned int, int> m_areaMap;   // +0x00
    std::stack<int> m_areaStack;             // +0x18
};

// ---------------------------------------------------------------------------
// CGuildAgitManager
// ---------------------------------------------------------------------------
class CGuildAgitManager
{
public:
    CGuildAgitManager();              // 0x0856f2c6 W
    ~CGuildAgitManager();             // 0x0856f146 W

    void AllocGuildAgitArea(unsigned int guildKey);       // 0x082aa41c W
    void SetGuildAgitAreaDomain(int start, int end);      // 0x086d2642 W
    int GetGuildAgitAreaIndex(unsigned int guildKey, int start);  // 0x086d2666 W
    void ReleaseGuildAgitArea(unsigned int guildKey);     // 0x086d268a W

    std::map<unsigned int, unsigned int> m_guildCountMap;  // +0x00
    CGuildAgitArea m_area;                                 // +0x18
};

// ============================================================================
// CGuildAgitArea 实现
// ============================================================================

CGuildAgitArea::CGuildAgitArea()
    : m_areaMap(), m_areaStack()
{
}

CGuildAgitArea::~CGuildAgitArea()
{
}

void CGuildAgitArea::AllocGuildAgitArea(unsigned int guildKey)
{
    int areaIdx = GetArea();
    if (areaIdx >= 0)
    {
        SetAreaMapping(guildKey, areaIdx);
    }
}

int CGuildAgitArea::GetArea()
{
    if (m_areaStack.empty())
    {
        return -1;
    }
    int areaIdx = m_areaStack.top();
    m_areaStack.pop();
    m_areaStack.pop();
    m_areaStack.pop();
    m_areaStack.pop();
    return areaIdx;
}

void CGuildAgitArea::SetAreaMapping(unsigned int guildKey, int areaIdx)
{
    m_areaMap.insert(std::make_pair(guildKey, areaIdx));
}

void CGuildAgitArea::SetGuildAgitAreaDomain(int start, int end)
{
    for (int area = start; area < end; ++area)
    {
        m_areaStack.push(area);
    }
}

int CGuildAgitArea::GetGuildAgitAreaIndex(unsigned int guildKey, int start)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(guildKey);
    if (it != m_areaMap.end())
    {
        return it->second - start;
    }
    return -1;
}

void CGuildAgitArea::ReleaseGuildAgitArea(unsigned int guildKey)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(guildKey);
    if (it != m_areaMap.end())
    {
        m_areaStack.push(it->second - 3);
        m_areaStack.push(it->second - 2);
        m_areaStack.push(it->second - 1);
        m_areaStack.push(it->second);
        m_areaMap.erase(it);
    }
}

// ============================================================================
// CGuildAgitManager 实现
// ============================================================================

CGuildAgitManager::CGuildAgitManager()
    : m_guildCountMap(), m_area()
{
}

CGuildAgitManager::~CGuildAgitManager()
{
}

void CGuildAgitManager::AllocGuildAgitArea(unsigned int guildKey)
{
    std::map<unsigned int, unsigned int>::iterator it =
        m_guildCountMap.find(guildKey);
    if (it != m_guildCountMap.end())
    {
        it->second += 1;
    }
    else
    {
        m_guildCountMap.insert(std::make_pair(guildKey, 1));
        m_area.AllocGuildAgitArea(guildKey);
    }
}

void CGuildAgitManager::SetGuildAgitAreaDomain(int start, int end)
{
    m_area.SetGuildAgitAreaDomain(start, end);
}

int CGuildAgitManager::GetGuildAgitAreaIndex(unsigned int guildKey, int start)
{
    return m_area.GetGuildAgitAreaIndex(guildKey, start);
}

void CGuildAgitManager::ReleaseGuildAgitArea(unsigned int guildKey)
{
    std::map<unsigned int, unsigned int>::iterator it =
        m_guildCountMap.find(guildKey);
    if (it != m_guildCountMap.end())
    {
        it->second -= 1;
        if (it->second == 0)
        {
            m_guildCountMap.erase(it);
            m_area.ReleaseGuildAgitArea(guildKey);
        }
    }
}
