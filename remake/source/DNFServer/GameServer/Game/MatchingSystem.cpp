#include <cstring>

#include "PvPTypes.h"   // PvP 子对象权威声明（原 PvP_deps.h 已并入）
#include "MatchRoomList.h"   // IMatch 基类

// ---- CMatchingSystem（ORIG 0x18 = IMatch* m_systems[6]，懒加载匹配子系统单例） ----
// 布局/语义依据 ORIG 0x085dfac6（Ctor）、0x085dfaf4（Dtor）、0x085dfba0（GetMatchingSystem）：
//   * 对象持有 6 个 IMatch* 槽位（type 0..5），Ctor 清零。
//   * GetMatchingSystem(type) 按类型懒 `new` 对应具体匹配子系统并缓存于槽位，
//     type > 5 返回 0。
//   * 每个匹配子系统的具体实现类：
//       type 0 -> CNormalMatch               （ORIG new 0x4，空 IMatch 子类 + 独立虚表）
//       type 1 -> CLadderMatch               （ORIG new 0x4，空 IMatch 子类 + 独立虚表）
//       type 2 -> CLeagueMatch               （ORIG new 0xfc，见 docs/class_func_reports/CLeagueMatch/）
//       type 3 -> CTournamentMatch           （ORIG new 0x4，空 IMatch 子类 + 独立虚表）
//       type 4 -> online_preliminary::COnlinePreliminary（ORIG new 0x70）
//       type 5 -> fair_pvp::CFairMatch       （ORIG new 0xc4，见 fair_pvp__CFairMatch/）
//   三个复杂类（CLeagueMatch / COnlinePreliminary / CFairMatch）的完整虚表与
//   构造函数逻辑分属匹配域其它批次（报告已具备），本 TU 仅还原懒加载骨架，
//   该类未交付前对应槽位返回 0（推断，见下方）。

// 空 IMatch 子类（ORIG 构造函数仅调 IMatch::IMatch() 并写独立 vtable，对象尺寸 4）。
class CNormalMatch     : public IMatch { public: CNormalMatch() : IMatch() {}     };
class CLadderMatch     : public IMatch { public: CLadderMatch() : IMatch() {}     };
class CTournamentMatch : public IMatch { public: CTournamentMatch() : IMatch() {} };

// 复杂匹配子系统：定义与构造逻辑未在本批次交付（属其它匹配域批次）。
// 此处仅按 ORIG 符号名预留，避免与前向声明冲突。

CMatchingSystem::CMatchingSystem()
{
    // ORIG 0x085dfac6：m_systems[0..5] 置 0。
    for (int i = 0; i < 6; ++i)
        m_systems[i] = 0;
}

CMatchingSystem::~CMatchingSystem()
{
    // ORIG 0x085dfaf4：i=0..5，对非空槽位经虚析构 delete。
    for (int i = 0; i < 6; ++i)
        delete m_systems[i];
}

void* CMatchingSystem::GetMatchingSystem(ENUM_PVP_MATCHING_TYPE type)
{
    // ORIG 0x085dfba0：type>5 返回 0；否则按类型懒构造并缓存于槽位。
    int t = (int)type;
    if (t > 5)
        return 0;
    if (m_systems[t] != 0)
        return m_systems[t];
    switch (t)
    {
    case 0: m_systems[t] = new CNormalMatch;     break;
    case 1: m_systems[t] = new CLadderMatch;     break;
    case 2:
        // CLeagueMatch（0xfc）：构造逻辑未交付（顶层依赖，见 PvP_Room.cpp 调用链）。
        // 返回 0，待 CLeagueMatch 批次落地后接线【推断】。
        return 0;
    case 3: m_systems[t] = new CTournamentMatch; break;
    case 4:
        // online_preliminary::COnlinePreliminary（0x70）：构造逻辑未交付【推断】。
        return 0;
    case 5:
        // fair_pvp::CFairMatch（0xc4）：构造逻辑未交付【推断】。
        return 0;
    default:
        return 0;
    }
    return m_systems[t];
}
