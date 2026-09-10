// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_AssaultMgrDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ pvp_assault::CAssaultMgr Wave6 补批实现（WAMgr：only_orig 23 符号清零）。
// 转写来源：docs/class_func_reports/pvp_assault__CAssaultMgr/ 各报告（## 汇编 为准，
// 每个定义前标注 ORIG T/W 地址与尺寸；ORIG 内联浮点常量 _DAT_08c1d18c = -100.0f）。
//
// 本 TU 同时承载 OnRequestAssault / check_party_teleport_user 链上的独立依赖定义：
//   IsStayGate(CUser*)                                ORIG T 0x82edc05 (0x81)
//   CPowerWarLog 生命周期/统计小方法族                  ctor T 0x848021e / Reset T 0x8480232 /
//                                                     Inc* W 见 CPowerManager.h 字段注释
//   CPowerManager::GetPowerWarLog()                   ORIG W 0x82f0906
#include "CAssaultMgr.h"

#include <map>
#include <cmath>

#include <utility>

#include "MSG_DIE_ASSAULT_PLAYER.h"
#include "GameTypes.h"            // ENUM_ERROR
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "CInventory.h"
#include "CEquipItem.h"
#include "Inven_Item.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_power_manager/s_event_manager/s_villageMonsterMgr/s_systemTime_
#include "CPartyTelePort.h"      // CParty::GetPartyTelePort 返回类型
#include "CSystemTime.h"          // CSystemTime::getCurSec
#include "CDataManager.h"         // G_CDataManager（m_serverParameter@+0x68）
#include "CPowerManager.h"
#include "CGameManager.h"         // G_CGameManager()->GetParty(int)
#include "CEnvironment.h"        // G_CEnvironment()->m_serverEnvir.m_serverGroup(+0x378)
#include "GameWorld.h"            // G_GameWorld()/getUserVillage()/IsPowerWarSafeZone()
#include "Village.h"              // Village::get_gate_area
#include "CVillageMonsterMgr.h"   // village_attacked::OnEvent
#include "CPrivateStoreMgr.h"     // private_store 单例链
#include "CExpertJob.h"           // expert_job::CAlchemist::GetAssaultPrevent
#include "TimerDispatcher.h"      // TimerCompleteLoadAssaultTimeout::registNextTimer
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent / CEventBase::IsEventing
#include "LogManager.h"

#include "W6_AssaultMgrDeps/IsStayGate.inc"

namespace pvp_assault
{

namespace
{

// ---------------------------------------------------------------------------
// ORIG 0x0822f6d2：内联 getAssaultPlace(CUser) 的公共前奏 —— 读用户挂接场次 id。
// （函数体内联消失，仅保留语义形状。）
// ---------------------------------------------------------------------------
inline int userIdPlaceId(CUser* user)
{
    return user->GetAssaultPlace();
}

// expert_job::CAlchemist::GetAssaultPrevent 为 static（无 this 槽，
// CExpertJob.h:272 权威声明，ORIG T 0x085d137c）。§4.5 正解：直接静态调用
// `CAlchemist::GetAssaultPrevent(user, flag)`，无需成员函数指针 ABI 转接。
// （DisPatcherCore.cpp 已同型直呼。）

// PowerWarOn：ORIG 各调用点均为 3 条内联形（s_power_manager 装载+call
// IsPowerWarEventOn），无独立符号；P49 —— -O0 下 inline 函数仍发真 call，
// 必须宏展开（[b23S3C]）。
#define PowerWarOn() (GlobalData::s_power_manager->IsPowerWarEventOn())

// CDataManager 内尚未具名化的三个 int 标量（+0x600/+0x604/+0x608，
// importBloodDungeon 区；字段化归 CDataManager 域批次，本批按具名视图读取）。
// 视图成员访问让 4.4 折叠单条 disp（ORIG `mov 0x604(%eax),%eax`）；inline helper
// 在 -O0 发真 call（P49）、裸 char* 偏移多发 add，均不可用。
struct CDataManagerTailView
{
    unsigned char m_reserved[0x600];
    int m_field600;
    int m_field604;
    float m_field608;                                   // ORIG `flds 0x608(%eax)`
};
#define DM_FIELD(n) \
    (reinterpret_cast<const CDataManagerTailView*>(G_CDataManager())->m_field##n)

}  // unnamed namespace

// ===========================================================================
// Reset —— ORIG T 0x082ed966 (0x0f)：清 id 计数器。
// ===========================================================================
#include "W6_AssaultMgrDeps/Reset.inc"
}

// ===========================================================================
// _GetNextAssaultPlaceId —— ORIG T 0x082ed976 (0x87)
// 自增回卷时借静态 count 以 cMyTrace 报警并再 +1。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（P227 全表达式临时链：ORIG ctor 前 `mov count,%ebx` 跨 ctor
// 保活 0x082ed9a3 逐条复现，39=39；旧 N7/N82 判词作废）；ORIG 0x082ed976。
#include "W6_AssaultMgrDeps/_GetNextAssaultPlaceId.inc"
}

// ===========================================================================
// _GetAssaultPlace(unsigned int, int) —— ORIG T 0x082eda94 (0xd6)
// map 未命中抛 ENUM_ERROR(0x15)；authenValue 不匹配抛 ENUM_ERROR(0x8)。
// ===========================================================================
#include "W6_AssaultMgrDeps/_GetAssaultPlace.inc"
}

// ===========================================================================
// FrocedCloseAssaults —— ORIG T 0x082ed5f4 (0x7d)
// 遍历全部场次逐一 EndAssault（非空指针才调）。
// ===========================================================================
#include "W6_AssaultMgrDeps/FrocedCloseAssaults.inc"
}

// ===========================================================================
// OnChangeHp —— ORIG T 0x082ef570 (0x9f)
// 取用户所在场次转派 ChangeHp；异常路径 SendCmdErrorPacket(0x98,err&0xff)。恒返 1。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnChangeHp.inc"
}

// ===========================================================================
// OnAbuseUser —— ORIG T 0x082efc54 (0x5d)：catch 空吞，恒返 1。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnAbuseUser.inc"
}

// ===========================================================================
// OnConnectP2P —— ORIG T 0x82ef27a (0xa5)：错误码包 0x95。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnConnectP2P.inc"
}

// ===========================================================================
// OnRevivalUser —— ORIG T 0x082ef464 (0x10b)
// local result=0；static RevivalUser(user,result)；若中途异常：
// PacketGuard(header(0,0x97)/byte(1)/byte(0)/byte(err)/finalize(true)) 发给该用户。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnRevivalUser.inc"
}

// ===========================================================================
// OnCompleteLoad —— ORIG T 0x082ef1e2 (0x97)：错误码包 0x94。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnCompleteLoad.inc"
}

// ===========================================================================
// OnDieUser —— ORIG T 0x082ef3be (0xa5)：错误码包 0x96。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnDieUser.inc"
}

// ===========================================================================
// AvailableAssault —— ORIG T 0x082f0022 (0x219)：全程为可攻击性校验，失败即抛。
// ===========================================================================
#include "W6_AssaultMgrDeps/AvailableAssault.inc"
}

// ===========================================================================
// OnEvadeAssaultPlace —— ORIG T 0x082efbf6 (0x5d)：catch 空吞，恒返 1。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnEvadeAssaultPlace.inc"
}

// ===========================================================================
// 四个超时入口——模式均为取场次转派 + catch 吞 + 恒返 1。
// ORIG 地址随注释；place 具名局部（P0 槽往返：mov -0x10）后转派。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnTimeoutConnectP2P.inc"
}

#include "W6_AssaultMgrDeps/OnTimeoutEndAssault.inc"
}

#include "W6_AssaultMgrDeps/OnTimeoutCloseAssault.inc"
}

#include "W6_AssaultMgrDeps/OnTimeoutCompleteLoad.inc"
}

// ===========================================================================
// OnConnectP2PAfterAssault —— ORIG T 0x082ef320 (0x9d)
// 会战结束后阶段：P2P 直连改经队伍转发；失败错误码包 0xf9。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnConnectP2PAfterAssault.inc"
}

// ===========================================================================
// OnTimeoutConnectP2PAfterAssault —— ORIG T 0x082ef9d8 (0x79)
// [b28d-solo-W6_AssaultMgrDeps] AE（P214① 正极性短路 &&：4.4 直产 ORIG `je;je;mov $1,%eax;jmp;
// mov $0,%eax;test;je` 物化岛，40=40；旧「bool ABI 跨版本」判词作废）；ORIG 0x082ef9d8。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnTimeoutConnectP2PAfterAssault.inc"
}

// ===========================================================================
// OnTimeoutCompleteLoadAfterAssault —— ORIG T 0x082ef86e (0x169)
// TimerCompleteLoadAfterAssaultTimeout 回调主体（LfAssaultPlace 只交付了 registNextTimer）。
// [b28d-solo-W6_AssaultMgrDeps] AE（95=95）：catch(...) 改 catch(int) 补 `cmp $0x1,%edx` landing；
// 返回类型 void→int（P159：ORIG 尾 `mov $0x1,%eax` 0x082ef9cb）；旧 N93 判词作废。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnTimeoutCompleteLoadAfterAssault.inc"
}

// ===========================================================================
// _SendVictoryInfo —— ORIG T 0x082efcb2 (0x161)
// 包 header(0,0x10a)；PowerSide==1 时 [胜方连数 A][B]，否则反向；
// finalize(true) 后两队分别 send_to_party 或单人 Send。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（105=105）：单 party 槽 -0xc 复用两处 GetParty + P297 反条件
// `je <Send>` 布局；旧 N93 判词作废。ORIG 0x082efcb2。
#include "W6_AssaultMgrDeps/_SendVictoryInfo.inc"
}

// ===========================================================================
// check_party_teleport_user —— ORIG T 0x082efe14 (0x12d)
// 双方队伍传送状态 ∈[0,3] 时分别记 0x9c/0x9d；notifyErr 时回错包。返回错误码。
// ===========================================================================
#include "W6_AssaultMgrDeps/check_party_teleport_user.inc"
}

// ===========================================================================
// GetRequestAssaultNeedMoney —— ORIG T 0x082edc86 (0x333)
// 价格表 + chaos 时效折算 + 混沌点权重 + 等级差系数（浮点公式逐指令转写，
// ROUND 采用 ORIG fstcw(0xc)/fistpl 截断形态，等价 lrintf）。
// ===========================================================================
// ORIG 的浮点→整型截断序列（fnstcw/fldcw RC=0x0c/fistpl）由 4.4 -O0 的
// `static_cast<int>(float)` 直接产出；lrintf 形会发真 call。
#define ROUND_NEAREST_EVEN(v) (static_cast<int>(v))

// [b28d-solo-W6_AssaultMgrDeps] 封顶 N85（253/257 insn）：已修 GetAutoClientRate 取 target（真 bug）、
// 去 event 判空 / powerWarOn 局部 / chaosCapSec、ROUND 改 static_cast<int> 截断内联、
// CDataManager+0x600/604/608 具名视图（+0x608 为 float）。残余 = 4 处 x87 比较
// lowering（ORIG fucompp/fnstsw/test$5,ah/sete 各多 1 条）+ x87 求值序临时槽
// （-0x50/-0x4c/-0x48 vs -0x40 区；具名 float 临时探针反致浅槽漂移 0x0c）。
// ORIG 0x082edc86。
#include "W6_AssaultMgrDeps/GetRequestAssaultNeedMoney.inc"
}

// ===========================================================================
// OnRequestAssault —— ORIG T 0x082edfba (0x11bb)：袭击请求全量校验 + 创建场次。
// ===========================================================================
// [b28c-L7][SIGFIX:pvp_assault::CAssaultMgr::OnRequestAssault] int→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...(CUser*, CUser*, int*)"，tsv 第 14 行；
// mangled 不含返回类型⇒符号不变；体内 return 值仅 0/1）。
// [b28d-solo-W6_AssaultMgrDeps] 封顶 N82/N86（1156/1097，gap +59 = 4.4 帧/临时槽形态差）：
// §6.2 三件套①p167 CLEAR 202/202（0 missing/0 extra）；②逐块对齐 Ghidra C 并修
// 2 个真语义 bug（0x082ee06e attacker 的 AssaultedUserId、0x082ef0ce 错误包
// 漏 put_byte(4)）；③帧 sub $0x120 vs ORIG $0xdc + 缺 push %edi（4.4 临时槽更多）。
// ORIG 0x082edfba。
#include "W6_AssaultMgrDeps/OnRequestAssault.inc"
}

}  // namespace pvp_assault

// ===========================================================================
// IsStayGate —— ORIG T 0x082edc05 (0x81)，全局自由函数（被本 TU 使用）。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（40=40）：P225 `while (x==0) { return true; break; }` 复现 4.1
// 槽比零 sete/test/je 物化；P212 全头部声明对齐双预清；area 内联保 %ebx 跨 call。
// 旧 P197④「不可翻」判词作废。ORIG 0x082edc05。
#include "W6_AssaultMgrDeps/IsStayGate_1.inc"
}

// ===========================================================================
// CPowerWarLog —— 生命周期与统计小方法族（ORIG 符号地址逐一标注）。
// ===========================================================================
#include "W6_AssaultMgrDeps/CPowerWarLog.inc"
}

// [b28d-19] AE（P54 逐字段具名赋值修复，34=34）；ORIG 0x08480232。
#include "W6_AssaultMgrDeps/Reset_1.inc"
}

#include "W6_AssaultMgrDeps/IncAssaultAttemptCount.inc"
}
#include "W6_AssaultMgrDeps/IncAssaultAttemptFailureCount.inc"
}
#include "W6_AssaultMgrDeps/IncAssaultAttemptSuccessCount.inc"
}
#include "W6_AssaultMgrDeps/IncLoadingTimeOutCountSecede.inc"
}
#include "W6_AssaultMgrDeps/IncP2PTimeOutCountSecede.inc"
}
#include "W6_AssaultMgrDeps/IncP2PBadCountFailure.inc"
}
#include "W6_AssaultMgrDeps/IncP2PBadCountSecede.inc"
}
#include "W6_AssaultMgrDeps/IncStartSuccessCount.inc"
}
#include "W6_AssaultMgrDeps/IncStartFailureCount.inc"
}
#include "W6_AssaultMgrDeps/IncTotalJoinAssaultUser.inc"
}
#include "W6_AssaultMgrDeps/IncTotalLeaveAssaultUser.inc"
}

#include "W6_AssaultMgrDeps/GetPowerWarLog.inc"
}

// [b28d-solo-W6_AssaultMgrDeps] 封顶 N82（275/278）：p167 CLEAR 40/40
// （0 missing/0 extra）；残余 = 帧 sub $0x178 vs ORIG push %ebx+sub $0x174、
// cMyTrace 兄弟块临时槽方向（ORIG -0x158→-0xa8 深→浅 vs 4.4 -0x28→-0xe8 浅→深，
// P221）与 ORIG 字段值 %ebx 跨 ctor 保活（P227）。ORIG 0x084802cc。
#include "W6_AssaultMgrDeps/PrintPowerWarAssultReport.inc"
}
