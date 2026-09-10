// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_LinkMisc4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkMisc4 批次 DB 请求 TU（4 个 makeRequest）。
// 逐函数对照 docs/class_func_reports/{DB_EventGrowthWeaponReset,
// DB_EventUpdateGrowthWeaponTimepiece,DB_ReqChangeCharacName,
// DB_TestTradeRestrict}/makeRequest.md。语义：向 DB 队列（QUEUE_IDX = 2 = DB_Q）
// 发一条 CStreamGuard 请求；表头值、字段写入顺序、Acquire 行号常量均取自报告。
// 声明唯一来源为各自同名头文件。
#include <cstring>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "SigTypes.h"
#include "DB_EventGrowthWeaponReset.h"
#include "DB_EventUpdateGrowthWeaponTimepiece.h"
#include "DB_ReqChangeCharacName.h"
#include "DB_TestTradeRestrict.h"
#include "DBDispatcher.h"
#include "DBMgr.h"
#include "MySQL.h"
#include <map>

// ============================================================================
// DB_EventGrowthWeaponReset::makeRequest @ 0x08449980
// mangled _ZN25DB_EventGrowthWeaponReset11makeRequestEj
// Acquire("DBThread.cpp", 0xb95e)；<<0x316(int) → <<-1 → <<accId(u32 0x080e2292)
// ============================================================================
#include "DB_LinkMisc4/makeRequest.inc"
}

// ============================================================================
// DB_EventUpdateGrowthWeaponTimepiece::makeRequest @ 0x084497ea
// mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece11makeRequestEjsi
// Acquire("DBThread.cpp", 0xb941)；<<0x315 → <<-1 → <<accId(u32) →
// <<timepiece(short 0x0822ad66) → <<value(int 0x080c8c56)
// ============================================================================
#include "DB_LinkMisc4/makeRequest_1.inc"
}

// ============================================================================
// DB_ReqChangeCharacName::makeRequest @ 0x08426590
// mangled _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
// Acquire("DBThread.cpp", 0x64de)；<<0xc3(int) → <<accId(int) →
// SIG_CHANGE_CHARAC_NAME 权威布局见 SigTypes_A.h（总 0x30），按报告偏移写入。
// ============================================================================
// [SweepDBLoadEtcMain 本波] 旧 [b28d-L1]「N7/P168 封顶」**作废**，DIFF→AE：
//   ① +0x2c 改走 SigTypes_A.h 匿名 union `m_group` 视角（完整类型成员直访问，
//      复现 ORIG 08426655 `mov 0x378(%eax),%eax; mov %eax,%edx; mov -0xc(%ebp),%eax;
//      mov %dl,0x2c(%eax)`；旧局部尾视图 cast 恒产 `mov %al,0x2c(%ebx)` + 指针
//      %ebx 跨 call 缓存，探针 /tmp/p211p.cpp f1↔f3）；
//   ② +0x24 改走 `m_arg24u` unsigned 视角（P211 档③ 异型 ⇒ RHS-first+edx 中转，
//      复现 ORIG 0842663c `mov 0x14(%ebp),%edx; mov -0xc(%ebp),%eax; mov %edx,0x24(%eax)`；
//      探针 /tmp/p211q.cpp f1(int←int LHS)↔f2(uint←int RHS)）。
// 两处 union 视角均 sizeof 0x30 / 偏移 / 对齐 / mangled 零变化。
#include "DB_LinkMisc4/makeRequest_2.inc"
}

// ============================================================================
// DB_TestTradeRestrict::makeRequest @ 0x0844a132
// mangled _ZN20DB_TestTradeRestrict11makeRequestEj
// Acquire("DBThread.cpp", 0xba73)；<<0x31c(int) → <<-1 → <<accId(u32)
// ============================================================================
#include "DB_LinkMisc4/makeRequest_3.inc"
}

// ============================================================================
// DB_EventGrowthWeaponReset::dispatch @ 0x084498fe
// mangled _ZN25DB_EventGrowthWeaponReset8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_9, 0) → Stream>>m_id(u32) →
// set_query("deLete from event_1207_growthweapon where m_id = %u", m_id) →
// exec(true) 成功返回 true。
// ============================================================================
#include "DB_LinkMisc4/dispatch.inc"
}

// ============================================================================
// DB_EventUpdateGrowthWeaponTimepiece::dispatch @ 0x084496f4
// mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_9, 0) → >>m_id(u32) → >>timepiece(short) →
// >>infinityweapon_id(int)；id==0 时 UPDATE 不带 reward_occ_date，
// 否则带 reward_occ_date = now()；exec 成功返回 true。
// ============================================================================
#include "DB_LinkMisc4/dispatch_1.inc"
}

// ============================================================================
// DB_TestTradeRestrict::dispatch @ 0x0844a0be
// mangled _ZN20DB_TestTradeRestrict8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_3, 0) → >>charac_no(u32) → UPDATE trade_restrict=1 →
// exec 后恒返回 true。
// ============================================================================
#include "DB_LinkMisc4/dispatch_2.inc"
}

// ============================================================================
// DB_ReqChangeCharacName::dispatch @ 0x084262ea
// mangled _ZN22DB_ReqChangeCharacName8dispatchEiiP6Stream
// 基类 DBDispatcher::dispatch 校验失败 → false；随后：
// GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<SIG_CHANGE_CHARAC_NAME>() 读请求体 →
// escape_string 名字 → env+0x68 m_dbConnInfo[DB_HANDLE_2].m_db(+0x42) 库名 →
// _hasSameCharacName 重名检查（!=0 直接 _sendResult 返回）→
// UPDATE charac_info；exec&&affected!=0 失败 → _sendResult(2) →
// GetDBHandle(DB_HANDLE_8,0) 读 guild_member.guild_id（失败/无行清零 m_arg28）→
// _sendResult(slot, 0)。
// ============================================================================
#include "DB_LinkMisc4/dispatch_3.inc"
}

// ============================================================================
// DB_ReqChangeCharacName::_hasSameCharacName @ 0x084267c2
// mangled _ZN22DB_ReqChangeCharacName18_hasSameCharacNameEP5MySQLPKcS3_
// set_query("seLect delete_flag from %s.charac_info where charac_name='%s'")；
// exec 失败 → 2；无行 → 0；fetch/get_int 失败 → 2；
// delete_flag==0 → 0x14，delete_flag!=0 → 0x18。
// ============================================================================
// [SweepDBLoadEtcMain 本波] 旧 [b28d-L1]「N93/P168a④ 封顶」**作废**，DIFF→AE：
//   ① 短路守卫改失败极性 `if (!(db->fetch() && db->get_int(0, delete_flag))) return 2;`
//      ⇒ 复现 ORIG 08426830/08426851 双 `xor $1,%eax; test %al,%al` 寄存器物化
//      与 08426864 `test %al,%al; je`（旧具名成功 bool 多 `movb %al,-0x9;
//      cmpb $0,-0x9` 栈化物化，P226/P316 同族）；
//   ② 尾部三目 `delete_flag==0?0x14:0x18` 改 if-return 链 ⇒ 复现 ORIG 08426874
//      `test %eax,%eax; je <0x14 块>`（三目恒反产 `jne <0x18>`，P47 镜像）。
#include "DB_LinkMisc4/_hasSameCharacName.inc"
}

// ============================================================================
// DB_ReqChangeCharacName::_sendResult @ 0x084266a4
// mangled _ZN22DB_ReqChangeCharacName11_sendResultEPK22SIG_CHANGE_CHARAC_NAMEii
// Acquire("DBThread.cpp", 0x64f8) → <<0xc3 → <<slot →
// GetInBuffer<SIG_CHANGE_CHARAC_NAME>() 回填请求体（+0x28←sig+0x24、+0x24←result、
// +0x00←sig+0x00、+0x2c←sig+0x28、strncpy name 0x1d）→ put(DISPATCH_Q)。
// ============================================================================
// [SweepDBLoadEtcMain 本波] 旧 [b28d-L1]「N18a 封顶」**作废**，DIFF→AE：
//   读侧改 `out->m_arg28 = sig->m_arg24u;`（uint←uint 同型 ⇒ 单段
//   `mov 0x24(%eax),%edx`，复现 ORIG 0842672e；int 视角恒两段 regmove，
//   P211 档③ 同型化即消中转）。写侧 `out->m_arg24 = result;` 保持 int 视角
//   LHS-first（ORIG 0842673a）。
#include "DB_LinkMisc4/_sendResult.inc"
}
