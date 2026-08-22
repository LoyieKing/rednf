#ifndef DNF_GAME_CGUILD_AGIT_MANAGER_H_
#define DNF_GAME_CGUILD_AGIT_MANAGER_H_

// df_game_r 还原 —— CGuildAgitManager（公会驻地分区管理器，G2-4 批次）。
// 原类定义位于 CGuildAgitManager.cpp 内部；因 CGameManager.cpp 需真实调用
// AllocGuildAgitArea（原 asm 桥 _ZN17CGuildAgitManager18AllocGuildAgitAreaEj）
// 且 Village.cpp 需 SetGuildAgitAreaDomain，拆到本头作为唯一声明点
// （CGuildAgitManager.cpp / Village.cpp 亦 include）。
// 布局（与 CSchoolMgr/CSchoolArea 同构）：
//   CGuildAgitManager：+0x00 std::map<uint,uint> m_guildCountMap
//                      +0x18 CGuildAgitArea m_area（sizeof 0x28）
//   CGuildAgitArea：+0x00 std::map<uint,int> m_areaMap
//                   +0x18 std::stack<int> m_areaStack

#include <map>
#include <stack>

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

// 全局单例（定义在 CGameManager.cpp）
extern CGuildAgitManager* g_guildAgitMgr;

#endif  // DNF_GAME_CGUILD_AGIT_MANAGER_H_
