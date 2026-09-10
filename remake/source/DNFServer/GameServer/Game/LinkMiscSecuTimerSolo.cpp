// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LinkMiscSecuTimerSolo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LinkMiscSecuTimerSolo.cpp —— 杂项/安全/定时器域剩余链接期未定义符号补齐。
// 各符号 ORIG 地址注释随注（nm -C df_game_r 核实）；实现逐指令转写自
// objdump。本文件由 LinkMiscSecuTimerSolo 批次注册（cmake/game/CMakeLists.txt）。
// 符号归属：
//   Timer* 域（TimerDispatcher.h 声明；dispatch_sig 已由 TimerDispatcher.cpp 提供）
//   STAIInfoScript（STMonsterScript.h 内联）/ GetLinearDataForMonsterAbility
//   CServerProxyMgr<CGuildServerProxy>::GetServerProxy（模板显式实例化）
//   competition_reward_result ctor（PacketTypes.h）
//   getScriptFileFullPath / findFileWithServerToken（RDAR 脚本域）
//   processModuleTagSet（RDAR 脚本模块标签域）
//   user_creature::CCreatureMgr::CheckDiedCreature（CInventory.h 声明）
//   CAutoMarketConditionsControl::LoadDatabase + GetMarketConditionRate +
//     SendNotiSetDurability 族 + DB_UpdateAutoMarketConditionsControl::makeRequest
// ---------------------------------------------------------------------------
// 撞名 structural 复核（batch-12，nm 实址切片，Main 要求）：
//   * __tcf_0：ORIG 同名 10 枚（每 TU 一枚）。本 guard 表真身 = 0x0896a850
//     （processModuleTagSetPSsRi atexit 引用），size 0x2b 与本 .o __tcf_0
//     0x2b 全等，体 17 条逐条对位（mov base; lea 0x28(eax); mov base; cmp;
//     je; sub $4,%ebx; mov esp; call _ZNSsD1Ev; jmp; add; pop×3; ret）。
//     compare 工具按名取到 0x0828f940（CDeathTowerRanking 域，另一 TU）——
//     索引伪差集（P93/P154 同族），非实现缺口。
//   * _Z41__static_initialization_and_destruction_0ii：本 TU 无全局动态对象
//     （静态局部表走函数内 guard），体 = 纯 ios_base::Init+atexit 16 条
//     （size 0x40），与 ORIG 同形态实例 0x08083c60（0x40/16 条）逐行一致；
//     compare 取到的是他 TU 47 条含对象实例——同名键不同归属，_GLOBAL__I_
//     键名族（P108/P128）结构噪音。
// ============================================================================

#include "TimerDispatcher.h"
#include "TimerQueue.h"

#include "STMonsterScript.h"

#include "CServerProxyMgr.h"
#include "CGuildServerProxy.h"
#include "CGuildServerProxy.h"   // CEventManager 完整定义（CServerProxyMgr 模板析构）

#include "PacketTypes.h"

#include "CInventory.h"          // CCreatureMgr
#include "CCreature.h"           // CCreature::CheckDiedCreature

#include "CDataManager.h"        // CAutoMarketConditionsControl
#include "CEnvironment.h"
#include "GameWorld.h"
#include "GameTypes.h"           // cMyTrace
#include "CreatureScript.h"      // g_moduleNameTag
#include "DNFLexWrapper.h"       // ScanType
#include "LogManager.h"
#include "PackSet.h"             // PackSet::is_exist（ORIG T 0x08ac498c）

#include "GlobalData.h"           // s_stream_pool / s_msgq_mgr / s_systemTime_
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"             // SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL
#include "InterDispatcher.h"   // Inter_MonitorServerEventStart
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"

#include <time.h>
#include <string>
#include <string>
#include <vector>
#include <map>

#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy / ss_strcat

// ---------------------------------------------------------------------------
// RDAR 脚本域全局（定义于 ScriptExtras.cpp；ORIG B 0x949b168 / 0x949b180）
// ---------------------------------------------------------------------------
extern std::vector<std::string> g_szScriptFilePriorityBaseDirectory;
extern char g_szScriptFileBaseDirectory[0x104];
// s_bUseScriptPack 为 ORIG RDARScript.cpp TU-local static（d 0x93f4d24，重建定义
// 于 ScriptExtras.cpp）；跨 TU 经其真实访问器 UseScriptPack（ORIG T 0x088bb7cf）。
extern bool UseScriptPack();  // [b28e P248 档② 两全形] 与 ScriptExtras.cpp 同步：
                              // 定义侧 `static bool s_bUseScriptPack` + `bool` 返回
                              // ⇒ 本体 `movzbl mem; ret` 直返（无归一化、identical 保持）；
                              // 本 TU 调用点 `call; xor $1; test %al; je` = ORIG 同形。
                              // [历史] 仅改返回类型（字段仍 char）会令本体插 test/setne
                              // （ScriptExtras 合计 293→292）⇒ 已按 P248 档② 字段同型化
                              // 消解；调用点仍差 1 条 call vs TU-local movzbl（P189 封顶）。
extern PackSet g_script_package;  // ORIG B 0x9494fe0（OnlyOrigBatch12.cpp 定义）

// ===========================================================================
// findFileWithServerToken  ORIG T 0x88bb7db
// pathStr(path) 副本 + 默认构造 full；find_last_of("/",npos)，无则
// find_last_of("\\",npos)；命中则 full = substr(0,pos+1); full += "(R)";
// full += substr(pos+1,size())；ss_strcpy 回写 path，pack 模式查
// g_script_package.is_exist，否则 access(path,0)==0；命中即 true（path 保留
// full），未命中 ss_strcpy 还原 pathStr 后 false；pos==-1 直接 false。
// [b28e 定稿 · 181=181 gap 0] **P248 档② 源字段同型化（两全形）**：ORIG
// 0x088bb982..0x088bb98e 为 `movzbl 0x93f4d24,%eax; xor $1,%eax; test %al,%al; je`
// （s_bUseScriptPack 与 findFileWithServerToken 在 ORIG 同属 RDARScript.cpp）。
// 重建把 RDAR 域拆两个 TU（定义在 ScriptExtras.cpp），`_ZL16s_bUseScriptPack` 为
// 内部链接 ⇒ 只能经真实访问器 `call _Z14UseScriptPackv`（P189 禁 extern 桥）⇒
// 调用点恒差 1 条 call vs TU-local movzbl（**不可消除**）。
// 三形净账实测（净 identical 红线）：
//   ①char 字段 + char 返回（原状）：182 条 gap +1（`call; test; sete; test; je`）；
//   ②char 字段 + bool 返回：消费点 181/gap 0，但定义侧本体被插 `test; setne`
//     落 DIFF（ScriptExtras 合计 **293→292，identical 账 −1 ⇒ 净负，已回退**）；
//   ③**bool 字段 + bool 返回（现形，P248 档②）**：返回表达式本身已是 bool ⇒
//      本体仍是 `movzbl mem; ret` 直返（identical 保持），消费点
//      `call; xor $1; test %al,%al; je` = 181 条 = ORIG（gap 0）
//      ⇒ ScriptExtras **103/190/0/9 → 103/191/0/8**（ae +1、diff −1、identical 不减）。
// 其余语义（含 pack/PackSet 双分支、full 默认构造+substr 临时 operator= 链）与
// ORIG 切片逐条对位。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/findFileWithServerToken.inc"
}

// ===========================================================================
// getScriptFileFullPath  ORIG T 0x88bba4e
// 依次用优先根目录表（g_szScriptFilePriorityBaseDirectory）拼 dir/name，
// 命中 access/findFileWithServerToken 即采用；否则落到
// g_szScriptFileBaseDirectory。out 尺寸由 size 限制。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/getScriptFileFullPath.inc"
}

// ===========================================================================
// processModuleTagSet  ORIG T 0x08a97637（RDARScriptSkill.cpp 模块标签域）
// 解析 "<module>"/"<sub>" 标签对：moduleId/subId 输出 + 前置 tag 消费。
// type 覆写为 3（ORIG 直接改参数）；标签表 g_moduleNameTag[0x20]（开/闭对）。
// batch-12 逐条对位修正：
//  ① dungeon 分支 ORIG 797 `movzbl -0x15;xor$1;test;je tail` ⇔ subMatched==1
//     跳过赋值直落尾部（return 0），即 `if (!subMatched) {subId=3; log}`；
//     原实现条件反了。
//  ② 扫描子标签 j 循环：ORIG 861 循环体地址 `mov 0x8(%ebp); mov (%eax);
//     add;add1;shl2` 索引 = 2*moduleId+1（固定元素，逐 token 推进 *tag），
//     j（-0xc）仅作循环计数；原实现误用 2*j+1。
//  ③ 尾部消费条件 ORIG 900 `movzbl -0x1c(flag);xor$1;test;je ret1` ⇔
//     flag==0 才进 `if (moduleId != -1) ScanType`，即 `if (!flag && ...)`；
//     原实现条件反了。
//  ④ do 块：ORIG 83a foundSub 初始化仅一次，matched（-0xd）首次写入在
//     ScanType 之后（854），回边条件 `cmpb $0,-0xd; je 岛`（89f）；
//     for(;;)+尾部 if(matched)break 复现该形。
//  ⑤ 命中块第三与条件（tag 相等检查）写嵌套 if：4.4 对三目 && 链尾操作数
//     物化 bool 岛（+5），ORIG 718 为 `call;test;je` 直短路；
//     嵌套形消物化且块排布同 ORIG。
// 表型（Main 批准，batch-12）：g_moduleNameTag 改 [17][2] pair 表（ORIG
// B 0x88 = 17 对，切片 `2*i` 寻址折 `shl $3` stride-8 实证），本函数以
// [i][0]/[i][1] 形寻址逐处对位。残留 N：pvp `type==9||type==5`
// ORIG 4.1 常量岛物化 test%al（N3，4.4 无合法写法）；73b/73e/741 空块
// nop;jmp 中继（跨版本 jump-threading，N）。
// ===========================================================================
// ORIG T 0x08a97637（mangled _Z19processModuleTagSetRiS_PSsib：int&,int& 形参）。
#include "LinkMiscSecuTimerSolo/processModuleTagSet.inc"
}

// processModuleTagSet(std::string*, int&)  ORIG T 0x0896a87b（RDARScriptSkill.cpp）
// 局部 static 开/闭标签对表（函数局部 _ZZ19processModuleTagSetPSsRiE13moduleNameTag，
// guard 惰性构造），5 组 {开,闭} 对：[expert job none] 对（idx 0）不参与主循环。
// i=1..4 命中开标签（arr[i][0]）写 out=i 返回 0；命中闭标签（arr[i][1]）返回 0；
// 否则返回 1。
// 2D 形证据（batch-12）：ORIG guard 段构造/清理链按 pair 记账（outer 计数 -0x54
// 初值 4、pair 基址步进 +8、清理外循环 `sub -0x54;shl $0x3` 步长 8 = 2×string），
// 主体 arr[i][0] 寻址为 `shl $0x3; add base`（stride 8 折叠）；[10] 平表 + `2*i`
// 表达式在 4.4.7 下不产生该形态。[5][2] 形整函数 438 条与 ORIG 逐条对位
// （/tmp/pt2.md diff 仅剩 guard 数据地址与字面量常量，AE 口径归一）。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/processModuleTagSet_1.inc"
}
// GetLinearDataForMonsterAbility  ORIG T 0x08891bae
// diff>=span → high；diff<=0 → low；否则 low + (high-low)*diff/span。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/GetLinearDataForMonsterAbility.inc"
}

// ===========================================================================
// Timer 域注册/生命周期。RegistNextTimer 族（TimerNatType/TimerNatTypeUser/
// TimerQueueSizeLog/TimerStatisticsDetailChannelInfo）已由 TimerDispatcher.cpp
// 权威定义（本文件重复实现于 2026-08-29 删除，避免多重定义）。
// 其余 Timer 生命周期 ctor/dtor 仍在本 TU。
// ===========================================================================

// TimerBossDieTimeout ctor W 0x0863c7fa / dtor W 0x0863fe2e（仅 vptr）。
__attribute__((weak)) TimerBossDieTimeout::TimerBossDieTimeout() throw()
{
}

__attribute__((weak)) TimerBossDieTimeout::~TimerBossDieTimeout() throw()
{
}

// TimerStartVillageAttacked ctor W 0x0863cd92 / dtor W 0x0863ee98。
__attribute__((weak)) TimerStartVillageAttacked::TimerStartVillageAttacked() throw()
{
}

__attribute__((weak)) TimerStartVillageAttacked::~TimerStartVillageAttacked() throw()
{
}

// ===========================================================================
// CServerProxyMgr<CGuildServerProxy>::GetServerProxy 显式实例化
// （ORIG W 0x0811d3b8；模板定义在 CServerProxyMgr.h，缺实例化点）。
// ===========================================================================
template CGuildServerProxy*
CServerProxyMgr<CGuildServerProxy>::GetServerProxy(ENUM_SERVER_GROUP);

// ===========================================================================
// competition_reward_result::competition_reward_result  ORIG W 0x081b47a8
// （+0x00/+0x04 int 清零 + +0x08 字节清零）。
// ===========================================================================
// b28c-l23 修复：兄弟 lane 已给冻结头 PacketTypes.h:2011 该 ctor 声明加
// `throw ()`（P216），本 TU 的定义必须同步异常规范，否则 4.4 报
// "declaration ... throws different exceptions" 整 TU 编译失败。
__attribute__((weak)) competition_reward_result::competition_reward_result() throw()
{
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
}

// ===========================================================================
// user_creature::CCreatureMgr::CheckDiedCreature  ORIG T 0x08339acc：
// m_equippedCreature 非空且其 CheckDiedCreature(m_user) 为真 → true。
// 嵌套 if 形（非 && 短路）：4.4.7 对 `if (a && b) return true;` 会追加 bool
// 物化尾块（+5），ORIG 21 条为统一 leave 双失败出口，嵌套写形与其逐条一致。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/CheckDiedCreature.inc"
}

// ===========================================================================
// GetMarketConditionRate  ORIG T 0x082f843f
// 找市场条件档位：outIdx = 首个 arr[outIdx]*rate > amount 的下标（<=4 扫描；
// 全不满足为 5），钳到 nCount+1；非 0 时 outRate = arr[outIdx+4]。
// P0 判据（ORIG 切片逐条）：无独立 i 局部——循环索引直接走 outIdx 引用槽
// （0x20(%ebp) 每次重读），源形 for(outIdx=0;...;outIdx=outIdx+1)；自增为
// 双读引用 5 条 lea/store 形；钳位比较 RHS 先物化（`lea 0x1(nc);cmp;jge`），
// 即源码写 `nCount + 1 < outIdx`；尾索引 outIdx+4 经 scratch 槽（-0x4）
// store/load 往返。余 fucompp/fnstsw/sahf vs fucomip 为 N（x87 跨版本）。
// [b28c-L23 N85 → 实测可路由翻正] 本件在默认旗标下的 8 行残差中，4 行是
// `fucompp/fnstsw %ax/sahf` vs 4.4 的 `fucomip %st(1),%st/fstp %st(0)`（N85 x87
// 比较两向）。**fresh 实测**：本 TU 加 `-march=i486 -mtune=generic` 后该段完全消失、
// ndiff 8→0（orig=71 ours=71 指令全等，见探针记录）——4.4 默认 i686 目标才发
// fcomi 系。按 Main 路由纪律本波不改 dnf_game_cxx.sh，故现路由下仍记 DIFF(N85)，
// 已把该 TU 列入「4.4.6/i486 路由变更后优先复测」清单。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/GetMarketConditionRate.inc"
}

// ===========================================================================
// MakeNotiSetDurability  ORIG T 0x082f84f8：header(0,0xb3) + put_int + finalize(1)。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/MakeNotiSetDurability.inc"
}

// SendNotiSetDurability(int)  ORIG T 0x082f8540：广播耐久度比率包。
#include "LinkMiscSecuTimerSolo/SendNotiSetDurability.inc"
}

// SendNotiSetDurability()  ORIG T 0x082f85a8：m_durCurrent*100 转 int 后广播。
#include "LinkMiscSecuTimerSolo/SendNotiSetDurability_1.inc"
}

// ===========================================================================
// DB_UpdateAutoMarketConditionsControl  ORIG T 0x08433d70 makeRequest /
// 头 0x15b；SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL（SigTypes.h，pad 0x30）
// 逐字段填充（+0x00..+0x28，DB 侧按此布局读）。
// ===========================================================================
class DB_UpdateAutoMarketConditionsControl
{
public:
    // [SweepTimerDispatcher2] 判据㉓：ORIG 该符号为 **T**（我方原类内 inline ⇒ W）
    // ⇒ 改为类外定义发射强符号（调用点仅本 TU 578 行）。
    static void makeRequest(unsigned long long a, unsigned long long b,
                            unsigned long long c, unsigned long long d,
                            int e, int f, int g);
};

#include "LinkMiscSecuTimerSolo/makeRequest.inc"
}

// ===========================================================================
// CAutoMarketConditionsControl::LoadDatabase  ORIG T 0x082f8684
// （LoadDatabase.md 全量转写）：
//   基准值 = msg+0x68 非 0 ? (msg+0x0,+0x4) :
//             ((msg+0x44,0x48)+(msg+0x3c,0x40)) - ((msg+0x18,0x1c)+(msg+0x10,0x14))
//             + (msg+0x20,0x24)；
//   三次 GetMarketConditionRate（gold/equip/dur 段）；msg+0x38 变化则通知；
//   仅 channel no==1 时 DB_UpdateAutoMarketConditionsControl 上报。
// P0 判据修正（batch-12）：ORIG 帧（sub $0x9c）无 amount/field30/34/38/v1/v2
// 槽——base 为唯一 64 位局部（-0x28/-0x24，有槽：三处 fildll -0x28 复读 +
// trace/diff 复读 ⇒ 具名局部），(double)msg+0x8 在 3 个调用点各自内联转换
// （每点 `mov msg; mov 0x8/0xc; mov scratch pair; fildll; test 高位; jns;
// fldt 2^64; faddp` = 4.1 utodf 形），phase 值直接进参数寄存器（%ecx 单槽
// 即弃）；msg+0x38 在调用参数与 dur 变化比较两处分别重读（O0 无 CSE，P48）。
// base/diff 为 signed long long：ORIG base 转 double 是裸 fildll（无符号
// 修正），diff = 整数 sub/sbb，(ll)diff<0 经 `cmpl $0,-0x1c; jns` 高字判负。
// trace 变参：ORIG 4.1 在 cMyTrace ctor call 前把 %qd/%u/%d 全部求值入
// 槽（-0x64=+0x3e8、-0x60=+0x380）/callee-saved（edi=this[0]、ebx/esi=
// msg+0x8/0xc）（N80 跨版本装载时刻；本侧两具名局部+其余表达式，push 段
// 指令等量，寄存器归属差记 N）。
// [b28e 定稿] 248/246（改前 250/246）；残差 2 条 =
// ①trace msg 双字段 4.1 共享单次基址装载、4.4 每局部重读（+1，N18a 族）；
// ②push 段 4.4 寄存器中转多一跳（+1，N7）。
// **本轮已修 −2**：`long long diff = base - (long long)msg->m_view.m_optimumGold`
// 的**冗余 cast** 是 4.4 RHS-first 预装载（`mov 0xc(%ebp),%eax; mov 0xc(%eax),%edx;
// mov 0x8(%eax),%eax; mov %eax,%ecx; mov %edx,%ebx; mov -slot,%eax; mov -slot+4,%edx;
// sub %ecx,%eax; sbb %ebx,%edx`，多 2 条 regmove）的根因（P41 变体：32 位平台对
// 同宽 signed↔unsigned 的显式 cast 触发 4.4 转换中转）；删 cast 写
// `base - msg->m_view.m_optimumGold` 后为 `mov -slot,%eax; mov -slot+4,%edx;
// mov 0xc(%ebp),%ecx; mov 0xc(%ecx),%ebx; mov 0x8(%ecx),%ecx; sub %ecx,%eax;
// sbb %ebx,%edx` = ORIG 4.1 的 LHS-first 形，逐条对位（250→248）。
// 另：`register unsigned long long optGoldNow = …; (int)(optGoldNow>>32)` 拆字形
// 实测 **252**（多 shift/槽化）⇒ 已回退记 N；帧差 0x10（0xac vs 0x9c）与槽号整体
// 偏移属 N82/N126（版本首槽基址常数）。语义（base 唯一 64 位局部/三处内联 utodf/
// 整数 diff 减法/clamp/channel==1 上报参数）与 ORIG 逐条一致。
// ===========================================================================
// ===========================================================================
// Inter_MonitorServerEventStart::SendEventNotiPacket  ORIG T 0x084cf2de
// 构造 header(0,0x45) + byte(noti) + short(eventIdx) + short(interval) 包；
// user 为空广播全服（GameWorld::send_all），否则单发（CUser::Send）。
// ===========================================================================
#include "LinkMiscSecuTimerSolo/SendEventNotiPacket.inc"
}
#include "LinkMiscSecuTimerSolo/LoadDatabase.inc"
}
