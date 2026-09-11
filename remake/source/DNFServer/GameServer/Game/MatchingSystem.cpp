// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MatchingSystem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
// [S6 · N2Free151to170 父泳道授权] 再删**用户提供的空 ctor**（保持纯 `: public IMatch {}`）：
//   有用户 ctor ⇒ `new T()` 走 default-init，4.4 不预清零；无用户 ctor（隐式 non-trivial
//   ctor，因基类带虚函数）⇒ `new T()` 走 value-init ⇒ 复现 ORIG 0x085dfbe7/0x085dfc28/
//   0x085dfcbe 的 `mov %ebx,%eax; movl $0x0,(%eax); mov %eax,(%esp); call C1` 预清序列。
//   探针 /tmp/s6/p5.cpp（4.4.7 -O0 -m32）：`struct D1 : Base { };` 发预清，`D1() {}` 不发。
//   TU-local 类，全仓 grep 仅本文件引用；C1 符号名/对象尺寸/vtable 名均不变。
class CNormalMatch     : public IMatch { };
class CLadderMatch     : public IMatch { };
class CTournamentMatch : public IMatch { };

// 复杂匹配子系统：定义与构造逻辑未在本批次交付（属其它匹配域批次）。
// 此处仅按 ORIG 符号名预留，避免与前向声明冲突。

#include "MatchingSystem/CMatchingSystem.inc"


#include "MatchingSystem/dtor_CMatchingSystem.inc"


#include "MatchingSystem/GetMatchingSystem.inc"

