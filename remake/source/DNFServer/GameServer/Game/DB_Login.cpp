// ============================================================================
// [b28d-38 判词] 本 TU 27 件最终状态（compare_tu_game_orig 口径，ORIG 地址见报告）：
//  CheckLoginIP 0x08415472 AE（SGB->SGBC 宏形 + std::strrchr _ZSt7strrchrPci）
//  CreateAccount 0x08412eb8 NEAR（N18a/P75：hangame_flag 实参 movzbl 目标寄存器/装载序，7 形探针）
//  GetUserFatigueCount 0x084155xx DIFF（N7 间接 call 目标 4.4.7 恒 %eax + 装载序；p167 CLEAR）
//  GetUserInGameEventHistory 0x08415xxx DIFF（P30/N4 add-vs-lea 常量折叠；p167 CLEAR）
//  _isPunishedHackUser 0x0842xxxx DIFF（N82 槽/帧；p167 CLEAR 10/10）
//  RemovePreminuInfo 0x084128c4 DIFF（const_iterator 双端转换已修，消混合 operator!=；残差槽/物化）
//  UpdateLoginStat 0x0840xxxx DIFF（N88 惰性求值位置+槽；p167 CLEAR 13/13）
//  GetGameDBInfo 0x08414772 DIFF（setcc 物化/N88；p167 CLEAR 5/5）
//  GetLoginCommonData 0x0841090c DIFF（SGB->SGBC 已修；余 setcc 物化 P197④）
//  getCharacLinkSystemData 0x0840xxxx DIFF（块序/N93；p167 CLEAR 16/16）
//  _LoadEventItem 0x08411d7e DIFF（p167 缺迭代器 begin/end/++/->，我方为索引形——待改）
//  GetOntimeUserRecvIdx 0x0841xxxx DIFF（N7+装载序；p167 CLEAR 10/10）
//  IsThisIPLoginPermissionGranted 0x0844b982 NEAR（SGBC/SGFC 宏形已修 116=116；N82 槽）
//  _LoadDNFEventEntry 0x08411bda DIFF（EXTRA tagDNFEventEntry::~ x2+_Unwind_Resume：EH 尾段 N91/N104）
//  _DecryptUserInfo 0x0841300e DIFF（p167 MISSING strlen x2 语义缺口）
//  GetAccountCharacterLevelUpOnceGift 0x0840xxxx DIFF（N7/setcc；p167 CLEAR 11/11）
//  GetBadgeCount 0x0840xxxx DIFF（N82 槽/帧；p167 CLEAR 16/16）
//  GetPunishUserInfo 0x0840xxxx DIFF（setcc/N82；p167 CLEAR 14/14）
//  _getGameDBCharacView 0x084149ce DIFF（SGBC 宏形已修；N82 槽；p167 CLEAR 13/13）
//  GetGrowthCreatureEvent 0x0840xxxx DIFF（N7/setcc；p167 CLEAR 13/13）
//  GetBingoEvent 0x08448bee DIFF（N82 槽；p167 CLEAR 22/22）
//  _getRewardInfo 0x0840xxxx DIFF（N82/P183；p167 CLEAR 18/18）
//  GetMercenarySystemInfo 0x08415618 DIFF（N82；p167 CLEAR 27/27）
//  getBreakAwayMemberData 0x0840xxxx DIFF（N82；p167 CLEAR 44/44）
//  GetPremiumInfo 0x0840xxxx DIFF（p167 MISSING operator-> x21：需去 (*it). 缓存改 it-> 形）
//  dispatch 0x08425374 DIFF（p167 MISSING G_GameWorld() x1 语义缺口）
//  GetAccountInfo 0x0842xxxx DIFF（p167 MISSING NumberToString x2 + EXTRA memset x7 语义缺口）
// ----------------------------------------------------------------------------
// [b28d-solo-DB_Login 最终判词] 单文件泳道收口（check_tu_routed.sh + tu_diff gap 口径）。
// TU 计数：identical=48 ae=129 near=1 diff=19（含非清单件 CCompressedBlob::resize）；
// 本清单 27 件 = AE/IDENTICAL 8 + NEAR 1 + N 封顶 18 + 未解决 0。TU COMPILE OK 0 错误。
//  p167 差集全部 CLEAR（27/27；CheckLoginIP 的 strrchr 符号差除外，见下）。
//  AE/IDENTICAL（8）：
//   CheckLoginIP 0x08415472 AE（SGBC 宏形 + std::strrchr；p167 `_ZSt7strrchrPci` vs
//       `strrchr` = 工具链头 `__CORRECT_ISO_CPP_STRING_H_PROTO` 版本差 N，语义等价）
//   _LoadEventItem 0x08411d7e AE（P167 迭代器化；真语义 bug：Arrange 第二实参
//       ORIG 为 serverId 0x10(%ebp)，旧码误传 &entries）
//   RemovePreminuInfo 0x084128c4 AE（P245 去 static 整帧回正 + P232 dead int 槽 +
//       begin→end 调序 + P225 continue 形复现 exec 后 dead 读 sig+0x2938）
//   _LoadDNFEventEntry 0x08411bda AE（P12/N84 删 tagDNFEventEntry 用户空析构消 2×D1
//       +_Unwind_Resume；for(i<get_n_rows()&&fetch()) 拆 body；P223 五独立 nop 岛；
//       P209 unsigned i 复现 seta）
//   IsThisIPLoginPermissionGranted 0x0844b982 AE（三兄弟块 trace 全表达式临时链 P221/P227）
//   GetLoginCommonData 0x0841090c AE（rows 具名槽 + col++ 递增索引 P191/P0）
//   GetBadgeCount 0x08414512 AE（get_short 直传 sig->m_dailyBadge[i]；P227 链式 trace）
//   _isPunishedHackUser 0x08412ac8 AE（expireSec int 化 P217；if(nowFlag==0)return 0 极性）
//  NEAR/N 封顶（19）：
//   CreateAccount 0x08412eb8 NEAR gap=2（N18a：hangame_flag 实参 4.4 恒 movzbl→%eax
//       中转；P267 双 cast / P268 mask / 字段 uchar 三档试尽不翻）
//   GetUserFatigueCount 0x084150e8 DIFF gap=5（N7 间接 call 目标 4.4.7 恒 %eax + 装载序）
//   GetUserInGameEventHistory 0x084152b0 DIFF gap=9（N1：ORIG `add $0x3fa0; add sig;
//       add $0xc` 三段被 4.4 折 $0x3fac+lea；十形探针既定）
//   _DecryptUserInfo 0x0841300e DIFF gap=6（N7/P75 实参装载序；p167 CLEAR）
//   UpdateLoginStat 0x08414d50 DIFF gap=4（N18a：三条 store 的 RHS-first 中转；
//       P259 消 stream 局部 + P82 异型视图已修，余中转封顶）
//   GetGameDBInfo 0x08414772 DIFF gap=13（N18b 内存直比 + P30/N4 数组寻址序；
//       P0 去 world 局部/P222 正极性包裹/P209 unsigned viewRet/P28 声明序已修）
//   getCharacLinkSystemData 0x0840ffca DIFF gap=1（P223/N：尾 `jmp exit` 岛不可造；
//       P168a 极性翻转 + P227 链式 trace 已修，26→1）
//   GetOntimeUserRecvIdx 0x08414f5c DIFF gap=6（N18a：除法中间值落 -0xc 槽；
//       P48/P0 去 mid 缓存已修，32→6）
//   GetAccountCharacterLevelUpOnceGift 0x0844834e DIFF gap=27（N82 槽/帧；P0/P140 去
//       evt 局部与判空、P158 bool 槽、P222 正极性包裹已修，56→27）
//   GetPunishUserInfo 0x08410cf6 DIFF gap=3（N18a：m_punishInfoCount unsigned 化可翻
//       但会改 ARAD_LoginSecurity/InterDispatcher 三处比较符号性 ⇒ 不采纳）
//   GetGrowthCreatureEvent 0x08448102 DIFF gap=35（N82 槽/帧；P0/P140/P158/P222 已修，66→35）
//   _getGameDBCharacView 0x084149ce DIFF gap=3（N4/P30 数组寻址 add-vs-lea；
//       P253 三缓冲 `={0}` rep stos 清零 + P87/P227 链式 trace 已修，32→3）
//   GetBingoEvent 0x08448bee DIFF gap=4（N7/N4 尾残差；P0/P140/P198/P227 已修 86→4；
//       真语义 bug：ORIG 成功路径落 08448eac `mov $0` ⇒ return 0）
//   _getRewardInfo 0x084115f8 DIFF gap=177（N82 槽/帧 + N78 memset 展开；p167 CLEAR 18/18）
//   GetMercenarySystemInfo 0x08415618 DIFF gap=169（N82 槽/帧 + P30/N4；p167 CLEAR 27/27）
//   getBreakAwayMemberData 0x084101ae DIFF gap=105（N78 对齐 memset 展开 + N82 帧；
//       p167 CLEAR 44/44）
//   GetPremiumInfo 0x08411ee8 DIFF gap=256（N82 槽/帧；P157 去 eventId 缓存后
//       p167 CLEAR 84/84）
//   dispatch 0x08425374 DIFF gap=374（N82 帧 0x40↔0x90；p167 CLEAR 56/56）
//   GetAccountInfo 0x0841325e DIFF gap=455（N78 memset 展开 + N80 varargs + N82 帧；
//       p167 CLEAR 90/90：补 get(7)/get(9) trace 的 NumberToString 语义缺口 x2 +
//       8 处 memset 改 `={0}` 消 EXTRA memset x7）
// 真语义 bug 修复（3）：_LoadEventItem Arrange 第二实参、GetBingoEvent 成功返回值、
//   GetAccountInfo 两处 trace 的 NumberToString 实参。
// .h 改动（2，均已 R2 广播）：DB_Login.h 三函数去 static（P245）；tagDNFEventEntry
//   删用户空析构（P12/N84）。SigTypes_D.h 的 m_loginType uchar 试验已回退（无收益）。
// ----------------------------------------------------------------------------
// [b28d-solo-DB_Login 本轮续修] 20 件清单复核 + 新范式（P307–P320/N115–N124）回扫：
// TU 计数 identical=48 ae=130 near=2 diff=17（含非清单件 CCompressedBlob::resize）；
// 本清单 20 件 = AE 2（CheckLoginIP/UpdateLoginStat）+ NEAR 2（CreateAccount/
// _getGameDBCharacView）+ N 封顶 16 + 未解决 0。TU COMPILE OK 0 错误。
// 本轮改法（6 件，零 .h 改动、零 ABI/mangled 影响）：
//  UpdateLoginStat DIFF(4)→AE：P211 档③ PktView 字段同型化（m_accId→unsigned int、
//      m_gcNo→int）⇒ RHS 直装 %edx 无 eax 中转（探针 probe_store f2/f5）。
//  _getGameDBCharacView DIFF(5)→NEAR(2)：P311 具名 Slot 数组视图（CharacDataView，
//      文件头匿名 namespace）复现 `imul $0x94; lea; add $0xe0` 三步；余 2 行 = i/sig
//      装载序（N82附/P112）。
//  GetBingoEvent DIFF(5)→DIFF(2)：P0 resetBase 声明 unsigned 修实参装载时刻（ORIG
//      NumberToString 后重载 -0x18）；余 N107 lea-vs-add。
//  GetGameDBInfo DIFF(20)→DIFF(17)：P311 槽视图修循环数组寻址；余 N-P209L。
//  GetAccountCharacterLevelUpOnceGift DIFF(32)→DIFF(15)：P0/P183 具名 int* 局部
//      （ORIG -0xc 槽两次回读）替 jobType/level 双局部 + 两次写回。
//  GetGrowthCreatureEvent DIFF(37)→DIFF(17)：P0/P183 具名 char* 行指针（ORIG -0x10）
//      + P232 死 int 局部（ORIG -0xc），四个 getter 直写字段。
// 新负例/判据（已随各函数头判词登记）：
//  N-P209L：具名局部与常量 `==` 比较 4.4 恒折 `cmpl $imm,mem`；P310 的 `<` setcc 形
//      不适用（unsigned/int/long+显式提升+L 后缀四形实测）。
//  N-P133 回退：拆独立守卫（exec/get_n_rows）实测 244→254 反劣，保持合并短路。
//  N107 再证：cast 取址恒 add-vs-lea；同函数无 cast 字段（+0x4138）则 lea 可复现。
//  N1 分配律：(i+0x866)*8+8 被 4.4 折 (i+0x867)*8（probe_gift f5）。
//  N121/P223 边界：尾 `test; jmp exit` + nop 岛，probe_tail fA–fF 六形全败。
// 待 Main 裁决：GetUserFatigueCount 的 N7（4.4.7 间接 call 目标恒 %eax）属后端敏感
//  族——若本 TU 路由到 4.4.6 可试翻，但影响整 TU 其余 177 件，不自行改路由表。
// 探针工件：/tmp/b28d_solo/{probe_cast2,probe_pun,probe_arr,probe_gift,probe_tail,
//   probe_jmp2,probe_store}.cpp（4.4.7 -m32 -O0 实测）。
// 无子子代理（R4 合规）。
// ----------------------------------------------------------------------------
// [S216_DB_Login2 单文件泳道收口] 19 件清单（/tmp/b28d_tu/DB_Login.tsv）最终状态
// （check_tu_routed.sh .o 口径 + show_func_diff ndiff + p167 差集）：
// TU 计数 identical=48 ae=133 near=2 diff=14（含非清单件 CCompressedBlob::resize）；
// 本清单 19 件 = AE/IDENTICAL 4 + NEAR 2 + N 封顶 13 + 未解决 0。COMPILE OK 0 错误。
// p167 调用点差集 19/19 CLEAR（CheckLoginIP 的 `_ZSt7strrchrPci` vs `strrchr` 头版本差除外）。
// 本轮新收 AE（3）：
//   _getGameDBCharacView NEAR(2)→AE：P235 数组视角（SigTypes_D.h 匿名 union
//       m_characView）⇒ 复现 `mov i;mov sig;imul $0x94;lea;add $0xe0` 装载序。
//   GetOntimeUserRecvIdx DIFF(8)→AE：`int mod = (int)(sig->m_accId % 10u);`
//       （P217/P310 类型旋钮）⇒ 4.4 除法 magic 中间值不再 spill 到 -0xc。
//   GetUserInGameEventHistory DIFF(18)→AE：P235/P311 历史表视角
//       （SigTypes_D.h m_ingameEventHistoryView）⇒ 复现 `add $0x3fa0;add sig;add $0xc` 三步链。
// 本轮新收 NEAR（1）：
//   GetUserFatigueCount DIFF(10)→NEAR(8)：NumberToString 内联进 set_query 实参 ⇒
//       occDay `movzbl -0x11(%ebp),%esi` 装载时刻回到 ORIG 位（余 N7 间接 call 目标
//       4.4.7 恒 %eax，4.4.6 后端可翻，属 Main 路由裁决）。
// 本轮实质改善（2）：
//   GetGameDBInfo DIFF(22)→DIFF(6)：P235 视角修 memmove/比较地址链 + int==unsigned
//       异型比较修两段装载；余 N-P209L（`==` 常量比较恒折 cmpl mem；switch 双 case
//       形 gap 0 但 ndiff 8、显式 (unsigned) 提升形均不翻，已实测）。
//   _getRewardInfo DIFF(420)→DIFF(382)：P10/P248 档② flags/latest char→int（`orb`→`orl`）、
//       拆独立守卫 + 空 then 正向形修 execOk 槽物化、`time(&now)` 传址形修 time 调用形态；
//       余 N82 帧 0x10 + N-P133 第二守卫 setne/je 极性 + N78 段。
// N 封顶复核（13 件，逐件范式试尽后维持）：
//   CreateAccount NEAR(4) N18a/P75：P267 双 cast 序 / P268 mask / P226 语句表达式三形
//       本轮再测均不翻（movzbl %al,%eax vs ORIG %edx）。
//   getCharacLinkSystemData DIFF(1) N121：P285 正/负条件空 then+else return 两形实测
//       （正形反劣 gap2，负形与原形逐条同）⇒ epilogue-threading 尾 jmp+nop 岛封顶。
//   _DecryptUserInfo DIFF(12) N7/P75：Hex2Binary/Decrypt 第三实参 cast 互换四形不翻。
//   GetBingoEvent DIFF(3) N107：union{unsigned;int} 同址 int 视图实测仍产 add+mov
//       （探针 s216_probe2 fA/fB）⇒ 成员常量取址 lea-vs-add 封顶。
//   GetPunishUserInfo DIFF(5) N18a/P211：RHS `(unsigned int)get_n_rows()` cast 形实测
//       仍 LHS-first（4.4 折叠同宽 cast）⇒ 字段侧 unsigned 化牵动三泳道，维持封顶。
//   GetAccountCharacterLevelUpOnceGift DIFF(30) N1/N115：P205 显式拆形
//       `(i+0x866)*8+8` 实测仍折 `(i+0x867)*8`。
//   GetGrowthCreatureEvent DIFF(34) N1/N115：同上（0x4150+4 折 0x4154）。
//   GetMercenarySystemInfo DIFF(447) N82/N4：帧 0x60↔0x70、槽 -0x14↔-0x1c、
//       数组 add-vs-imul/lea；setl↔setle 为极性镜像（P196 折痕，非真反）。
//   getBreakAwayMemberData DIFF(272) N78/N82：ORIG `push esi;push ebx;add $N,%esp`
//       大帧 vs 4.4 `sub $0x98`；N78 对齐 memset 展开。
//   GetPremiumInfo DIFF(711) N82：p167 84/84 CLEAR（旧判词 operator-> MISSING 已消）。
//   dispatch DIFF(1014) N82：794 insn 超大口径（§6.2 三件套），p167 56/56 CLEAR，
//       帧 0x40↔0x90、ORIG shl/lea/add 展开族 vs 4.4 setne/sar。
//   GetAccountInfo DIFF(1190) N78/N80/N82：1045 insn 超大口径，p167 90/90 CLEAR。
// P0：本文件 0 处 `register ... asm("reg")` 绑定（清绑历史见 GetGrowthCreatureEvent 头注释）。
// .h 改动（2 处，均已 R2 广播且消费方复测零回归）：SigTypes_D.h
//   ①SIG_LOGIN_DATA::m_characData → 匿名 union + m_characView 数组视角；
//   ②+0x3fa0 区 8 成员 → 匿名 union + m_ingameEventHistoryView 历史表视角。
//   两处 sizeof 0x4358 / 全部偏移 / mangled 零变化；消费 TU = DB_Login.cpp + InterDispatcher.cpp。
// 无子子代理（R4 合规）。
// ----------------------------------------------------------------------------
// df_game_r 还原 —— DB_Login（DBThread 登录分发器，Wave5 仅ORIG清零批）
// 权威报告：docs/class_func_reports/DB_Login/*.md（汇编 + Ghidra 反编译 C），
// 全部 52 个 ORIG 符号逐符号转写；SQL 字符串原样保留（大小写亦原样）。
//
// 字段化状态（本轮）：SIG_LOGIN_DATA 权威布局已在共享头 SigTypes_D.h 具名
// （Inter_LoginInfo/dispatch_sig 批），本 TU 已把偏移一致且语义同名的访问
// 全部切换为具名字段（errCode/accId/时间/奖励/punish 数组/角色槽/徽章/
// 疲劳/安全区/geo 等）；仅保留共享头为 pad 或命名与 ORIG 写入语义错位的
// 少数区域（0xa1 login_ip、0x39b0/0x39dc/0x3a3a、0x2184/0x2204、0x394c、
// premium 行容量超共享头声明、0x3fac 历史表、0x4140/0x4154/0x4338 事件块等）
// 以 SGF<T>(sig, off) 裸偏移访问并逐处注释，宿主后续对齐共享头后落点。
// ============================================================================

#include "DB_Login.h"

#include <string.h>
#include <cstring>   // [b28d-38] std::strrchr（ORIG 调 _ZSt7strrchrPci）
#include <stdlib.h>
#include <time.h>

#include "GlobalData.h"        // s_db_mgr/s_systemTime_/s_event_manager/s_psimpleSSO/
                               // s_msgq_mgr/s_big_stream_pool/s_stream_pool
#include "DBMgr.h"
#include "MySQL.h"
#include "SigTypes.h"          // SIG_LOGIN_DATA / SIG_UPDATE_LOGIN
#include "STBreakAwayMemberData.h"  // STBreakAwayMemberData::clear（ORIG W 0x844d584）
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"         // cMyTrace
#include "CSimpleSSO.h"        // WongWork::CSimpleSSO / stOutputData_t
#include "CRijndael.h"
#include "SyncScriptDeps.h"    // toTString（轻量声明头）
// ---- G_* 访问器/域类最小镜像（符号由权威 TU 提供；共享头稳定后可切回权威头）----
CEnvironment* G_CEnvironment();     // StreamDeps.h 已声明（权威）
class GameWorld
{
public:
    bool IsIntegratedPvPBaseChannel() const;   // ORIG T
    int GetChannelType() const;                // ORIG T
    bool IsSchoolPvPChannel() const;           // ORIG T
};
GameWorld* G_GameWorld();

class CDataManager
{
public:
    void* get_event_script_mng();
    int GetPvPMode();    // ORIG W 0x844d4ae（LinkClosureA_Data.cpp 实现）
};
CDataManager* G_CDataManager();

class CUser;
class CGameManager
{
public:
    CUser* getUser(int slotId, int increId);   // ORIG T
};
CGameManager* G_CGameManager();

// §4.4/§4.3：CEventBase/CEventManager 统一取权威头（原本 TU 局部镜像
// `class CEventBase;` + 仅含 GetRepeatEvent 的最小 CEventManager 已删）。
// 权威 CEventBase 声明序已按 ORIG vtable 0x08b4bae0 排好（IsEventing =
// slot 13 = vtable+0x34，CGuildServerProxy.h:129），故两处 is-eventing 判定
// 改真实虚成员调用（范式 P5）。实测：仅 include 权威头对本 TU .o 逐函数
// 全等（零扰动），无需 asm 槽直调。
#include "CGuildServerProxy.h"

namespace EventClassify
{

class CEventEntity {};                          // 仅作 get_event_entity 返回类型占位
class CEventScriptMng
{
public:
    bool is_eventing(int key);                  // ORIG T 0x0810ba36
    CEventEntity* get_event_entity(int key);    // ORIG T 0x0810b96a
    unsigned char get_today_time_day();         // ORIG T 0x0810c6ca
};
}  // namespace EventClassify
namespace online_preliminary
{
class COnlinePreliminary
{
public:
    static unsigned int GetPlayCountPerOneTeam();  // T 0x0855fee8（ORIG 无 this，静态实证）
};
}  // namespace online_preliminary

namespace WongWork
{
class CUserPremium
{
public:
    static bool IsRestrictedPremium(int type);    // T 0x086af7c4（ORIG 无 this，静态实证）
};
}  // namespace WongWork

// CHARAC_LOAD_MERCENARY 权威定义已迁至 SigTypes.h（Inter_LoginInfo/dispatch_sig 批）。

#include "DNFFunctionLib.h"    // DNFFLib::Hex2Binary
namespace OS_API               // CInventory.h 同款最小声明（snprintf wrapper）
{
int snprintf(char* buf, unsigned int n, const char* fmt, ...);
}
char* NumberToString(unsigned int value, int radix);   // C++ linkage（DNFFunctionLib.h 同款）
class CUser;
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);                          // T 0x0846c0a8（GameUtils/独立批次）
long getPrevScheduleTime(int hour, long time);                       // T 0x0846c2e9（GameUtils.cpp）
int getDailyInitializeTime(int hour, int min, int sec);              // T 0x086b1f5a（GameUtils.cpp）
long GetCurrentResetBaseTime(long t, int hour);                      // T 0x0810912a（_Z23GetCurrentResetBaseTimeli）

bool uncompress_zip(char* dst, unsigned int& dstLen,
                    const char* src, unsigned int srcLen);           // 归属 APSystemOnlyOrig 批（Main 协调）

namespace Taiwan {
bool GarenaCheckTimeStamp(unsigned int tokenTime, unsigned int nowSec,
                          unsigned int loginTime, unsigned int lastPlayTime); // T 0x08173080（Taiwan 域批）
}

// DB_MouseRegister::convertPassPadInfo（static，ORIG T 0x08422e6a；
// c++filt 形参 (unsigned int, char*, int&, char&, int&, bool&, char*, bool&,
// char*, long&, bool&)；mangled _ZN16DB_MouseRegister18convertPassPadInfoEjPcRiRcS1_RbS0_S3_S0_RlS3_。
// 报告尾部 eax 残留 0x48/0x49（失败计数字路径）被上层 dispatch 直接消费
// （==0x48→errcode5、==0xa6→9），故以 int 错误码承载（mangled 不含返回类型）。
// 定义在 MouseRegisterEJ.cpp。）
class DB_MouseRegister
{
public:
    static int convertPassPadInfo(unsigned int mid, char* mousepass,
                                  int& failCnt, char& cancelCnt,
                                  int& cancelCnt2, bool& validityExpired,
                                  char* ip, bool& flag2, char* buf3,
                                  long& validityTime, bool& flag3);
};

namespace WongWork {
// 五个活动物品编排器（static，定义归 WongWork EventATI/Sigong/Muz/Alttle/FCCombo 域批）
struct EventATI    { static void ArrangeATIEventItem(unsigned int mid, unsigned int param); };      // T 0x083fa1d3
struct EventSigong { static void ArrangeSigongEventItem(unsigned int mid, unsigned int param); };   // T 0x083fa855
struct EventMuz    { static void ArrangeMuzEventItem(unsigned int mid, unsigned int param); };      // T 0x083fb16a
struct EventAlttle { static void ArrangeAlttleEventItem(unsigned int mid, unsigned int param); };  // T 0x083fb595
struct EventFCCombo{ static void ArrangeFCComboEventItem(unsigned int mid, unsigned int param); }; // T 0x083fb976
} // namespace WongWork

// get_compressed_blob_data 支撑 blob 缓冲（ORIG W CCompressedBlob 0x844de52 起；
// 全源码此前无 owner，唯一权威定义落本 TU，布局按 ctor/resize/reset 反汇编）。
class CCompressedBlob
{
public:
    CCompressedBlob() : m_data(0), m_cap(0) {}       // 0x844de52
    ~CCompressedBlob()                               // 0x844de6a
    {
        if (m_data != 0)
        {
            delete[] m_data;
            m_data = 0;
        }
    }
    void resize(int cap)                             // 0x844e02a：仅扩容不缩
    {
        if (cap <= m_cap)
        {
            return;
        }
        delete[] m_data;
        // ORIG 0x0844e05f `movl $0x0,(%eax)`：delete[] 后显式清指针再 new。
        // （ORIG 4.1 delete[] 展开双 null 检 `test;je` ×2，4.4 恒单检 ⇒ 1 行 N 差。）
        m_data = 0;
        m_data = new char[cap];
        m_cap = cap;
    }
    void reset()                                     // 0x844e080：memset(m_data,0,m_cap)
    {
        memset(m_data, 0, m_cap);
    }
    // 权威实现见 WindowsPackFiles.cpp:1196（同 8B 布局 m_buf@0/m_size@4 ↔ m_data/m_cap）。
    bool uncompress(MySQL* db, int fieldIdx, char* dst, int dstLen, int align);

    char* m_data;                                    // +0x00
    int m_cap;                                       // +0x04
};
// ============================================================================
// SIG_LOGIN_DATA 字段访问 helper（本 TU 局部；共享头字段化后可整体替换）
// ============================================================================
// O0 下 inline 不生效；ORIG 为直接 lea 字段访问，必须 always_inline 强制展开。
template <typename T>
inline __attribute__((always_inline)) T& SGF(void* sig, unsigned int off)
{
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(sig) + off);
}
inline __attribute__((always_inline)) char* SGB(SIG_LOGIN_DATA* sig, unsigned int off)
{
    return reinterpret_cast<char*>(sig) + off;
}
// 常量偏移直写：宏把 off 作为字面量替换，-O0 下折叠进基址/地址表达式
// （SGF<T>(sig,off) 模板的 off 是运行期参数，会多做一次 add）。
#define SGFC(type, sig, off) (*reinterpret_cast<type*>(reinterpret_cast<char*>(sig) + (off)))
#define SGBC(sig, off) (reinterpret_cast<char*>(sig) + (off))

namespace   // 残留裸偏移速查（已字段化区域不再需要；均出自对应报告 ORIG 指令）
{
// 共享头 SigTypes_D.h 已具名覆盖的访问一律用具名字段；仅下列 SigTypes_D 为
// pad/隐式区或命名错位的区域保留常量访问（每处带语义注释）。
const unsigned int OFF_CLIENT_IP      = 0x0018;   // checkValidPassword clientHash 输入缓冲
const unsigned int OFF_LOGIN_IP       = 0x00a1;   // 登录 ip 字符串（SigTypes_D 名 m_blackIpCheckBuf）
const unsigned int OFF_BIRTH_DATE     = 0x00d0;   // reg_date DATE_FORMAT uint（SigTypes_D pad）
const unsigned int OFF_CHARAC_VIEW    = 0x00e0;   // 0x24×0x94 角色视图数组首（m_characData）
const unsigned int OFF_SECURITY_VALID = 0x39dc;   // security card 有效标志（SigTypes_D pad）
const unsigned int OFF_SSO_LOGINIP    = 0x3a3a;   // login_ip[0x10]（共享头名 m_webAddress）
const unsigned int OFF_SEC_FAILCNT    = 0x39e4;   // 安全卡 fail_cnt（共享头名 m_secCardCertCnt）
const unsigned int OFF_SEC_CERTFLAG   = 0x39e8;   // cert_flag uint（SigTypes_D pad）
const unsigned int OFF_SEC_CERTCNT    = 0x39ec;   // cert_cnt byte（共享头名 m_secCardCertFlag）
const unsigned int OFF_SEC_QPOS       = 0x3a14;   // q_pos int（共享头名 m_secCardCancelCnt）
const unsigned int OFF_SEC_QPOS_STR   = 0x3a18;   // q_pos 串槽 char[5]（共享头名 m_secCardAnswers）
const unsigned int OFF_HISTORY_TBL    = 0x3fac;   // 0x168 = 30×{int type,h1,h2}（m_ingameEventHistory）
const unsigned int OFF_GROWTH_WEAPON  = 0x4140;   // 事件块 A（0x14，共享头 raw）
const unsigned int OFF_GROWTH_CREATURE= 0x4154;   // 0x2c×11（m_growthCreatureEvent 具名条目可用）
const unsigned int OFF_ONCE_GIFT      = 0x4338;   // 0x20（共享头 raw）
const unsigned int OFF_BINGO_EVT      = 0x4138;   // SIG_BINGO_EVENT 起点
// [b28d-solo-DB_Login] P311 角色槽视图：0x24×0x94 步长数组（+0xe0 起）。具名
// 完整类型成员直访问复现 ORIG `imul $0x94; lea (%edx,%eax,1); add $0xe0` 三步
// （探针 probe_arr f4 逐条全等）；char 数组 + 指针算术恒把 add $0xe0 提前（f1）。
// 仅表达布局，不实例化、零符号、零 sizeof/偏移影响。
struct CharacSlotView { int m_characNo; char m_rest[0x90]; };
struct CharacDataView { char m_pad[0x00e0]; CharacSlotView m_slots[0x24]; };
}

// ============================================================================
// 生命周期（ORIG 0x0840f48e / 0x0840f4aa / 0x0840f4da）
// ============================================================================

DB_Login::DB_Login()
{
    // ORIG 仅置 vptr（DBDispatcher 基类空构造隐式展开）
}

DB_Login::~DB_Login()
{
}

// tagDNFEventEntry::~tagDNFEventEntry ORIG W 0x0818d8d0（纯 ret；头内 inline 已一致）

// ---- ::DBDispatcher::dispatch（ORIG T 0x083fbf7c）----
// slot==-1 恒通过；否则要求 CGameManager::getUser(slot, userIdx+0x7d0) 非空。
bool DBDispatcher::dispatch(int userIdx, int slot, Stream* stream)
{
    (void)stream;
    if (slot != -1)
    {
        CUser* user = G_CGameManager()->getUser(slot, userIdx + 0x7d0);
        if (user == 0)
        {
            return false;
        }
    }
    return true;
}
// ============================================================================
// _getGameDB（ORIG T 0x0840f4f8）/ _GetDNFAccountDB（ORIG T 0x08411bae）
// ============================================================================

MySQL* DB_Login::_getGameDB(ENUM_DB_HANDLE_IDX idx)
{
    return GlobalData::s_db_mgr->GetDBHandle(idx, E_SERVER_GROUP_NONE);
}

MySQL* DB_Login::_GetDNFAccountDB()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    return db;
}

// ============================================================================
// dispatch（ORIG T 0x0840f51c —— 登录主流程，0xAAE 字节）
// 【N81 blocked 归因（batch-15）】gap~116 = 八处 SSORecord 数组索引的 4.1 IMAD 九段
// 内联（`mov i,%eax; add%eax,%eax; add i,%eax; shl $3; lea(base,i); add $0x3a68; mov`
// 逐读重算、无 CSE，0x0840f7xx 段实证）vs 4.4 恒 `imul $0x18` 单条 lea 合并 +
// 三处 je/jne 块序（4.1 fall-through 反向）——表达式不可控（N81/N17 既定不可翻）。
// [b28d-solo-DB_Login] verdict=DIFF ndiff=438 gap=124（N82 帧 0x40↔0x90 + 上列 N81
// SSORecord 索引展开；p167 CLEAR 56/56 语义完整，§6.2 三件套口径收）。
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=438 gap=124（N82/N81；p167 CLEAR 56/56）
bool DB_Login::dispatch(int userDbIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userDbIdx, slot, stream))
    {
        return false;
    }
    SIG_LOGIN_DATA* sig = stream->GetOutBuffer<SIG_LOGIN_DATA>();
    if (sig == 0)
    {
        return false;
    }

    // b28d-68：ORIG 0x0840f573/0x0840f584 两次独立 G_GameWorld()（P48/P157 禁指针缓存）
    bool needUserDBInfo =
        (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
         G_GameWorld()->GetChannelType() != 0xe);
    if (needUserDBInfo)
    {
        DBMgr::stUserDBInfo_t info;
        info.m_userDBIdx = 0;
        info.m_userDBType = 0;
        info.m_serverGroup = sig->m_channelRestrictField;   // serverGroup(short→int)
        GlobalData::s_db_mgr->addUserDBInfo(slot, info);
        GlobalData::s_db_mgr->setUserIdx(slot);
    }

    // ---- GetAccountInfo 失败 => 错误码 1 ----
    if (!GetAccountInfo(sig))
    {
        sig->m_errCode = 1;
        return SendResult(userDbIdx, slot, sig);
    }

    SaveLoginTime(sig->m_accId,
                  sig->m_timeStamp,
                  SGBC(sig, OFF_LOGIN_IP));
    sig->m_punishTradeAlertType = 0;

    // ---- 封禁/惩罚检查失败 => 错误码 0xf ----
    if (!GetPunishUserInfo(sig))
    {
        sig->m_errCode = 0xf;
        return SendResult(userDbIdx, slot, sig);
    }

    sig->m_restingUserRestrict = 0;
    sig->m_totalHackUserTradeCnt = 0;
    sig->m_totalHackUserTradeGold = 0;
    sig->m_totalHackUserTradeGoldHi = 0;

    int punishCount = sig->m_punishInfoCount;
    for (int i = 0; i < punishCount; ++i)
    {
        const SIG_LOGIN_PUNISH_INFO& e = sig->m_punishInfos[i];
        // punish_type == 1：封禁中（含到期时间窗判断）
        if (e.m_type == 1)
        {
            long long endSec = e.m_endTime;        // unix end_time
            long long curSec = GlobalData::s_systemTime_.getCurSec();
            bool active = !(endSec < curSec && endSec != 0);
            if (active)
            {
                if (e.m_endTime == 0)
                {
                    if (e.m_field0c == 1)       // apply_flag
                        sig->m_errCode = 10;
                    else if (e.m_field0c == 2)
                        sig->m_errCode = 11;
                }
                else
                {
                    int val = e.m_stdDropRate;         // punish_value
                    if (val > 25) val = 25;
                    sig->m_loginErrDetail = val;                   // ban_msg 序号
                }
                return SendResult(userDbIdx, slot, sig);
            }
        }
        // punish_type 2 / 0x3ea
        if (e.m_type == 2 || e.m_type == 0x3ea)
        {
            long long endSec = e.m_endTime;
            long long curSec = GlobalData::s_systemTime_.getCurSec();
            bool active = !(endSec < curSec && endSec != 0);
            if (active)
            {
                if (e.m_endTime == 0)
                {
                    if (e.m_field0c == 1)
                        sig->m_errCode = 10;
                    else if (e.m_field0c == 2)
                        sig->m_errCode = 11;
                }
                else
                {
                    int val = e.m_stdDropRate;
                    if (val > 25) val = 25;
                    sig->m_errCode = val + 12;
                }
                return SendResult(userDbIdx, slot, sig);
            }
        }
        // punish_type 3 / 0x3eb（无时间窗直接拦截）
        if (e.m_type == 3 || e.m_type == 0x3eb)
        {
            if (e.m_endTime == 0)
            {
                if (e.m_field0c == 1)
                    sig->m_errCode = 10;
                else if (e.m_field0c == 2)
                    sig->m_errCode = 11;
            }
            else
            {
                int val = e.m_stdDropRate;
                if (val > 25) val = 25;
                sig->m_errCode = val + 12;
            }
            return SendResult(userDbIdx, slot, sig);
        }
        // punish_type 4：写通知码 0xd/0xe 到 +0x38de（short）
        if (e.m_type == 4)
        {
            if (e.m_field0c == 1)
                sig->m_punishTradeAlertType = 0xd;
            else if (e.m_field0c == 2)
                sig->m_punishTradeAlertType = 0xe;
        }
        // punish_type 10：置 +0x3f0c 标志
        if (e.m_type == 10)
        {
            sig->m_restingUserRestrict = 1;
        }
    }

    _getRewardInfo(sig);

    // ---- PassPad 换算（+0x38e0 有效且 +0x390e 非 1 时执行）----
    // ORIG：res = _convertPassPadInfo(sig)；==0x48 → errcode 5；==0xa6 → 9；其它非零 → 6。
    int res = 0;
    if (sig->m_passPadEnable != 0 &&
        sig->m_passPadCertified != 1)
    {
        res = _convertPassPadInfo(sig);
    }
    if (res != 0)
    {
        if (res == 0x48)
            sig->m_errCode = 5;
        else if (res == 0xa6)
            sig->m_errCode = 9;
        else
            sig->m_errCode = 6;
        return SendResult(userDbIdx, slot, sig);
    }

    sig->m_errCode = 0;

    vecDNFEventEntry_t entries;
    _LoadDNFEventEntry(sig->m_accId, entries);
    GetPremiumInfo(0 /*int 占位形参，ORIG 未使用*/, sig, entries);

    // ---- 通道 7（公会战）：检查 BreakAway 数据完整 ----
    // b28d-68：ORIG 0x0840fcad 独立 G_GameWorld() 调用（无缓存）
    if (G_GameWorld()->GetChannelType() == 7)
    {
        if (sig->m_tournamentAccountFlag == 0 || sig->m_tournamentServerGroup == 0)
        {
            sig->m_errCode = 8;
            int ret = SendResult(userDbIdx, slot, sig);
            return ret;
        }
    }
    // ---- 通道 0xe（线上预选）：pvp mask 校验 ----
    // b28d-68：ORIG 0x0840fd12 独立 G_GameWorld() 调用（无缓存）
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        bool blocked;
        if (sig->m_onlinePrelimCharacNo == 0 || sig->m_onlinePrelimServerGroup == 0)
        {
            blocked = true;
        }
        else
        {
            unsigned int modeMask = sig->m_onlinePrelimPlayType;
            unsigned int pvpMode = (unsigned int)G_CDataManager()->GetPvPMode();
            blocked = ((pvpMode & modeMask) == 1);
        }
        if (blocked)
        {
            sig->m_errCode = 8;
            int ret = SendResult(userDbIdx, slot, sig);
            return ret;
        }
    }

    // ---- GetGameDBInfo：1 => 重查失败错误码 0xf；否则更新登录统计 ----
    if (GetGameDBInfo(sig) != 1)
    {
        UpdateLoginStat(userDbIdx, sig);
    }
    else
    {
        sig->m_errCode = 0xf;
    }

    if (sig->m_secCardActivate != 0 && sig->m_secCardCertified != 1)
    {
        _getSecurityCardInfo(sig);
    }

    getCharacLinkSystemData(sig);
    sig->m_breakAwayUvListFlag = (getBreakAwayMemberData(sig) == true) ? 1 : 0;
    sig->m_joinedSchoolPointEventFlag =
        _isJoinedSchoolPointEvent(sig->m_accId) ? 1 : 0;
    GetDailyCleanPadPoint(sig);
    IsThisIPLoginPermissionGranted(sig);
    GetReturnUserInfo(sig);
    GetLoginCommonData(sig);
    GetCheckPickUpRandomOptionItem(sig);
    GetOntimeUserRecvIdx(sig);
    GetUserFatigueCount(sig);
    GetUserInGameEventHistory(sig);
    CheckLoginIP(sig);
    GetMercenarySystemInfo(sig);
    GetBingoEvent(sig);
    GetGrowthWeaponEvent(sig);
    GetGrowthCreatureEvent(sig);
    GetAccountCharacterLevelUpOnceGift(sig);

    return SendResult(userDbIdx, slot, sig);
}

// ============================================================================
// getCharacLinkSystemData（ORIG T 0x0840ffca）
// ============================================================================

// [S216_DB_Login2 复核] verdict=DIFF ndiff=1 gap=1（N121：ORIG 尾
// `xor $1,%eax; test %al,%al; jmp <exit>` + nop 岛 = 4.1 if-join 岛；4.4 恒
// epilogue-threading 折叠，探针 probe_tail fA–fF 六形（while+break/空 if/goto/
// 空 else）全败。语义与调用点全等）。
// S216：P285 两形实测——正向 if+空 else{return} 反劣（gap 2）；负条件空 then+else
// return 与原形逐条同 ⇒ 维持 N121 epilogue-threading 封顶。
void DB_Login::getCharacLinkSystemData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // P48 no-CSE：ORIG 三处各自 fresh NumberToString()（两条 set_query + cMyTrace）
    // 与两处 fresh get_n_rows()。
    db->set_query("seLect message_flag from charac_link_message where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    // [b28c-L9] SIGFIX:DB_Login::getCharacLinkSystemData —— ORIG
    // __PRETTY_FUNCTION__ 串为 `void DB_Login::getCharacLinkSystemData(...)`，
    // 且尾 0x084101a0 `xor $1,%eax; test %al,%al; jmp exit`（值无分支消费者）
    // ⇒ 原码非「返回错误码」而是 void + 嵌套 if/else（旧 bool 早返形系误建模；
    // 0x084101a7 的 nop 是 4.1 为 n_rows!=1 空 else 发射的中继岛，P184/P223）。
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x2f54, "LINK_LOG : charac_link_message DB ERROR!!");
    }
    else if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into charac_link_message(m_id,message_flag) values(%s,0)",
                      NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x2f5d, "LINK_LOG : charac_link_message DB INSERT FAIL!!");
        }
    }
    else if (db->get_n_rows() == 1)
    {
        // [b28d-solo-DB_Login] P168a/P222：ORIG `call; xor $1; test; je <get_byte 段>`
        // ⇒ 失败(trace)块直落、成功块后置；旧 if(fetch) 正极性产 `test; je <trace>`。
        if (!db->fetch())
        {
            // P227：全表达式临时链 ⇒ NumberToString 结果在 ctor 前求值并跨 ctor
            // 保活 %ebx（ORIG push %ebx / mov %eax,%ebx）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2f65, 5)(
                "LINK_LOG : charac_link_message DB ERROR!! M_ID(%s)",
                NumberToString(sig->m_accId, 0));
        }
        else
        {
            // P225/P223：ORIG 尾 `xor $1; test %al,%al; jmp exit`（get_byte 失败的死物化）
            // ⇒ while+break 出 xor/test 段，早返出尾 jmp（4.4 对空 if 恒 DCE）。
            // [S216] 追加试 P285 两形（正/负条件空 then + else return）：负条件形与原形
            // 逐条同（gap=1 仅缺尾 jmp+nop）；正条件形反劣（丢 xor，gap=2）⇒ 确认 N121
            // epilogue-threading 封顶。
            if (!db->get_byte(0, reinterpret_cast<char&>(sig->m_characLinkMessageFlag)))
            {
                return;
            }
        }
    }
}

// ============================================================================
// GetLoginCommonData（ORIG T 0x0841090c）
// ============================================================================

int DB_Login::GetLoginCommonData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect  member_bonus_fatigue , update_time , login_ip , inform_notice  from login_common where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    // [b28d-solo-DB_Login] P0：ORIG 有 rows 具名槽（-0x10，`cmpl $0,-0x10`）；
    // 列号用递增计数器 col（-0xc，`mov -0xc,%eax; addl $1,-0xc` = col++）。
    int rows = db->get_n_rows();
    if (rows == 0)
    {
        sig->m_memberBonusFatigueInc = 0;       // +0x3f5e
        // [b28d-38] SGB() 函数形 off 为运行期参数 ⇒ -O0 物化 `movl $imm,槽; add 槽`
        // 3 条/处；ORIG 为单条 `add $imm,%eax` ⇒ 改宏形 SGBC（off 字面量折叠，P0 右值直传）。
        memset(SGBC(sig, 0x4114), 0, 0x10);     // login_ip（SigTypes_D 隐式区）
        memset(SGBC(sig, 0x4125), 0, 0x10);     // inform_notice
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int col = 0;
    int bonusFatigue = 0;
    if (!db->get_int(col++, bonusFatigue))
    {
        return 0;
    }
    int updateTime = 0;
    if (!db->get_int(col++, updateTime))
    {
        return 0;
    }
    sig->m_memberBonusSaveTime = (unsigned int)updateTime;  // +0x3f60 ORIG local_20[0]
    sig->m_memberBonusFatigueInc = (short)bonusFatigue;     // +0x3f5e
    if (!db->get_str(col++, SGBC(sig, 0x4114), 0x10))   // 第 3 列续接索引读取 login_ip
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// GetReturnUserInfo（ORIG T 0x08410ad8）
// ============================================================================

int DB_Login::GetReturnUserInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect unix_timestamp(expire_time), first_login from return_user where m_id=%s and expire_time > now()",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows == 0)
    {
        sig->m_returnUserDbFlag = false;        // +0x3f04
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_int(0, reinterpret_cast<int&>(sig->m_returnUserExpireTime)))  // +0x3f08
    {
        return 0;
    }
    int firstLogin = 0;
    if (!db->get_int(1, firstLogin))
    {
        return 0;
    }
    sig->m_returnUserFirstLogin = (firstLogin == 0);  // +0x3f05
    sig->m_returnUserDbFlag = true;                   // +0x3f04
    return 1;
}

// ============================================================================
// GetDailyCleanPadPoint（ORIG T 0x08410c12）
// ============================================================================

int DB_Login::GetDailyCleanPadPoint(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect cleanpad_point from member_login where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_uint(0, reinterpret_cast<unsigned int&>(sig->m_cleanPadDecreaseCnt)))  // +0x3a4c
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// GetPunishUserInfo（ORIG T 0x08410cf6）—— member_punish_info 行载入
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=3 gap=1（N18a/P211 档③：ORIG
// `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x3a50(%eax)` = 字段与
// get_n_rows() 的 int 返回**异型**；共享头 m_punishInfoCount 为 int（同型 ⇒ 4.4
// LHS-first 2 条）。视图取址三形帧膨胀（probe_pun f1–f3 push ebx+sub 4）、局部
// 指针形多地址槽（f5）；改共享头牵动 ARAD_LoginSecurity/InterDispatcher 三处比较
// 符号性 ⇒ 封顶）。
// S216：RHS `(unsigned int)db->get_n_rows()` cast 形实测仍 LHS-first（4.4 折叠同宽
// cast）⇒ 维持 N18a/P211 封顶（字段侧 unsigned 化牵动 ARAD_LoginSecurity/
// InterDispatcher 三处比较符号性，跨泳道代价 > 1 条收益）。
bool DB_Login::GetPunishUserInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect punish_type, unix_timestamp(occ_time), punish_value, apply_flag, unix_timestamp(start_time), unix_timestamp(end_time) from member_punish_info where m_id=%s and apply_flag>=1 LIMIT %d",
        NumberToString(sig->m_accId, 0), 0x32);
    // [b28d-solo-DB_Login] P133 合并短路：ORIG `xor$1; test; jne <mov$1 岛>;
    // …get_n_rows…; jne <mov$0>; test; je`（exec 失败与零行汇聚同一返回 1 出口）。
    // P48 no-CSE：三处各自 fresh get_n_rows()（判零 / m_punishInfoCount / 循环上界）。
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return true;   // ORIG 0x8410d7d/0x8410d89：无惩罚数据，正常继续
    }
    unsigned int i = 0;                          // P191：ORIG 双初始化（此处 + for 各一条）
    // [b28d-solo-DB_Login] P211 档③：ORIG `mov %eax,%edx; mov 0xc(%ebp),%eax;
    // mov %edx,0x3a50(%eax)`（RHS-first+中转 3 条）= 原码字段与 get_n_rows() 的 int
    // 返回**异型**（探针 probe_pun f6）。共享头字段 int（同型 ⇒ 4.4 折 LHS-first 2 条）；
    // 视图取址三形（f1/f2/f3）恒 push ebx+sub 4 帧膨胀，局部指针形（f5）多地址槽
    // ⇒ 字段侧旋钮须改共享头，牵动 ARAD_LoginSecurity/InterDispatcher 三处比较符号性，
    // 跨泳道代价 > 本件 1 条 jmp 收益 ⇒ 维持 N18a 封顶（3 行）。
    sig->m_punishInfoCount = db->get_n_rows();   // +0x3a50
    for (i = 0; i < db->get_n_rows(); ++i)
    {
        // P157/P183：ORIG 无引用缓存，每次重算完整 i*0x18 地址链。
        if (!db->fetch()) return false;
        if (!db->get_int(0, sig->m_punishInfos[i].m_type)) return false;                // punish_type
        if (!db->get_uint(1, reinterpret_cast<unsigned int&>(
                sig->m_punishInfos[i].m_field04))) return false;                        // occ_time
        if (!db->get_int(2, sig->m_punishInfos[i].m_stdDropRate)) return false;         // punish_value
        if (!db->get_int(3, sig->m_punishInfos[i].m_field0c)) return false;             // apply_flag
        if (!db->get_uint(4, reinterpret_cast<unsigned int&>(
                sig->m_punishInfos[i].m_field10))) return false;                        // start_time
        if (!db->get_uint(5, reinterpret_cast<unsigned int&>(
                sig->m_punishInfos[i].m_endTime))) return false;                        // end_time
    }
    return true;
}

// ============================================================================
// _isJoinedSchoolPointEvent（ORIG T 0x08410f74）
// ============================================================================

bool DB_Login::_isJoinedSchoolPointEvent(unsigned int mid)
{
    if ((bool)(GlobalData::s_systemTime_.getCurSec() > 0x4834a970))
    {
        return false;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect school_id from event_school_point where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    return true;
}

// ============================================================================
// _isAutoPunishedFirstUser（ORIG T 0x0841102e）
// ============================================================================

bool DB_Login::_isAutoPunishedFirstUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect punish_flag from auto_punish_first_user where m_id=%s and punish_flag=1",
                  NumberToString(mid, 0));
    // ORIG 0x8411085：exec 失败或 rows==0 → 聚合 bad → return false
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    return true;
}

// ============================================================================
// _getAutoPunishedSecondUserData（ORIG T 0x084110c6）
// ============================================================================

bool DB_Login::_getAutoPunishedSecondUserData(unsigned int mid, unsigned int& tradeCnt,
                                              unsigned long long& tradeGold)
{
    char punishFlag = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect trade_cnt,trade_gold,punish_flag,unix_timestamp(occ_time) from auto_punish_second_user where m_id=%s",
        NumberToString(mid, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    db->fetch();
    db->get_uint(0, tradeCnt);
    db->get_ulonglong(1, tradeGold);
    db->get_byte(2, punishFlag);
    int occTime = 0;
    db->get_int(3, occTime);    // ORIG：读入后未使用（死存储，保留语义注释）
    (void)occTime;
    return punishFlag != 0;
}

// ============================================================================
// _IsPunishedUser（ORIG T 0x084129d6）/ _isPunishedHackUser（ORIG T 0x08412ac8）
// ============================================================================

unsigned int DB_Login::_IsPunishedUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect count(*) from user_ban where m_id=%s and status=1",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int cnt = 0;
    if (!db->get_int(0, cnt) || cnt == 0)
    {
        return 0;
    }
    return 1;
}

unsigned int DB_Login::_isPunishedHackUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect occ_time,period,now_flag from member_punish_hack where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int occTime = 0;
    int period = 0;
    int nowFlag = 0;
    if (!db->get_int(0, occTime)) return 0;
    if (!db->get_int(1, period)) return 0;
    if (!db->get_int(2, nowFlag)) return 0;
    if (period != 0)
    {
        // [b28d-solo-DB_Login] ORIG 0x08412c0d..0x08412c1c 为 32 位 imul/lea
        // （`imul $0xe10,%eax,%edx; lea (%edx,%eax,1),%eax; mov %eax,-0xc`）⇒ int。
        int expireSec = period * 0xe10 + occTime;
        if (expireSec < GlobalData::s_systemTime_.getCurSec())
        {
            return 0;
        }
    }
    // P47/P56：ORIG `test; jne <mov$1 尾块>; mov$0` ⇒ if(==0) return 0; return 1;
    if (nowFlag == 0)
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// _getSecurityCardInfo（ORIG T 0x084111e2）
// ============================================================================

void DB_Login::_getSecurityCardInfo(SIG_LOGIN_DATA* sig)
{
    // P1 组件视图：ORIG 单指令 disp 读/写点（get_* 传引用处保持 SGFC add 两步形态）
    struct VSecValid { char m_pad[0x39dc]; unsigned char m_valid; };        // +0x39dc
    struct VCertFlag { char m_pad[0x39e8]; unsigned int m_certFlag; };      // +0x39e8
    struct VOccTime  { char m_pad[0x3958]; long m_occTime; };               // +0x3958（card 有效期基准）
    struct VEnvSched { char m_pad[0x37c]; int m_dailyResetHour; };          // CEnvironment +0x37c
    if (((VSecValid*)sig)->m_valid == 0)             // +0x39dc（SigTypes_D pad 区保留）
    {
        sig->m_secCardActivate = false;             // +0x39e0
        return;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect card,fail_cnt,validity_time>unix_timestamp(now()),cert_flag, cert_cnt, q_pos from member_security_card where m_id=%s and apply_flag=1",
        NumberToString(sig->m_accId, 0));
    db->exec(true);
    if (db->get_n_rows() == 0)
    {
        sig->m_secCardActivate = false;             // +0x39e0
        return;
    }
    unsigned int validity = 0;
    db->fetch();
    db->get_str(0, sig->m_secCardSerial, 0x27);                    // card → serial(+0x39ed)
    db->get_int(1, SGFC(int, sig, OFF_SEC_FAILCNT));               // fail_cnt
    db->get_uint(2, validity);                                    // validity_time>now()
    db->get_uint(3, SGFC(unsigned int, sig, OFF_SEC_CERTFLAG));    // cert_flag
    db->get_byte(4, SGFC(char, sig, OFF_SEC_CERTCNT));             // cert_cnt（ORIG 先读列 4）
    db->get_short(5, SGFC(int, sig, OFF_SEC_QPOS));                // q_pos（int 引用重载）
    db->get_str(5, SGBC(sig, OFF_SEC_QPOS_STR), 5);                // ORIG：列 5 再读 str 到 +0x3a18（长 5）
    if (validity == 1)
    {
        sig->m_secCardCertified = true;             // +0x39e1
    }
    sig->m_secCardActivate = true;                  // +0x39e0

    // cert_flag != 0 且当日已过疲劳重置点 → 清零 cancel 计数
    // [b28c-L9] ORIG 0x084113aa..0x084113f0：`bool hit = (certFlag!=0) && call(...)` 短路形——
    // 首条件 je 与 call 失败 je 共享 L_false 岛（mov $0,%eax），成功后 mov $1,%eax 汇合
    // 再 test %al（P0：值全程经 %eax 无槽往返 ⇒ 原码是表达式不是具名中间量）；
    // 两 callee-saved 由实参右→左求值定：curSec→%ebx、occTime→%esi（P52/P87）。
    if (({ ((VCertFlag*)sig)->m_certFlag != 0
           && CheckDailyScheduleTime(
                  ((VEnvSched*)G_CEnvironment())->m_dailyResetHour,
                  ((VOccTime*)sig)->m_occTime,
                  GlobalData::s_systemTime_.getCurSec()); }))
    {
        ((VCertFlag*)sig)->m_certFlag = 0;
        db->set_query("upDate member_security_card set cancel_cnt=0 where m_id=%s",
                      NumberToString(sig->m_accId, 0));
        db->exec(true);
    }
}

// ============================================================================
// _getPassPadInfo（ORIG T 0x0841144a）
// ============================================================================

int DB_Login::_getPassPadInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    sig->m_passPadEnable = 0;
    sig->m_passPadCancelCnt = 0;                    // +0x3918
    sig->m_passPadFailCnt = 0;                      // +0x3914
    memset(sig->m_passPadPassword, 0, 0x24);        // +0x38e1（仅清 0x24 缓冲）
    db->set_query("seLect mousepass,fail_cnt,cancel_cnt from member_mousepass where m_id=%s and enable_flag = '1'",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 1;
    }
    if (!db->get_str(0, sig->m_passPadPassword, 0x24))
    {
        return 1;
    }
    if (!db->get_int(1, sig->m_passPadFailCnt))
    {
        return 1;
    }
    if (!db->get_int(2, sig->m_passPadCancelCnt))
    {
        return 1;
    }
    sig->m_passPadEnable = 1;
    if (sig->m_passPadFailCnt > 2)
    {
        return 0x49;    // fail_cnt>=3 触发二次认证错误码
    }
    return 0;
}

// ============================================================================
// _convertPassPadInfo（ORIG T 0x08411af0）
// 【推断】返回值：ORIG 尾部无显式 ret 赋值，eax 残留自尾调的
// DB_MouseRegister::convertPassPadInfo（其真实源码应返回错误码 int，
// dispatch 以 0x48/0xa6 分支消费）；此处按该语义透传。
// ============================================================================

// ORIG T 0x08411af0：lea 0x390d/0x3910/0x3905/0x3914/0x390c/0x3918/0x38e0/
// 0x38e1/0x390e/0x3a3a 直取地址压栈调 convertPassPadInfo，尾部无回写。
// mousepass 实参为 m_passPadPassword+0x24（0x3905），ip 实参为 m_passPadPassword。
int DB_Login::_convertPassPadInfo(SIG_LOGIN_DATA* sig)
{
    return DB_MouseRegister::convertPassPadInfo(
        sig->m_accId,                                  // mid
        sig->m_passPadPassword + 0x24,                 // mousepass（0x3905）
        sig->m_passPadFailCnt,                         // fail_cnt(int&)
        sig->m_passPadVersion,                         // cancel_cnt(char&)
        sig->m_passPadCancelCnt,                       // cancel_cnt2(int&)
        sig->m_passPadEnable,                          // validityExpired(bool&)
        sig->m_passPadPassword,                        // ip（0x38e1）
        sig->m_passPadCertified,                       // flag2(bool&)
        sig->m_webAddress,                             // buf3（0x3a3a）
        sig->m_passPadCheckTime,                       // validity_time(long&)
        sig->m_passPadAdvideChangePwd);                // flag3(bool&)
}

// ============================================================================
// _getRewardInfo（ORIG T 0x084115f8）—— member_security_grade 安全等级汇总
// ============================================================================

// [S216_DB_Login2] verdict=DIFF ndiff=382 gap=44（show_func_diff 行级口径；N82 槽/帧 + N78 memset
// 展开 + N-P133 bool 槽物化：拆独立守卫实测 244→254 反劣已回退；p167 CLEAR 18/18）。
// S216 本轮 420→382：①P10/P248 档② flags/latest char→int（`orb`→`orl` 宽度对齐）；
// ②拆独立守卫 `if(!exec)return 0;` + `if(rows!=0){}else{return 0;}` 修 execOk 槽物化
// 与 setne 极性；③`time(&now)` 传址形修 time 调用形态（ORIG lea+call+回读槽）。
// 余：N82 帧 0x10 + N-P133 第二守卫 setne/je 极性 + N78 memset 段 + ORIG `or` 11 处。
int DB_Login::_getRewardInfo(SIG_LOGIN_DATA* sig)
{
    sig->m_secuType = 0;          // +0x3f0d 注册位图
    sig->m_secuReward = false;    // +0x3f0e 重新校验标记
    sig->m_rewardType = 0;        // +0x3f0f 最新注册类型位
    sig->m_passPadEnable = 0;
    sig->m_passPadCertified = 0;
    sig->m_secCardActivate = false;     // +0x39e0
    sig->m_secCardCertified = false;    // +0x39e1

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect unix_timestamp(last_check_time), unix_timestamp(m_opt_reg), unix_timestamp(pc_opt_reg), unix_timestamp(security_card_reg), unix_timestamp(goblin_pass_mod), unix_timestamp(member_pc_reg), unix_timestamp(gatekeeper_otp_reg), goblin_validity_time>unix_timestamp(now()), security_card_validity_time>unix_timestamp(now()), validity_ip from member_security_grade where m_id = %s ",
        NumberToString(sig->m_accId, 0));
    // [b28d-solo-DB_Login] N-P133：ORIG 两守卫为**寄存器直判**
    // （`call exec; xor $1,%eax; test %al,%al; je` + `call get_n_rows; setne %al;
    //  test %al,%al; je`，无槽）；4.4 对 bool callee 与具名 bool 局部恒插
    // `mov %al,-N; movzbl` 两段（拆独立守卫形实测 244→254 反劣，已回退）
    // ⇒ 保持合并短路 + execOk 具名槽（ORIG 槽深差 -0x14 vs -0x24 同属 N82）。
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() != 0)
    {
    }
    else
    {
        return 0;
    }

    // 以 KST 日界（-0x2a30/+0x12750=86400 边界）推 last_check_time 基准
    time_t now;
    time(&now);                       // ORIG `lea -0x18(%ebp); mov (%esp); call` 传址形
    int dayOff = (int)(now % 0x15180);
    int base = (int)now - dayOff;
    base += -0x2a30;
    if (dayOff > 0x1274f)
    {
        base = (int)now - dayOff + 0x12750;
    }

    unsigned int col[9];
    for (int i = 0; i < 9; ++i)
    {
        col[i] = 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    db->get_uint(0, col[0]);                        // last_check_time
    db->get_uint(1, col[1]);                        // m_opt_reg
    db->get_uint(2, col[2]);                        // pc_opt_reg
    db->get_uint(3, col[3]);                        // security_card_reg
    db->get_uint(4, col[4]);                        // goblin_pass_mod
    db->get_uint(5, col[5]);                        // member_pc_reg
    db->get_uint(6, col[6]);                        // gatekeeper_otp_reg
    db->get_uint(7, col[7]);                        // goblin_validity>now()
    db->get_uint(8, col[8]);                        // sec_card_validity>now()

    char validIp[16];
    char ipMatch = 1;
    db->get_str(9, validIp, 0x10);    // ORIG：get_str 直接覆盖，无预清零
    if (strncmp(validIp, SGBC(sig, OFF_LOGIN_IP), 0x10) != 0)
    {
        ipMatch = 0;
    }

    if (col[4] != 0)
    {
        sig->m_passPadEnable = 1;     // goblin_pass_mod → mousepass 区有效位
    }
    if ((col[7] != 0) && (ipMatch != '\0'))
    {
        sig->m_passPadCertified = 1;       // +0x390e mousepass 换算已注册
        SGFC(char, sig, 0x390c) = 2;                // +0x390c cancel_cnt 类型
    }
    if (col[3] != 0)
    {
        sig->m_secCardActivate = true;              // +0x39e0 security_card_reg 行存在
    }
    if ((col[8] != 0) && (ipMatch != '\0'))
    {
        sig->m_secCardCertified = true;             // +0x39e1 有效期内
    }

    int flags = 0;                                   // +0x3f0d 注册位图（ORIG `or` 32 位，P10）
    int latest = 0;                                  // +0x3f0f 最新注册类型位
    if (col[1] != 0) flags |= 1;                     // m_opt
    if (col[2] != 0) flags |= 2;                     // pc_opt
    if (col[3] != 0) flags |= 8;                     // security_card
    if (col[4] != 0) flags |= 0x10;                  // goblin_pass
    if (col[5] != 0) flags |= 4;                     // member_pc
    if (col[6] != 0) flags |= 0x20;                  // gatekeeper_otp
    sig->m_secuType = (unsigned char)flags;

    if (col[1] != 0) latest |= 1;
    else if (col[2] != 0) latest |= 2;
    else if (col[5] != 0) latest |= 4;
    else if (col[6] != 0) latest |= 0x20;
    if (col[3] != 0) latest |= 8;
    else if (col[4] != 0) latest |= 0x10;
    sig->m_rewardType = (unsigned char)(sig->m_rewardType | (unsigned char)latest);

    if (sig->m_secuType != 0 && (int)col[0] < base)
    {
        sig->m_secuReward = true;                    // 需要重新校验标记
    }
    return 0;                                        // ORIG 恒返 0（布尔由 sig 字段承载）
}

// ============================================================================
// _LoadDNFEventEntry（ORIG T 0x08411bda）
// ============================================================================

void DB_Login::_LoadDNFEventEntry(unsigned int mid, vecDNFEventEntry_t& out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    // [hc 收] G_CEnvironment()+0x378 = Server_Envir::m_serverGroup（Ordb dnf_event_entry
    // server_id 字段；与 WarRoom::ClearReward/DB_UpdateLogin 同源 +0x378 访问）。
    // [b28d-solo-DB_Login] P0：ORIG 无 serverId 具名槽（值在 %ebx 跨 NumberToString
    // 保活），内联右值直传；P82 视图成员访问折叠单条 disp（裸 (char*)+off 会多 add）。
    struct EnvView { char m_pad[0x378]; unsigned int m_serverGroup; };
    db->set_query(
        "seLect event_id,unix_timestamp(occ_date),server_id,charac_no,unix_timestamp(obtain_date) from dnf_event_entry where m_id=%s and server_id in(0,%d)",
        NumberToString(mid, 0),
        reinterpret_cast<EnvView*>(G_CEnvironment())->m_serverGroup);
    if (!db->exec(true))
    {
        return;
    }
    // [b28d-solo-DB_Login] ORIG 0x08411c58..0x08411d6b：尾部条件块（seta; test; jne body）
    // + body 首 fetch；5 个 get_int 各带独立 nop 岛落到增量（P223 多岛形）；
    // exec/fetch 失败落 epilogue（return）。
    // P209：ORIG `seta` ⇒ 无符号比较（i 声明 unsigned 使 int→unsigned 提升）。
    for (unsigned int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->fetch())
        {
            return;
        }
        tagDNFEventEntry entry;
        if (!db->get_int(0, entry.m_eventId)) continue;
        if (!db->get_int(1, entry.m_occDate)) continue;
        if (!db->get_int(2, entry.m_serverId)) continue;
        if (!db->get_int(3, entry.m_characNo)) continue;
        if (!db->get_int(4, entry.m_obtainDate)) continue;
        out.push_back(entry);
    }
}

// ============================================================================
// _LoadEventItem（ORIG T 0x08411d7e，static）
// 【推断】第二 Arrange 实参：ORIG 指令把 vector& 的指针值当 uint 原样传入
// （Ghidra C 同证），忠实保留。
// ============================================================================

void DB_Login::_LoadEventItem(SIG_LOGIN_DATA* sig, int serverId,
                              vecDNFEventEntry_t& entries)
{
    // [b28d-solo-DB_Login] P167 差集：ORIG 走 begin/end/operator->/operator++/operator!=
    // （我方旧索引形多 size/operator[]）；迭代器双缓存照 P13，箭头形照 P167 新判据。
    int seen[10] = {0};
    vecDNFEventEntry_t::iterator it = entries.begin();
    vecDNFEventEntry_t::iterator itEnd = entries.end();
    while (it != itEnd)
    {
        // P223 负条件 goto 尾标签：复现 ORIG `setg; test; jne <nop 岛>` 布局。
        if (it->m_eventId > 9)
        {
            goto next;
        }
        seen[it->m_eventId] = 1;
    next:
        ++it;
    }
    for (int eventId = 0; eventId < 10; ++eventId)
    {
        if (seen[eventId] == 0)
        {
            continue;
        }
        switch (eventId)
        {
        case 2:
            WongWork::EventATI::ArrangeATIEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 3:
            WongWork::EventSigong::ArrangeSigongEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 5:
            WongWork::EventMuz::ArrangeMuzEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 6:
            WongWork::EventAlttle::ArrangeAlttleEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 7:
            WongWork::EventFCCombo::ArrangeFCComboEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        default:
            break;
        }
    }
}

// ============================================================================
// GetPremiumInfo（ORIG T 0x08411ee8）—— member_broadcast / member_premium
// 汇总进 sig 的 premium/事件区；过期条目收集后交 RemovePreminuInfo 删除。
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=388 gap=-11（N82 槽/帧；P157 去 eventId
// 缓存后 p167 CLEAR 84/84 语义完整）。
void DB_Login::GetPremiumInfo(int /*param*/, SIG_LOGIN_DATA* sig,
                              vecDNFEventEntry_t& entries)
{
    MySQL* dbPremium = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    MySQL* dbAccount = _GetDNFAccountDB();
    std::vector<int> removedIds;            // ORIG local_58（未读取，仅保序拷贝语义）
    std::vector<int> expiredIds;            // ORIG local_64
    int rowIdx = 0;

    sig->m_premiumCount = 0;
    sig->m_restrictedGoodsCount = 0;
    SGFC(unsigned int, sig, 0x394c) = 0;   // ORIG 清零 +0x394c（pad 槽，无具名字段）

    for (vecDNFEventEntry_t::iterator it = entries.begin(); it != entries.end(); ++it)
    {
        // [b28d-solo-DB_Login] P157：ORIG 逐点重读 it->m_eventId（GetPremiumInfo 段
        // 23×ptEv，我方旧 eventId 缓存仅 2×）⇒ 删除具名缓存，全部使用点写 it->m_eventId。
        if (it->m_eventId >= 0xa029 && it->m_eventId <= 0xa02b)
        {
            // ---- 广播/PvP 预选特殊事件 ----
            dbAccount->set_query(
                "seLect m_id,charac_no,server_id,unix_timestamp(start_time),charac_name from member_broadcast where event_id=%d and m_id=%s and end_time>now() limit 1",
                it->m_eventId, NumberToString(sig->m_accId, 0));
            dbAccount->exec(true);
            if (dbAccount->get_n_rows() == 0)
            {
                expiredIds.push_back(it->m_eventId);
                continue;
            }
            if (!dbAccount->fetch())
            {
                break;
            }
            DBMgr::stUserDBInfo_t info;
            unsigned int characNo = 0;
            unsigned int serverId = 0;
            unsigned int midFromDb = 0;
            bool ok = dbAccount->get_uint(0, midFromDb);          // m_id（分库键）
            info.m_userDBIdx = (int)midFromDb;
            if (ok) ok = dbAccount->get_uint(1, characNo);
            if (ok) ok = dbAccount->get_uint(2, serverId);
            unsigned int startTime = 0;
            if (ok) ok = dbAccount->get_uint(3, startTime);
            if (!ok)
            {
                continue;
            }
            info.m_userDBType = characNo;
            info.m_serverGroup = serverId;

            if (G_GameWorld()->GetChannelType() == 7)
            {
                if (it->m_eventId != 0xa029 ||
                    !dbAccount->get_str(4, SGBC(sig, 0x39b0), 0x1e))
                {
                    continue;
                }
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                bool pass =
                    (it->m_eventId == 0xa02a &&
                     online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam() == 1) ||
                    (it->m_eventId == 0xa02b &&
                     online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam() != 1);
                if (!pass)
                {
                    continue;
                }
            }

            if ((int)startTime <= GlobalData::s_systemTime_.getCurSec())
            {
                if (G_GameWorld()->GetChannelType() == 7 || G_GameWorld()->GetChannelType() == 0xe)
                {
                    // ORIG 汇编 0x8412236-0x841224c：addUserDBInfo 第 1 参 =
                    // 本函数第 2 参（SIG_LOGIN_DATA* sig）指针值（0xc(%ebp)→0x4(%esp)），
                    // 非 m_accId；32 位指针→int 原样直传。
                    GlobalData::s_db_mgr->addUserDBInfo(
                        reinterpret_cast<int>(sig),
                        info);
                }
                if (it->m_eventId == 0xa02a)
                {
                    sig->m_onlinePrelimCharacNo = characNo;
                    sig->m_onlinePrelimServerGroup = serverId;
                    sig->m_onlinePrelimPlayType |= 1;
                }
                else if (it->m_eventId == 0xa02b)
                {
                    sig->m_onlinePrelimCharacNo = characNo;
                    sig->m_onlinePrelimServerGroup = serverId;
                    sig->m_onlinePrelimPlayType |= 2;
                }
                else
                {
                    sig->m_tournamentAccountFlag = characNo;
                    sig->m_tournamentServerGroup = serverId;
                }
            }
        }
        else if (!(it->m_eventId >= 50000 && it->m_eventId <= 89999))
        {
            if (it->m_eventId >= 70000 && it->m_eventId <= 89999)
            {
                // 落表事件：登记到 sig 事件 id 列表
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = it->m_eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
            else if (it->m_eventId == 59000)
            {
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = it->m_eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
            else if (!WongWork::CUserPremium::IsRestrictedPremium((int)it->m_eventId))
            {
                // ---- 常规 member_premium 查询 ----
                dbPremium->set_query(
                    "seLect pre_type,unix_timestamp(service_start),unix_timestamp(service_end) from member_premium where m_id=%s and event_id=%d and service_end>now() and server_id in(0,%d) order by service_end desc limit 1",
                    NumberToString(sig->m_accId, 0),
                    it->m_eventId, it->m_serverId);
                if (!dbPremium->exec(true))
                {
                    continue;
                }
                if (dbPremium->get_n_rows() == 0)
                {
                    expiredIds.push_back(it->m_eventId);
                    continue;
                }
                for (unsigned int r = 0; r < dbPremium->get_n_rows() && dbPremium->fetch(); ++r)
                {
                    int preType = 0;
                    int svcStart = 0;
                    int svcEnd = 0;
                    if (!dbPremium->get_int(0, preType))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x3628, 5);
                        trace("DB_Login::GetPremiumInfo, get(1) ERROR m_id=%s",
                              NumberToString(sig->m_accId, 0));
                        return;    // ORIG：失败即收尾返回（不进收尾的 RemovePreminuInfo）
                    }
                    if (!dbPremium->get_int(1, svcStart))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x362e, 5);
                        trace("DB_Login::GetPremiumInfo, get(2) ERROR m_id=%s",
                              NumberToString(sig->m_accId, 0));
                        return;
                    }
                    if (!dbPremium->get_int(2, svcEnd))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x3634, 5);
                        trace("DB_Login::GetPremiumInfo, get(3) ERROR m_id=%s",
                              NumberToString(sig->m_accId, 0));
                        return;
                    }

                    bool skip =
                        (G_GameWorld()->GetChannelType() == 7 && (preType == 0x16 || preType == 0x1b)) ||
                        (G_GameWorld()->GetChannelType() == 0xe && (preType == 0x16 || preType == 0x1b));
                    if (!skip)
                    {
                        if (rowIdx > 0x69)
                        {
                            break;
                        }
                        const unsigned int slotOff = (rowIdx + 0x229) * 0x10 + 0x14;
                        SGFC(unsigned int, sig, slotOff) = it->m_eventId;
                        SGFC(int, sig, (rowIdx + 0x229) * 0x10 + 8) = preType;
                        SGFC(int, sig, (rowIdx + 0x229) * 0x10 + 0xc) = svcStart;
                        SGFC(int, sig, (rowIdx + 0x229) * 0x10 + 0x10) = svcEnd;
                        rowIdx += 1;
                        unsigned short cnt = sig->m_restrictedGoodsCount;
                        sig->m_restrictedGoods[cnt] = it->m_eventId;
                        sig->m_restrictedGoodsCount = cnt + 1;
                    }
                }
            }
            else
            {
                // 受限 premium 事件：只登事件 id
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = it->m_eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
        }
    }

    sig->m_premiumCount = rowIdx;
    RemovePreminuInfo(sig, expiredIds);
}

// ============================================================================
// RemovePreminuInfo（ORIG T 0x084128c4，成员）
// ============================================================================

void DB_Login::RemovePreminuInfo(SIG_LOGIN_DATA* sig, std::vector<int>& ids)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    // [b28d-solo-DB_Login] P232/P28：ORIG 有 `movl $0x0,-0xc(%ebp)` dead int 槽
    // （db=-0x10），且 begin() 先于 end() 求值（-0x18/-0x14 临时 → -0x1c/-0x20 转换）。
    int slotPad = 0;
    // [b28d-38] P167：ORIG 两端均 const_iterator（end() 亦转换构造）；混合比较
    // 会实例化 operator!=<PK,P> 变体（我们原形）。
    std::vector<int>::const_iterator it = ids.begin();
    std::vector<int>::const_iterator endIt = ids.end();
    for (; it != endIt; ++it)
    {
        db->set_query("deLete from dnf_event_entry where m_id=%s and event_id=%d",
                      NumberToString(sig->m_accId, 0), *it);
        // ORIG 0x08412996..0x084129a9：`xor $1; test; jne <nop 岛>` + dead 读
        // sig+0x2938（共享头 pad 区，原码字段未具名；P171 死读照抄 + P223 goto 尾标签）。
        if (!db->exec(true))
        {
            goto nextEntry;
        }
        // 共享头 pad 区（m_pad28d8 内 +0x60）：具名成员下标 ⇒ 常量折叠单条 disp 读
        // （SGFC 的 (char*)sig+off 会多一条 add，N107）。
        if ((unsigned char)sig->m_pad28d8[0x2938 - 0x28d8])
        {
            continue;
        }
    nextEntry:
        ;
    }
    (void)slotPad;
}

// ============================================================================
// IsPCRoomIP（ORIG T 0x08412c52）
// ============================================================================

bool DB_Login::IsPCRoomIP(char* ip)
{
    MySQL* db = _GetDNFAccountDB();
    if (ip == 0 || ip[0] == '\0')
    {
        return false;
    }
    std::string prefix(ip);     // 去掉最后一段 ".x"：a.b.c → a.b
    std::string suffix(ip);     // 取最后一段数字：x（start_ip/end_ip 比较值）
    prefix.erase(prefix.find_last_of("."), std::string::npos);
    int lastOctet = atoi(suffix.erase(0, suffix.find_last_of(".") + 1).c_str());
    db->set_query("seLect count(*) from dnf_pcroom where ip='%s' and start_ip <= %d and end_ip >= %d",
                  prefix.c_str(), lastOctet, lastOctet);
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())
    {
        return false;
    }
    int cnt = 0;
    if (!db->get_int(0, cnt) || cnt == 0)
    {
        return false;
    }
    return true;
}

// ============================================================================
// CreateAccount（ORIG T 0x08412eb8）
// 【注】ORIG 把 SIG_LOGIN_DATA* 直接当 user_id 字符串传给 %s（两处 set_query/
// 日志同证；SIG 起始即 user_id 缓冲），忠实保留。
// ============================================================================

// [b28d-solo-DB_Login] verdict=NEAR ndiff=3 gap=0（N18a：hangame_flag 实参 ORIG
// `movzbl %al,%edx` 单段直落 %edx，4.4 恒 `movzbl %al,%eax`；P267 双 cast / P268
// mask / 字段 uchar / 探针 probe_cast2 八形全败，助记符序列相同 ⇒ NEAR 封顶）。
// S216：P267 双 cast 序 / P268 mask / P226 语句表达式三形再测均不翻
// （movzbl %al,%eax vs ORIG movzbl %al,%edx）⇒ 维持 NEAR 封顶。
bool DB_Login::CreateAccount(SIG_LOGIN_DATA* sig, MySQL* db)
{
    // [b28c-L9] 残差 3 = N18a：hangame_flag 实参 ORIG `movzbl %al,%edx; mov %edx,0xc(%esp)`
    // 单段直落 %edx，4.4 恒 `movzbl %al,%eax` + 独立中转（探针 P200/P226 形不可造）；
    // 助记符序列同 ⇒ NEAR 封顶。
    if (sig == 0 || db == 0)
    {
        return false;
    }
    db->set_query("call usp_create_account('%s')", sig);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x37e6, "[Taiwan, Account] Fail create account. id=%s", sig);
        return false;
    }
    register const char* logId = reinterpret_cast<const char*>(sig);   // P87：ORIG ctor 前载 %ebx 跨调用
    cMyTrace trace(__PRETTY_FUNCTION__, 0x37ea, 0);
    trace("[Taiwan, Account] Success create account. id=%s", logId);

    db->set_query(
        "seLect m_id, passwd,first_ssn, second_ssn,DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H'),user_name,email,hangame_flag,reg_date from member_info where user_id='%s' and hangame_flag=%d",
        sig, (unsigned int)(unsigned char)sig->m_loginType);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x3804, "[Taiwan, Account] Fail select member_info m_id=%s", sig);
        return false;
    }
    return true;
}

// ============================================================================
// _DecryptUserInfo（ORIG T 0x0841300e）
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=9 gap=0（N7/P75：Hex2Binary 第三实参
// ORIG 多 `mov %eax,%edx` 中转、Decrypt 侧我方多该中转；(int)/(unsigned int)/
// 无 cast 三形均不翻。p167 CLEAR）。
// S216：Hex2Binary/Decrypt 第三实参 cast 互换（(int)↔(unsigned int)）四形实测
// 逐条不变（同宽 cast 被 4.4 折叠）⇒ 维持 N7/P75 封顶。
bool DB_Login::_DecryptUserInfo(char* hex, char* out, unsigned int mid)
{
    // b28d-68 修法：ORIG 0x0841300e。①Initialize 后 cipherBuf 0x80 字节 rep stos 清零
    // （原注释「未清零」误判，ORIG 0x08413075..0x08413089 实证）；②strlen(hex) 三处
    // 表达式直用（ORIG 每处独立 call，P0 禁缓存局部）；③cMyTrace 全表达式临时链
    // （ORIG 槽 -0x4c/-0x3c/-0x2c 深→浅，P221/P227）；④Decrypt 返回值比较用 while
    // 形复现 4.1 setne/test/je 物化（P225）；⑤成功/失败直接 return 常量（ORIG 无 ok 局部）；
    // ⑥ret 入口初始化 0x6fffffff（ORIG 0x0841301a）。
    // 残差 N7/P75：Hex2Binary 第三实参 ORIG 多 `mov %eax,%edx` 中转、Decrypt 第三实参
    // 我方多该中转（4.4 先 lea 第一实参地址 ⇒ len/out 落 edx）；(int)/(unsigned int)/
    // 无 cast 三形实测均不翻（P248 三档试尽）。p167 CLEAR、gap=0（140=140）。
    unsigned int ret = 0x6fffffff;
    CRijndael cipher;
    cipher.Initialize("sbTgiYsfxpfg5fox", 0x20, CRijndael::sm_chain0, 0x10,
                      0x10, 0, 0);
    unsigned char cipherBuf[128] = {0};
    if (strlen(hex) >> 1 >= 0x81)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3815, 5)(
            "DB_Login::_DecryptUserInfo ERROR m_id=%s", NumberToString(mid, 0));
        return false;
    }
    if (!DNFFLib::Hex2Binary(hex, cipherBuf, (int)(strlen(hex) >> 1)))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x381b, 5)(
            "DB_Login::_DecryptUserInfo::Hex2Binary ERROR m_id=%s", NumberToString(mid, 0));
        return false;
    }
    ret = cipher.Decrypt(reinterpret_cast<const char*>(cipherBuf), out,
                         strlen(hex) >> 1);
    while (ret != 0x6fffffff)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3820, 5)(
            "DB_Login::_DecryptUserInfo::Decrypt ERROR m_id=%s ret=%u",
            NumberToString(mid, 0), ret);
        return false;
    }
    return true;
}

// ============================================================================
// GetAccountInfo（ORIG T 0x0841325e）
// 【N78/N80 blocked 归因（batch-15）】gap~138 主体 = 三块 4.1 memset 前端内联展开
// （serverPwd 0x84 步进字清 + dec 区 0x21×3 + 0x81 步进清，ORIG ~193 行 vs 4.4 恒
// call ~40 行；N78 五探针既定不可翻）+ 四处 cMyTrace varargs 前置物化（N80：4.1 于
// ctor call 前右起求 varargs 并 callee-saved 跨 call，4.4 恒 ctor 后求，三形实测败）。
// 可达部分已修：0x3b1b trace 分支重建、birthTm 逐字段直写（ORIG 无 memset 实证）、
// /100 无符号 magic、NumberToString 中转槽消除（1045/899）。
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=960 gap=-22（N78 memset 前端内联展开 +
// N80 varargs 前置物化 + N82 帧；p167 CLEAR 90/90：已补 get(7)/get(9) trace 的
// NumberToString ×2 并消 EXTRA memset ×7）。
bool DB_Login::GetAccountInfo(SIG_LOGIN_DATA* sig)
{
    const char* userId = reinterpret_cast<const char*>(sig);   // SIG 起始即 user_id
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect m_id, passwd,first_ssn, second_ssn,DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H'),user_name,email,hangame_flag,reg_date from  member_info  where  user_id='%s' and hangame_flag=%d",
        userId, (unsigned int)(unsigned char)sig->m_loginType);   // OFF_HANGAME_FLAG -> m_loginType @+0xa0
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x397f, 5);
        trace("DB_Login::GetAccountInfo, exec() ERROR m_id=%s", userId);
        return false;
    }

    // 查询为空则尝试建号
    if (db->get_n_rows() == 0 && !CreateAccount(sig, db))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3986, 0);
        trace("[Taiwan, Auth] Fail create account. id=%s", userId);
        return false;
    }

    if (!db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39af, 5);
        trace("DB_Login::GetAccountInfo, fetch() ERROR m_id=%s", userId);
        return false;
    }
    if (!db->get_uint(0, sig->m_accId))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39b5, 5);
        trace("DB_Login::GetAccountInfo, get(0) ERROR m_id=%s", userId);
        return false;
    }

    if (db->get_n_rows() != 1)
    {
        // 重复账号：member_abnomal 记录重叠登录
        int nRows = db->get_n_rows();
        db->set_query("upDate member_abnomal set overlab_count=%d where user_id=%s",
                      nRows, NumberToString(sig->m_accId, 0));
        bool updOk = db->exec(true);
        if (!(updOk && db->getAffectedRowCount() != 0))
        {
            db->set_query("inSert into member_abnomal(user_id, overlab_count) values(%s,%d)",
                          NumberToString(sig->m_accId, 0),
                          db->get_n_rows());
            db->exec(true);
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39c2, 5);
        trace("DB_Login::GetAccountInfo, get_n_rows()!=1 ERROR m_id=%s", userId);
        return false;
    }

    // ---- passwd 列 + SSO 校验 ----
    // [b28d-solo-DB_Login] P214③/N78 突破口：ORIG 为内联展开（非 call memset）
    // ⇒ 改显式零初始化（`= {0}`），4.4 复现 rep stos/展开而非 memset call。
    char serverPwd[0x84] = {0};
    if (!db->get_str(1, serverPwd, 0x84))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39c8, 5);
        trace("DB_Login::GetAccountInfo, get(1) ERROR m_id=%s", userId);
        return false;
    }

    WongWork::CSimpleSSO::stOutputData_t ssoOut;
    if (!GlobalData::s_psimpleSSO->checkValidPassword(
            sig->m_accId, serverPwd,
            SGBC(sig, OFF_CLIENT_IP), ssoOut))
    {
        // ORIG 0x84136c0-0x841370e：LogManager::logFormat（1,"DBThread.cpp",
        // __PRETTY_FUNCTION__,0x39da,"[Taiwan, Auth] Fail query member_login. (mid:%s)"）
        // 后返回 false（汇编 0x4a6 call LogManager::logFormat，非 cMyTrace）。
        LogManager::logFormat(
            1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x39da,
            "[Taiwan, Auth] Fail query member_login. (mid:%s)",
            NumberToString(sig->m_accId, 0));
        return false;
    }

    // stOutputData_t → sig 布局映射（逐字段，见 CSimpleSSO.h 与报告 0x84131xx 区指令）。
    // ORIG 0x8413713-0x8413790：+0x3958=lastPlayTime(4B!)、+0x3950=accountFail、
    // +0x3954=totalAccountFail、+0x3960=reportCnt、+0x3964=reliableFlag、
    // +0x3968=tradeGoldDaily、+0x396c=schoolId、+0x3970=rating、
    // +0x395c=dungeonGainGold、+0x3974..0x39bb schoolName 区清零。
    sig->m_lastPlayTime = ssoOut.m_lastPlayTime;                // +0x3958 ORIG mov %edx 4B 写
    sig->m_accountFail = ssoOut.m_accountFail;                  // +0x3950
    sig->m_totalAccountFail = ssoOut.m_totalAccountFail;        // +0x3954
    sig->m_reportMannerlessUserCount = ssoOut.m_reportCnt;      // +0x3960
    sig->m_reliablePersonFlag = ssoOut.m_reliableFlag;          // +0x3964
    sig->m_overTradeGoldCount = ssoOut.m_tradeGoldDaily;        // +0x3968
    sig->m_schoolNo = ssoOut.m_schoolId;                        // +0x396c
    sig->m_rating = ssoOut.m_rating;                            // +0x3970
    memset(sig->m_schoolName, 0, sizeof(sig->m_schoolName));    // +0x3974..0x39bb 清零

    GameWorld* world = G_GameWorld();
    if (world->IsSchoolPvPChannel() && sig->m_schoolNo != 0)
    {
        GetSchoolName(sig->m_schoolNo, sig->m_schoolName);
        GetBadgeCount(sig);
    }

    sig->m_tutorialSkipable = ssoOut.m_tutorialSkipable;        // +0x39a2
    SGFC(char, sig, OFF_SECURITY_VALID) = ssoOut.m_securityFlag; // +0x39dc（SigTypes_D 该区 pad，ORIG 写入保留）
    strncpy(SGBC(sig, OFF_SSO_LOGINIP), ssoOut.m_loginIp, 0x10); // +0x3a3a（共享头名 m_webAddress，语义错位保留）
    sig->m_dungeonGainedGold = ssoOut.m_dungeonGainGold;        // +0x395c

    long long curSec = GlobalData::s_systemTime_.getCurSec();
    if (CheckDailyScheduleTime(*reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x37c),
                               ssoOut.m_lastPlayTime, curSec))
    {
        sig->m_dungeonGainedGold = 0;
    }

    sig->m_userEventCharacterFlag = ssoOut.m_eventCharacFlag;   // +0x3f10

    // ---- first_ssn / second_ssn 解密 ----
    char firstSsn[33] = {0};
    char decFirst[33] = {0};
    char secondSsn[33] = {0};
    char decSecond[33] = {0};
    char email[129] = {0};
    char decEmail[129] = {0};

    unsigned int firstVal = 0;
    unsigned int secondVal = 0;
    if (!db->get_str(2, firstSsn, 0x21))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a3f, 5);
        trace("DB_Login::GetAccountInfo, get(2) ERROR m_id=%s", userId);
        return false;
    }
    if (firstSsn[0] != '\0')
    {
        if (!_DecryptUserInfo(firstSsn, decFirst,
                              sig->m_accId))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x3a48, 5);
            trace("_DecryptUserInfo(1) ERROR m_id=%s",
                  NumberToString(sig->m_accId, 0));
            return false;
        }
        firstVal = (unsigned int)atoi(decFirst);
    }
    if (!db->get_str(3, secondSsn, 0x21))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a51, 5);
        trace("DB_Login::GetAccountInfo, get(3) ERROR m_id=%s", userId);
        return false;
    }
    if (secondSsn[0] != '\0')
    {
        if (!_DecryptUserInfo(secondSsn, decSecond,
                              sig->m_accId))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x3a5a, 5);
            trace("_DecryptUserInfo(2) ERROR m_id=%s",
                  NumberToString(sig->m_accId, 0));
            return false;
        }
        secondVal = (unsigned int)atoi(decSecond);
    }

    // ---- DATE_FORMAT 生日/注册时间 ----
    unsigned int regDate = 0;
    if (!db->get_uint(4, regDate))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a71, 5);
        trace("DB_Login::GetAccountInfo, get(4) ERROR m_id=%s", userId);
        return false;
    }
    if (!db->get_str(5, SGBC(sig, 0x2184), 0x80))    // user_name → sig+0x2184
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a93, 5);
        trace("DB_Login::GetAccountInfo, get(5) ERROR m_id=%s", userId);
        return false;
    }
    if (!db->get_str(6, email, 0x81))               // email（加密串）
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3aa6, 5);
        trace("DB_Login::GetAccountInfo, get(6) ERROR m_id=%s", userId);
        return false;
    }
    if (email[0] != '\0' &&
        !_DecryptUserInfo(email, decEmail, sig->m_accId))
    {
        return false;
    }
    // ORIG 汇编 0x8413e91-0x8413eac：lea 0x2204 / snprintf(dst,0x7f,decEmail)
    // —— 直传解密串为格式参（3 参，无 "%s" 包装），原样复刻。
    OS_API::snprintf(SGBC(sig, 0x2204), 0x7f, decEmail);

    unsigned int hangameFlag = 0;
    if (!db->get_uint(7, hangameFlag))
    {
        // [b28d-solo-DB_Login] ORIG 0x08413ef3：此两处 trace 实参为
        // NumberToString(m_accId,0)（非 userId 指针）；P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x3ac0, 5)(
            "DB_Login::GetAccountInfo, get(7) ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        return false;
    }
    sig->m_hangameUser = (hangameFlag != 0);            // +0x38dd

    unsigned int regRaw = 0;
    if (!db->get_uint(8, regRaw))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3ae6, 5)(
            "DB_Login::GetAccountInfo, get(9) ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        // 注意：ORIG 此分支记日志后不返回，继续走时间换算
    }
    SGFC(unsigned int, sig, OFF_BIRTH_DATE) = regRaw;   // +0xd0（SigTypes_D 该区 pad，ORIG 写保留）
    OS_API::snprintf(sig->m_birth, sizeof(sig->m_birth), "%d%d",   // +0x2284
                     (int)firstVal, (int)secondVal);

    firstVal = firstVal / 10000;
    secondVal = secondVal / 1000000;
    sig->m_birthYear =                                    // +0xc8
        (secondVal < 3) ? (int)(firstVal + 0x76c) : (int)(firstVal + 2000);
    sig->m_regDate = regDate / 100;                       // +0xcc

    struct tm birthTm;
    // ORIG 0x084142xx：无 memset 调用——逐字段直写（`movl $0xffffffff,-0x1c4(%ebp)`
    // = tm_isdst=-1 word 直落实证），wday/yday/gmtoff 未初始化即入 mktime。
    birthTm.tm_isdst = -1;
    birthTm.tm_year = (int)(regDate / 1000000) - 0x76c;
    birthTm.tm_mon = (int)((regDate % 1000000) / 10000) - 1;
    // ORIG Ghidra 1436..：先 tm_hour=(regDate%1000000)%10000（unsigned），再
    // (uint)/100→mday、(uint)%100→hour ⇒ mul $0x51eb851f/shr $5 无符号 magic。
    birthTm.tm_hour = (int)((regDate % 1000000) % 10000);
    birthTm.tm_mday = (int)((unsigned int)birthTm.tm_hour / 100);
    birthTm.tm_hour = (int)((unsigned int)birthTm.tm_hour % 100);
    birthTm.tm_min = 0;
    birthTm.tm_sec = 0;
    sig->m_regTime = (unsigned int)mktime(&birthTm);      // +0xd4

    // ---- Garena token 复用检查 ----
    unsigned int nowSec = GlobalData::s_systemTime_.getCurSec();
    unsigned int loginTime = ssoOut.m_loginTime;
    unsigned int lastPlay = ssoOut.m_lastPlayTime;
    if (!Taiwan::GarenaCheckTimeStamp(sig->m_timeStamp, nowSec,   // +0x3a30
                                      loginTime, lastPlay))
    {
        if (!GetWhiteAccount(sig))
        {
            LogManager::logFormat(
                1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x3b14,
                "[Taiwan, Auth] Fail Token reuse login. (mid:%s, token_time:%u, current_time:%u, login_time:%u, last_play_time:%u)",
                NumberToString(sig->m_accId, 0),
                sig->m_timeStamp, nowSec, loginTime, lastPlay);
            return false;
        }
        // ORIG Ghidra 1474：white-success 亦 5-vararg（token_time 先 reload %eax）。
        LogManager::logFormat(
            1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x3b0f,
            "[Taiwan, Auth] White Account Success Token reuse login. (mid:%s, token_time:%u, current_time:%u, login_time:%u, last_play_time:%u)",
            NumberToString(sig->m_accId, 0),
            sig->m_timeStamp, nowSec, loginTime, lastPlay);
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3b1b, 0);
        trace("[Taiwan, Auth] Success Token reuse login. (mid:%s, token_time:%u, current_current:%u, login_time:%u, last_play_time:%u)",
              NumberToString(sig->m_accId, 0),
              sig->m_timeStamp, nowSec, loginTime, lastPlay);
    }
    return true;
}

// ============================================================================
// GetWhiteAccount（ORIG T 0x084143ac）
// ============================================================================

bool DB_Login::GetWhiteAccount(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect m_id from member_white_account where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return false;
    }
    if ((bool)(db->get_n_rows() != 0))
    {
        return true;
    }
    return false;
}

// ============================================================================
// GetSchoolName（ORIG T 0x08414446）
// ============================================================================

int DB_Login::GetSchoolName(int schoolId, char* name)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10, E_SERVER_GROUP_NONE);
    db->set_query("seLect school_name from school_info where school_id=%d", schoolId);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_str(0, name, 0x28))
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// GetBadgeCount（ORIG T 0x08414512）
// ============================================================================

bool DB_Login::GetBadgeCount(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10, E_SERVER_GROUP_NONE);
    for (int i = 0; i < 3; ++i)
    {
        sig->m_dailyBadge[i] = 0;                  // +0x399c green/red/blue 清零
    }
    int initTime = getDailyInitializeTime(6, 0, 0);
    db->set_query(
        "seLect daily_green_badge, daily_red_badge, daily_blue_badge from school_member where school_id=%d and m_id = %s and  last_play_time >= from_unixtime(%d)",
        sig->m_schoolNo,
        NumberToString(sig->m_accId, 0), initTime);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    // [b28d-solo-DB_Login] P0/P244：ORIG 无 pretty/green/red/blue 具名局部——
    // get_short 直接传 sig->m_dailyBadge[i]（`add $0x399c,%eax`），trace 用
    // 全表达式临时链（sig 入 %ebx 跨 ctor，P87/P227）。
    if (!db->get_short(0, sig->m_dailyBadge[0]))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3b60, 5)(
            "DB_Login::GetBadgeCount, get(0) ERROR m_id=%s",
            reinterpret_cast<const char*>(sig));
        return 0;
    }
    if (!db->get_short(1, sig->m_dailyBadge[1]))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3b66, 5)(
            "DB_Login::GetBadgeCount, get(1) ERROR m_id=%s",
            reinterpret_cast<const char*>(sig));
        return 0;
    }
    if (!db->get_short(2, sig->m_dailyBadge[2]))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3b6b, 5)(
            "DB_Login::GetBadgeCount, get(2) ERROR m_id=%s",
            reinterpret_cast<const char*>(sig));
        return 0;
    }
    return 1;
}

// ============================================================================
// GetGameDBInfo（ORIG T 0x08414772）
// ============================================================================

// [S216_DB_Login2] verdict=DIFF ndiff=6 gap=2（N-P209L：具名局部与常量比较
// 4.4 恒折 `cmpl $imm,mem`，ORIG 4.1 载寄存器两段；unsigned/int/long + 显式提升 +
// `L` 后缀四形实测均折 mem（P310 的 `<` setcc 形不适用 `==`）。P311 具名 Slot
// 视图已修数组寻址（20→17）。
// S216 本轮 22→6：P235 数组视角（m_characView）修 memmove src/dst 与槽比较地址链；
// int==unsigned 异型比较修两段装载（P211 档③）；余 N-P209L（switch 双 case 形
// gap 0 但 ndiff 8、显式 (unsigned) 提升形均不翻，已实测）。
int DB_Login::GetGameDBInfo(SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] N-P209L：ORIG 两次比较均两段寄存器形
    // `mov -0x14(%ebp),%eax; cmp $imm,%eax`（4.1 对局部载寄存器）；4.4 对具名局部
    // 与常量比较恒折内存直比。unsigned/int/long + 显式 (unsigned) 提升 + `L` 后缀
    // 四形实测均仍折 mem（P310 的 `<` setcc 形不适用于 `==`）⇒ 跨版本封顶。
    int viewRet = _getGameDBCharacView(sig);
    // [S216] N-P209L 复核：switch 双 case 形（gap 2→0 但 ndiff 6→8，4.4 产
    // je+叶块）与 `(unsigned int)` 显式提升形均不可翻 ⇒ 维持 if 链。
    if (viewRet == 0xffffffff)
    {
        return 1;
    }
    if (viewRet == 1)
    {
        return 0;
    }
    // [b28d-solo-DB_Login] P0：ORIG 无 world 具名局部（G_GameWorld 返回值直传
    // GetChannelType）；P222 正极性包裹复现 `sete; test; je <整块>`。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        // P28：ORIG 槽 i=-0x10（深）/count=-0xc ⇒ 先声明 i 后 count。
        int i = 0;
        int count = sig->m_characCount;          // +0xdc 槽计数
        for (; i < count; ++i)
        {
            if (sig->m_characView.m_slots[i].m_characNo ==
                sig->m_tournamentAccountFlag)           // server_id 匹配（int==unsigned 异型，P211 档③）
            {
                memmove(&sig->m_characView.m_slots[0],
                        &sig->m_characView.m_slots[i], 0x94);
                if (SGFC(char, sig, 0x39b0) != 0)       // charac_name 非空才覆写（SigTypes_D 隐式区）
                {
                    strncpy(sig->m_characData + 4, SGBC(sig, 0x39b0), 0x1e);
                }
                sig->m_characCount = 1;
                break;
            }
        }
        if (i == count)
        {
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// SaveLoginTime（ORIG T 0x084148a6）
// ============================================================================

void DB_Login::SaveLoginTime(unsigned int mid, int loginTime, char* ip)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    char table[20];                          // ORIG 槽序：table 深于 db/trace（-0x30/-0x1c/-0xc）
    memset(table, 0, sizeof(table));
    memcpy(table, "member_login", 0xd);      // ORIG 原样拷贝（含终结符）
    db->set_query(
        "inSert into %s(m_id, login_time, login_ip) values(%s, %d, '%s') on duplicate key upDate login_time=%d, login_ip='%s'",
        table, NumberToString(mid, 0), loginTime, ip, loginTime, ip);
    if (!db->exec(true))
    {
        register char* errId = NumberToString(mid, 0);   // P87：ORIG ctor 前载 %ebx 跨调用
        cMyTrace(__PRETTY_FUNCTION__, 0x3bd1, 0)(
            "DB_Login::SaveLoginTime, inSert ERROR m_id=%s, logintime=%d, ip=%s",
            errId, loginTime, ip);
    }
}

// ============================================================================
// get_compressed_blob_data（ORIG T 0x083fbc79）+ CCompressedBlob::uncompress
// （ORIG W 0x844df40，按反汇编忠实转写）
// ============================================================================

bool CCompressedBlobUncompress(CCompressedBlob& blob, MySQL* db, int col,
                               char* out, int outCap, int recordSize)
{
    // 容量 = outCap + outCap/6 + 0xc（ORIG imul 0x66666667 有符号除 6 公式）
    int cap = outCap + outCap / 6 + 0xc;
    blob.resize(cap);
    blob.reset();
    unsigned int compressedLen = (unsigned int)db->get_binary_length(col);
    if (!db->get_binary(col, blob.m_data, cap))
    {
        return false;
    }
    unsigned int outLen = (unsigned int)cap;
    if (!uncompress_zip(out, outLen, blob.m_data, compressedLen))
    {
        return false;
    }
    if (recordSize != 0 && (outLen % (unsigned int)recordSize) != 0 && outLen != 0)
    {
        return false;
    }
    return true;
}

// 其余 TU 声明均为 bool（mangled 不含返回型）；ORIG 尾 `call uncompress; leave; ret`
// bool→bool 透传零转换（int 返回+`?1:0` 反产 test/setne 尾段，实测 +5 行）。
bool get_compressed_blob_data(MySQL* db, int col, char* out, int outCap, int recordSize)
{
    static CCompressedBlob compressed_blob;   // ORIG _ZZ24get_compressed_blob_dataP5MySQLiPciiE15compressed_blob 0x0943d24c
    // ORIG 0x083fbc79 直接调成员 uncompress（容量公式 outCap + outCap/10 + 0xc，
    // 见 WindowsPackFiles.cpp），不经过 CCompressedBlobUncompress（/6 公式）。
    return compressed_blob.uncompress(db, col, out, outCap, recordSize);
}

// ============================================================================
// _getGameDBCharacView（ORIG T 0x084149ce）
// ============================================================================

// [S216_DB_Login2] verdict=AE ndiff=0 gap=0（0x084149ce 167=167）。
// 收口改法 = P235 数组视角：SIG_LOGIN_DATA 增匿名 union 同址视角 m_characView
// （SigTypes_D.h，SIG_CHARAC_SLOT_VIEW/SLOTS_VIEW），`sig->m_characView.m_slots[i]
// .m_characNo` 令 4.4 产 `mov i,%eax; mov sig,%edx; imul $0x94,%eax,%eax;
// lea (%edx,%eax,1),%eax; add $0xe0,%eax` 逐条 = ORIG（探针 /tmp/s216_probe1.cpp
// fA=reinterpret_cast 形恒 base 先 vs fD=视角形 i 先）。p167 CLEAR。
int DB_Login::_getGameDBCharacView(SIG_LOGIN_DATA* sig)
{
    MySQL* db = _getGameDB(DB_HANDLE_2);
    db->set_query(
        "seLect info,hash_key,password(info),slot_effect_count,charac_slot_limit from charac_view where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return -1;
    }
    if (db->get_n_rows() == 0)
    {
        return 1;
    }
    if (!db->fetch())
    {
        return -1;
    }
    if (!get_compressed_blob_data(db, 0, SGBC(sig, OFF_CHARAC_VIEW),
                                  0x14d0, 0x94))
    {
        // P87/P227：全表达式临时链 ⇒ m_accId 入 %ebx 跨 ctor 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x3beb, 5)(
            "DB_Login::_getGameDBCharacView uncompress fail, m_id(%d)",
            sig->m_accId);
        return -1;
    }

    // [b28d-solo-DB_Login] P253/P214③：ORIG 0x08414b00..0x08414b40 对三个 0x84
    // 缓冲各发 `rep stos` 0x21 dwords 清零（-0xa4 缓冲清零后未使用，P171 照抄）。
    char deadBuf[0x84] = {0};
    char hashKey[0x84] = {0};
    char pwdHash[0x84] = {0};
    (void)deadBuf;
    if (!db->get_binary(1, hashKey, 0x84))    // ORIG：get_binary 全量覆盖，无预清零
    {
        return -1;
    }
    if (!db->get_binary(2, pwdHash, 0x84))
    {
        return -1;
    }
    if (!db->get_ubyte(3, sig->m_characSlotCurCount))    // +0x3fa0
    {
        return -1;
    }
    if (!db->get_ubyte(4, sig->m_characSlotMaxCount))    // +0x3fa1
    {
        return -1;
    }

    sig->m_characCount = 0;                              // +0xdc
    // [b28d-solo-DB_Login] P311 具名 Slot 数组视图（CharacDataView，见文件头
    // 匿名 namespace）：ORIG 地址链 `mov -0xc(%ebp),%eax; mov 0xc(%ebp),%edx;
    // imul $0x94,%eax,%eax; lea (%edx,%eax,1),%eax; add $0xe0,%eax; mov (%eax),%eax`
    // （i 先算、基址后并、0xe0 最后加）⇒ 完整类型成员直访问逐条复现（探针 f4）；
    // 旧 `sig->m_characData + i*0x94`（char 数组 + 指针算术）恒把 add $0xe0 提前。
    // ORIG：36 个槽位（i<=0x23；blob 解压 0x14d0 = 36×0x94），非 37
    for (int i = 0; i <= 0x23; ++i)
    {
        // [S216/P235] 改具名数组视角（SigTypes_D.h 匿名 union）⇒ 4.4 产
        // `mov i,%eax; mov sig,%edx; imul; lea; add $0xe0; mov (%eax),%eax` 逐条 = ORIG。
        if (sig->m_characView.m_slots[i].m_characNo != 0)
        {
            sig->m_characCount += 1;
        }
    }
    return 0;
}

// ============================================================================
// SendResult（ORIG T 0x08414c6e）/ UpdateLoginStat（ORIG T 0x08414d50）
// ============================================================================

int DB_Login::SendResult(int userDbIdx, int code, SIG_LOGIN_DATA* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x3c49), true);
    guard.operator*()->operator<<(userDbIdx);
    guard.operator*()->operator<<(code);
    guard.operator->()->put_binary(sig, 0x4358);  // SIG_LOGIN_DATA 全量回传
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// [b28d-solo-DB_Login] verdict=AE（P211 档③同型化：PktView m_accId→unsigned int、
// m_gcNo→int 使 RHS 直装 %edx 无 eax 中转，ndiff 4→0；P259 消 stream 具名局部）。
int DB_Login::UpdateLoginStat(int userDbIdx, SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P259：ORIG 无 stream 具名局部（Acquire 返回值直接作
    // CStreamGuard ctor 第 2 实参；guard 槽 -0x14）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x3c5d), true);
    guard.operator*()->operator<<(0x2b);                      // 包 id（SIG_UPDATE_LOGIN 上报）
    guard.operator*()->operator<<(userDbIdx);
    // [b28d-solo-DB_Login] P82/P211 档③：ORIG 三条 store 均 RHS-first+disp 直写
    // （目标与 RHS 异型）⇒ 具名视图逐字段访问（裸 cast+偏移恒 LHS-first+lea）。
    struct __attribute__((packed)) PktView
    {
        unsigned int m_accId;        // +0x00（与 RHS sig->m_accId 同型 ⇒ RHS 直装 %edx 无中转）
        int          m_gcNo;         // +0x04（与 RHS sig->m_gcNo 同型 ⇒ 同上）
        unsigned int m_serverGroup;  // +0x08（目标 unsigned vs RHS int）
        char         m_ip[0x14];     // +0x0c
    };
    PktView* pkt = reinterpret_cast<PktView*>(
        guard.operator->()->GetInBuffer<SIG_UPDATE_LOGIN>());
    pkt->m_accId = sig->m_accId;
    pkt->m_serverGroup = (int)sig->m_channelRestrictField;   // +0x3a38 serverGroup(short→int)
    pkt->m_gcNo = sig->m_gcNo;                              // +0x9c
    strncpy(pkt->m_ip, SGBC(sig, OFF_LOGIN_IP), 0x14);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ============================================================================
// GetCheckPickUpRandomOptionItem（ORIG T 0x08414e7a）
// ============================================================================

int DB_Login::GetCheckPickUpRandomOptionItem(SIG_LOGIN_DATA* sig)
{
    sig->m_checkPickUpRandomOptionItemFlag = false;      // +0x3fa8
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect check_count from check_pick_up_random_option_item where m_id = %s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows != 0)
    {
        if (!db->fetch())
        {
            return 0;
        }
        if (!db->get_ubyte(0, reinterpret_cast<unsigned char&>(sig->m_checkPickUpRandomOptionItemFlag)))
        {
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// GetOntimeUserRecvIdx（ORIG T 0x08414f5c）
// ============================================================================

// [S216_DB_Login2] verdict=AE ndiff=0 gap=0（0x08414f5c 104=104；下列 N18a/N82 判词作废）
// 序列 4.4 中途 store/load 回读 mod 槽（ORIG 全程寄存器算完再存 -0xc）；P0 去 mid
// 缓存后 32→6）。
// S216 收口（原 N18a/N82 判词作废）：`int mod = (int)(sig->m_accId % 10u);`
// （P217/P310 类型旋钮）⇒ 4.4 除法 magic 中间值不再 spill 到 -0xc，逐条 = ORIG。
int DB_Login::GetOntimeUserRecvIdx(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    // [b28d-solo-DB_Login] P0/P48：ORIG 槽 -0xc 存 `m_accId % 10` 结果（具名局部），
    // 且 NumberToString 前**再读一次** m_accId（无 mid 缓存）。
    sig->m_ontimeLastRecvIdx = 0;                        // +0x3fa4
    int mod = (int)(sig->m_accId % 10u);
    db->set_query("seLect last_no from event_1112_ontime_user_%u where m_id=%s",
                  mod, NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_ontimeLastRecvIdx = 1;
        db->set_query("inSert into event_1112_ontime_user_%u(m_id,last_no) values (%s,%u )",
                      mod, NumberToString(sig->m_accId, 0),
                      sig->m_ontimeLastRecvIdx);
        if (!db->exec(true))
        {
            return 0;
        }
    }
    else
    {
        if (!db->fetch())
        {
            return 0;
        }
        if (!db->get_int(0, sig->m_ontimeLastRecvIdx))
        {
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// GetUserFatigueCount（ORIG T 0x084150e8）
// ============================================================================

// [S216_DB_Login2] verdict=NEAR ndiff=8 gap=0（N7：entity vtable+0x20 间接 call
// 目标 4.4.7 恒 %eax / ORIG %edx，且 occDay 装载时刻差 1 位；4.4.6 后端可翻但属
// Main 路由裁决。指令数 118=118、p167 CLEAR）。
// S216 本轮 DIFF→NEAR：NumberToString 内联进 set_query 实参 ⇒ occDay
// `movzbl -0x11(%ebp),%esi` 装载时刻回到 ORIG 位；余 N7 间接 call 目标寄存器
// （4.4.7 恒 %eax vs ORIG %edx，4.4.6 后端可翻，属 Main 路由裁决）。
int DB_Login::GetUserFatigueCount(SIG_LOGIN_DATA* sig)
{
    // [b28c-L9] 残差 6 = 全为跨版本寄存器/调度差（N7）：
    // ① entity vtable+0x20 间接 call：ORIG funcptr→%edx/实参→%eax，4.4 恒互换；
    // ② `movzbl -0x11(%ebp),%esi`（occDay）装载时刻相对 G_CDataManager 调用点差 1 位。
    // 指令数 118=118、助记符序列全等 ⇒ NEAR 封顶。
    // ORIG：无 mng 具名局部，每处重新求值链式表达式（P48 无 CSE）
    if (!(static_cast<EventClassify::CEventScriptMng*>(
              G_CDataManager()->get_event_script_mng()))->is_eventing(0x56))
    {
        return 0;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    sig->m_fatigueValue = 0;                             // +0x3faa
    sig->m_fatigueKind = 0;                              // +0x3fa9

    EventClassify::CEventEntity* entity = 0;
    entity = static_cast<EventClassify::CEventEntity*>(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_event_entity(0x56));
    if (entity == 0)
    {
        return 0;
    }
    unsigned char occDay = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())->get_today_time_day();

    // ORIG vtable+0x20 槽：取事件表名（std::string），经 toTString 恒等转 const char*；
    // 结果不落槽，直接进实参位（ORIG +0x0f4 `mov %eax,0x8(%esp)`）。
    typedef std::string* (*GetTableFn)(void*);
    db->set_query("seLect fatigue from %s where m_id=%s and occ_day = %d",
                  toTString(*((*reinterpret_cast<GetTableFn*>(
                      *reinterpret_cast<void**>(entity) + 0x20))(entity))),
                  NumberToString(sig->m_accId, 0), (unsigned int)occDay);
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows > 0)
    {
        short fatigue = 0;
        bool ok = db->fetch();
        if (!ok)
        {
            return 0;
        }
        ok = db->get_short(0, fatigue);
        if (!ok)
        {
            return 0;
        }
        sig->m_fatigueValue = fatigue;
        sig->m_fatigueKind = occDay;                     // ORIG param[0x3fa9] = local_15(day)
    }
    return 1;
}

// ============================================================================
// GetUserInGameEventHistory（ORIG T 0x084152b0）
// ============================================================================

// [S216_DB_Login2] verdict=AE ndiff=0 gap=0（0x084152b0 119=119；下列 N1 判词作废）
// `add $0x3fa0; add 0xc(%ebp); add $0xc` 三步，4.4 恒折 $0x3fac + lea；十形探针 +
// P311 二维视图思路均不翻）。
// S216 收口（原 N1 判词作废）：P235/P311 历史表数组视角（SigTypes_D.h 匿名 union
// m_ingameEventHistoryView）⇒ 4.4 复现 `add $0x3fa0; add sig; add $0xc` 三步链，
// 逐条 = ORIG（探针 /tmp/s216_probe3.cpp fH）。
int DB_Login::GetUserInGameEventHistory(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    memset(SGBC(sig, OFF_HISTORY_TBL), 0, 0x168);    // 30 行 × {type,h1,h2}
    db->set_query(
        "seLect type , history_1 ,history_2 from event_ingame_history where m_id=%s order by type desc",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int nRows = db->get_n_rows();
    bool ok;
    for (int i = 0; i < nRows; ++i)
    {
        if (nRows > 0x1d)
        {
            return 1;                    // ORIG 上限即断（>29 行不再读取）
        }
        ok = db->fetch();
        if (!ok)
        {
            break;
        }
        // [S216/P235] 改具名历史表数组视角（SigTypes_D.h 匿名 union）⇒ 4.4 产
        // `imul/三 add 链 + add $0x3fa0 + add sig + add $0xc` 逐条 = ORIG（探针
        // /tmp/s216_probe3.cpp fH）；旧裸 char* 算术恒折 $0x3fac（N1）。
        ok = db->get_int(0, sig->m_ingameEventHistoryView.m_rows[i].m_type);
        if (!ok)
        {
            break;
        }
        ok = db->get_int(1, sig->m_ingameEventHistoryView.m_rows[i].m_history1);
        if (!ok)
        {
            break;
        }
        ok = db->get_int(2, sig->m_ingameEventHistoryView.m_rows[i].m_history2);
        if (!ok)
        {
            break;
        }
    }
    return 1;
}

// ============================================================================
// CheckLoginIP（ORIG T 0x08415472）
// ============================================================================

// [b28d-solo-DB_Login] verdict=AE（SGBC 宏形 off 字面量折叠 + std::strrchr；p167
// 唯一差 `_ZSt7strrchrPci` vs `strrchr` = 头版本 N，语义等价）。
bool DB_Login::CheckLoginIP(SIG_LOGIN_DATA* sig)
{
    if (sig == 0)
    {
        return 0;
    }
    sig->m_ingameHackItemFlag = false;           // +0x4124 ip 变更标志清零

    // [b23S3C] P48/§3.2②：偏移地址用 SGBC 宏直算（ORIG 每用点 `mov 0xc;add $imm`
    // 2 条）；SGB always_inline 实参 off 仍落槽（+3 条/处、帧 0x58→0x78），
    // 非模板参数化不可免；truthy 直判去 `!=0`（ORIG test%al je 无 setne 物化）。
    // dot 声明先于双缓冲（ORIG 预清序 -0x10(dot) → curIp → oldIp）。
    char* dot = 0;
    char curIp[16] = {0};
    char oldIp[16] = {0};
    strcpy(curIp, SGBC(sig, OFF_LOGIN_IP));
    // ORIG：双 16 字节缓冲，当前 ip 与旧登录 ip(+0x4114)
    strcpy(oldIp, SGBC(sig, 0x4114));

    // 单 dot 变量两次复用（ORIG 两 strrchr 结果同写 -0x10 槽）。
    // [b28d-38] P167：ORIG 调 `_ZSt7strrchrPci`（std::strrchr）非 C 库 strrchr。
    dot = std::strrchr(curIp, '.');              // 截掉最后一段（去端口化前缀）
    if (dot != 0)
    {
        *dot = '\0';
    }
    dot = std::strrchr(oldIp, '.');
    if (dot != 0)
    {
        *dot = '\0';
    }
    if (strcmp(curIp, oldIp) != 0)
    {
        if (SGBC(sig, 0x4114)[0])
        {
            sig->m_ingameHackItemFlag = true;
        }
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
        db->set_query("upDate login_common set login_ip='%s' where m_id=%u",
                      SGBC(sig, OFF_LOGIN_IP),
                      sig->m_accId);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__, 0x3e63,
                                  "LOGIN_IP : IP Address UDATE DB ERROR!!");
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// GetMercenarySystemInfo（ORIG T 0x08415618）
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=219 gap=-9（N82 帧 0x60↔0x70 + N1 常量
// 拆分（ORIG `add $0xf0; movzwl 0xa(%eax)` vs 我方 `add $0xe0; ... 0x1a`）+ N115
// outgoing；p167 CLEAR 27/27）。
bool DB_Login::GetMercenarySystemInfo(SIG_LOGIN_DATA* sig)
{
    if (sig == 0)
    {
        return 0;
    }
    MySQL* dbBonus = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* dbItems = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);

    int outIdx = 0;
    for (int i = 0;
         i < sig->m_characCount && i < 0x24; ++i)
    {
        // [hc 收] sig->m_characData 为 SIG_LOGIN_DATA 角色视图数组（步长 0x94），
        // +0x1a 即槽内等级(short)字段裸偏移（§5.3 线格式序列化缓冲豁免）。
        short level = *reinterpret_cast<short*>(sig->m_characData + i * 0x94 + 0x1a);       // 槽内 charac 等级(short)
        if (*reinterpret_cast<short*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa8e0) <= level)
        {
            // P183：ORIG 每次使用均由 sig 重新展开 charac_no（sig+0xe0 视图）与
            // mercenary 基址，禁具名缓存。
            dbBonus->set_query(
                "seLect mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period from charac_link_bonus where charac_no=%u",
                *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
            if (!dbBonus->exec(true))
            {
                return 0;
            }

            sig->m_mercenaries[outIdx].Reset();
            strncpy(sig->m_mercenaries[outIdx].m_name,
                    sig->m_characData + i * 0x94 + 4, 0x1e);        // 角色名
            sig->m_mercenaries[outIdx].m_characNo =
                *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94);
            sig->m_mercenaries[outIdx].m_level =
                *reinterpret_cast<short*>(sig->m_characData + i * 0x94 + 0x1a);

            dbItems->set_query(
                "seLect slot,it_id from user_items where charac_no=%u and expire_date>now() and stat=0 order by slot limit 8",
                *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
            if (!dbItems->exec(true))
            {
                // ORIG（反编译 C）：exec 失败即跳过本角色的物品装载与
                // charac_link_bonus 处理，继续下一角色（最终仍 return 1），
                // 非提前 return 1 终结全函数。
                continue;
            }
            int nRows = dbItems->get_n_rows();
            int itemId = 0;
            if (nRows == 8)
            {
                for (int expectSlot = 0; expectSlot < nRows; ++expectSlot)
                {
                    if (!dbItems->fetch())
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3ea2, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u", 0x3ea2,
                              *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
                        break;
                    }
                    int gotSlot = 0;
                    if (!dbItems->get_int(0, gotSlot))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3ea8, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u", 0x3ea8,
                              *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
                        return 0;
                    }
                    if (!dbItems->get_int(1, itemId))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3eae, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR charac_no=%u", 0x3eae,
                              *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
                        return 0;
                    }
                    if (gotSlot != expectSlot)
                    {
                        sig->m_mercenaries[outIdx].m_avatarBonus = 0;   // 连续槽位破坏标志（+0x32）
                        break;
                    }
                    sig->m_mercenaries[outIdx].m_avatarItems[expectSlot] = itemId;
                }
            }
            else
            {
                sig->m_mercenaries[outIdx].m_avatarBonus = 0;
            }
            if (dbBonus->getAffectedRowCount() == 0)
            {
                dbBonus->set_query(
                    "inSert into charac_link_bonus (charac_no,mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period) values(%u,0,0,-1,-1)",
                    *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
                if (dbBonus->exec(true))
                {
                    sig->m_mercenaries[outIdx].m_field28 = 0;      // mercenary_start_time(+0x28)
                    sig->m_mercenaries[outIdx].m_field2c = 0;      // mercenary_finish_time(+0x2c)
                    sig->m_mercenaries[outIdx].m_field30 = 0xff;   // mercenary_area(+0x30)
                    sig->m_mercenaries[outIdx].m_field31 = 0xff;   // mercenary_period(+0x31)
                    outIdx += 1;
                    sig->m_mercenaryCount += 1;
                }
            }
            else
            {
                bool ok = dbBonus->fetch();
                // ORIG 这两处 get_int 走 _ZN5MySQL7get_intEiRj（unsigned int&，
                // ORIG 0x80e22ae；m_field28/m_field2c 同为 +0x28/+0x2c 的 4 字节，
                // 与报告 uint* 视图口径一致）。
                if (ok) ok = dbBonus->get_int(0, reinterpret_cast<unsigned int&>(sig->m_mercenaries[outIdx].m_field28));
                if (ok) ok = dbBonus->get_int(1, reinterpret_cast<unsigned int&>(sig->m_mercenaries[outIdx].m_field2c));
                if (ok) ok = dbBonus->get_byte(2, reinterpret_cast<char&>(sig->m_mercenaries[outIdx].m_field30));
                if (ok) ok = dbBonus->get_byte(3, reinterpret_cast<char&>(sig->m_mercenaries[outIdx].m_field31));
                if (ok)
                {
                    outIdx += 1;
                    sig->m_mercenaryCount += 1;
                }
            }
        }
    }
    return 1;
}


// ============================================================================
// GetGrowthWeaponEvent（ORIG T 0x08447ea6）
// ============================================================================

bool DB_Login::GetGrowthWeaponEvent(SIG_LOGIN_DATA* sig)
{
    // P1 组件视图：ORIG 清块/写块单指令 disp（movb/movw/movl $imm,0x414x(%eax)）
    struct __attribute__((packed)) GWView {
        char m_pad[0x4140];
        char m_has;                            // +0x4140
        char m_pad1[3];
        unsigned int m_characNo;               // +0x4144
        unsigned int m_weaponId;               // +0x4148
        unsigned int m_infWeaponId;            // +0x414c
        unsigned short m_timepiece;            // +0x4150
        unsigned short m_serverId;             // +0x4152
    };
    bool ok;
    MySQL* db;
    // [b28c-L9] **P0 清绑**：原 `register CEventBase* evt asm("eax") = …` 系历史寄存器
    // 绑定（N96 已登记全域禁用），删绑后按 P226/BM11 正向路径改**单语句内联形**——
    // ORIG +0x1b 的 GetRepeatEvent 结果无槽往返（`mov (%eax),%edx; …; mov %eax,(%esp);
    // call *%edx`）⇒ 按 P0 原码本无具名局部，直调虚槽 +0x34 写在条件表达式内。
    if (!(GlobalData::s_event_manager->GetRepeatEvent(100))->IsEventing(0))
    {
        return 1;
    }
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect server_id, charac_no, growthweapon_id, timepiece, infinityweapon_id from event_1207_growthweapon where m_id = %d",
        sig->m_accId);
    ok = db->exec(true);                       // ORIG %al→-0xd 槽回读
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0xb487,
                              "LoadGrowthWeaponEvent Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    if ((bool)(db->get_n_rows() != 0))
    {
        if (!db->fetch())
        {
            return 0;
        }
        ((GWView*)sig)->m_has = 1;
        if (!db->get_short(0, SGFC(short, sig, OFF_GROWTH_WEAPON + 0x12))) return 0;   // server_id
        if (!db->get_int(1, SGFC(int, sig, OFF_GROWTH_WEAPON + 4))) return 0;          // charac_no
        if (!db->get_int(2, SGFC(int, sig, OFF_GROWTH_WEAPON + 8))) return 0;          // growthweapon_id
        if (!db->get_short(3, SGFC(short, sig, OFF_GROWTH_WEAPON + 0x10))) return 0;   // timepiece
        if (!db->get_int(4, SGFC(int, sig, OFF_GROWTH_WEAPON + 0xc))) return 0;        // infinityweapon_id
    }
    else
    {
        // ORIG 清块写入序：每 store 重读 sig（无跨 store 缓存）
        ((GWView*)sig)->m_has = 0;                           // +0x20f movb
        ((GWView*)sig)->m_serverId = 0;                      // +0x219 movw
        ((GWView*)sig)->m_characNo = 0;                      // +0x225 movl
        ((GWView*)sig)->m_weaponId = 0;                      // +0x232 movl
        ((GWView*)sig)->m_timepiece = 0;                     // +0x23f movw
        ((GWView*)sig)->m_infWeaponId = 0;                   // +0x24b movl
    }
    return 1;
}

// ============================================================================
// GetGrowthCreatureEvent（ORIG T 0x08448102）
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=17 gap=0（P0/P183 具名 char* 行指针 +
// P232 死 int 局部修复 37→17；余 = 帧 outgoing 差 4（N115）+ N1 常量折叠
// `(i+0x866)*8+8` → `(i+0x867)*8`（探针 probe_gift f5））。
// S216：P205 显式拆形实测仍折（0x4150+4 → 0x4154）⇒ 维持 N1/N115 封顶。
bool DB_Login::GetGrowthCreatureEvent(SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P0/P140：ORIG 无 evt 具名局部、无判空（GetRepeatEvent
    // 返回值直传虚调用）；P5 真虚调用原地展开。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x67)->IsEventing(0))
    {
        return 1;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect server_id, charac_no, job_type, charac_name from event_1208_growthcreature where m_id = %d",
        sig->m_accId);
    // P158/P168a④：ORIG `mov %al,-0x19; movzbl; xor $1; test; je` ⇒ bool 具名局部。
    bool execOk = db->exec(true);
    if (!execOk)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0xb4b9,
                              "LoadGrowthCreatureEvent Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    // P222/P214①：ORIG `test %eax; setne; test; je` ⇒ 正极性包裹（!=0 走 for）。
    if (db->get_n_rows() != 0)
    {
        for (unsigned int i = 0; i < db->get_n_rows(); ++i)
        {
            if (!db->fetch())
            {
                return 0;
            }
            if (i > 10)
            {
                return 0;
            }
            // [b28d-solo-DB_Login] P0/P183：ORIG 有具名 char* 局部 -0x10
            // （`i*0x2c; add $0x4150; add sig; add $0x4` ⇒ ptr = 行首）与死 int
            // 局部 -0xc（P232 帧律，`movl $0x0,-0xc(%ebp)`），四个 getter 直接写
            // 字段（ptr / ptr+4 / ptr+8 / ptr+0xc），无 serverId/characNo/jobType
            // 三个临时槽与写回。
            char* rowPtr = reinterpret_cast<char*>(sig) + i * 0x2c + OFF_GROWTH_CREATURE;
            int rowPad = 0;
            (void)rowPad;
            if (!db->get_short(0, *reinterpret_cast<short*>(rowPtr))) return 0;
            if (!db->get_int(1, *reinterpret_cast<int*>(rowPtr + 4))) return 0;
            if (!db->get_short(2, *reinterpret_cast<int*>(rowPtr + 8))) return 0;
            if (!db->get_str(3, rowPtr + 0xc, 0x1d)) return 0;
        }
    }
    else
    {
        memset(SGBC(sig, OFF_GROWTH_CREATURE), 0, 0x1e4);
    }
    return 1;
}

// [b28d-solo-DB_Login] verdict=DIFF ndiff=15 gap=0（P0/P183 具名 int* 局部修复
// 32→15；余 = 帧 outgoing 槽差 0x10（N115）+ N1 常量折叠（probe_gift f5））。
// S216：P205 显式拆形 `(i+0x866)*8+8` 实测仍折 `(i+0x867)*8` ⇒ 维持 N1/N115 封顶。
bool DB_Login::GetAccountCharacterLevelUpOnceGift(SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P0/P140：同 0x67 件（无 evt 局部/判空）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x68)->IsEventing(0))
    {
        return 1;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query("seLect job_type, level from event_1208_level_achieve where m_id = %d",
                  sig->m_accId);
    bool execOk = db->exec(true);           // P158：ORIG bool 槽（-0x15）
    if (!execOk)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xb4e8,
                              "LoadAccountCharacterLevelUpOnceGift Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    if (db->get_n_rows() != 0)              // P222 正极性包裹
    {
        for (unsigned int i = 0; i < db->get_n_rows(); ++i)
        {
            if (!db->fetch())
            {
                return 0;
            }
            if (i > 3)
            {
                return 0;
            }
            // [b28d-solo-DB_Login] P0/P183：ORIG 有**具名 int* 局部**（-0xc 槽，
            // 两个 getter 各回读一次：`mov -0xc(%ebp),%eax; mov %eax,0x8(%esp)` /
            // `mov -0xc(%ebp),%eax; add $0x4,%eax`）⇒ 原码是「指针局部 + 下标直读」，
            // 非 jobType/level 两个 int 局部（多 2 槽 + 2 次字段写回）。
            // 地址链 N1：ORIG `add $0x866; shl $3; add sig; add $0x8` 被 4.4 折成
            // `(i+0x867)*8`（探针 probe_gift f5）⇒ 该 3 条常量折叠差封顶。
            // [S216] N1 复核：P205 显式拆形 `(i+0x866)*8+8` 实测仍被 4.4 折成
            // `(i+0x867)*8`（ndiff 30 不变）⇒ 维持 OFF_ONCE_GIFT 常量形。
            int* giftSlot = reinterpret_cast<int*>(
                reinterpret_cast<char*>(sig) + OFF_ONCE_GIFT + i * 8);
            if (!db->get_short(0, giftSlot[0])) return 0;
            if (!db->get_int(1, giftSlot[1])) return 0;
        }
    }
    else
    {
        memset(SGBC(sig, OFF_ONCE_GIFT), 0, 0x20);
    }
    return 1;
}

// ============================================================================
// GetBingoEvent（ORIG T 0x08448bee）
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=2 gap=-1（N107：成员常量偏移取址 ORIG
// `lea 0x413c(%eax),%edx` vs 4.4 `add+mov`；P0 resetBase unsigned 化修装载时刻
// （5→2）。p167 CLEAR）。
// S216：union{unsigned int;int} 同址 int 视图实测仍产 add+mov（探针
// /tmp/s216_probe2.cpp fA/fB）⇒ 维持 N107（成员常量取址 lea-vs-add）封顶。
bool DB_Login::GetBingoEvent(SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P0/P140：ORIG 无 bingo 引用局部（地址直传 clear）、
    // 无 evt 局部/判空（GetRepeatEvent 返回值直传虚调用）。
    reinterpret_cast<SIG_BINGO_EVENT*>(SGBC(sig, OFF_BINGO_EVT))->clear();
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
    {
        return 1;                            // 事件未开启：视为无数据成功
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    // [b28d-solo-DB_Login] P0：ORIG 无 curSec 具名槽（getCurSec 结果 eax 直传），
    // +0x37c 为 disp 直读（P82 视图；裸 cast+off 多一条 add）。
    struct EnvView { char m_pad[0x37c]; int m_dailyResetHour; };
    // [b28d-solo-DB_Login] P0/P75：ORIG 实参在 NumberToString **之后**从 -0x18 槽
    // 重载（`mov -0x18(%ebp),%edx`），无 cast 中转 ⇒ 具名局部即 unsigned int。
    unsigned int resetBase = (unsigned int)GetCurrentResetBaseTime(
        GlobalData::s_systemTime_.getCurSec(),
        reinterpret_cast<EnvView*>(G_CEnvironment())->m_dailyResetHour);
    db->set_query(
        "seLect board,reward from event_1208_bingo where m_id = %s and occ_date > from_unixtime(%u)",
        NumberToString(sig->m_accId, 0),
        resetBase);
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();             // P0：ORIG 具名槽 -0x14
    if (rows == 0)
    {
        return 1;
    }
    // P198①：ORIG 单一 bool 槽 -0xd 预清 + 独立赋值语句（fetch/get_int/get_byte 复用）。
    bool ok = false;
    ok = db->fetch();
    if (!ok)
    {
        // P227：全表达式临时链（NumberToString 入 %ebx 跨 ctor）。
        cMyTrace(__PRETTY_FUNCTION__, 0xb71b, 5)(
            "DB_Login::GetBingoEvent, fetch() ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    // [b28d-solo-DB_Login] ORIG 0x08448d7a..：col=0 递增计数器（col++）+ 复用同一
    // bool 槽 -0xd；get_int/get_byte 直接写 sig 字段（无 reward/board 局部）；
    // trace 传递增后的 col（P198①/P227）。
    int col = 0;
    ok = db->get_int(col++, reinterpret_cast<int&>(sig->m_bingoEventId));   // +0x413c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb723, 5)(
            "DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    ok = db->get_byte(col++, sig->m_bingoWinFlag);                          // +0x4138
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb729, 5)(
            "DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    // [b28d-solo-DB_Login] 真语义修正：ORIG 成功路径落 08448eac `mov $0x0,%eax`
    // （Ghidra C `uVar3 = 0`）⇒ 返回 0（仅 rows==0 与事件未开启返回 1）。
    return 0;
}

// ============================================================================
// IsThisIPLoginPermissionGranted（ORIG T 0x0844b982）
// ============================================================================

bool DB_Login::IsThisIPLoginPermissionGranted(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    unsigned int allowCnt = 0;
    sig->m_geoAllowSkipFlag = false;                 // +0xb5
    db->set_query("seLect count(*) from geo_allow where allow_ip = '%s'",
                  SGBC(sig, OFF_LOGIN_IP));
    if (!db->exec(true))
    {
        // [b28d-solo-DB_Login] P221/P227：兄弟块三 trace 全表达式临时链复现 ORIG
        // 槽序（-0x3c/-0x2c/-0x1c 深→浅）。
        cMyTrace(__PRETTY_FUNCTION__, 0xbc49, 5)(
            "[QUERY_ERROR] (select from geo_allow)");
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 1;
    }
    if (!db->fetch() || !db->get_uint(0, allowCnt))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbc52, 5)(
            "[QUERY_ERROR] (fetch get_uint)");
        return 0;
    }
    if (allowCnt != 0)
    {
        sig->m_geoAllowSkipFlag = true;              // +0xb5 geo_allow 命中
    }
    db->set_query("insert into member_join_info(m_id, reg_date, ip) values(%d, now(), '%s')",
                  sig->m_accId,
                  SGBC(sig, OFF_LOGIN_IP));
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbc63, 5)(
            "[QUERY_ERROR] (insert into member_join_info)");
        return 0;
    }
    return 1;
}

// ============================================================================
// getBreakAwayMemberData（ORIG T 0x084101ae）
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=127 gap=30（N82 帧：ORIG
// `push esi/ebx + sub 0x80` vs 4.4 `sub 0x98` + N78 memset 0x1f 内联展开；
// p167 CLEAR 44/44）。
bool DB_Login::getBreakAwayMemberData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect accrue_cera, play_info, last_reward_time, first_reward_time, luck_point, last_update_time, add_info, second_reward_time, quest_time from churn_member_info where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x307b,
                              "BREAK_AWAY : churn_member_info DB ERROR!!");
        return 0;
    }

    char playInfo[31];                       // ORIG local_5f[0x1f]
    if (db->get_n_rows() == 0)
    {
        for (int i = 0; i < 0x1f; ++i)
        {
            playInfo[i] = 0;                 // ORIG 4.1 内联 memset(buf,0,0x1f)
        }
        memset(playInfo, 0x30, 0x1e);        // '0'×30（初始 play_info 占位）
        playInfo[29] = '1';                  // ORIG movb $0x31,-0x3e = buf+0x1d
        db->set_query(
            "inSert into churn_member_info(m_id,accrue_cera,play_info,last_reward_time,first_reward_time,server_id,charac_no,item_id,add_info,luck_point,last_update_time,second_reward_time) values(%s,0,'%s',0,0,0,0,0,0,0,0,0)",
            NumberToString(sig->m_accId, 0), playInfo);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__, 0x308c,
                                  "BREAK_AWAY : churn_member_info DB INSERT FAIL!!");
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x308f, 0);
        trace("BREAK_AWAY : churn_member_info MEMBER INSERT!! M_ID(%s)",
              NumberToString(sig->m_accId, 0));

        // STBreakAwayMemberData::clear（ORIG W 0x844d584；rec = +0x3f18 起 0x41 字节）
        reinterpret_cast<STBreakAwayMemberData*>(&sig->m_breakAwayAccureCera)->clear();
        memcpy(sig->m_breakAwayPlayInfo, playInfo, 0x1f);
        return 1;
    }

    if (!db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x309a, 5);
        trace("BREAK_AWAY : churn_member_info NO MEMBER USER!! M_ID(%s)",
              NumberToString(sig->m_accId, 0));
        return 0;
    }

    if (!db->get_int(0, sig->m_breakAwayAccureCera)) return 0;                   // accrue_cera
    if (!db->get_str(1, reinterpret_cast<char*>(sig->m_breakAwayPlayInfo), 0x1f)) return 0;  // play_info
    if (sig->m_breakAwayPlayInfo[0] == '\0')
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x30ab,
                              "BREAK_AWAY : ABNOMAL PLAY_INFO DATA!! M_ID(%s)",
                              NumberToString(sig->m_accId, 0));
        return 0;
    }
    if (!db->get_uint(2, sig->m_breakAwayLastRewardTime)) return 0;   // last_reward_time(+0x3f3c)
    if (!db->get_uint(3, sig->m_breakAwayRewardBoundA)) return 0;   // first_reward_time(+0x3f40)
    if (!db->get_int(4, sig->m_breakAwayLuckPoint)) return 0;             // luck_point(+0x3f54)
    if (!db->get_uint(5, sig->m_breakAwayLastUpdateTime)) return 0;   // last_update_time(+0x3f50)
    if (!db->get_int(6, sig->m_breakAwayDungeonClearCnt)) return 0;             // add_info(+0x3f48)
    if (!db->get_uint(7, sig->m_breakAwayRewardBoundB)) return 0;   // second_reward_time(+0x3f44)
    if (!db->get_uint(8, sig->m_breakAwayQuestTime)) return 0;   // quest_time(+0x3f4c)

    // last_reward_time 跨日（days>0）→ luck_point 清零并回写
    if (sig->m_breakAwayLastRewardTime != 0)
    {
        long prev = getPrevScheduleTime(0, (long)sig->m_breakAwayLastRewardTime);
        int curSec = GlobalData::s_systemTime_.getCurSec();
        unsigned int days = (unsigned int)((curSec - prev) / 0x15180);
        unsigned int zero = 0;
        unsigned int maxDays = std::max<unsigned int>(zero, days);   // ORIG call _ZSt3maxIjERKT_S2_S2_
        if (maxDays != 0 && sig->m_breakAwayLuckPoint != 0)
        {
            sig->m_breakAwayLuckPoint = 0;                     // luck_point 清零
            db->set_query("upDate churn_member_info set luck_point=%d where m_id=%s",
                          sig->m_breakAwayLuckPoint,
                          NumberToString(sig->m_accId, 0));
            bool updOk = db->exec(true);
            if (!(updOk && db->getAffectedRowCount() != 0))
            {
                LogManager::logFormat(1, "DBThread.cpp",
                                      __PRETTY_FUNCTION__,
                                      0x30d1, "BREAK_AWAY : LUCK POINT UPDATE ERROR!!");
                return 0;
            }
        }
    }

    // quest_time!=0 且已过期且命中奖励时点 → add_info=-1 / quest_time=0 回写
    if (sig->m_breakAwayQuestTime != 0)
    {
        unsigned int questTime = sig->m_breakAwayQuestTime;
        unsigned int nowSec = GlobalData::s_systemTime_.getCurSec();
        if (questTime < nowSec &&
            (sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundA ||
             sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundB))
        {
            sig->m_breakAwayDungeonClearCnt = -1;                    // add_info = -1
            sig->m_breakAwayQuestTime = 0;            // quest_time = 0
            db->set_query("upDate churn_member_info set add_info=%d, quest_time=0 where m_id=%u",
                          sig->m_breakAwayDungeonClearCnt,
                          sig->m_accId);
            bool updOk = db->exec(true);
            if (!(updOk && db->getAffectedRowCount() != 0))
            {
                LogManager::logFormat(1, "DBThread.cpp",
                                      __PRETTY_FUNCTION__,
                                      0x30e6, "BREAK_AWAY : ADD INFO UPDATE ERROR!!");
                return 0;
            }
        }
    }
    return 1;
}