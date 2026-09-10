// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ARAD_LoginSecurity/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— ARAD 登录安全域（Wave6 ARAD 批次独占 TU）
//
// 覆盖 ORIG 符号清单（nm -S df_game_r 逐一核对 mangled）：
//   _ZN4ARAD26Arad_Dispatcher_Login_ImplC1Ev/C2Ev   0x0818eed0
//   _ZN4ARAD26Arad_Dispatcher_Login_ImplD1Ev/D2Ev   0x0818eef4 / D0 0x0818ef24
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl11db_dispatchEiiP6Stream 0x0818ef4c
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl14inter_dispatchEP5CUserPci 0x0818ef42
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl17punish_user_checkEP14SIG_LOGIN_DATA
//     0x0818f970
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl22account_password_checkEP14SIG_LOGIN_
//     DATAPc 0x0818f746
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl23pad_certifiaction_checkEP14SIG_LOGIN_
//     DATA 0x0818f4a6
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl16premium_info_setE...(vector,int)
//     0x0818f1ca
//   _ZN4ARAD12PUNISH_CHECK19punish_check_by_ARS/HUB/RES/SYS/WEBE... 0x08196d1a..
//     0x08196dbf
//   _ZN4ARAD11PVP_CHANNEL{17,18,20,23,24,25}... 0x0819763c..0x081977b5
//   _ZN4ARAD16AradWhiteAccountC1Ev/C2Ev          0x081ab518
//   _ZN4ARAD16AradWhiteAccountD1Ev/D2Ev          0x081ab55a
//   _ZN4ARAD16AradWhiteAccount4LoadEv            0x081ab5f8
//   _ZN4ARAD16AradWhiteAccount14IsWhiteAccountEj 0x081ab59c
//
// SIG_LOGIN_DATA 字段访问说明：SigTypes_D.h 已按 ORIG 偏移字段化（m_punishInfos/
// m_restrictedGoods/m_passPadPassword 等），本 TU 一律具名成员访问；仅对头内
// 仍为 pad 的字节区使用 TU-local 同址视图（见下方 SigPad394cView）。
//
// [Main 高优先级排查 2026-09-10 · jcc 目标下标逐位核对] 本 TU 全 161 函数用
// /tmp/jcc_check.py 提取每条 jmp/jcc 的 `<func+0xNN>` 目标序列（ORIG 用 nm -S
// 区间切片 + objdump --no-show-raw-insn），**不经 compare_tu clean() 的 <T> 归一**
// 直接比对：mismatched=1/161，唯一不一致为 N123 撞名件
// `_Z41__static_initialization_and_destruction_0ii`（ORIG +0x21 vs 我方 +0x3e，
// 同名 local 副本撞名假差）；其余 160 件（含 punish_user_check/premium_info_set/
// pad_certifiaction_check/pvp_grade_check/db_dispatch 等全部分支密集件）目标下标
// 逐位一致（回边与出口均对齐）⇒ 无「goto 退出标签落在 if 体内致循环提前退出」类
// bug（本 TU 无 import*Script 解析器；唯一 goto 为 punish_user_check 的
// `punish_next:`，标签在循环体内 `++index` 前，目标已核对）。
// ============================================================================

#include "ARAD_LoginSecurity.h"

#include <cstring>
#include <time.h>

#include "CDataManager.h"      // G_CDataManager / PvPChannel 等级接口（W 符号）
#include "CGM_Manager.h"
#include "CSimpleSSO.h"
#include "CRijndael.h"
#include "DB_Login.h"          // DB_Login::tagDNFEventEntry 完整类型
#include "DBMgr.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "PacketBuf.h"         // G_GameWorld（与 GameWorld.h 同源声明）
#include "Stream.h"
#include "StreamDeps.h"        // G_CEnvironment

namespace {

// SIG_LOGIN_DATA +0x394c（m_pad391c 区内，ORIG `movl $0,0x394c(%eax)` 清零）的具名视图
struct SigPad394cView
{
    char m_pad[0x394c];
    int  m_field;
};

}  // namespace

// ---- TU 依赖声明（定义在其它已登记/待登记 TU；本 TU 仅按 ORIG 契约调用）----
#include "ARAD_LoginSecurity/CheckDailyScheduleTime.inc"
namespace ARAD
{

// ============================================================================
// ARAD::PUNISH_CHECK —— 处罚检查表驱动
// ============================================================================

// ctor @ 0x081ab518：std::set 构造后立即 Load()
#include "ARAD_LoginSecurity/AradWhiteAccount.inc"
}

// punish_check_by_HUB @ 0x08196dbf（size 0xf）
#include "ARAD_LoginSecurity/punish_check_by_HUB.inc"
}

// punish_check_by_RES @ 0x08196dce（ORIG 纯 ret）
#include "ARAD_LoginSecurity/punish_check_by_RES.inc"
}


// punish_check_by_WEB @ 0x08196d1a（arg==0 → 0x3e9）
#include "ARAD_LoginSecurity/punish_check_by_WEB.inc"
}

// punish_check_by_SYS @ 0x08196d88（arg==0 → 0x3ea）
#include "ARAD_LoginSecurity/punish_check_by_SYS.inc"
}

// punish_check_by_ARS @ 0x08196d51（arg==0 → 0x3eb）
#include "ARAD_LoginSecurity/punish_check_by_ARS.inc"
}

// ============================================================================
// ARAD::PVP_CHANNEL —— 渠道类型判定
// ============================================================================

// is_integrated_pvp_channel @ 0x0819763c：
// channelType < 0x11 && (1 << channelType) & 0x1e700
// ORIG 0x08197653: cmpl $0x10; ja → 0（无符号 > 0x10）；0x08197665: shl 无掩码
#include "ARAD_LoginSecurity/is_integrated_pvp_channel.inc"
}

// is_broadcast_channel @ 0x08197684（与 is_all_pvp_channel 同语义实现）
#include "ARAD_LoginSecurity/is_broadcast_channel.inc"
}

// is_all_pvp_channel @ 0x081976c7
// [b28d-46 P248 档①] 返回类型 int→bool（mangled 不含返回类型）：调用点 ORIG
// `call; xor $1; test %al; je` 的唯一复现解是 bool callee + 裸 `!call()`
// （P168a 定论：int callee 一切调用点写法恒产 setne）。本头仅本 TU 消费。
#include "ARAD_LoginSecurity/is_all_pvp_channel.inc"
}

// is_winpoint_pvp_channel @ 0x0819770a：channelType ∈ {4, 8}
// [b28d-46 判词·AE] 17/17 指令数全等、调用点差集零。槽变量 + 双 case switch
// （P251/P286 应用）复现 ORIG `mov -0xc,%eax; cmp $K,%eax` 寄存器比较链；
// if 短路链在 4.4 恒折 `cmpl $K,mem`（旧 b28 P209/N18b 判词作废）。
#include "ARAD_LoginSecurity/is_winpoint_pvp_channel.inc"
}

// is_common_channel @ 0x0819773b：非综合 pvp 且渠道号 != 7
#include "ARAD_LoginSecurity/is_common_channel.inc"
}

// check_Login_server_group @ 0x08197781
#include "ARAD_LoginSecurity/check_Login_server_group.inc"
}

// ============================================================================
// ARAD::pvp_grade_check ——【objdump】ORIG 0x0819ebde（无独立报告）
// bool pvp_grade_check(CUser*)
//   channelType ∉ {8,13} → 直接 true；
//   否则 level 不在 [minLevelPvP, maxLevelPvP) → trace(fmt@0x8ba8d3c)+false；
//   否则 IsAvailablePvPChannel(get_pvp_grade()) 为真 → true，
//        否则 trace(fmt@0x8ba8d7c)+false。
// ============================================================================

#include "ARAD_LoginSecurity/pvp_grade_check.inc"
}

// ============================================================================
// ARAD::AradWhiteAccount —— 登录白名单账号集合
// ============================================================================

// dtor @ 0x081ab55a：先 clear() 清空 set，再由编译器析构基类 std::set<unsigned int>。
#include "ARAD_LoginSecurity/dtor_AradWhiteAccount.inc"
}

// Load @ 0x081ab5f8：固定三条白名单账号（非文件解析）。
#include "ARAD_LoginSecurity/Load.inc"
}
#include "ARAD_LoginSecurity/IsWhiteAccount.inc"
}

// ============================================================================
// ARAD::Arad_Dispatcher_Login_Impl —— 登录库分派器（code=1）
// ============================================================================

// ctor @ 0x0818eed0：base(1)，随后写派生 vptr（编译器生成）
#include "ARAD_LoginSecurity/Arad_Dispatcher_Login_Impl.inc"
}

// dtor @ 0x0818eef4；D0 0x0818ef24 条件 operator delete 由编译器生成
#include "ARAD_LoginSecurity/dtor_Arad_Dispatcher_Login_Impl.inc"
}

// inter_dispatch @ 0x0818ef42 —— ORIG 纯 ret 恒返 0（忠实空体，非占位）
#include "ARAD_LoginSecurity/inter_dispatch.inc"
}

// account_password_check @ 0x0818f746（0x229 指令）。
// 流程：
//   out = stOutputData_t()
//   sso.getGameStartTime(uid=+0xc0, loginTime, expireTime, out) 失败 →
//       trace "get game start time error : user_idx[%d]" 返 -1
//   s_GM_Manager->IsGm(uid):
//     GM 分支：strncmp(clientPwd, data+0x18, 0x84)==0 → 0；否则
//              trace "gm user password error user_id[%d]" 返 -1
//     普通分支：把 out 各字段搬入 data 的 SSO 槽位 +
//               CheckDailyScheduleTime 命中则清 +0x395c +
//               strncpy(data+0x3a3a, out.m_loginIp, 0x10)；返 0
#include "ARAD_LoginSecurity/account_password_check.inc"
}

// pad_certifiaction_check @ 0x0818f4a6（0x29f 指令，static 成员）。
// data+0x38e0(byte)/+0x38e1[0x24]/+0x3905[0x24]/+0x390c(byte)/+0x3914(int)/
// +0x3918、Goblin 检查窗口格式 "%Y-%m-%d 06:00:00"（+0x15180=当日 06:00 对齐秒偏移）。
#include "ARAD_LoginSecurity/pad_certifiaction_check.inc"
}

// premium_info_set @ 0x0818f1ca（0x2db 指令）。
// entries 事件入账遍历：eventIdx∈[50000..89999] 有效。
//   [70000..89999] 或受限 premium → 压栈到 data+(count+0xa4c)*4+0xc 并 count++（ushort@+0x293a）；
//   [0xa029..0xa02b] → arad_sp_select_member_brodcast(p, entry, serverGroup)；
//   其余 → arad_sp_select_member_premium(p, entry, dstA(空), copy(dstB))。
#include "ARAD_LoginSecurity/premium_info_set.inc"
}

// db_dispatch @ 0x0818ef4c（0x27d 指令）
#include "ARAD_LoginSecurity/db_dispatch.inc"
}

// punish_user_check @ 0x0818f970：entries 表 i*0x18 步长——time@+0x3a68、
// type@+0x3a54；cur≤time 才做 WEB→SYS→ARS 三段判定，命中即截断 count。
#include "ARAD_LoginSecurity/punish_user_check.inc"
}

}  // namespace ARAD
