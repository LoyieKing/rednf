#include <cstring>

#include "CLeagueMatch.h"    // type 2：CLeagueMatch（new 0xfc）
#include "OnlinePreliminary.h"  // type 4：online_preliminary::COnlinePreliminary（new 0x70）
#include "CFairMatch.h"      // type 5：fair_pvp::CFairMatch（new 0xc4）

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
// [b22] 删显式 : IMatch() 初始化列表——4.4 对带用户初始化列表的派生 ctor 先
// 发 vptr=0 预清+重载 this（比 ORIG 4.1 多 2 条）；隐式基类构造路径消之。
class CNormalMatch     : public IMatch { public: CNormalMatch() {}     };
class CLadderMatch     : public IMatch { public: CLadderMatch() {}     };
class CTournamentMatch : public IMatch { public: CTournamentMatch() {} };

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
    // ORIG 0x085dfba0：type>5 返回 0（`ja` 直通直返 eax=0）；否则跳表逐 case 内查
    // 空槽并懒构造缓存于 m_systems[type]，共同出口 `mov (this+idx*4),%eax; mov -0x1c`
    // 走 result 局部（-0x1c）。判空在 case 内，非 switch 前一次性预查。
    // [b21 N·N90] 结构性修正：跳表逐 case 判空 + result 局部（-0x1c）对齐 ORIG。
    //   残余 N90 值初始化 codegen：ORIG(4.1) 对三个 4 字节空 IMatch 子类
    //   （CNormalMatch/CLadderMatch/CTournamentMatch）在 `new <T>()` 后、ctor 前发
    //   `movl $0x0,(%eax)` value-init 预清，4.4 对该 trivial 用户 ctor 类不发（ctor 直设
    //   vptr）。源形不可复现（probe 实证 4.4 恒不预零）。父子母各 case new+ctor+槽位
    //   写回、返回统一走 m_systems[type]，语义闭合，封 N。
    void* result = 0;
    switch (type)
    {
    case 0:
        if (!m_systems[type]) m_systems[type] = new CNormalMatch();
        break;
    case 1:
        if (!m_systems[type]) m_systems[type] = new CLadderMatch();
        break;
    case 2:
        if (!m_systems[type]) m_systems[type] = new CLeagueMatch();
        break;
    case 3:
        if (!m_systems[type]) m_systems[type] = new CTournamentMatch();
        break;
    case 4:
        if (!m_systems[type]) m_systems[type] = new online_preliminary::COnlinePreliminary();
        break;
    case 5:
        if (!m_systems[type]) m_systems[type] = new fair_pvp::CFairMatch();
        break;
    default:
        return 0;
    }
    result = m_systems[type];
    return result;
}
