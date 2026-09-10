// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_AssaultPlaceDepsB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// W6_AssaultPlaceDepsB.cpp —— Wave6 后半组 30 个 pvp_assault::CAssaultPlace
// ORIG-only 方法（/tmp/only_orig/pvp_assault.txt 54-83 行）。
// 全部语义取自 docs/class_func_reports/pvp_assault__CAssaultPlace/ 对应报告，
// 汇编为准；控制流 / 局部变量 / 调用序 / 常量逐字节转写。
#include "CAssaultMgr.h"

#include <string.h>

#include "CPowerManager.h"          // IsPowerWarEventOn / GetPowerWarLog /
                                    // IncreaePowerWarPointInChannel / ENUM_POWER_SIDE_TYPE
#include "CUser.h"                  // Send / GetParty / get_area / get_posX /
                                    // get_unique_id / get_acc_id / get_charac_no /
                                    // getHackAnalyzer / getPowerSide / cUserHistoryLog
#include "CUserCharacInfo.h"
#include "CParty.h"                 // send_to_party / get_user / get_member_count /
                                    // SetAssaultState / straight-victories 族
#include "PacketGuard.h"            // PacketGuard : InterfacePacketBuf
#include "GlobalData.h"             // s_power_manager / s_systemTime_ / s_guild_proxy_mgr
#include "GameWorld.h"              // G_GameWorld / send_to_vill / send_all / get_user_id_list
#include "CGuildServerProxy.h"      // SendPowerWarPoint / CServerProxyMgr 使用点
#include "CServerProxyMgr.h"        // GetServerProxy 完整类型
#include "CHackAnalyzer.h"          // WongWork::CHackAnalyzer::addServerHackCnt
#include "MSG_DIE_ASSAULT_PLAYER.h"
#include "LogManager.h"
#include "GameTypes.h"              // cMyTrace

#include "Packet_Guild_Change_Power_War_Point.h"   // 本批新建权威头（广播过）

// W7_BuildHarmonize 去重：pvp_assault::GetDirectionByTeam（ORIG T 0x82e564c，
// team 0→6、1→4、其他→0）唯一 owner 为 CAssaultMgr.cpp。
namespace pvp_assault { int GetDirectionByTeam(ENUM_ASSAULT_TEAM team); }

namespace {

}  // namespace

using pvp_assault::CAssaulter;

namespace pvp_assault
{

// ---------------------------------------------------------------------------
// 包构造 helper 组
// ---------------------------------------------------------------------------

// ORIG T 0x82e8810 _MakeCompleteLoad(PacketGuard*, int)
#include "W6_AssaultPlaceDepsB/_MakeCompleteLoad.inc"
}

// ORIG T 0x82e886a _MakeStartAssault(PacketGuard*)
#include "W6_AssaultPlaceDepsB/_MakeStartAssault.inc"
}

// ORIG T 0x82e891e _MakeDieAssaultPlayer(PacketGuard*, int)
#include "W6_AssaultPlaceDepsB/_MakeDieAssaultPlayer.inc"
}

// ORIG T 0x82e8978 _MakeRevivalAssaultPlayer(PacketGuard*, int)（ORIG 未用 this）
#include "W6_AssaultPlaceDepsB/_MakeRevivalAssaultPlayer.inc"
}

// ORIG T 0x82e88a0 ... 见 A 组 _MakeEndAssault —— 此处不重复。
// ORIG T 0x82e852e _MakeMoveAssaultPlace(PacketGuard*)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 200/200（p167 30/30 CLEAR）
//   修法：m_field104 直读、i 改 unsigned（setbe/ja/jbe）、P191 双初始化、P198③ 双槽
//   预清、continue 形；P0 槽-局部律——第二段复用 count/countIdx（ORIG 仅 5 个标量
//   槽：leaderUid0/1 + i + count + countIdx，listCount/listIdx 为多余局部 ⇒ 帧 +0x10）。
//   残差：N82/N95 槽平移（4.4 标量槽整体深 4）。
#include "W6_AssaultPlaceDepsB/_MakeMoveAssaultPlace.inc"
}

// ---------------------------------------------------------------------------
// 发送组
// ---------------------------------------------------------------------------

// ORIG T 0x82e6d92 _SendPacketToVill(PacketGuard&)
#include "W6_AssaultPlaceDepsB/_SendPacketToVill.inc"
}

// ORIG T 0x82e7986 _SendUserMiniInfo()
#include "W6_AssaultPlaceDepsB/_SendUserMiniInfo.inc"
}

// ORIG T 0x82e7a5e _SendUserUdpIpPort()
#include "W6_AssaultPlaceDepsB/_SendUserUdpIpPort.inc"
}

// ORIG T 0x82e8de2 _MakeUserUdpIpPort(PacketGuard*)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 166/166（p167 33/33 CLEAR）
//   get_mtu 段（同型 int 实参）4.4 恒 packet 先载，ORIG 为 `mov %eax,%edx` 值先
//   中转 ⇒ P226 裸语句表达式 `p->put_int(({ get_mtu(); }))` 复现（cast/双 cast
//   两形实测不翻），ndiff 7→0。
#include "W6_AssaultPlaceDepsB/_MakeUserUdpIpPort.inc"
}

// ORIG T 0x82ecdbc _SendVictoryBonus(CUser*, int, int, int, int)
#include "W6_AssaultPlaceDepsB/_SendVictoryBonus.inc"
}

// ORIG T 0x82ec808 _SendPowerWarAssaultResultToUser(CUser*, int)
#include "W6_AssaultPlaceDepsB/_SendPowerWarAssaultResultToUser.inc"
}

// ORIG T 0x82e8300 _MoveAssaultPlace()
#include "W6_AssaultPlaceDepsB/_MoveAssaultPlace.inc"
}

// ---------------------------------------------------------------------------
// 连接 P2P 统计组
// ---------------------------------------------------------------------------

// ORIG T 0x82e927e _IsCannotConnectP2P(int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 26/26（p167 1/1 CLEAR）
//   修法：bool 返回 + 合并短路（byte!=0 && !Empty()）+ 嵌套 if 复现两 je 共享尾岛；
//   P311 成员数组视图 `m_assaulters[a].m_connectIp[b]` 复现 ORIG 装载序（ndiff 11→1，
//   唯一残差 +0x15 常量折叠由 AE 口径归一）。
// [b28c-L7] 返回类型 int→bool（判词见 CAssaultMgr.h）：ORIG 0x82e927e 体尾
// `mov $0x0/$0x1,%eax` 与 bool 归一形一致；改型使两处调用点复现
// `xor $1,%eax` / `test %al,%al` 直测形。
#include "W6_AssaultPlaceDepsB/_IsCannotConnectP2P.inc"
}

// ORIG T 0x82e93bc _GetConnectP2PPoint(int)
#include "W6_AssaultPlaceDepsB/_GetConnectP2PPoint.inc"
}

// ORIG T 0x82e9412 _GetConnectP2PMaxMinPoint(int&, int&)
#include "W6_AssaultPlaceDepsB/_GetConnectP2PMaxMinPoint.inc"
}

// ORIG T 0x82e92c6 _ClearConnectP2PAbsolute(int)
#include "W6_AssaultPlaceDepsB/_ClearConnectP2PAbsolute.inc"
}

// ORIG T 0x82e9358 _ClearConnectP2PRelative(int)
#include "W6_AssaultPlaceDepsB/_ClearConnectP2PRelative.inc"
}

// ---------------------------------------------------------------------------
// PowerWar 组
// ---------------------------------------------------------------------------

// ORIG T 0x82e72fa _PowerWarAssaultCheckIn()
#include "W6_AssaultPlaceDepsB/_PowerWarAssaultCheckIn.inc"
}

// ORIG T 0x82e7362 _CheckPowerWarInPlayer(...) 返回恒 1
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 186/187（p167 22/22 CLEAR）
//   残差：N104 EH 尾块相对序——ORIG 正常路径 PacketGuard D1 块落在 _Unwind_Resume
//   之前（`call _SendPacketToVill; lea -0x1c; call D1; jmp`），4.4 把 D1 移到
//   unwind 段之后；块内容逐条全等（ndiff 7 全为该 3 行块 + jmp 的 rotate）。
//   §6.2 三件套：p167 22/22 CLEAR、gap=1、块内容全等 ⇒ 语义完成，byte 层封顶。
#include "W6_AssaultPlaceDepsB/_CheckPowerWarInPlayer.inc"
}

// ORIG T 0x82ec558 _SendStraightVictories(CUser*)
#include "W6_AssaultPlaceDepsB/_SendStraightVictories.inc"
}

// ORIG T 0x82eceb2 _SendPowerWarPointToGuildServer(CUser*, ENUM_POWER_WAR_WINNER_OR_LOSER, int, int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 333/329（p167 36/36 CLEAR）
//   修法：①删独立 side 局部（P0：ORIG 直写 pkt.m_powerSide@-0x5f=pkt+0xa，
//   后续一律读该成员）；②删 playTime 具名局部（内联为 EndPvP 第三实参，
//   ORIG 经 %ebx 直传）；③winPoint/losePoint 改 int 乘法取低字节（消 4 条
//   movsbl 截断链，ORIG 0x82ed0xx 为 `imul; mov %al`）；④winAliveCnt 形参写回、
//   直写 pkt.m_characNo[idx]、P227 trace 临时链、u 函数头预清（ndiff 260→202）。
//   残差：N95/N82 槽序（4.4 按首次使用序给 u 最深 -0x34 vs ORIG -0x2c，且
//   winPoint/losePoint 与 u 的相对序相反）+ N86b（`losePoint += bonus` 4.4 恒
//   折内存 add，ORIG 为 movzbl/lea/mov 四条形）。
// [E02·R5 fresh 实测] verdict=DIFF 335/329（our 331→329）；p167 调用点差集
//   36=36 CLEAR（MISSING/EXTRA 仅 Packet_Guild_Change_Power_War_Point 与
//   cMyTrace 的 C1/C2 同址别名 = P93 良性）⇒ 语义缺口 0，残差纯形态（槽序）。
//   本轮修法⑤（P0/P52/P134）：删具名 `CGuildServerProxy* proxy` 局部，改链式
//   `GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
//    ->SendPowerWarPoint(pkt);` —— ORIG 0x82ed2xx 为 `call GetServerProxy;
//   lea -0x69(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call
//   SendPowerWarPoint`（结果 %eax 直通 receiver 实参，无栈往返）；我方旧形多
//   `mov %eax,-0x20(%ebp)` + 回读一对（−2 insn）。
//   反劣实验记录（勿重试）：删 cMyTrace 实参位 `static_cast<int>(winPoint)` /
//   `static_cast<int>(losePoint)` 显式提升 ⇒ our 329→331（+2）；此处 cast 是
//   uchar→int 提升必需形（ORIG `movzbl -0x3d,%eax; movzbl %al,%edi`），与
//   C28L0「vararg 冗余 cast 反劣」方向相反，非冗余。
#include "W6_AssaultPlaceDepsB/_SendPowerWarPointToGuildServer.inc"
}

// ORIG T 0x82ec558/0x82eceb2 直读补充：_SendStraightVictories 末尾 endp。
// （注：上面的 EndPvP reason 参数按 ORIG mangled 为字面 1（0x082ed187 前 mov $1）。）

// ---------------------------------------------------------------------------
// TimeoutCompleteLoad
// ---------------------------------------------------------------------------

// ORIG T 0x82ea31a TimeoutCompleteLoad()
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 262/262（p167 39/39 CLEAR）
//   修法：continue 形、P227 cMyTrace 全表达式临时链、删 u!=0 过度防御（P171）、
//   第二段循环底部条件（i<=7 + 循环后 DoConnectP2P）。残差仅 .o 重定位（AE）。
// [b28c-L7][SIGFIX:pvp_assault::CAssaultPlace::TimeoutCompleteLoad] void→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...TimeoutCompleteLoad()"，tsv 第 15 行；
// ORIG 两出口分别落 1 / 0，此前 void 化把返回值整体丢弃 = 建模缺失）。
#include "W6_AssaultPlaceDepsB/TimeoutCompleteLoad.inc"
}


// ---------------------------------------------------------------------------
// ChangeHp
// ---------------------------------------------------------------------------

// ORIG T 0x82ed752 ChangeHp(CUser*, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 155/155（p167 19/19 CLEAR，槽位集全等）
//   修法：删 pwVill 具名局部（内联短路直测）、party 判断改正极性、循环 continue 形、
//   m_parties[].Get()→m_field00 直读。残差仅 .o 重定位显示（AE 口径）。
// [b21 N·P198-②/P197-④] 语义已闭合：`pwVill = IsPowerWarEventOn() && getCurCharacVill()==7`
//   短路判定 ORIG(4.1) 直测 `test %al,%al; je`（无槽），4.4-O0 恒把中间结果
//   `mov %al,-off; cmpb` 物化（P197-④ 源形不可择）；末段 allDead 循环 `GetState()!=8`
//   反亦 `setne` 物化 vs ORIG `cmp $8; jne`；循环回边 `jmp;nop` 合并差（N82）。逐调用
//   逐分支语义逐一相等（_GetUserIndex/m_assaulters[idx].ChangeHp/_MakeChangeHp/Send 三叉/
//   CloseAssault 段全对齐），封 N。
#include "W6_AssaultPlaceDepsB/ChangeHp.inc"
}

// ---------------------------------------------------------------------------
// DieUser 双版本
// ---------------------------------------------------------------------------

// ORIG T 0x82ebda6 DieUser(int, int, MSG_DIE_ASSAULT_PLAYER&)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 528/529（p167 61/61 CLEAR）
//   残差：N82 帧槽（0xdc vs 0xcc，killerName -0x45 vs ORIG -0x61）+ P63 jmp/nop
//   中继；§6.2 三件套（gap=1 + 调用点差集归零 + 槽位集对齐）判语义完成。
//   本轮复核：ORIG 0x82ebda6 函数头有 killer(-0x2c)/victim(-0x28) 双预清 +
//   killerName(-0x61) 清零；三形实测（头部双预清 390 / 仅 killer 391 / 原位
//   347）以原位最优 ⇒ 保持原位，槽序差归 N82。
// §6.2 巨件核销（b25）：P167 修前 61/66 [多4 PartyInfo::Get 多1 memset]→
// 修后 61/61 [0缺0多]；gap 15→-2（orig 531 / our 533）；修法：①killerName
// memset 改 `= {0}` 初始化形（复现 ORIG 字节/字/双字对齐手工清零展开，
// N-78b 回扫）；②m_parties[i].Get() 改 m_parties[i].m_field00 直读（ORIG
// 内联解引用无 call）；③商比较加 (int) 复现 ORIG divl+jle 有符号比较
// （P196 无符号反）；④hack 命名局部删除（ORIG getHackAnalyzer 返回值
// 直传入参无槽）；⑤pw bool 局部删除（ORIG && 汇聚条件内联无槽）；
// ⑥side/winnerType 改 int 槽（ORIG movsbl 物化后 dword 存）。残差归因
// N82 帧槽（0xdc vs 0xcc）+ P63 jmp/nop 中继（详见
// docs/asm_translation_patterns.md 负例表）。
#include "W6_AssaultPlaceDepsB/DieUser.inc"
}

// ORIG T 0x82ed3fe DieUser(CUser*, int, MSG_DIE_ASSAULT_PLAYER&)
#include "W6_AssaultPlaceDepsB/DieUser_1.inc"
}

// ---------------------------------------------------------------------------
// MakeTeam / 组队操作组
// ---------------------------------------------------------------------------

// ORIG T 0x82e7d0a MakeTeam(CUser*, CUser*, int&)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 218/217（p167 20/20 CLEAR）
//   修法：①P226 裸语句表达式复现 get_posX 的 %ebx 跨调用保活；②真 bug——then
//   （posX1<posX2）应为 `_SeparateTeam(user1,user2)`+m_winTeam=0，原写 then/else
//   实参序颠倒；③P212 槽序——i 提至函数头并声明在 team 计数前、team0Count 在
//   team1Count 前，复现 ORIG names(-0x4c..-0x30)>i(-0x2c)>team0(-0x28)>team1(-0x24)
//   （ndiff 153→51）。
//   残差：N107（`&u->m_historyLog` 取址 4.4 恒 add vs ORIG lea；六形探针全败）
//   + N82附（StartPvP 9 实参的 callee-saved/栈临时分配对调）。
#include "W6_AssaultPlaceDepsB/MakeTeam.inc"
}

// ORIG T 0x82e71c4 _GetTeamCountAliveUser(int*)
#include "W6_AssaultPlaceDepsB/_GetTeamCountAliveUser.inc"
}

// ORIG T 0x82e7018 _AddUser(CUser*, ENUM_ASSAULT_TEAM)
#include "W6_AssaultPlaceDepsB/_AddUser.inc"
}

// ORIG T 0x82e70c8 _AddTeam(CUser*, ENUM_ASSAULT_TEAM)
#include "W6_AssaultPlaceDepsB/_AddTeam.inc"
}

// ORIG T 0x82e782a _MoveUser(pvp_assault::CDisposition*, int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 70/70（p167 7/7 CLEAR，446 后端）
//   修法：continue 形复现 nop 岛。
//   [b28d-solo-W6_AssaultPlaceDepsB] 真 bug 修正：ORIG fm 基址 -0x34（GetTeam→
//   -0x34/+4=-0x30/GetUser=-0x2c 三证），param_2→-0x28=+0xc=m_destVill、
//   param_3→-0x24=+0x10=m_destArea；调用点 W6_AssaultPlaceDepsA.cpp:809 传
//   (m_pos.m_x=vill, m_pos.m_y=area) 与 ForcedMove 的 m_destVill/m_destArea 语义
//   自洽。原写 m_destArea/m_x 整体错位一格（m_destVill 未初始化 + 覆盖矩阵 x）。
//   残差：N7（间接 call 目标 4.4.7 恒 %eax，ORIG 用 %edx）。
#include "W6_AssaultPlaceDepsB/_MoveUser.inc"
}

}  // namespace pvp_assault
