// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GameWorld/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— GameWorld（G2-4，世界/频道/在线用户聚合根）
// 本文件按 ORIG 反汇编（docs/class_func_reports/GameWorld.md + 弱符号直读）
// 逐函数转录，identical 口径验证：source/toolchain/check_tu_game_orig.sh。
// 布局见 GameWorld.h（sizeof 0x1b0，ORIG create() new 0x1b0 实证）。
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "GameWorld.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"   // 权威 CInventory/PacketGuard（本文件原本地 PacketGuard 已删除）
#include "InterSelectCreateDnfEventInfo.h"   // Reward 权威定义（本地局部类已删除，§4.4 合并）
#include "CItemList.h"
#include "Inven_Item.h"
#include "CDataManager.h"
#include "CraneMinigameManager.h"  // CraneMinigameManager::updateCraneItemNeedMaterial
#include "CUserProc.h"       // APSystem::CUserProc::SetTodayActionAndCheckMedalReward
#include "QuickParty.h"      // QuickParty::CQuickPartySystemManager（CDungeon.h 本地最小声明在前置）
#include "CDungeon.h"        // CDungeon::get_min_level（getDungeonMinimumRequiredLevel）
#include "CEnvironment.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CGM_Manager.h"  // 权威 CGM_Manager::IsGm（s_GM_Manager 指针解引用）
#include "CNetwork.h"
#include "CSystemTime.h"
#include "CParty.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "Stream.h"
#include "TaiwanInternal.h"
#include "LogManager.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"   // CServerProxyMgr<CStatisticServerProxy> 完整类型
#include "CHackLog_InvalidAreaMove.h"
#include "Arad_EventPeriodDataManager.h"
#include "Packet_MiniCraneSeed.h"
#include "STQuestScript.h"      // initTownScript / importTownScript / TownScript
#include "CPrivateStoreMgr.h"   // private_store::GetInstancePrivateStoreMgr
#include "CSchoolMgr.h"         // g_schoolMgr
#include "CGuildAgitManager.h"  // g_guildAgitMgr
#include "CPowerManager.h"      // GlobalData::s_power_manager 方法
#include "CAssaultMgr.h"        // pvp_assault::GetInstanceAssaultMgr
#include "CSyncSlangFilter.h"   // CSyncSlangFilter::GetInstance / Filter
#include "CTitleBook.h"         // CTitleBook::sendListOtherUser
#include "CMyFileLog.h"         // CMyFileLog
#include "CFatigueBatteryHandle.h"
#include "CBoosterGage.h"
#include "CEventScriptMng.h"    // EventClassify::CEventScriptMng
#include "CTimeGate.h"
#include "CPartyTelePort.h"     // CPartyTelePort::get_teleport_state
#include "Arad_DataManager.h"   // ARAD::Arad_DataManager / Singleton / notifyOpenMessageDialog
#include "HeroMissionEvent.h"   // ARAD::Singleton<HeroMissionEvent>
#include "AradServerStateMessage.h"
#include "InterSelectPcroomDailyReward.h"
#include "PacketTypes.h"         // MSG_ITEM_HYPER_LINK_MESSAGE 权威定义（原 TU 局部 struct 已删除）
#include "DB_AccountCargoSync.h" // DB_PassPadUpdateFailCnt / APSystem::DB_UpdateActionPoint
#include "CEventStayTime.h"      // CEventStayTime::RewardGoGoFighter 权威（原局部视图 ODR 清理）
#include "Packet_Monitor_Guild_Chat_Hyper_Link.h"    // case 6 超链接公会消息
#include "Packet_Monitor_Member_Chat_Hyper_Link.h"   // case 8 超链接会员消息
#include "Packet_Monitor_MegaPhone_Hyper_Link.h"     // case 0xb/0xc/0xe/0x23 超链接喇叭消息（set 声明，定义已报 Main）
#include "Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"  // case 1 超链接跨服消息
#include "Packet_Monitor_Other_Channel_Chat.h"  // case 1 跨服私聊监控包（ORIG 0x86ca0b9 布局）
#include "../../../shared/packet/include/Packet_Monitor_Guild_Chat.h"  // case 6 公会聊天监控包（ORIG 0x86ca6ac；Game/ 空壳头同名，必须用 shared 完整版）
#include "../../../shared/packet/include/Packet_Monitor_Member_Chat.h"  // case 8 会员聊天监控包（ORIG 0x86ca7dc）
#include "../../../shared/packet/include/Packet_Monitor_MegaPhone.h"  // case 0xb/0xc/0xe 喇叭监控包（ORIG 0x86ca8b8）

// ============================================================================
// CStreamGuard::GetInBuffer 模板实参（全局类型：C++98 不允许局部类型作为模板
// 实参；全局同名类型使 guard.GetInBuffer<SIG_*>() 自然产生 ORIG 模板实例符号
// _ZN12CStreamGuard11GetInBufferI24SIG_UPDATE_STATISTIC4LEVEEPT_v 与
// _ZN12CStreamGuard11GetInBufferI17SIG_NAT_TYPE_USEREEPT_v）

// ============================================================================
// sync_script::CSyncScript 最小声明（不 include sync_script.h：其 ST* 镜像
// 声明与本 TU 经 CUser.h/CDataManager.h 引入的权威 STEquipmentScript/
// EquipmentParameterInfo 冲突；本 TU 仅需 GetInstanceSyncScript +
// truncate/insert_town_info，声明与 sync_script.h 同签名（ORIG mangled 一致））
// ============================================================================
namespace sync_script
{
class CSyncScript
{
public:
    bool insert_town_info_to_db(const TownScript& script);  // _ZN11sync_script11CSyncScript22insert_town_info_to_dbERK10TownScript
    bool truncate_town_info_table();                        // _ZN11sync_script11CSyncScript24truncate_town_info_tableEv
};
CSyncScript* GetInstanceSyncScript();   // _ZN11sync_script21GetInstanceSyncScriptEv
}

// ============================================================================
// 跨类方法（原 extern "C" asm 桥已删除；均改用权威头真实调用：
//   CInventory::insertItemIntoInventory — CInventory.h
//   CUser::GetServerGroup / getPowerSide / GetEventCreateDnfReward — CUser.h
//   CGM_Manager::IsGm — CGM_Manager.h（s_GM_Manager 为指针，按 ORIG 解引用））
// ============================================================================


// ServerParameterScript 方法现由真实 ServerParameterScript.h 提供（m_serverParameter 成员），
// 旧 23 错拼长度 asm 桥接已随调用迁移删除。

// ---- Taiwan 事件流（CEventStayTime::RewardGoGoFighter 依赖）----

// ============================================================================
// 全局数据（ORIG：MAX_VILLAGE_NUM 0948b2b0 / g_townScriptFileList 09500ea0）
// ============================================================================
int MAX_VILLAGE_NUM;
std::map<int, std::string> g_townScriptFileList;

// ============================================================================
// b28c-lane3：Village::get_gate_area 定义由 Village.cpp 迁入（P210 帧继承②）。
// ORIG 中 Village 函数与 GameWorld 函数地址相邻（get_gate_area 0x086c3b58 →
// GameWorld::C1 0x086c3d4a）⇒ 同 TU，4.1 对本 caller 精确继承小帧 sub $0x4；
// 我方拆 TU 后 4.4 仅见声明恒保守 sub $0x18。Village.cpp 内无本函数调用
//（grep 实证），符号全局唯一、mangled 不变。
// ============================================================================
#include "GameWorld/get_gate_area.inc"
}

// ============================================================================
// 单例
// ============================================================================
// 判据㉓（符号绑定，Main 已批准单点修法）：ORIG `_Z11G_GameWorldv` 0x080da3a7 为
// W 弱绑定，我方原为 T 强绑定 ⇒ 定义加 weak（只改本 .cpp，零头文件风险）。
__attribute__((weak)) GameWorld* G_GameWorld()
{
    return GlobalInstance<GameWorld>::inst_ptr();
}

#include "GameWorld/CREATE_GameWorld.inc"
}

#include "GameWorld/DESTROY_GameWorld.inc"
}

// ============================================================================
// 构造 / 析构 / 销毁 / 初始化
// ============================================================================
#include "GameWorld/GameWorld.inc"
}

#include "GameWorld/dtor_GameWorld.inc"
}

#include "GameWorld/destroy.inc"
}

// [b28d-solo-GameWorld] _ZN9GameWorld4initEv verdict=AE 275/275（b28d-solo 本轮）
// 修法：P0/P183 —— `CEnvironment* env = G_CEnvironment();` 具名指针缓存删除，改
// 表达式直传 `G_CEnvironment()->get_gc_no_hardcode(...)`（ORIG 0x086c42aa 结果直作
// receiver、无 env 槽）；删后 gcNo 槽由 -0x20 回到 ORIG -0x1c，多出的 1 call+1 store
// 归零。p167 CLEAR 40=40。
#include "GameWorld/init.inc"
}

// ============================================================================
// 频道
// ============================================================================
#include "GameWorld/InsertChannel.inc"
}

#include "GameWorld/DeleteChannel.inc"
}

#include "GameWorld/UpdateChannelUser.inc"
}

// [b28d-solo-GameWorld] _ZN9GameWorld30updateDetailChannelServiceInfoEv verdict=NEAR 309/309
// （b28d-solo 本轮；助记符序列全等，仅槽号差 8）。
// 修法：①P163 补未列 case 0 撑宽 range（0x086cb1e8 `cmp $0xa;ja` 直接索引）；
// ②P38/P198① v 提到 switch 外预清复用；③P13 —— ORIG 0x086cb471 begin 后立即
// 0x086cb486 调 end（-0x80，循环前缓存）⇒ 具名 `last` 缓存 + goto 尾条件；
// ④P259 —— `Stream* stream=Acquire(...)` 具名局部删，内联进 CStreamGuard 实参
// （ORIG 0x086cb4d1 %eax 直传，无 stream 槽）；⑤0x086cb5e2 `jne <body>; jmp <exit>`
// 极性：反转条件 + 显式 `return` 出口（4.4 对 `if(cit!=last) goto LOOP3;` 恒产
// `je exit; jmp body`）。余 N82/N115 槽组平移 8（it -0x78 vs -0x80）封顶；p167 CLEAR 49=49。
// [A11-round5 复核] P326 适用性判定**不成立**：P326 前提为「ORIG 比较调点是非 const
// `_Rb_tree_iterator::operator==` 且存在 const 转换临时三级槽」；本件 ORIG 调点
// 0x086cb5e2 段为 `_ZNKSt17_Rb_tree_iteratorISt4pairIKiiEEneERKS3_`（const 成员、两级
// 槽 it/cit=-0x78 + last=-0x80），无转换临时可落；p167 的 MISSING `ne` / EXTRA `eq`
// 即源码比较方向差，改 `!=` 已实测回退（见下 COND3 判词）⇒ 维持 N82/N115 封顶。
#include "GameWorld/updateDetailChannelServiceInfo.inc"
}
// [b28d-solo-GameWorld] _ZN9GameWorld30UpdateStatistic4ChannelUserLevEv verdict=AE
// b28d-solo 修法（两处真修）：①P13 —— ORIG 0x086cd18b 先调 end()（last=-0x1c）、
// 0x086cd1a6 后调 begin()（it=-0x20），条件块 0x086cd20d 直接用 last 比较（无每轮
// end call）⇒ 原码缓存 end 迭代器（旧实现每轮 `it != m_UserInWorld.end()`；p167
// 多重集同计数故 CLEAR 掩盖了位置差）；②P0 —— `int level=get_charac_level();
// sig->m_count[level]++` 具名版多 `mov %eax,槽/mov 槽,%eax` 两条，ORIG 0x086cd1ec
// call 结果 %eax 直作 scaled 索引 ⇒ 内联表达式 `sig->m_count[get_charac_level()]++`。
// 余 N82/N93 槽组整体平移（guard/it/sig/info，帧 0x30 vs 0x40）不再试。
#include "GameWorld/UpdateStatistic4ChannelUserLev.inc"
}

#include "GameWorld/GetChannelUserCount.inc"
}

#include "GameWorld/_GetChannelType.inc"
}

#include "GameWorld/GetChannelType.inc"
}

// [b28d-solo-GameWorld] 本轮 30+ 形穷举（register one=1u<<ct / one<<=ct / one 预清 /
// 字段 unsigned / 冗余 cast / while+break / for(;;) / 语句表达式 / 逗号 / const register /
// unsigned long ct / 双条件合并 / 提前 return）实测 4.4.6 恒将 ct 落 %ebx 或触发 N6 折叠 @0x080f131e
// [A11-round5 复核] 本族 5 件（IsIntegratedPvPBaseChannel @0x080f131e / IsPVPChannel
// @0x081424e8 / IsFreePvPChannel @0x081a0ab2 / IsWinPointPvPChannel @0x085dfa92 /
// IsSameTeamChannelJoin @0x082343c8）本轮以 8 源形探针（register ct+one / one 先声明 /
// 声明后赋值 / 表达式直读 / 内联移位 / uchar ct / const register / one 前置求值）在
// 4.4.6、4.4.7、4.4.4 三后端逐形复编（探针 /tmp/r5/probeC.cpp h1..h8）：ORIG 4.1 把 ct
// 留 %eax（0x082343cf `mov 0x20(%eax),%eax` → cmp → `mov %eax,%ecx`），4.4 三后端恒
// 落 %ebx，唯一净差 = `mov %eax,%ebx`(cmp 前) + `mov %ebx,%eax`(shl 前) 两条中转；
// 表达式直读形触发 N6 位折叠（shr/and $1）反劣。⇒ N96/N7 封顶，后继勿再试源形。
// [b28d-solo-GameWorld] _ZNK9GameWorld26IsIntegratedPvPBaseChannelEv verdict=DIFF ndiff=2
// N96/N7 封顶：ORIG 4.1 register ct 留 %eax（0x080f1322 装载一次供 cmp/shl 复用），
// 4.4.6 恒落 %ebx（多 mov %eax,%ebx + mov %ebx,%eax）；表达式直读形触发 N6 折叠
// （shr/and $1）反劣，register 形为最优可接受形。
#include "GameWorld/IsIntegratedPvPBaseChannel.inc"
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证（ORIG 4.1 ct 留 %eax 供 cmp/shl 复用）@0x081424e8
// [b28d-solo-GameWorld] _ZNK9GameWorld12IsPVPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族：ORIG ct 留 %eax @0x081424e8，4.4.6 落 %ebx；余 2 regmove，不可造）。
#include "GameWorld/IsPVPChannel.inc"
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x081a0ab2
// [b28d-solo-GameWorld] _ZNK9GameWorld16IsFreePvPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x081a0ab2，余 2 regmove）。
#include "GameWorld/IsFreePvPChannel.inc"
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x085dfa92
// [b28d-solo-GameWorld] _ZNK9GameWorld20IsWinPointPvPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x085dfa92，余 2 regmove）。
#include "GameWorld/IsWinPointPvPChannel.inc"
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x082343c8
// [b28d-solo-GameWorld] _ZN9GameWorld21IsSameTeamChannelJoinEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x082343c8，余 2 regmove）。
#include "GameWorld/IsSameTeamChannelJoin.inc"
}

#include "GameWorld/IsSchoolPvPChannel.inc"
}

#include "GameWorld/IsPvPSkilTreeChannel.inc"
}

#include "GameWorld/IsEquipSlotSwitchChannel.inc"
}

#include "GameWorld/IsPvPVillageMapChannel.inc"
}

#include "GameWorld/IsCharacterPvPExpRevisionChannel.inc"
}

#include "GameWorld/IsCharacterLevelRevisionChannel.inc"
}

#include "GameWorld/IsCharacterStatRevisionChannel.inc"
}

#include "GameWorld/is_dungeon_tag_matching_channel.inc"
}

// ============================================================================
// 在线用户
// ============================================================================
#include "GameWorld/InsertLoginUser.inc"
}

#include "GameWorld/EraseLoginUser.inc"
}

#include "GameWorld/InsertUserToIDMap.inc"
}

#include "GameWorld/DeleteUserInIDMap.inc"
}

#include "GameWorld/GetIDUserInIDMap.inc"
}

// [b28d-sweep] **真语义 bug 修复**：私商分支虚调用目标还原为真实虚成员调用
// `GetInstancePrivateStoreMgr()->Enter(user)`（P5/P113）。ORIG 0x086c5212
// `mov (%eax),%edx; add $0x4,%edx; mov (%edx),%ecx; call *%ecx` = IUserPos 槽1；
// CPrivateStoreMgr vtable 0x8cbeb38 槽0=Exit(0x85ca994)/槽1=Enter(0x85caa8a)。
// 旧实现 `((void**)storeMgr)[1]` 取对象 +0x04 成员值当函数指针 = 真 bug。
// 271/273 → **271/271**（slotnorm 零残差）；p167 46/46（仅 C1/C2 别名）。
// [b28d-solo-GameWorld] _ZN9GameWorld16reach_game_worldEP5CUser verdict=DIFF 271/271
// 本轮修法（在既有 P50/P61/P192/try-catch 之上）：①P50 声明点 = 发射点 —— 两个
// iterator 声明移到 key（uniqueId/accId）**之前**，复现 ORIG 0x086c4ea0/0x086c4f5b
// 的 C1Ev 先于 get_unique_id/get_acc_id；②P227 全表达式临时链 —— getUserVillage
// 失败分支 cMyTrace 实参 getCurCharacVill 在 ctor 前求值存 %ebx（ORIG 0x086c5185）；
// ③**真语义 bug 修正**：私商分支虚调用改真实虚成员调用（见上方 b28d-sweep 判词）。
// 余 N82：槽组平移（slotnorm 零残差）。p167 46/46（C1/C2 别名对）。
// [A11-round5 复核] P190 撑帧反证：本件 ORIG userIt=-0x50 vs 我方 -0x48（我方浅 8）
// ⇒ 实测 2 个取址 double framePad 撑帧 +8：verdict NEAR 271/271 → **DIFF**（near 4→3 /
// diff 12→13）已回退 ⇒ 残差非帧尺寸差，系 4.1/4.4 槽基与槽组方向差（N82/N126 族）。
// 既有修法（保留）：P50 复活
// （ORIG 0x086c4ea0/0x086c4f5b iterator C1Ev 先声明后赋值）+ P61/P236 具名 key
// （ORIG uniqueId=-0x4a/accId=-0x44/-0x28）+ P192 value_type 直构 +
// **真 try/catch 补缺**（ORIG 0x086c50fb `__cxa_begin_catch` + 0x086c510a
// `strerror(errno)` + 0x086c513a logFormat 0x52d + 0x086c5144 `__cxa_end_catch`，
// 原先缺失 ⇒ p167 MISSING）+ 具名 size/maxUser（-0x10/-0xc）。
#include "GameWorld/reach_game_world.inc"
}

// [b28d-sweep] 298/296 → **298/294**：g_guildAgitMgr 对象化（原指针建模 ⇒ 调用点
// `mov g,%eax; mov %eax,(%esp)`；ORIG 0x086c55e5 `movl $0x943e0e0,(%esp)`）。
// 余 N7/N104：get_area 结果 %esi vs %ebx + EH 尾块相对序。
// [b28d-solo-GameWorld] _ZN9GameWorld16leave_game_worldEP5CUser verdict=DIFF 298/294
// 本轮修法：P50 声明点 = 发射点 —— 两个 iterator 声明移到 key（uniqueId/accId）
// 之前，复现 ORIG 0x086c539c 段 C1Ev 先于 get_unique_id/get_acc_id（旧序差 2 块）。
// 余 N7/N82：get_area 结果 %esi vs %ebx（ORIG 保 %esi 供 check_valid_area 第2实参）
// + 槽组平移；p167 CLEAR 54=54。
// 既有修法（保留）：P50 复活
// （两个 iterator C1Ev 先声明后赋值；p167 MISSING 实证）+ 具名 key + **真 try/catch
// 补缺**（ORIG 0x086c568f `__cxa_begin_catch` + 0x086c569e `strerror(errno)` +
// 0x086c56ce logFormat 0x5a7 "[EXCEPTION Leave GameWorld] - code = %s" +
// 0x086c56d8 `__cxa_end_catch`；原实现缺 try ⇒ p167 MISSING）。
#include "GameWorld/leave_game_world.inc"
}

#include "GameWorld/make_unique_id.inc"
}

#include "GameWorld/find_session.inc"
}

#include "GameWorld/find_from_world.inc"
}

#include "GameWorld/find_user_from_world_byaccid.inc"
}

#include "GameWorld/find_user_from_world_byaccid_loop.inc"
}

 CUser* GameWorld::find_user_by_charac_name(const char* name)
 {
     // ORIG 0x86c9… 族：for-init 声明（begin sret 直落槽）；四个子条件各自
     // 独立物化（setg / test;setne;test / sete;test）⇒ 嵌套单层 if 形，
     // 单层 && 链在 4.4 会被折叠成直跳
     for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
          it != m_UserInWorld.end(); it++)
     {
         CUser* user = it->second;
         if (user != NULL)
         {
             if (user->get_state() > 2)
             {
                 if (user->getCurCharacR() != NULL)
                 {
                     if (strcmp(user->getCurCharacName(), name) == 0)
                     {
                         return user;
                     }
                 }
             }
         }
     }
     return NULL;
 }

// [b28d-solo-GameWorld] _ZN9GameWorld13arrange_usersEv verdict=AE 119=119
// b28d-solo 修法：P221/P227 —— 具名 `cMyTrace trace(...); trace(...)` 改全表达式
// 临时链 `cMyTrace(...)(...)`（NumberToString 实参在 ctor 前求值并跨 ctor 保活 %ebx）
// ⇒ 4.4 复现 ORIG it=-0x2c/trace=-0x24 槽方向（旧具名形 trace 恒占最深 -0x2c 而
// it 落 -0x1c，N82附 封顶判词作废）。P0 内联 env 表达式（ORIG env 无槽往返）保留。
#include "GameWorld/arrange_users.inc"
}

#include "GameWorld/CheckUserCount.inc"
}

#include "GameWorld/GetUserCount.inc"
}

#include "GameWorld/get_UserCount_InWorld.inc"
}

// [b28d-solo-GameWorld] _ZN9GameWorld17UpdateNatTypeUserEv verdict=NEAR 173/174
// （b28d-solo 本轮；仅 4.4 循环体前 1 条对齐 nop 差，助记符/操作数全等）。
// 修法：①P283 第一循环 goto 尾条件（ORIG 0x086cd324 `jmp COND`）；②P28 增量
// ppEv 前缀 ⇒ `++it`；③P259 Acquire 内联进 CStreamGuard 实参（0x086cd3f6 段无
// stream 具名槽）+ 具名 type/count（ORIG -0x14/-0x10）；④**第二循环改 for 形**
// —— ORIG 0x086cd52f `test %al; jne <LOOP2>; jmp <exit>`，旧 goto 形 4.4 恒产
// `je exit; jmp body`（极性反）；for 形逐条复现（含每轮 end 调用位置）。
// 余 N112（4.4 循环出口对齐 nop，TU 旗标不可改）；p167 CLEAR 30=30。
#include "GameWorld/UpdateNatTypeUser.inc"
}

// ============================================================================
// 广播
// ============================================================================
#include "GameWorld/send_all.inc"
}

#include "GameWorld/send_all_1.inc"
}

#include "GameWorld/send_all_excepted_user.inc"
}

#include "GameWorld/send_all_with_state.inc"
}

#include "GameWorld/send_all_dungeon_inout_message.inc"
}

#include "GameWorld/send_all_dungeon_inout_message_1.inc"
}

#include "GameWorld/send_user_dungeon_inout_message.inc"
}

#include "GameWorld/send_to_area.inc"
}

#include "GameWorld/send_to_area_1.inc"
}

#include "GameWorld/send_to_area_2.inc"
}

#include "GameWorld/send_to_user.inc"
}

#include "GameWorld/send_to_vill.inc"
}

// CBelong 唯一声明点=CSchoolMgr.h（b26 NL4 上移；ORIG send_group 0x86c93d 调
// vtable 槽0，`call *%edx; test %al,%al; je`）。
#include "GameWorld/send_group.inc"
}

#include "GameWorld/send_server_group.inc"
}

// [b28d-11b] N：槽位差 4（N82）@0x086c878a
// [b28d-solo-GameWorld] _ZN9GameWorld22send_party_info_to_allEP6CPartyi verdict=AE 309=309
// 修法：P0/P197④ 内联 noTitle 谓词（去具名 bool 落槽）+ P41/P162 删冗余 (short) cast
// （GetPartyIndex 返回 int，ORIG 无 cwtl）。
#include "GameWorld/send_party_info_to_all.inc"
}

#include "GameWorld/_makeBasicInfoHeader.inc"
}

// [L11·round1 判词更新] _ZN9GameWorld17send_AllBasicInfoEP5CUser：ndiff 33→**10**
// （172/172，p167 28=28 CLEAR）。本轮修法 = P221/P227 两处具名 `cMyTrace trace(f,l,v);
// trace(...)`（ORIG 无独立具名对象，实参在 ctor 前右起求值并跨 ctor 保活）改**全表达式
// 临时链** `cMyTrace(f,l,v)(...)`：帧与主槽组一次对齐（PacketGuard -0x24→-0x44 ✓、
// it -0x2c→-0x4c ✓、begin 临时 -0x5c ✓、cMyTrace 两对象 -0x38/-0x28 就位）。
// 残余 4 处（**不可造，N 封顶**，均附 ORIG 0x086c83fc 段实证）：
//  ① O[51:55] `make_basic_info` 实参装载寄存器 %eax vs %edx（ORIG lea→%eax 直压 0x4(%esp)
//     再装 info；4.4 需 %edx 中转）；
//  ②/③ 两个 cMyTrace 槽 -0x34/-0x24 vs 我方 -0x38/-0x28（差 4）；
//  ④ `m_UserInWorld.end()` sret 临时 ORIG -0x38 vs 我方 -0x18（差 0x20）。
// ②③④ 同根：4.1 在**序言**一次性分配匿名临时槽（end 临时最深），4.4 恒在**使用点**
// 分配（end 出现在循环条件 ⇒ 最浅）。实测两形均不翻：显式 `iterator(m_UserInWorld.end())`
// 拷贝临时形（ndiff 10→10）、操作数交换 `end() != it`（10→11）⇒ 判 N（P75/N82 匿名临时区）。
// 既有修法（保留）：P50 复活
// （ORIG 0x086c842a `_Rb_tree_iterator::C1Ev` 0x086d3826 + 0x086c8466 begin sret
// -0x5c → 拷贝进 it(-0x4c) ⇒ 先声明后赋值）+ P197③ info 预清（0x086c842f
// `movl $0x0,-0x10`）+ P283 goto 尾条件 + P171 照抄 error 分支 `jmp COND`
// （跳过 ++it，原码即如此）。
#include "GameWorld/send_AllBasicInfo.inc"
}

// [b28d-11b] 修法：具名局部 cur/remain + user 声明前置 + `while (user==NULL){goto L;}`+
// 尾标签 L:;（新范式：while+goto 复现 4.1 setcc 物化 + 尾岛）→ IDENTICAL 73 @0x086c8694
#include "GameWorld/send_buddy_conn_list.inc"
}

// [b28d-11b] 修法：&& 链拆嵌套 if（P225 推广）79→74；残余 N：4.4 折叠条件链 + body 块序 @0x086c8008
#include "GameWorld/send_power_war_hp_of_vill.inc"
}

// ============================================================================
// 村庄 / 区域
// ============================================================================
#include "GameWorld/GetVillage.inc"
}

#include "GameWorld/getUserVillage.inc"
}

#include "GameWorld/getUserVillage_1.inc"
}

#include "GameWorld/get_gate_area_1.inc"
}

// [b28d-solo-GameWorld] _ZNK9GameWorld21get_area_center_pointEiiRiS0_ verdict=AE 80=80
// 修法：P169 外层正向 if 包内层 goto PASS（两失败路径共享后置 RET0 岛，见函数内注释）
// ⇒ 4.4 复现 ORIG `js RET0; mov MAX,%eax; cmp; jl PASS; RET0: mov $0; jmp end`。
#include "GameWorld/get_area_center_point.inc"
}

// [b28d-11b] N：push ebx + 帧差（N82/P210）@0x086c4814
// [b28d-solo-GameWorld] _ZNK9GameWorld18is_available_pointEiiii verdict=AE 112=112
#include "GameWorld/is_available_point.inc"
}

#include "GameWorld/check_valid_area.inc"
}

// [b28d-11b] 修法：village 声明前置翻槽序（4.4 先声明深/4.1 先声明浅）+ while 复现 sete 物化 → IDENTICAL 73 @0x086cec84
#include "GameWorld/isDungeonEntranceArea.inc"
}

#include "GameWorld/getDungeonMinimumRequiredLevel.inc"
}

#include "GameWorld/GetWorldMapIndex.inc"
}

#include "GameWorld/get_user_id_list.inc"
}

#include "GameWorld/get_disposition.inc"
}

// ORIG CUserModifier 最小镜像（df_game_r modify_user 0x86c9…：`mov (%eax),%eax;
// mov (%eax),%edx … call *%edx` vtable 槽0=Modify(CUser*)，thiscall 栈传 this+user；
// 无虚析构 ⇒ slot0。与 InterDispatcher.cpp:16732 同口径镜像。）
class CUserModifier
{
public:
    virtual void Modify(CUser* user) = 0;
};

#include "GameWorld/modify_user.inc"
}

// [b28d-11b] N：帧差 + push ebx（N82/P210）@0x086c45aa
// [b28d-solo-GameWorld] _ZN9GameWorld45check_restrictive_commercial_transaction_zoneEiiii
// verdict=AE 82=82：P248档②（Area.h m_field6c uchar→bool 消 xor/极性）+ P283/P13
// （goto 尾条件 + 每轮 end()）+ 具名 pair pt（ORIG C1Ev @0x086c45d7）。
// 历史：旧判词 DIFF 82/83（m_field6c uchar + 内联 it->first/second）已被上述
// AE 修法取代；Area.h 改动已 R2 广播（S175_Area 复验零回归，Area.cpp 用自身
// TU 局部定义保持 uchar = P248 两端冲突判例）。
#include "GameWorld/check_restrictive_commercial_transaction_zone.inc"
}

// ============================================================================
// 移动
// ============================================================================
// [b28d-11b] N：xor 物化 + 块序（P168a/N93）@0x086c5706
#include "GameWorld/move_position.inc"
}

// [b28d-11b] 修法：party/curVill/curArea 声明序翻槽 + && 链拆嵌套 if 复现 sete +
// party 预清前移 + ret 移入内层块 + while(ret!=0){return ret;} 复现 setne
// → IDENTICAL 205 @0x086d1002
#include "GameWorld/check_move_area.inc"
}

// [b28d-11b] N：帧/槽差（N82）@0x086c5a84
// [b28d-sweep] 864/872 → **864/864**（gap 0）：真因 = g_guildAgitMgr 建模错误
// （头声明指针 4B，ORIG 为 B 段 0x58 对象）；对象化后 4 处调用点由
// `mov g,%eax; mov %eax,(%esp)` 变 `movl $g_guildAgitMgr,(%esp)`，与 ORIG
// 0x86c658d/0x86c65be/0x86c65ec/0x86c661a 逐条一致（每处 -2 条 ×4）。
// [L11·round1 判词更新] verdict=**AE/IDENTICAL** 864/864（ndiff 230→0；p167 127/127 CLEAR）。
// 本轮 5 项修法（均以 ORIG 0x086c5a84 段实证，逐条复编核消）：
//  ① P227 全表达式临时链（guild-agit 分支 cMyTrace 具名→链式，230→60）；
//  ② P248 档①/P292 被调方返回类型：CHackLog_InvalidAreaMove::isMovableRoute
//     `char`→`bool`（ORIG 调用点 `call; xor $0x1,%eax; test %al,%al; je` 实证，
//     60→57；该头/实现为 Main 授权的无主 TU，已 grep 全仓仅本文件调用）；
//  ③ 声明顺序：`char buf[5]={0}` 提到 hackA/hackB 之前（ORIG O[591] 段清零在前，57→53）；
//  ④ P194/P222 正极性包裹：destArea 两处守卫 `if (destArea < 0){neg}else{pos}` →
//     `if (destArea >= 0){pos}else{neg}`（ORIG `js` 负值跳走 + pos 直落，53→7）；
//  ⑤ P198① 预清两句式 + P225 while+break：destVillage 段 `&&` 链内赋值（4.4 多 4 条
//     `mov $1/$0` 物化）拆为独立语句；moveTown `int moveTown=0; moveTown=call(...);`
//     预清 + `while (moveTown != 0){…; return false;}` 复现 `cmpl $0,-0x38; setne %al;
//     test %al,%al; je`（7→**0**）。
// 旧判词「P75/N82 封顶」作废：真因是 ②③④⑤ 四处源形，非跨版本结构差。
#include "GameWorld/move_area.inc"
}

#include "GameWorld/goto_dungeon.inc"
}

#include "GameWorld/out_from_dungeon.inc"
}

#include "GameWorld/goto_pvp.inc"
}

#include "GameWorld/out_from_pvp.inc"
}

#include "GameWorld/goto_warroom.inc"
}

#include "GameWorld/out_from_warroom.inc"
}

#include "GameWorld/gotoBlueMarble.inc"
}

#include "GameWorld/outFromBlueMarble.inc"
}

// [b28d-solo-GameWorld] _ZN9GameWorld7fishingEP5CUser verdict=AE 263/263（b28d-solo 本轮）
// b28d-solo 修法（四处真修）：①P167 —— `CItemList* itemList=G_CDataManager()->
// getItemList(); itemList->create_item(...)` 改直读 `G_CDataManager()->m_itemList`
// 字段（ORIG 0x086c7cf1 `mov 0xc(%eax),%eax`；getItemList 访问器 4.4 -O0 不内联
// ⇒ 旧形 p167 EXTRA 1 个调用点）；②P59/P168a —— `if (check_valid_area(...)){body}
// else{fail}` 改否定早退 `if (!check_valid_area(...)) {trace;DisConnSig;return;}`
// （ORIG `call; xor $1; test %al; je <主流程>`）；③P222 —— areaType 分支交换 then/else
// 并反条件（ORIG `cmp $0x2; je <鱼获块>` then 后置）；④P227 全表达式临时链（trace
// 实参 getCurCharacName 跨 ctor 保活 %ebx）。
// ⑤本轮新增：`if (result < 1) {失败} else {成功}` 臂序反置 —— ORIG 0x086c7db6
// `jle <失败块>`（条件取反跳 else、成功块 fall-through）⇒ 改正极性
// `if (result > 0) {成功} else {失败}`（4.4 对 `< 1` 恒产 `jg <成功>` 反向布局）。
// 现 263=263 逐条归一化全等、p167 CLEAR 45=45、check_tu_routed verdict=AE。
#include "GameWorld/fishing.inc"
}

#include "GameWorld/IsPowerWarSafeZone.inc"
}

#include "GameWorld/IsPowerWarSafeZone_1.inc"
}

// ============================================================================
// 禁言 / 禁移动族（ORIG 086cd908..086ced1d；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

#include "GameWorld/IsForbiddenChat.inc"
}

#include "GameWorld/IsForbiddenMove.inc"
}

#include "GameWorld/WhenIsTheEndOfForbiddenChat.inc"
}

#include "GameWorld/WhenIsTheEndOfForbiddenMove.inc"
}

#include "GameWorld/GetLeftTimeOfForbiddenChat.inc"
}

#include "GameWorld/GetLeftTimeOfForbiddenMove.inc"
}

// ============================================================================
// 每日重置族（ORIG 086cc340..086cd15f；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

#include "GameWorld/EnableChat.inc"
}

#include "GameWorld/EnableMove.inc"
}

#include "GameWorld/DisableChat.inc"
}

#include "GameWorld/DisableMove.inc"
}


#include "GameWorld/ResetMidnight.inc"
}

#include "GameWorld/resetDailyData.inc"
}

#include "GameWorld/ResetAllCoins.inc"
}

#include "GameWorld/ResetAllCleadpadPoint.inc"
}

// [b28d-11b] 修法：内联 bool 条件（P198②）+ 内联 makeRequest 实参（P52/P87）→ IDENTICAL 109 @0x086cc992
#include "GameWorld/ResetAllPassPadFailCount.inc"
}

#include "GameWorld/ResetAllEventData.inc"
}

// [b28d-solo-GameWorld] _ZN9GameWorld15ResetAllFatigueEv verdict=DIFF-N
// b28d-solo 修法：镜像 if —— ORIG 0x086cc3f2 `cmp $0x2,%eax; setle %al; test;
// jne <循环尾>` ⇒ `if (get_state() <= 2) goto L_next;` + 尾标签（P223/AddDailyItem 族），
// 旧 `if (state > 2) {body}` 恒产 setg/je 反向形。
// 残余 N83/N91/N82 封顶：p167 EXTRA _ZnwjPv/_ZdlPvS_（4.1 placement-new 内联 vs
// 4.4 extern call，N83）+ _Unwind_Resume×2/_ZNSsD1Ev×2（EH landing 差异，N91）+
// 帧 0x7c vs 0x8c 槽组平移（N82）。
#include "GameWorld/ResetAllFatigue.inc"
}

// ============================================================================
// 缓存 / 商店 / 任务重置族（ORIG 086cf684..086d13ba）
// ============================================================================

#include "GameWorld/ResetCacheCharactorMemory.inc"
}

#include "GameWorld/ResetCacheCharactorMemory_1.inc"
}

#include "GameWorld/ResetEntireReliablePerson.inc"
}

#include "GameWorld/ResetOneADayItemShop.inc"
}

#include "GameWorld/testResetOneADayItemShop.inc"
}

// [b28d-11b] 修法：else 尾 goto END → 裸 return; 复现 ORIG 尾 nop 岛 → IDENTICAL 76 @0x086d12b2
#include "GameWorld/SetWeekendBounsEvent.inc"
}

#include "GameWorld/DeleteDailyItem.inc"
}

// [b28d-11b] 修法：it++（P28 增量侧）+ 镜像 if（setle/jne）+ 负条件 goto 尾标签
// 补尾 jmp 岛（P223）→ IDENTICAL 116 @0x086ccdce
#include "GameWorld/AddDailyItem.inc"
}

#include "GameWorld/ReselectDailyQuest.inc"
}

#include "GameWorld/DailyEventModify.inc"
}

#include "GameWorld/ResetCraneItemNeedMaterial.inc"
}

// ORIG 全局自由函数 @0x086cbc85（定义于 CUserOnlyOrig.cpp）
#include "GameWorld/deleteSpecificItems.inc"

#include "GameWorld/deleteSpecificItems_1.inc"
}

// [b28d-11b] 修法：it++（P28 增量侧）158→138；残余 N82 帧/槽 @0x086d14d4
// [b28d-solo-GameWorld] _ZN9GameWorld20UpdateServerSnapShotEv verdict=DIFF-N 186/147
// N-DEP 封顶（等价替代）：ORIG 用 boost::posix_time::second_clock::local_time() +
// gregorian::to_iso_extended_string + posix_time::to_simple_string（39 条 boost 链）。
// b28d 实测：真 boost 形（仓内 Library3rd/Boost/Include 头 + 已挂 -I）可编译且本件
// 186=186 指令数精确对齐（NDIFF 76 全槽号差），但 ORIG 的 boost 是预编译库产物
// （0x086d4419/0x086d28b5/0x086d288c/0x080cde16/0x080ce4bc 为独立 T 符号），头内
// 实现展开致 TU +273 符号、missing 140→227（+87）且本仓无 libboost 定义 ⇒ 越界，
// 维持 localtime_r+strftime 等价替代（同值语义）。P225 推广拆嵌套 if 已复现两条件
// 的 setne 物化（150→147）。帧 0xac vs 0xcc 系 boost 对象槽组。
// [L11·round1 判词更新] ndiff 91→**79**（186/147）。本轮修法 = P221/P227：具名
// `cMyTrace trace(...)` 改全表达式临时链。残余不可造（附 ORIG 0x086d14d4 段实证）：
//  ① p167 MISSING 三件 boost（`second_clock::local_time` / `to_simple_string` /
//     `to_iso_extended_string`）+ EXTRA `CSystemTime::getCurSec` —— 维持 N-DEP
//     （真 boost 形实测 TU missing_in_game 135→222，+82 个 C++11 ABI 别名仅NEW ⇒ 纪律禁）；
//  ② ORIG 六个实参值先落 -0x80..-0x6c 槽（4.1 具名局部）再统一装载，我方内联无槽：
//     属版本装载策略差（P244/P0 反向），补具名亦无法补回 boost 的 string 链。
#include "GameWorld/UpdateServerSnapShot.inc"
}

#include "GameWorld/UpdateMiniCraneSeed.inc"
}

// ============================================================================
// 频道判定 / 统计访问器 / 力战族（ORIG 82343ba..86cf56c）
// ============================================================================

#include "GameWorld/InitChannelType.inc"
}


#include "GameWorld/IsEnchantRevisionChannel.inc"
}

#include "GameWorld/IsCheckUnusableEquipmentChannel.inc"
}

#include "GameWorld/getStatisticsPvP.inc"
}

#include "GameWorld/GetStatisticsNatType.inc"
}

#include "GameWorld/getStatisticsShusiaService.inc"
}

#include "GameWorld/getStatisticsToObserveCrash.inc"
}

#include "GameWorld/CancelPowerUp.inc"
}

// [b28d-11b] N：浮点/双精度局部槽位差 0x18 + 帧差 0x10（P252/N82）@0x086cf1fe
#include "GameWorld/GetPowerUpDomain.inc"
}

#include "GameWorld/SendPowerUpList.inc"
}

#include "GameWorld/OnEndPowerWar.inc"
}

#include "GameWorld/OnRefreshPowerWarProcessInfo.inc"
}

// [b28d-11b] N：局部/槽位差（N82）@0x086cef36
// [b28d-solo-GameWorld] _ZN9GameWorld23OnBanAbuserFromPowerWarEv verdict=AE 140=140
// 修法：P197③ 两段式预清 `CUser* user = NULL;` + `if (m_UserInWorld.size() != 0)` 守卫
// （复现 ORIG 0x086cef3e movl $0,-0x18 + 0x086cef50 call size; test; setne; test; je）
// + P221 全表达式临时链 cMyTrace(...)(...)（具名 trace 槽 -0x2c vs ORIG -0x3c；130→139→140）。
#include "GameWorld/OnBanAbuserFromPowerWar.inc"
}

#include "GameWorld/CleanupPowerWarVillToStart.inc"
}

// [b28d-11b] N：帧差 0x10 + PacketGuard 槽位（N82）@0x086cad68
#include "GameWorld/get_user_info.inc"
}

#include "GameWorld/make_packet_chat_msg.inc"
}

#include "GameWorld/make_packet_chat_msg_hyper_link.inc"
}

// [b28d-11b] 修法：memset→char path[256]={0} 复现 4.1 rep stos（P214③）56→45；残余 N80 变参求值序/具名局部 @0x086c9638
// [b28d-solo-GameWorld] _ZN9GameWorld15WriteLogChatMsgEP5CUsercPc verdict=NEAR 75=75
// 助记符全等，仅 N82/P112 槽分配差：ORIG path=-0x120/log=-0x20/state=-0x134/pvpIdx=-0x130；
// 4.4.6 path=-0x128/log=-0x28/state=-0x20/pvpIdx=-0x1c（标量区与聚合对象区方向相反，声明序不可翻）。
// [L11·round1 追加实测（判据⑩边界）] 两形均零效（复编复测 ndiff 恒 10）：
//   ① `int state; int pvpIdx;` 前置到 `char path[256]` 之前 + 赋值留原位（纯声明重排）；
//   ② 声明/赋值分离（`int state; … state = user->get_state();`）。
// ⇒ 本件 4.4 按**首次使用序**分配：`char path[256]={0}` 的 rep stos 是最早使用点 ⇒
// 占最深槽，标量只能在浅区；ORIG 4.1 按类型分组（标量浅/聚合深）相反 ⇒ 判 N（P112/N98）。
#include "GameWorld/WriteLogChatMsg.inc"
}

#include "GameWorld/send_peerTypemsg_sameWorldUser.inc"
}

#include "GameWorld/check_peerTypemsg_sameWorldUser.inc"
}

// [b28d-11b] N：帧差 0x10 + 槽位（N82）@0x086c975e
// [b28d-sweep] **真缺口修复**：case 3 与 case 0x22 在 ORIG 中共享同一 tail —— 跳表
// 0x8cfba20[3]=0x86ca20b（needClean 逻辑）、[0x22]=0x86ca3de（make_packet_chat_msg +
// send_to_area），case 3 正常路径**落到** 0x86ca3de（Ghidra C 亦为 fallthrough）。
// 旧实现两个 case 各写一份 ⇒ 多 1×{make_packet_chat_msg, get_area, getCurCharacVill,
// send_to_area}（p167 EXTRA 各 +1）。改 fallthrough 后 1299→1267（-32 条），
// p167 EXTRA 4 项归零。余 1255/1267：N82/N87/N89/N91（string/PacketGuard 临时与 EH
// landing 块序；p167 残差仅 C1↔C2 / D1↔D2 同址别名对 +1/+2 个 EH 析构）。
// [b28d-solo-GameWorld] _ZN9GameWorld13send_chat_msgEP5CUserctjPciS2_i15ENUM_NOTIPACKET
// verdict=DIFF 1255/1267 N82/N87/N89 封顶（帧 0x21c vs 0x22c + 大帧槽组平移）。
// b28d-solo 本轮真修：case 0xd/0xf 原只写一次 `int nameLen=strlen(...)` + put_int(nameLen)；
// ORIG 0x086caa08 为 `put_int(strlen(from->getCurCharacName()))` 内联 + 0x086caa2d 具名
// nameLen（两次独立 strlen/getCurCharacName）⇒ p167 MISSING 各 1 归零。
// b28d-solo 本轮归一化复核（46 块残差抽样）：①N87 —— ORIG 0x086c9784 裸
// `shl %cl` 无掩码 vs 4.4 恒插 `and $0x1f`；②符号序 —— ORIG `movsbl; cmp $0x1e;
// ja` vs 4.4 `cmpb $0x1e,mem; jg`（等价，P196 判据：值域 char 无 ≥0x80 分叉）；
// ③string 临时槽组/寄存器（N82/N89 STL 内联）。p167 CLEAR 语义完成。
// [A11-round5 C05/D08 复查 · 无落点] ①尾 fail 块（ORIG 尾 16 条独立 mov $0/$1,%eax）
// 本件尾部无该类块；②F2「两个具名局部比较」—— diff 的 `cmp $0x1e,%al` 属「常量 vs
// 局部」（C05② 边界）⇒ N18b，且修之使 1253→1255 反超 ORIG 1255（不落地）；
// ③jmp 行差 0（无早返中继岛，D08 法无对象）；④点 4781 `if (village == 0) {…break;}`
// 对应 ORIG 0x086c… `call getUserVillage; mov %eax,-0x30(%ebp); cmpl $0x0,-0x30(%ebp);
// sete %al; test %al,%al; je` vs 我方 `…; jne`（P197④/P295 族形式命中）—— 该件已
// N89/N87/N82 封顶（单点不归零）且改形扰动本 1255 条巨件的块布局 ⇒ 判风险>收益留痕。
// [A11-round5 C05④ 分诊] ORIG-only `setne %al; test %al,%al; je` 逐点核（按 Main 硬性顺序）：
// ①callee 侧**无** uchar↔bool 错配 —— 0x086ca4xx 段 `call CUserCharacInfo::GetAssaultPlace;
// test %eax,%eax; setne %al` 与我方 `call …; sete %al` 同为 `test %eax` + setcc，仅
// **极性互补**（落点互补）⇒ 语义等价、非「我方缺归一化」形；②故 P225 原文形（`while`）
// 不适用（P225 针对 setcc **缺失**）⇒ 判 P197④ 族（4.1 setcc 物化 vs 4.4 折叠）N。
// p167 调用点差集（/tmp/tu_GameWorld.o）：MISSING G_GameWorld×3 / getCurCharacName×1 /
// strlen×1；EXTRA make_packet_chat_msg×1 / send_to_area×1 / G_CGameManager×2 /
// PacketGuardC1/D1×3 / basic_string D1×1（ORIG 部分 case 走 G_GameWorld()-> 自由函数
// receiver 形 = P34/P277 族 + 分支调用点归位，巨件未逐 case 收敛，语义待三件套复验）。
#include "GameWorld/send_chat_msg.inc"
}

// [b28d-11b] N：帧差 + 位折叠（N6/N82）@0x086cfa10
// [b28d-solo-GameWorld] _ZN9GameWorld24send_chat_msg_hyper_linkEP5CUserR27MSG_ITEM_HYPER_LINK_MESSAGE15ENUM_NOTIPACKET
// verdict=DIFF 1321/1318 N82/N6/N89 封顶（帧 0xa5c vs 0x35c + 位折叠 + 大帧槽组）。
// [A11-round5 轴 B 修复（已落地，净收益）] case 0xb/0xc/0xe/0x23（本文件 5277 行区）
// ORIG 0x086d0af7/0x086d0afc/0x086d0b01 = `call CUserCharacInfo::getCurrCharacBlackCount;
// cmp $0x12c,%eax; seta %al; test %al,%al; jne <case 尾 0x086d0e86>`（**无符号**正极性早退）；
// 旧文 `if (from->getCurrCharacBlackCount() < 0x12d)` 产 `cmp $0x12c; setle`（有符号）。
// 改 `if (from->getCurrCharacBlackCount() > 0x12cu) { break; }` + 裸块后我方与 ORIG 逐条同形；
// 单件 diff 段 2079→**2075（−4 行）**、TU 四数 195/454/4/12 零回退（20:08:16）⇒
// Main 轴 B「反向（unsigned 向 ORIG 对齐）」修法第二例（同 E08）。
// b28d-solo 本轮真修：同 send_chat_msg case 0xd/0xf —— `put_int(strlen(...))` 内联 +
// 具名 nameLen 两次独立 strlen/getCurCharacName（p167 MISSING 各 1 归零）。
// p167 调用点差集（/tmp/tu_GameWorld.o）：MISSING G_GameWorld×3 / getCurCharacName×1 /
// strlen×1；EXTRA G_CGameManager×2 / basic_string D1×1（同 P34/P277 族 receiver 形 +
// 巨件分支调用点未逐 case 收敛，语义待三件套复验）。
#include "GameWorld/send_chat_msg_hyper_link.inc"
}

// ============================================================================
// ProcessAllUsers 模板（ORIG 实例 0863d3bc / 0816270c）
// ============================================================================

// CEventStayTime::RewardGoGoFighter 权威声明已提升至 CEventStayTime.h
// （此前的局部遮蔽视图按 ODR 清理移除）。

#include "GameWorld/operator.inc"
}

template <typename T>
#include "GameWorld/ProcessAllUsers.inc"
}

template void GameWorld::ProcessAllUsers<CEventStayTime::RewardGoGoFighter>(
    CEventStayTime::RewardGoGoFighter);
template void GameWorld::ProcessAllUsers<InterSelectCreateDnfEventInfo::Reward>(
    InterSelectCreateDnfEventInfo::Reward);
