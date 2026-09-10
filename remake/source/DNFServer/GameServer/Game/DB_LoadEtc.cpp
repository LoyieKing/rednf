// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_LoadEtc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_LoadEtc.cpp
// 目标：清零 /tmp/only_orig/DB_LoadEtc.txt 全部 40 个 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadEtc/<fn>.md（汇编为准）忠实转写。
// SIG_LOAD_ETC 子区偏移见 SigTypes.h 权威布局注记：
//   +0x340 PvpResultType(0x84) | +0xc9c0..+0x14a98 mixed 区（文档化偏移）
//   | +0x14a98 blueMarbleEnterCount | +0x14a99 SIG_ATTENDANCE_EVENT(0x11)
//   | +0x14aaa SIG_COMBO_SKILL 兼容区[0xb6]（尾至 +0x14b60 含 seria deco 残段）
// CHARAC_QUEST_SHOP_DATA 权威定义与区间 [0xd5a7,0xd651)=0xaa 双实例证据
// 见 DB_LoadEtc.h。
// ============================================================================

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "DB_LoadEtc.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"
#include "LogManager.h"        // logFormat
#include "CUserCharacInfo.h"    // stAuraAvatarOption::reset（ORIG W 0x844d616）
#include "CGuildServerProxy.h"  // CEventManager / CEventBase::IsEventing
#include "CDataManager.h"       // G_CDataManager / get_event_script_mng
#include "CEventScriptMng.h"    // EventClassify::CEventScriptMng/CEventEntity 完整类型
#include "CEnvironment.h"       // G_CEnvironment()->m_serverEnvir.m_fatigueTime(+0x37c)
#include "SyncScriptDeps.h"     // toTString(const std::string&)
#include "AdvanceAltarZip.h"    // uncompress_zip 权威声明
#include "TowerOfDespairMgr.h"  // TOD_Layer(unsigned short)
#include "WongWorkEvents_A.h"   // WongWork::EventCommon::LoadCharacEventItem
#include "DB_CreateCharac.h"    // dispatch 内 DB_CreateCharac 栈对象（W6_TailB 域）
#include "GameWorld.h"          // G_GameWorld()->IsPvPSkilTreeChannel

class CUser;   // CEventBase::IsEventing(CUser*) 形参前置
// ---- 本 TU 转接声明（UNLINKED_CALLEE，源码暂无 owner；照常调用，保持
// ORIG 全局 mangled 名以便链接器命中真实 T 符号）----
// ORIG T _Z20get_skill_table_nameb @0x083fbdc6 —— 调用方固定传 false。
char const* get_skill_table_name(bool use2nd);
// ORIG T _Z22getComboSkillTableName20ENUM_SKILL_TREE_KIND @0x0844a38b。
char const* getComboSkillTableName(ENUM_SKILL_TREE_KIND kind);
// ORIG T _Z24get_compressed_blob_dataP5MySQLiPcii @0x083fbc79
// （定义在 DB_Login.cpp:1967）。
#include "DB_LoadEtc/get_compressed_blob_data.inc"
#include "DB_LoadEtc/getDailyInitializeTime.inc"

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

// ORIG T GetCurrentResetBaseTime(long, int) @0x0810912a
// （定义 LinkClosureA_Misc.cpp）。
#include "DB_LoadEtc/GetCurrentResetBaseTime.inc"
#include "DB_LoadEtc/DB_LoadEtc.inc"
}

// ============================================================================
// ~DB_LoadEtc @0x08403ba0（D1/D2）/ deleting dtor @0x08403bd0（D0）
// ============================================================================
#include "DB_LoadEtc/dtor_DB_LoadEtc.inc"
}

// ============================================================================
// DB_LoadEtc::GetUserRecord @0x084075c4：纯 ret 1（单指令函数体）。
// ============================================================================
#include "DB_LoadEtc/GetUserRecord.inc"
}

// ============================================================================
// DB_LoadEtc::GetUserPvpBase @0x08407db4：纯 ret 1。
// ============================================================================
#include "DB_LoadEtc/GetUserPvpBase.inc"
}

// ============================================================================
// DB_LoadEtc::GetUserPvp @0x08407dc2。
// 26 列 pvp_result 读取入 sig+0x340 PvpResultType；随后按本地当日零点重算
// m_nextResetTime(+0x68)，若晚于 last_play_time(col22) 清 +0x60；
// 若 pvp_grade_ext_update_time(col25)+86400 < 当前秒清 +0x5c。返 0=OK
// 1=无行 -1=DB/取值错误。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x08407dc2；385 vs 390（帧/槽），p167 待核。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=739 (orig=385/ours=390)：N82 帧/槽错位（4.1 不复用
// 栈槽 vs 4.4 复用）+ N95 槽基差；p167 CLEAR 0/0、gap=5 ⇒ 语义完成、byte 层封顶（§6.2）。
// [b28d-SweepDBLoadEtc 本波] 具名 nowSec 槽（ORIG `call getCurSec; mov %eax,-0xc` +
// `cmp -0xc(%ebp),%eax; jge` 直跳）：旧内联 getCurSec() 致 4.4 setcc 物化；改后 385/386
//（gap 5→1）、p167 CLEAR。
// [b28d-SweepDBLoadEtc2 本波] **DIFF→AE（385/385）**，三处源形按报告逐条复现：
//   ①具名 `PvpResultType* pvp = &sig->m_pvpResult;`（ORIG 0x08407e45 `mov 0xc(%ebp),%eax;
//     add $0x340,%eax; mov %eax,-0x14`，P0 槽-局部对应律；旧直写 sig->m_pvpResult 使
//     sig 形参使用 28 次 vs ORIG 2 次）；
//   ②get_uint(23/24) 直接传字段地址 `(unsigned int&)pvp->m_todayPlayCount/m_todayLoseCount`
//     （ORIG 0x08408273/0x084082a1 `pvp+0x64/0x60`，无中间 u 局部，P0）；
//   ③localtime_r 改 P43 指针形 `struct tm* t = localtime_r(&now,&tmv);`，写序照 ORIG
//     +0x8→+0x4→+0x0（tm_hour/tm_min/tm_sec）；mktime 结果直存 +0x68 后重读比较（无
//     nextReset 局部）；gradeExtUpdateSec 裸声明（ORIG 0x08408338 无前置清零）。
#include "DB_LoadEtc/GetUserPvp.inc"
}
// ============================================================================
// DB_LoadEtc::LoadLitmitItem @0x08405c60：sub $0x30 后直接 ret 1（如实空体）。
// ============================================================================
#include "DB_LoadEtc/LoadLitmitItem.inc"
}

// ============================================================================
// DB_LoadEtc::dispatch @0x08403bee
// 调用序/分支/错误行号严格按报告汇编（0x14c6/0x14db/0x14e3/0x1566 等）。
// ============================================================================
// [b29-Lane21] **AE（残差 0）**：`bool sendOk` 提前声明（先声明→更深槽）+ 两句式赋值
//   ⇒ ORIG 槽集 `-0xc(41)/-0xd(2)/-0x20(2)/-0x24(4)` 逐值全等（判据⑩）。
// b28d-31: N 封顶（N82）——ORIG 0x08403bee；我们多 3 条（帧/装载序），
// p167 待核，语义按 dispatch 协议等价。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=353 (orig=323/ours=326)：N82 帧/槽 + 本 TU callee
// 定义可见导致的帧继承差；p167 CLEAR 0/0 ⇒ 语义完成、byte 层封顶（§6.2）。
// [b28d-SweepDBLoadEtc2 本波] **DIFF→NEAR（327 vs 323，助记符序列全等，残差仅槽号
//   sig -0x10 vs ORIG -0xc / 装载序）**，并修真 bug：
//   ①`GetUserPvp` 判定改 **switch**（ORIG 0x08403c69 无槽 `test/cmp $1/cmp $-1/jmp` 链，
//     P251/P261；内层 GetUserPvpBase 同形），case 叶块物理序按 ORIG（case1→case-1→case0，
//     P324）；
//   ②LoadLitmitItem 错误路径 cMyTrace 改 P227 全表达式临时链（ORIG 0x08403d80 ctor 前
//     `mov 0x4(%eax),%ebx`）；
//   ③SendResult 改具名 bool 局部（ORIG 0x08404073 `mov %al,-0xd; movzbl; xor $1`，P102）；
//   ④**真 bug**：删 `if (!LoadQuestShop(sig)) return false;` 多余门禁——ORIG 0x08403f5e
//     调用后直接 LoadTitleBook（返回值丢弃），旧形在 LoadQuestShop 失败时提前返回 false。
#include "DB_LoadEtc/dispatch.inc"
}

// ============================================================================
// DB_LoadEtc::LoadProperDungeonClearCount @0x084073aa
// ============================================================================
#include "DB_LoadEtc/LoadProperDungeonClearCount.inc"
}

// ============================================================================
// DB_LoadEtc::LoadConditionEventInfo @0x8407296？—— 报告头地址 0x08407296；
// 列 current_step(+0x40c8)/reward_step(+0x40ca)，缺行清零。
// ============================================================================
#include "DB_LoadEtc/LoadConditionEventInfo.inc"
}

// ============================================================================
// DB_LoadEtc::_loadNPCRelasionship @0x08407482 —— 返回 LineFunc 错误码（0 成功）。
// npc 数据区 SIG_LOAD_ETC +0x3e6c(cnt)/+0x3e70(binary, ≤600 字节)。
// 错误码 0x1c80/0x1c84/0x1c87/0x1c8a/0x1c8d/0x1c90（见汇编各失败出口）。
// ============================================================================
// b28d-31: N 封顶（N82/N95）——ORIG 0x08407482；槽号 -0x10/-0xc 整体平移
// （4.4 局部槽基策略），助记符序列等价。
// [b28d-solo-DB_LoadEtc] AE ndiff=0 (orig=86/ours=86)：具名 nRows 槽（P0 槽律）+ get_uint
// 直写 sig->m_npcCount（去 npcCnt 中间局部）+ 删 static_cast<int> 中转（P265 档②）。
#include "DB_LoadEtc/_loadNPCRelasionship.inc"
}

// ============================================================================
// DB_LoadEtc::LoadRedeemItem @0x846a6a？—— 报告头地址 0x08406a6a；
// redeem_info blob → uncompress_zip → SIG_LOAD_ETC+0x40ce（m_expandData6 区）。
// ============================================================================

// [b29-Lane21] 已修：`unsigned int srcLen` + 去 cast ⇒ dst 先算，ORIG 装载序复现。
// 残差（N 封顶）：N7 寄存器角色（ORIG `mov sig,%eax; add $0x40ce,%eax` 就地加；4.4
//   `mov sig,%eax; lea 0x40ce(%eax),%edx`）——成员名/`&arr[0]`/cast/匿名 struct 视图
//   四形实测均不翻，语义等价（161=161）。
// [b28d-solo-DB_LoadEtc] N封顶（N7）：ORIG 0x08406a6a；161=161、p167 CLEAR 14/14。
// 已修（本波）：①`char blob[0x6c7] = { 0 };` 替 memset call（P214③/P253；ORIG 内联
//   rep stos 慢路 + 尾字节，指令数 129→161 与 ORIG 全等）；②get_binary 第 3 参复用
//   outLen 具名槽（P244；原直填 0x6c7 使 4.4 折常量）。
// 残差（不可翻，N7）：uncompress_zip 实参装载的整组寄存器角色互换——ORIG
//   `mov 0xc(%ebp),%eax; add $0x40ce,%eax` + `mov -0xc(%ebp),%edx`（dst→eax）；
//   4.4 恒 `mov -0xc(%ebp),%eax` + `mov 0xc(%ebp),%edx; add $0x40ce,%edx`（dst→edx）。
// 助记符序列与槽位全等，仅 eax/edx 角色互换（P230 补 N7 族；446 后端已应用）。
// 已修（前波）：exec/fetch 具名 ok + 三处 trace 改 P227 临时链。
#include "DB_LoadEtc/LoadRedeemItem.inc"
}

// ============================================================================
// DB_LoadEtc::LoadPvPMissionList @0x08406cce —— fair_pvp_score.pvp_mission_info
// blob → SIG_LOAD_ETC+0xcdd5（type8 expand 区，cap 0x198）。n_rows==0 也返 0。
// ============================================================================
// [b29-Lane21] **已归零（残差 0）**：`unsigned int srcLen` + 去 cast ⇒ ORIG
//   `mov 0xc(%ebp),%eax; add $0xcdd5,%eax; mov -0xc(%ebp),%edx`（dst 先算）复现。
// [b28d-solo-DB_LoadEtc] N封顶（P75 实参装载序）：ORIG 0x08406cce。
// 已修（本波）：①blob 改 `char blob[0x198] = { 0 };`（内联 rep stos，替 memset call，
//   P214③；帧 push edi/ebx + sub 0x1f0 对齐）；②`get_binary(0, blob, outLen)` 第 3 参
//   复用 outLen 具名槽（原直填 0x198 使 4.4 折常量，P244）。
// 残差（不可翻，P75）：uncompress_zip 实参装载序——ORIG 先 dst（`mov sig,%eax;
//   add $0xcdd5,%eax`）后 srcLen；4.4 恒先 srcLen。余者全等（含槽位与 rep stos）。
//   [b28d-solo 复测] 133=133、唯一 hunk 即该 3 行（srcLen 槽读与 dst 计算的先后）；
//   探针 p4.cpp e4（srcLen 改 unsigned）仅把 dst 换成 lea+edx 形态，仍差 ⇒ N7 封顶。
// 已修（前波）：exec/fetch 具名 ok + trace 临时链。
#include "DB_LoadEtc/LoadPvPMissionList.inc"
}

// ============================================================================
// DB_LoadEtc::LoadAchievement @0x08406422
// achievement zip → SIG_LOAD_ETC+0xb1a4（cap 0x1800），统计非零 0xc 条目计数入
// +0xc9a4，unix_timestamp(last_update_time) 入 +0xc9a8。
// ============================================================================
// [b29-Lane21] 已修：`unsigned int srcLen` ⇒ 实参装载序 srcLen 位置差消。
// 残差（N 封顶）：①N1 常量拆分（ORIG `mov sig,%ecx; …; add $0xb1a0,%eax; mov 0x4(%eax),%eax`
//   vs 4.4 `add $0xb1a4,%eax; mov (%eax),%eax`——元素内 +4 被折入基址常量）；
//   ②N7 dst 取址 add/lea + `mov 0xc(%ebp),%ecx` 装载序。
// [b28d-solo-DB_LoadEtc] N封顶（N1 常量合并 + P75 实参装载序）：ORIG 0x08406422。
// 已修（本波）：count 循环改 12 字节元素视图 `AchievementEntry m_entries[0x200]`
//   （基址 0xb1a0 + 元素内 +4，替代 `m_expandDataF + i*12` 的 cast 形）。
// 残差（不可翻，两处，余者全等 + nop 豁免）：
//   ①uncompress_zip 实参装载序——ORIG 先第 1 参（`mov sig,%eax; add $0xb1a4,%eax`）后
//     第 4 参 srcLen；4.4 恒先第 4 参（P75 跨版本实参装载时刻；删冗余 cast 无效）。
//   ②循环基址——ORIG `lea (sig,i*12); add $0xb1a0,%eax; mov 0x4(%eax)`（基址 sig 原始、
//     disp4 保留）；4.4 恒把基址偏移与元素内偏移合并为 `add $0xb1a4` + `mov (%eax)`
//     （N1 常量折叠；offset-0/成员数组/short 元素三形实测均合并）。
//     [b28d-solo 复测] 补测第四形：二维具名数组视图 `int m_entries[0x200][3]` +
//     `m_entries[i][1]` ⇒ 4.4 仍折 `add $0xb1a4` + `mov (%eax)`（P311 于本偏移不适用），
//     127=127 条；四形均合并 ⇒ 封顶。
// 已修（前波）：exec 具名 ok、outLen 顺序、get_uint 直写 m_achievementLastUpdate（去局部）。
#include "DB_LoadEtc/LoadAchievement.inc"
}

// ============================================================================
// DB_LoadEtc::LoadOnceGiveAvengerTitle @0x08406600
// ============================================================================
#include "DB_LoadEtc/LoadOnceGiveAvengerTitle.inc"
}

// ============================================================================
// DB_LoadEtc::LoadAuraAvatarOption @0x0840838e
// aura_avatar_option → stAuraAvatarOption 视图 @SIG_LOAD_ETC+0xd730。
// ============================================================================
// [b28d-solo-DB_LoadEtc] **AE（残差 0；92=92、p167 CLEAR 8/8）**：ORIG 0x0840838e。
// 已修（本波，本件归零）：①两步寻址 `add $0x35cc,%ecx` + `mov %edx,(%eax,%ecx,4)`——原
//   数组视图位于 offset 0 使 4.4 折 `lea 0x35cc(%edx),%ecx`；改成员数组位于真实偏移
//   0x35cc*4 的视图（P30）。
//   ②store 装载序——ORIG 0x084084af 为 index→value→base；跨类型 cast 视图恒 base-first
//   （探针 /tmp/dble_probe/p8.cpp k4），改 **SIG_LOAD_ETC 完整类型成员直访问**
//   `sig->m_auraOptionWords[optionType]`（SigTypes_C.h +0xd730 匿名 union 视角，Main 代落，
//   P235）复现 index→value→base（同探针 k1）；元素 unsigned int 与 RHS 异型消 4.4 的
//   int→unsigned 装载中转（探针 p9.cpp m2）。旧「N76 不可翻」判词作废。
// 残差：无（92=92、p167 CLEAR 8/8）。
#include "DB_LoadEtc/LoadAuraAvatarOption.inc"
}

// ============================================================================
// DB_LoadEtc::LoadCharacOption @0x084084e0
// options blob(+0xeac8,0x200)/best_clear_time blob(+0xec d8,24000)/负值钳位
// (+0xECDC..)/blueMarbleEnterCount(+0x14a98)/charac_inform_notice(+0x14a9a,16B)
// 及 best_clear_time 条目计数 +0xecd4。
// ============================================================================
// [b28d-solo-DB_LoadEtc] N封顶（N82/N107/N30）：ORIG 0x084084e0；168 vs 171、p167 CLEAR 11/11。
// 已修（本波）：列号改**自增具名局部** `int col = 0; … get_binary(col++, …)` ⇒ 复现 ORIG
//   的 `mov -0x18(%ebp),%eax; addl $0x1,-0x18(%ebp)`（原字面量列号使 4.4 直填常量
//   `movl $0x1,0x4(%esp)`，-10 条）。
// 残差（不可翻）：①N107/N30 地址形（`lea 0xeac8(%eax),%edx` vs `add $0xeac8,%eax`；
//   `mov 0xc(%eax,%edx,4)` 两步 vs 4.4 add+shl 链）；②N82 槽基差 4（db 槽 -0x20 vs
//   ORIG -0x1c；col 槽引入后整体深 4）；③trace 槽深差。
// [b28d-SweepDBLoadEtc 本波] 真 bug 修复：条目计数循环原 `for+if` 遍历全表统计非零数，
// ORIG 0x08408734 `test %eax,%eax; je 0x0840874e` 首条 0 即 break（Ghidra C
// `while (i<1000 && *(int*)(sig+i*0x18+0xecd8)!=0){++count;++i;}`）⇒ 改 while+break，
// 计数=从头连续非零条数。读数 168/171→168/172（残差 N1 折叠）。
// [b28d-SweepDBLoadEtc2 本波] 真 bug（读错偏移）：计数循环原读
//   `m_bestClearTime + idx*0x18 + 8`（=sig+0xece0+24*idx，元素内第 3 个 int），ORIG
//   0x08408718..0x0840873c 为 `lea (%ecx,%eax,1); add $0xecd0; mov 0x8(%eax)` = 
//   sig+0xecd8+24*idx（**元素首 int**）⇒ 删 +8。另 trace 改 P227 临时链
//   （ORIG 0x0840859f ctor 前 `mov 0x4(%eax),%ebx` 缓存 characNo）。
//   残差：N107/N30 地址形 + N82 槽基差 4（不可翻）。
// [b29-Lane21 复核] 四旋钮定位（ndiff 82→16，orig 168/our 170）：
//   ①删 `int* p` 指针局部改表达式直用（P0：ORIG 无指针槽往返）⇒ 整体槽基回退 4，
//     槽集与 ORIG 全等（-0xc/-0x10/-0x14/-0x18/-0x1c/-0x2c）；
//   ②全头部声明 count/i/idx（P212 按 ORIG 槽深序 db→col→count→i→idx）⇒ i/idx 槽位归位；
//   ③负值钳位改 sig 基址视图 `m_clamp[i+0x3b34]`（P291）⇒ 逐字复现 `mov 0xc(%eax,%edx,4)`；
//   ④条目计数改 `m_rows[idx].m_first`（行基址 +0xecd0/步长 0x18/行内 +8）。
// 残差（不可翻）：①N107 取址形 ×2（`lea 0xeac8(%eax)`/`lea 0x14a9a(%eax)` vs 4.4
//   `add+mov`）；②N4/P30 装载序（ORIG 先装索引 %edx 后装 %eax 基址，4.4 相反）。
// LoadCharacOption 负值钳位窗口视图：m_clamp 数组起 +0xC（元素 4 字节、索引
// i+0x3b34 落 sig+0xECDC）；具名数组成员让 4.4 产 ORIG 的
// `mov 0xc(%eax,%edx,4)`（edx=i+0x3b34）scaled 索引形（P291/P30）。
struct SigLoadEtcNegClampView
{
    char m_pad[0xc];               // +0x000..+0x00B（= m_fieldC 前的字段区）
    int  m_clamp[0x3b34 + 5];      // +0x00C..（索引 i+0x3b34 即 sig+0xECDC+i*4）
};

// LoadCharacOption options blob 视图（+0xEAC8）：具名成员数组经视图访问 ⇒
// 4.4 复现 ORIG `mov 0x14(%ebp),%eax; lea 0xeac8(%eax),%edx`（数组名退化直传恒折
// `add $0xeac8,%eax; mov %eax,%edx`）。
struct SigLoadEtcOptionsView
{
    char m_pad[0xeac8];
    char m_options[0x200];         // +0xEAC8
};


// LoadCharacOption charac_inform_notice 视图（+0x14A9A = attendanceEvent+1）：
// 视图基址=sig ⇒ ORIG `mov 0x14(%ebp),%eax; lea 0x14a9a(%eax),%edx`（cast/加法取址
// 恒折 `add $0x14a99,%eax; lea 0x1(%eax),%edx`）。
struct SigLoadEtcNoticeView
{
    char m_pad[0x14a9a];
    char m_notice[0x10];           // +0x14A9A（charac_inform_notice）
};

// LoadCharacOption best_clear_time 条目视图：行基址 sig+0xECD0、步长 0x18、
// 首 int 在行内 +8（ORIG `lea (%ecx,%eax,1); add $0xecd0; mov 0x8(%eax)`）。
struct SigLoadEtcBestClearTimeView
{
    struct Row
    {
        char m_pad8[8];
        int  m_first;                  // 行内 +0x08
        char m_rest[0xc];
    };
    char m_pad[0xecd0];                // +0x0000..+0xECCF
    Row  m_rows[1000];                 // +0xECD0，步长 0x18
};

#include "DB_LoadEtc/LoadCharacOption.inc"
}

// ============================================================================
// DB_LoadEtc::SendResult @0x08408766
// BigStreamPool::Acquire → guard<<userIdx<<slot→put_binary(sig,0x14b60) →
// MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)。
// ============================================================================
#include "DB_LoadEtc/SendResult.inc"
}

// ============================================================================
// DB_LoadEtc::_loadNPCRelasionship 见上；LoadQuestShop/_LoadQuestShop 下。
// ============================================================================

// ============================================================================
// DB_LoadEtc::_LoadQuestShop @0x08404b72（static）
// get fail 行号序列：qp/init_count 前 2 次 → cMyTrace ret0（0x17ea/0x17f1）；
// 自 max_hp 起 throw int(line)：0x17f7..0x1811、questPiece 0x1816（跳过 0x1810）。
// catch 后 memset(param_2+0xc,0,0x9a) 并 logFormat 行 0x181f。
// unroll 转录：列号用 local_14(idx) 取后自增（非字面量常数），throw 就地展开
// （__cxa_allocate_exception/__cxa_throw 逐列内联，非辅助函数），exec/fetch/
// qp/init_count 结果经 local_19(ret) 中转 → 指令数 460→690，与 ORIG 690 一致。
// [b28c-lane17] fresh 复验：690=690、助记符序列全等 ⇒ NEAR（残差全为槽号/
// callee-saved 分配差，N82 族——4.1 不按 lifetime 复用栈槽使全帧槽号错位，
// ndiff 与语义完成度反向）。按 §6.2 三件套口径判「语义完成、byte 层 N 封顶」。
// ============================================================================
#include "DB_LoadEtc/_LoadQuestShop.inc"
#       define QUESTSHOP_USHORT(field, line_)                                  \
        do                                                                     \
        {                                                                      \
            if (!db->get_ushort(idx++, out->field))                            \
            {                                                                  \
                throw line_;                                                   \
            }                                                                  \
        } while (0)

        QUESTSHOP_USHORT(m_data.m_maxHp, 0x17f7);                   // max_hp           +0x26
        QUESTSHOP_USHORT(m_data.m_maxMp, 0x17f8);                   // max_mp           +0x6c
        QUESTSHOP_USHORT(m_data.m_psyAttack, 0x17f9);               // psy_attack       +0x0c
        QUESTSHOP_USHORT(m_data.m_psyDefense, 0x17fa);              // psy_defense      +0x12
        QUESTSHOP_USHORT(m_data.m_magAttack, 0x17fb);               // mag_attack       +0x0e
        QUESTSHOP_USHORT(m_data.m_magDefence, 0x17fc);              // mag_defence      +0x10
        QUESTSHOP_USHORT(m_data.m_moveSpeed, 0x17fd);               // move_speed       +0x22
        QUESTSHOP_USHORT(m_data.m_attackSpeed, 0x17fe);             // attack_speed     +0x20
        QUESTSHOP_USHORT(m_data.m_hpRegen, 0x17ff);                 // hp_regen         +0x28
        QUESTSHOP_USHORT(m_data.m_mpRegen, 0x1800);                 // mp_regen         +0x46
        QUESTSHOP_USHORT(m_data.m_allElementResist, 0x1801);        // +0x3c
        QUESTSHOP_USHORT(m_data.m_fireElementResist, 0x1802);       // +0x34
        QUESTSHOP_USHORT(m_data.m_waterElementResist, 0x1803);      // +0x36
        QUESTSHOP_USHORT(m_data.m_lightElementResist, 0x1804);      // +0x3a
        QUESTSHOP_USHORT(m_data.m_darkElementResist, 0x1805);       // +0x38
        QUESTSHOP_USHORT(m_data.m_allElementAttack, 0x1806);        // +0x68
        QUESTSHOP_USHORT(m_data.m_fireElementAttack, 0x1807);       // +0x60
        QUESTSHOP_USHORT(m_data.m_waterElementAttack, 0x1808);      // +0x62
        QUESTSHOP_USHORT(m_data.m_lightElementAttack, 0x1809);      // +0x66
        QUESTSHOP_USHORT(m_data.m_darkElementAttack, 0x180a);       // +0x64
        QUESTSHOP_USHORT(m_data.m_psyCritical, 0x180b);             // +0x2a
        QUESTSHOP_USHORT(m_data.m_magCritical, 0x180c);             // +0x2c
        QUESTSHOP_USHORT(m_data.m_goodHit, 0x180d);                 // +0x4e
        QUESTSHOP_USHORT(m_data.m_evasion, 0x180e);                 // +0x42
        QUESTSHOP_USHORT(m_data.m_hitRecovery, 0x180f);             // +0x48
        QUESTSHOP_USHORT(m_data.m_separatePsyMagAttack, 0x1811);    // +0x76（跳过 0x1810）

#       undef QUESTSHOP_USHORT

        if (!db->get_uint(idx++, out->m_questPiece))            // quest_piece +0xa6
        {
            throw 0x1816;
        }
    }
    catch (int e)
    {
        memset(&out->m_data, 0, sizeof(out->m_data));
        LogManager::logFormat(1, "DBThread.cpp",
                              "static bool DB_LoadEtc::_LoadQuestShop(unsigned int,"
                              " CHARAC_QUEST_SHOP_DATA*)",
                              0x181f,
                              "DB_LoadEtc::LoadQuestShop, get(%d) ERROR charac_no=%u",
                              idx /*ORIG -0x10(%ebp)=local_14 列号*/, characNo);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadQuestShop @0x08404a68 —— 主/tag 双实例装载。
// 主区 = SIG_LOAD_ETC+0xd5a7，tag 区 = +0xd651（tag_charac_no=+0xc 非零时装载），
// 双区齐备后 +0xd6fb 置 1。错误走 cMyTrace 0x16c4/0x16d5。
// ============================================================================
// [b28d-solo-DB_LoadEtc] 旧判「N封顶（N107 成员常量偏移取址）」已作废（b29-Lane44 推翻）：
//   ORIG 两处 `lea 0xd5a7/0xd651(%eax),%edx` 对应**视图成员取址**形；旧探针用「pad+char 数组
//   退化」形（char→CHARAC_QUEST_SHOP_DATA* 仍需 cast）故见 add+mov。改用局部 trivial 镜像
//   视图 `QuestShopView` 的具名对象成员取址（类型匹配、无 cast）后 4.4 产单条 lea ⇒ 69/69 全等。
// 已修（前波）：exec 具名 ok + trace 临时链（P227）。
#include "DB_LoadEtc/LoadQuestShop.inc"
}

// ============================================================================
// DB_LoadEtc::LoadTitleBook @0x08406092 —— 4 个压缩列分别解压到
// +0x466a(cap0x10ae) / +0x5718(0x2fa8) / +0x86c0(0x1c98) / +0xa358(0xe4c)。
// ============================================================================
// [b29-Lane21] **已归零（残差 0）**：四列 srcLen 改 `unsigned int` + 去 cast ⇒ ORIG
//   4 处 `mov -0x18/-0x14/-0x10/-0xc(%ebp),%edx` 装载序全复现（221=221）。
// [b28d-solo-DB_LoadEtc] N封顶（N7/N82）：ORIG 0x08406092；p167 CLEAR 21/21。
// 已修（本波）：①四段 `char blob[N] = { 0 };`（各段独立作用域声明）替 memset call ⇒
//   复现 ORIG 四段内联 rep stos（P214③/P312），p167 EXTRA memset ×2 归零；
//   ②（前波）exec 具名 ok + trace 临时链。
// 残差（不可翻）：N7 族 uncompress_zip 实参寄存器角色互换 + N82 槽序（ORIG db=-0x20/
//   ok=-0x19/srcLen=-0x18..-0xc/outLen=-0x34..-0x40 各 4 个独立槽；4.4 按 lifetime 复用
//   ⇒ db=-0x1c。全头部声明（P212）实测把 db 顶到 -0xc 反劣，已回退）。
// [b29-Lane21 复核] 三旋钮定位（ndiff 114→8，orig/our 221=221）：
//   ①`bool ok` 后置到 db 声明之后 ⇒ 槽形翻正（ORIG db=-0x20/ok=-0x19；ok 前置时 4.4 给
//     db=-0x1c/ok=-0x1d，本件方向与 P212 相反）；
//   ②四段 srcLen/outLen 声明交换（ORIG `movl $0x10ae,-0x34` 在 get_binary_length call 之后）；
//   ③fetch 检查改直测 `if (!db->fetch())`（ORIG 无 ok 槽保存，仅 exec 走 ok=-0x19）。
// 残差（P75 实参装载序，4 处）：ORIG 先取第 4 参 srcLen 再算第 1 参 dst；4.4 恒 dst 先。
#include "DB_LoadEtc/LoadTitleBook.inc"
}

// ============================================================================
// DB_LoadEtc::LoadComboSkillInfo @0x0844a3b2 —— combo 区 job==9 才拉双树。
// ============================================================================
// b28d-31: N 封顶（P223 边界）——ORIG 0x0844a3b2；4.4 尾部多 [jmp;nop]
// epilogue-threading（`!call()` 等三形实测不消），语义等价。
// [b28d-solo-DB_LoadEtc] AE ndiff=0 (orig=43/ours=43)：P257 正向包裹 `if (job==9){...}`
// 消掉 4.4 的 epilogue-threading [jmp;nop] 尾岛（旧早返三形均留 1 岛）。
#include "DB_LoadEtc/LoadComboSkillInfo.inc"
}


// ============================================================================
// DB_LoadEtc::LoadComboSkill @0x0844a47a —— 单树连招装载（kind∈{0,1}）。
// ============================================================================
// [b29-Lane21] 残差 6 行 = N76 store 装载序：ORIG `mov -0xc(%ebp),%eax; mov %eax,%edx;
//   mov 0xc(%ebp),%eax; mov %dl,disp`（4 条，RHS-first + 中转）vs 4.4 `mov -0xc(%ebp),%edx;
//   mov 0xc(%ebp),%eax; mov %dl,disp`（3 条）；视图/成员/两值形已试 ⇒ N76 封顶。
// [b28d-solo-DB_LoadEtc] N封顶（N76 store 装载序）：ORIG 0x0844a47a。
// 已修（本波）：①**真 bug（P92/P196）**：getComboSkill 第 2 实参原写 sig->m_accId（+0），
//   ORIG 0x0844a5a4 为 `mov 0x4(%eax),%eax` = sig->m_characNo（+4）——该值即
//   getComboSkill 日志的 charac_no=%u，原码打印错误账号。
//   ②去 cs 具名局部（P183）+ 改成员视图 ComboSkillView（pad 0x14aae + tree1Count/tree2Count/
//   tree1Data[6]/tree2Data[6]）⇒ ORIG disp 形 `mov %dl,0x14aae(%eax)` / `lea 0x14ab4(%eax),%edx`
//   （cast 视图恒 `add $0x14aaa` + disp4，N107）。
//   ③四段改 **if / else-if** 链（count 写与两处调用各自成对）⇒ 复现 ORIG 的 `jmp` 块尾 +
//   `cmp $1` elif 布局（全独立 if 时 4.4 直落无 jmp）。
// 残差（不可翻，N76）：count 写点 store 装载序——ORIG `mov -0xc,%eax; mov %eax,%edx;
//   mov sig,%eax; mov %dl,disp`（RHS-first + eax→edx 中转）vs 4.4 恒 LHS-first 直存
//   `mov sig,%eax; mov -0xc,%edx; mov %dl,disp`；字段 unsigned char 与 int RHS 本已异型，
//   P211 两旋钮（字段/callee）无第三档可用。
//   [b28d-solo 复测] 去 `(unsigned char)` cast（`m_tree1Count = nRows;`）产物**完全相同**
//   （4.4 视 uchar←int 为可直存，P211 档③不适用）⇒ 维持去 cast 形；p167 CLEAR 15/15。
// 已修（前波）：去 kindVal 局部（P0 直比形参）+ exec 具名 ok + 四处 trace 临时链。
#include "DB_LoadEtc/LoadComboSkill.inc"
}

// ============================================================================
// DB_LoadEtc::getComboSkill @0x0844a704 —— 每行 7 个 short 填一个 comoboSkillData，
// 行数上限 min(count,6)；成功返回 1、任何 get 失败行号错误路径返回 0。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x0844a704（orig=124/ours=124，槽全等）。
// 已修（本波）：①循环改标准 `for (row = 0; row < count && row < 6; ++row)` + 尾 `return true;`
//   ⇒ 4.4 复现 ORIG 的「入口 jmp 底部条件块 + 短路物化 mov$1/$0」（P283/P214①）；
//   原 `for(;;)+if(!(…))return true` 使条件置顶（DIFF）。
//   ②去 slot 具名指针局部，且**关键**：地址式由 `(comoboSkillData*)((char*)out + row*0xe)`
//   改**数组下标 `out[row]`**（sizeof(comoboSkillData)==0xe，pack(1)）——指针 cast 形被 4.4
//   缓存到 -0x14 临时槽（row 被挤到 -0x18、三处 trace 槽整体浅 4）；下标形每次重算
//   地址（P183 地址重物化），row=-0x14/col=-0x10/k=-0xc/trace -0x44/-0x34/-0x24 全等。
//   ③k 循环 `k <= 5`（ORIG 尾部 setle 形，P51）。
// 已修（前波）：col 改 0 起取后自增（ORIG -0x10）+ 三处 trace 临时链。
#include "DB_LoadEtc/getComboSkill.inc"
}

// ============================================================================
// DB_LoadEtc::GetUserSkill @0x084075d2 —— 主 skill 行 + tag 双槽位 blob。
// §6.2 语义收口：12 列 get/blob 链、col 槽（-0x10）col++ 形、fail 岛行号
// （0x1d50..0x1df1）与 ORIG 对齐；AE 天花板=N82 trace 槽方向三证（Main
// batch-17 裁定按语义收口，不投纯文本序差）。
// 列序 12 列，目标偏移照汇编；n_rows==0 且 PvP 技能树频道时补插一行。
// 错误行号 0x1d50/0x1d5d/0x1d69/0x1d71/0x1d98/0x1dac/0x1db2/0x1dea/0x1df1。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x084075d2；p167 CLEAR（0/0），
// 残差为帧/槽分配（ORIG 478 vs 4.4 468）。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=624 (orig=478/ours=468)：N82 帧/槽错位（12 列 get/blob
// 链的 trace 槽方向）+ N89 镜像（get_compressed_blob_data 内联差）；p167 CLEAR 0/0 ⇒ 语义完成。
// [b28d-SweepDBLoadEtc 本波] 真 bug 修复：①col 初值 1→0（ORIG 0x084077e9 `movl $0x0,-0x10`
// + 首列 `get_uint(col++)`；旧 col=1 使后续 blob 列号整体 +1）；②去 sk 指针/spVals 局部（P183，
// ORIG 每处单条 lea/add disp 直写成员，改 m_mixedRegion[off] 常量索引）；③scriptVersion 直传
// 成员地址（ORIG `lea 0xcdd1(%eax),%edx`）；④tag 段 col 变量（ORIG -0xc）；⑤9 处 trace 改
// P227 全表达式临时链；⑥exec/fetch/get_uint 共 9 处改具名 ok 槽（ORIG -0x15）。
// 读数：gap +10（478/468）→ +1（478/477）；p167 46=46（仅 cMyTrace C1/C2 别名）；
// 残差=add+mov↔lea 跨版本（N107/P30）。
// [b29-Lane44 判据①复扫·已归零] 上记 9 处 add+mov↔lea 实为**可翻**（旧判 N107 系探针形不全）：
//   4.4 对「cast 取址 `(T*)&arr[i]`」恒产 `add $K,%eax; mov %eax,%edx`，对「视图成员/
//   指针算术/数组名退化」恒产单条 `lea K(%eax),%edx`；4.1 恰好相反 ⇒ 两侧互补，改我方表达式
//   形即可逐点对齐（探针 /tmp/bl_probe.cpp~bl_probe5.cpp，447/446 后端同形）：
//   ①ORIG lea 的 4 点（+0xc9c8/+0xcb64/+0xcd00/+0xcd04）→ 局部 trivial 镜像视图
//     `SkillMixedView` 具名成员取址（类型匹配、无 cast）；
//   ②ORIG `add K; mov %eax,%edx` 的 5 点（+0xc9cc/+0xcb68/+0xcd08/+0x29fd/+0x2b97，均为
//     数组名退化/指针算术）→ 整型加法形 `(char*)((unsigned int)sig + K)`（P319 同族，
//     32 位指针≡int，语义等价）。
//   读数：478/477（ndiff 18，9 hunk）→ **478/478 全等**（verdict AE 54，TU diff 12→11）。
#include "DB_LoadEtc/GetUserSkill.inc"
}

// ============================================================================
// DB_LoadEtc::LoadCharacItemStat @0x08445660 —— cooltime/effect blob 双列 +
// check_flag 压缩列（cap 5000）入 +0xd740。错行号 0xac8a/0xac91/0xac98。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x08445660（orig=151/ours=151，槽全等）。
// 已修（本波）：①`bool ok;` 声明前置于 db（ORIG ok=-0x11 深于 db=-0x10，P212）；
//   ②三处 get 结果复用同一 ok 槽（ORIG 三处 `mov %al,-0x11; movzbl; xor $1; test`；
//   原 get_binary/get_compressed 直测少 2 条槽往返）。
// 已修（N107 翻正）：cooltime 实参 `sig->m_cooltimeItem`（数组名退化）4.4 恒
//   `add $0x230c,%eax; mov %eax,%edx`；改 **`&sig->m_cooltimeItem[0]`**（显式取首元素地址）
//   即复现 ORIG `lea 0x230c(%eax),%edx`——N107 的可翻位（与 effect 的 `&m_effectItem[0]` 同形）。
// 已修（前波）：exec/fetch 具名 ok + col 槽取后自增 + 三处 trace 临时链。
#include "DB_LoadEtc/LoadCharacItemStat.inc"
}

// ============================================================================
// DB_LoadEtc::LoadItemMakingSkill @0x084040b4 —— 11 列 short 循环装载，
// 目标基址 sig+0x10+(col+0x6b78)*2（=0xD700+col*2）。错行号 0x15dd/0x15e8/0x15f2。
// ============================================================================
// [b28d-solo-DB_LoadEtc] N封顶（N1 常量折叠 + 求值序）：ORIG 0x084040b4。
// 已修（本波）：①`bool ok;` 声明前置于 db ⇒ 槽序对齐（ok=-0x11 深于 db=-0x10，帧 0x54；
//   P212「先声明深」）；②地址式改 offset-0 视图 `m_rows[0x6b80 + col]`（元素基址即 sig）。
// 残差（不可翻，N1）：ORIG `mov col,%eax; add $0x6b78,%eax; add %eax,%eax; add sig,%eax;
//   add $0x10,%eax`（index 先、base 用内存操作数、+0x10 独立）vs 4.4
//   `mov sig,%eax; mov col,%edx; add $0x6b80,%edx; add %edx,%edx; add %edx,%eax`
//   ——4.4 恒把基址 +0x10 折进索引常量（N1）。
//   [b28d-solo 复测] 补测第五形：视图 `{char m_pad[0x10]; short m_rows[0x6b78+11];}` +
//   `m_rows[0x6b78 + col]` ⇒ 4.4 仍折成 `add $0x6b78,%edx; add $0x8,%edx`（+0x10/2 折入
//   索引），118 vs 119 条；视图形/short/char/指针左结合四形 + 本形共五形均折 ⇒ 封顶。
// 已修（前波）：exec/fetch/get_short 复用具名 ok + trace 临时链 + **真 bug 修正**（exec 日志参数
//   原写 m_accId，ORIG 0x084040e4 为 +0x4 = m_characNo，P92 来源照抄）。
#include "DB_LoadEtc/LoadItemMakingSkill.inc"
}

// ============================================================================
// DB_LoadEtc::LoadTODUserState @0x08406eec —— charac_tower_despair 六列；
// 前置 exec/fetch/AffectedRowCount==1 任一失败均如实 ret 1；列失败走 cMyTrace 0。
// col2 last_clear_layer 经 TOD_Layer(unsigned short) 构造后取其 m_layer 落位。
// ============================================================================
// [b28d-solo-DB_LoadEtc] **AE（残差 0；232=232、p167 CLEAR 24/24）**：ORIG 0x08406eec。
// 已修（本波，本件归零）：`sig->m_todLastClearLayer = layer.m_layer` 改**具名视图 cast**
//   `reinterpret_cast<TODLayerSlotView*>(sig)->m_layer = layer.m_layer`（探针
//   /tmp/dble_probe/p1.cpp f7）⇒ 复现 ORIG 0x08407114 的 base-first 装载序
//   （`mov 0xc(%ebp),%eax` 先于 `movzwl -0x78(%ebp),%edx`）；直写具名字段 4.4 恒 RHS-first
//   （旧「N107 + P211 不可翻」判词作废）。位模式/movw 宽度不变，语义等价。
// 已修（前波）：①六处 trace 改 **P227 全表达式临时链** `cMyTrace(...)("fmt", sig->m_characNo)`
//   ⇒ trace 槽方向翻正（ORIG -0x74/-0x64/-0x54/-0x44/-0x34/-0x24 全等；具名 tt 形恒
//   反向 -0x28..-0x78 且 m_characNo 在 ctor 后才载）+ m_characNo 装 %ebx 跨 ctor 保活；
//   ②`unsigned short layerVal;` 去预清（ORIG 无 `movw $0,-0x76`，get_ushort 直写）；
//   ③末列改 `get_int(col++, …)`（ORIG 0x0840706x 有 `addl $1,-0x10`）。
// 残差：无（`lea 0xd72c(%eax),%edx` 取址形两侧本就一致；p167 CLEAR 24/24）。
// 已修（前波）：三守卫改正极性嵌套（exec/fetch/AffectedRowCount==1 共享尾 return true）。
#include "DB_LoadEtc/LoadTODUserState.inc"
}

// ============================================================================
// DB_LoadEtc::LoadVillageAttackInfo @0x0843f038 —— 按重置基准日统计当日
// attack_count/revenge_dungeon（+0xc9ac/+0xc9ad）；无记录两字节清零且 ret 0。
// ============================================================================
#include "DB_LoadEtc/LoadVillageAttackInfo.inc"
}

// ============================================================================
// DB_LoadEtc::LoadAccountFirstLogin @0x08405c6e —— 首登事件位图装载。
// bitset 位操作映射 std::bitset<32>::set/reset(pos, true) ORIG 外联符号
// （084506e4 set / 0845071e reset）。
// ============================================================================
// [b28d-solo-DB_LoadEtc] NEAR（N7 封顶）：ORIG 0x08405c6e。
// 残差（不可翻，N7/4.4.7）：间接虚调寄存器角色互换——ORIG
//   `mov (%eax),%edx; mov -0x14(%ebp),%eax; mov %eax,(%esp); call *%edx`
//   vs 4.4.7 `mov (%eax),%eax; mov -0x14(%ebp),%edx; …; call *%eax`
//   （P230 补：4.4.6 后端同 ORIG 用 %edx，本 TU 未路由 446 表 ⇒ 4.4.7 恒 %eax；改路由属 Main）。
// 已修：首段链式去 evMng 槽、三处 get_convert 重发（P48）、nRows 正极性 !=0 直落（P220）、
//   set_query 内联实参与 NumberToString（P259）。助记符序列全等。
#include "DB_LoadEtc/LoadAccountFirstLogin.inc"
}

// ============================================================================
// DB_LoadEtc::LoadAccountFirstLoginDaily @0x08405e0a —— 日频首登事件。
// ============================================================================
// [b28d-solo-DB_LoadEtc] NEAR（N7 封顶）：ORIG 0x08405e0a。
// 已修（本波）：①去 `bool ok = db->exec(true)` 具名槽改直测 `if (!db->exec(true))`
//   ⇒ ok 原占 -0xd，把 baseSec 挤到 -0x14 并令全槽组（evMng/entity/today/db/nowT/curTm/
//   resetTm）整体深 4；删除后逐槽回正（-0x20/-0x1c/-0x18/-0x14/-0x24/-0x50/-0x7c）。
//   ②nRows 分支改正极性 `if (nRows != 0) { set } else { reset }`（ORIG `je <reset块>`，P220）。
// 残差（不可翻，N7/4.4.7）：虚调 get_db_table_name 的目标寄存器——ORIG `mov (%eax),%edx; …
//   call *%edx` vs 4.4.7 `mov (%eax),%eax; …; call *%eax`（本 TU 未路由 446，P230 补）。
// 已修（前波）：首段链式、today 走链式、entity 用 evMng 槽。
#include "DB_LoadEtc/LoadAccountFirstLoginDaily.inc"
}

// ============================================================================
// DB_LoadEtc::LoadFairPvPScore @0x08404290 —— fair_pvp_score 14 列；
// n_rows==0 时补插并返回；give_item 压缩列（cap 0x650）→ +0xcf4b；
// daily 重置比较（last_play_time < getDailyInitializeTime(6,0,0) → +0xd59b=0）。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x08404290；474 vs 476（帧/寄存器分配），
// p167 待核。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=516 (orig=474/ours=476)：N82 帧/槽错位（14 列 + 13 处
// trace 的复用 bool ok 槽）；p167 CLEAR 0/0 ⇒ 语义完成、byte 层封顶（§6.2）。
//   [b28d-solo 复测] 474=474 vs 476、p167 CLEAR 20/20；P221/P227 临时链试形对 trace 槽
//   （ORIG -0xe8 深 vs 4.4 -0x28）**零效**（4.1 不复用 13 个 trace 槽，N82 真因）⇒ 封顶。
// [b29-Lane21 复核] 「trace 槽零效」系 fndiff 对齐错觉：两侧 lea -0xe8..-0x28 分布实测全等。
//   真修法三条已落：①`bool ok;` 前置声明（P212，ok=-0x15/db=-0x14 就位）；②`int col = 0;`
//   移到 fetch 检查之后（ORIG +0x1d1 col/i 连发）；③13 处具名 cMyTrace 改全表达式临时链
//   （P227：ctor 前 vararg 装 %ebx 跨 ctor 保活）。读数 ndiff 516→14（orig 474/our 476）。
// [b29-Lane21] 残差 11 行 = N1 常量拆分：ORIG `shl $2; add $0xcf10; add sig; lea 0x13`
//   vs 4.4 `add $0x33c8; shl $2; add sig; lea 3`（0xcf23 分解 mod16+0x13 vs mod4+3，同址
//   sig+0xcf23）；槽集两侧逐值全等 ⇒ 判据⑪ (a) 不成立，语义等价。
//   残差（N1 常量拆分，5 形实测）：ORIG `shl $2; add $0xcf10; add sig; lea 0x13` vs 4.4
//   `add $0x33c8; shl $2; add sig; lea 3`（0xcf23 非 4 对齐的拆分点不同）；&arr[K] /
//   arr+K / 视图指针 / 两段常量 / 行视图五形均折 add。
#include "DB_LoadEtc/LoadFairPvPScore.inc"
}

// ============================================================================
// DB_LoadEtc::LoadBoosterGage @0x08446d9e —— member_booster_gage gage 单列
// 入 +0xeccc（accId 维度）。错行号 0xafc4。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x08446d9e。
// 已修（本波）：①`ok = db->get_int(...)` 复用同一 bool 槽再取反（P102；原写
//   `if (!db->get_int(...))` 直测 call 结果 ⇒ 少 mov %al,槽 + movzbl 两条）；
//   ②`bool ok;` 声明前置于 db 指针局部 ⇒ 槽序对齐 ORIG（ok=-0x15 深于 db=-0x14；
//   P212 全头部声明「先声明深」——声明序反了 4.4 给 db=-0x18/ok=-0x11）。
// 已修（前波）：nRows 具名 + 独立第二守卫（P133）+ col 取后自增 + trace 临时链。
#include "DB_LoadEtc/LoadBoosterGage.inc"
}

// ============================================================================
// DB_LoadEtc::LoadPcRoomPlayTimeInfo @0x084066ae —— 事件关闭时如实 ret 1；
// play_time(+0xc9b3)/reward_cnt(+0xc9b7)。无记录 ret 1。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x084066ae。
// 已修（本波，N92 突破口 = P197① union 双视角）：playTime 局部改
//   `union { unsigned int u; int i; }`——get_uint 出参走 u（unsigned int&），store 走
//   与字段 m_totalPcRoomPlayTime(int) 同型的 i ⇒ 复现 ORIG 单段
//   `mov -0x10(%ebp),%edx; mov 0xc(%ebp),%eax; mov %edx,0xc9b3(%eax)`；
//   直接 `unsigned 局部 → int 字段` 恒多 `mov %eax,%edx` 中转（N92，探针实证），
//   字段侧改 unsigned 会使 CUser::GetEtcInfo 0x08677ae1 直存形多中转（探针 h7）⇒ 不改共享头。
// 已修（前波）：GetRepeatEvent 链式去 ev 局部、NumberToString 内联实参（P259）。
#include "DB_LoadEtc/LoadPcRoomPlayTimeInfo.inc"
}

// ============================================================================
// DB_LoadEtc::LoadUsedGiftFatigueInfo @0x08406810 —— CUsedFatigueGiveItem 事件
// 驱动的 fatigue_quantity 装载（账号表 idx9 / 角色表 idx2 双路径）。
// ============================================================================
// [b29-Lane21] 已修：两守卫改 `||` 短路 + 取反（复现 ORIG `xor $1,%eax`）+ 早返；`quantity`
//   声明上移至 exec 之后（ORIG 0x084069c4 `movl $0x0,-0x20`）。残差 14 行（N 封顶）：
//   N7 callee-saved 角色（ORIG %ebx / 4.4 %esi 承载 NumberToString 保活 + 返回值物化）+
//   P265 单段装载（`mov -0x20(%ebp),%edx` vs 4.4 两段）+ EH 尾归一 3 条。
// b28d-31: N 封顶（P167 未闭合）——ORIG 0x08406810；p167 MISSING
// string 拷贝构造/dtor ×2 + toTString(const string&) ×1 + _Unwind_Resume
//（ORIG 具名 string 临时生命周期 + 重载选择差），语义等价但调用点未归零。
// [b28d-solo-DB_LoadEtc] N封顶（N18a/N7）：ORIG 0x08406810；166 vs 168、p167 CLEAR 20/20。
// 已修（本波）：①两组守卫改**正极性包裹** `if (evMng != 0 && evMng->is_eventing(0x65)) {…}
//   return true;`（P214①）⇒ 复现 ORIG 的短路物化（`cmpl $0,-0x14; je` + `test;je` +
//   `mov $1/$0; test; je`）并把槽组翻正（mng=-0x14/entity=-0x10/isCharac=-0x15/db=-0xc
//   与 ORIG 全等；旧独立 if-return 使槽整体深 4）；②NumberToString 改内联实参（P259，
//   值留 %ebx 跨 toTString call）。
// 残差（不可翻）：①ORIG 取反段 `xor $1,%eax` 属负极性短路物化，正极性包裹形恒缺（4.4 直接
//   `test %al,%al; je`）；②`mov $1,%ebx; jmp` 尾岛与 get_n_rows()==0 的 sete 物化属
//   4.1 共享出口/短路物化，4.4 恒折；③quantity store 的 eax→edx 中转（N18a）。
// 已修（前波）：具名 std::string tableName（p167 归零）、分支极性翻转（!isCharacEvent 直落 idx9/m_id）。
// [b28d-SweepDBLoadEtc2 本波] **真 bug（返回值门禁反向）**：原 `if (get_n_rows()==0 ||
//   !db->fetch()) return true;` 把 fetch 失败当成功返回；ORIG 0x084069c4..0x084069f6
//   为 n_rows==0 → 返回 1（`sete %al` 物化，P225）、fetch 失败 → 返回 0 ⇒ 拆两条独立
//   if。读数：gap 3→3、sete 段两侧一致（助记符直方图差仅返回值归一 esi/ebx 形）。
#include "DB_LoadEtc/LoadUsedGiftFatigueInfo.inc"
}

// ============================================================================
// DB_LoadEtc::LoadAttendanceInfo @0x08448944 —— SIG_ATTENDANCE_EVENT 是当日
// 是否可签到的 out 参数（+0x00 flag，blob 其余不触）。事件关闭返 0。
// ============================================================================
#include "DB_LoadEtc/LoadAttendanceInfo.inc"
}

// ============================================================================
// DB_LoadEtc::LoadSeriaRoomAniDeco @0x0844a20a —— seria 房间动画装饰双 short
// 至 +0x14b5c/+0x14b5e（combo 区尾部残段，超 SIG_COMBO_SKILL 0xb2 的部分）。
// ============================================================================
#include "DB_LoadEtc/LoadSeriaRoomAniDeco.inc"
}

// ============================================================================
// DB_LoadEtc::LoadEventServerMessageInfo @0x084356a2 —— event_server_message
// 按服务器/频道装载进 SIG_LOAD_ETC 条目数组（base +0x2d38 起，stride 0x58）。
// entry 布局（ORIG 写点）：+0x00 kind(byte) | +0x01 index(byte) |
//   +0x08 strlen(size_t) | +0x0c message[50 内] | +0x40 strlen | +0x44 name[20]
//   | +0x58 update_time(int) —— 以文档化偏移直写，条目计数入 +0x2d38。
// ============================================================================
#include "DB_LoadEtc/LoadEventServerMessageInfo.inc"
}

// ============================================================================
// get_skill_table_name @0x083fbdc6：PvP 技能树频道或显式第二表 →
// "skill_fair_pvp.skill"，否则 "skill.skill"。
// ============================================================================
char const* get_skill_table_name(bool use2nd)
{
    if (G_GameWorld()->IsPvPSkilTreeChannel() || use2nd)
        return "skill_fair_pvp.skill";
    return "skill.skill";
}

// ============================================================================
// getComboSkillTableName @0x0844a38b：kind==1 → "combo_skill2"，
// 其余（含 0）→ "combo_skill"。
// ============================================================================
char const* getComboSkillTableName(ENUM_SKILL_TREE_KIND kind)
{
    // ORIG 0x0844a38b 精确形：kind==0 早返 "combo_skill"（test/jne+mov+jmp 岛）、
    // ==1 → "combo_skill_2nd"、default "combo_skill"。
    if (kind == 0)
        return "combo_skill";
    if (kind == 1)
        return "combo_skill_2nd";
    return "combo_skill";
}

// ---- OnlyOrigBatch3：CHARAC_QUEST_SHOP_DATA（ORIG C1 W 0x0848f862 / clear W 0x0848f83e）----
#include "DB_LoadEtc/CHARAC_QUEST_SHOP_DATA.inc"
}

#include "DB_LoadEtc/clear.inc"
}
