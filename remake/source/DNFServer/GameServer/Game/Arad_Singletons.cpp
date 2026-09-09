// ============================================================================
// df_game_r 还原 —— ARAD::Singleton<T> 显式实例化（唯一 owner）。
//
// ORIG nm --defined-only df_game_r 证据（13 个 Singleton<X> 实例）：
//   每个被使用的实例自然发射 W { Singleton() C1/C2, Get() } + Get()::obj/guard (+ V typeinfo)，
//   GetPtr() 只有【一个】实例有：W _ZN4ARAD9SingletonI16HeroMissionEventE6GetPtrEv @0x0811c9c1
//   （体 = call 其 Get() @0x0811c9ce，两者紧邻 ⇒ 同 TU 发射）。
//   ⇒ 原码从不写 `template class Singleton<X>;`（那会连带发出从未使用的 GetPtr）；
//     HeroMissionEvent 的 GetPtr 由真实调用点驱动：全局 `D events @0x936fa60`（size 0x62c =
//     79 × sizeof(EventTableEntry){int id;int apply_type;CEventBase* base;const char* name;
//     const char* explain}，我方建模在 CEventManager.cpp:604）的动态初始化里
//     0x0811b8d9 `call 811c9c1 <Singleton<HeroMissionEvent>::GetPtr()>`，返回值写入 0x9370058。
//     写入点归位为 events[76].base ——【已验证，非推断】，两条独立反证：
//       (1) 穷举 stride：0x0811a7a4 的 __static_init 内共 79 次 `mov %eax, <events 区间>`，
//           目标偏移 = 8,28,48,…即 (16k+8)，**全部 ≡ 8 (mod 0x14)**、下标 0..78 连续无缺；
//           1528(=0x5f8) mod 20 = 8 ⇒ 唯一自洽解释是「+0x08 字段 = base」的下标 (1528-8)/20=76。
//           若把该字段读成 +0x00/+0x04/+0x0c/+0x10 之一，则 (1528-Δ)/20 均非整数 ⇒ 排除。
//       (2) 内容对号：.data 里 events[76].name == "HeroMissionEvent"（id=164），而 78 次
//           `new + XxxEvent::XxxEvent()` 的类名与各自行 name 字符串 78/78 逐一相等；
//           76 是**唯一**不以 new 填充、而由 GetPtr() 取址的表项（HeroMissionEvent 是单例，
//           不参与 new）——类型名与被取单例的模板实参完全吻合。
//
// 重建侧手法：取消全部整类显式实例化，逐个降级为【成员级】显式实例化
//   `template Singleton<X>::Singleton();` + `template X* Singleton<X>::Get();`
//   （+ 仅 HeroMissionEvent 追加 `template X* Singleton<X>::GetPtr();`），
//   与 HeroMissionEvent.cpp:74 既有写法同族。模板体保持原形（不删不改、无 attribute 伪形）。
//   实测 GCC 4.4.7 -m32 -O0 -std=gnu++98：成员级实例化发 C1/C2 + Get + obj/guard，
//   未列出的 GetPtr 一发不发（/tmp/probe5.o）；列出时以模板体发射为 W，与 ORIG 链接性一致
//   （/tmp/probe6.o）。
// 本文件为唯一定义点，禁止他处重复。
// ============================================================================

#include "Arad_DataManager.h"        // ARAD::Singleton 模板、ARAD::ScriptManager、
                                     // ARAD::Arad_DataManager

#include "HeroMissionEvent.h"
#include "AvatarConvertServer.h"
#include "GlobalData.h"
#include "RestrictGeolocation.h"     // RestrictGeolocation 权威
#include "ServiceRestrictManager.h"  // ServiceRestrictManager 权威
#include "EmblemCompoundServer.h"
#include "LevelupSupportEventManger.h"
#include "CUser.h"                   // CerashopAddRestrict::Manager

namespace ARAD
{
// HeroMissionEvent：ORIG 13 个实例里唯一还发射 GetPtr() 的一个。ORIG 侧其实体由
// CEventManager.cpp 的 events[76].base 初始化驱动（上方证据 (1)(2)）；我方该表建模为零
// 初始化、暂无等价填充路径（差异已作为可观察行为问题单独向 Main 立项，归属 CEventManager.cpp
// 泳道），故此处先以成员级实例化补发该符号——同一模板体、同为 W、反汇编 13 字节逐指令一致，
// 本 TU 改动不引入任何可观察行为变化。后续若 CEventManager.cpp 泳道把 events[] 的 .data 初值
// 与启动期 new+ctor 填 base（含 events[76].base = GetPtr()）补回 ORIG 形态，这行 GetPtr 成员级
// 实例化应当删除以让符号回到自然宿主——但**该删除动作由 Main 统一下指令**：在新宿主落地之前
// 提前删会立刻造成 _ZN4ARAD9SingletonI16HeroMissionEventE6GetPtrEv 变 only_orig 缺口。
template Singleton<HeroMissionEvent>::Singleton();
template HeroMissionEvent* Singleton<HeroMissionEvent>::Get();
template HeroMissionEvent* Singleton<HeroMissionEvent>::GetPtr();

// 以下 9 个实例在 ORIG 无 GetPtr ⇒ 只实例化被实际发射的两个成员（基类构造 + 单例访问）。
template Singleton<AvatarConvertServer>::Singleton();
template AvatarConvertServer* Singleton<AvatarConvertServer>::Get();

template Singleton<RestrictGeolocation>::Singleton();
template RestrictGeolocation* Singleton<RestrictGeolocation>::Get();

template Singleton<EmblemCompoundServer>::Singleton();
template EmblemCompoundServer* Singleton<EmblemCompoundServer>::Get();

template Singleton<ServiceRestrictManager>::Singleton();
template ServiceRestrictManager* Singleton<ServiceRestrictManager>::Get();

template Singleton<LevelupSupportEventManger>::Singleton();
template LevelupSupportEventManger* Singleton<LevelupSupportEventManger>::Get();

template Singleton<LevelupSupport2ndEventManger>::Singleton();
template LevelupSupport2ndEventManger* Singleton<LevelupSupport2ndEventManger>::Get();

template Singleton<CerashopAddRestrict::Manager>::Singleton();
template CerashopAddRestrict::Manager* Singleton<CerashopAddRestrict::Manager>::Get();

template Singleton<ScriptManager>::Singleton();
template ScriptManager* Singleton<ScriptManager>::Get();

template Singleton<Arad_DataManager>::Singleton();
template Arad_DataManager* Singleton<Arad_DataManager>::Get();
}  // namespace ARAD
