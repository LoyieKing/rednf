// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_LoadCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_LoadCharac.cpp
// 目标：清零 /tmp/only_orig/DB_LoadCharac.txt 全部 13 个 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadCharac/<fn>.md（汇编为准）忠实转写。
// SIG_LOAD_CHARAC 权威定义见 SigTypes_C.h（+0xc CHARAC_DATA / +0x686 tag
// charac no / +0x68b TAG_CHARAC_DATA）。SIG 内 CHARAC_DATA/TAG_CHARAC_DATA
// 各区（village/exp/…）按 ORIG 报告 get_* callee 重载实证类型具名字段访问
// （2026-09-05 §4.3 字段化，绑定别名见 SigTypes_C.h/CCharacter.h 注释）。
// ============================================================================
// [b28d-solo-DB_LoadCharac 判词汇总] 本 TU 5 件（check_tu_routed.sh 口径；
// p167 调用点差集 5/5 CLEAR）：
//   _sendResult           80/80  AE（guard 作用域覆盖 trace ⇒ D1 落函数尾；P259 内联实参 + P227 临时链）
//   _getCharacInfo        464/464 AE（负极性短路平铺 f1 形复现 exec/fetch 汇聚块；
//                                   tag 段具名 bool ok + `return ok`）
//   _LoadEventBringUpInfo 158/158 AE（具名 int nRows / unsigned int ret 槽往返 + 三处
//                                   P227 全表达式临时链 + giveRewardFlag 直接布尔比较）
//   _GetTagCharacInfo     373/373 AE（col++ 自增列号局部 + `&out->m_name[0]` 显式取址
//                                   复现单段 lea 0x4）
//   _getCharacStat        644/644 N107 封顶（col++ 自增列号局部；gap 0 + p167 CLEAR
//                                   47/47 + 槽位集全等；残余 2 处 4.4↔4.1 add/lea 选择互换：
//                                   col5(+0x59) 4.4-lea vs ORIG-add+mov、
//                                   col25(+0xc5) 4.4-add+mov vs ORIG-lea，源形不可择）
// P0/N96：本文件无 register asm 绑定（历史绑定已清零，见 _UpdateSchoolCharac 注释）。
// ============================================================================

#include <string.h>
#include <string>

#include "DB_LoadCharac.h"

#include "SigTypes_C.h"       // SIG_LOAD_CHARAC / TAG_CHARAC_DATA / CHARAC_DATA
#include "MySQL.h"
#include "GlobalData.h"        // s_db_mgr / s_stream_pool / s_msgq_mgr
#include "DBMgr.h"             // GetDBHandle
#include "Stream.h"            // Stream::GetOutBuffer 模板
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"         // cMyTrace
#include "CDataManager.h"      // G_CDataManager / get_event_script_mng
#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng（is_eventing/get_db_table_name）
#include "GameWorld.h"         // G_GameWorld()->IsSchoolPvPChannel
#include "SyncScriptDeps.h"    // toTString(const std::string&)

// ORIG T _Z14NumberToStringji @0x0810904b（C++ linkage；定义归既有批次）。
extern char* NumberToString(unsigned int value, int radix);

// ============================================================================
// DB_LoadCharac::DB_LoadCharac @0x844d8c6（C1/C2 同址）
// ORIG：调 DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1/D2 同址 0x844d912 设 vptr→基类空 dtor→恒假 delete
// 分支；D0 0x844d912 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
#include "DB_LoadCharac/DB_LoadCharac.inc"
}

// ============================================================================
// ~DB_LoadCharac @0x844d912（D1/D2）/ deleting dtor @0x844d912（D0）
// ============================================================================
#include "DB_LoadCharac/dtor_DB_LoadCharac.inc"
}

// ============================================================================
// DB_LoadCharac::_getGameDB @0x08408c0a
// 直读 GlobalData::s_db_mgr → GetDBHandle(2, 0)，返回 MySQL*（本函数体不
// 用 this，但 ORIG 调用点 _getCharacInfo 仍按成员压 this，故非 static）。
// ============================================================================
#include "DB_LoadCharac/_getGameDB.inc"
}

// ============================================================================
// DB_LoadCharac::_getCharacInfo @0x08408cf4
// charac_info 33 列装载：get_n_rows()==1 才继续；exec/fetch/任一 get 失败
// 返 0；col32(guild_secede) 后按 tag_charac_no(+0x686) 非零装载
// TAG_CHARAC_DATA(+0x68b)（_GetTagCharacInfo 成功才置 1）。
// ============================================================================
#include "DB_LoadCharac/_getCharacInfo.inc"
}

// ============================================================================
// DB_LoadCharac::_getCharacStat @0x084093f6
// charac_stat 43 列装载（42 数据列）：exec/fetch/任一 get 失败返 0；
// col9(trade_gold_daily) 后、col10(open_flag) 前将 +0xd9 清零（ORIG 写点）。
// [b28d-solo] 644/644 + p167 CLEAR 47/47 + 槽位集全等；残余仅 2 处
// 4.4↔4.1 成员取址 add/lea 选择互换（col5 +0x59 / col25 +0xc5，源形探针
// 全败）⇒ N107 反向封顶，语义与帧/槽/调用点均一致。
// ============================================================================
#include "DB_LoadCharac/_getCharacStat.inc"
}

// ============================================================================
// DB_LoadCharac::_GetTagCharacInfo @0x08409fa6（static，无 this）
// tag_charac_info 23 列装载：exec/fetch 失败或 getAffectedRowCount()==0 返 0；
// 任一 get 失败返 0；全部成功返 1。
// [b28d-solo] 373/373 AE：ORIG 每列 `mov -0xc,%eax; addl $1,-0xc`（自增列号
// 局部 col++）+ `lea 0x4(%eax),%edx`（m_name 显式 &out->m_name[0] 取址形）。
// ============================================================================
#include "DB_LoadCharac/_GetTagCharacInfo.inc"
}

// ============================================================================
// DB_LoadCharac::_UpdateSchoolCharac @0x08408c30
// school_member 更新：job(+0x30)/grow_type(+0x33)/lev(+0x31)，
// where school_id=+0x682 and m_id=NumberToString(accId,0) and charac_no=+0x4。
// 返回 exec(true) 结果。
// 报告 08408c6f..08408c98 实证读取形态：movzbl+movsbl（unsigned char lvalue 经
// (char) 转换）与 movzwl+movswl（unsigned short lvalue 经 (short) 转换）→ 绑定
// **旧类型占位字段**（非 char/short 别名，别名绑 char& 供 _getCharacInfo 用）。
// ============================================================================
#include "DB_LoadCharac/_UpdateSchoolCharac.inc"
}

// ============================================================================
// DB_LoadCharac::_LoadEventBringUpInfo @0x08409d2e
// 事件 0x19b 表装载 use_material_cnt/give_reward_flag 两列：
//   exec 失败 → cMyTrace 0x225b 返 0；get_n_rows()==0 → 返 1（成功无行）；
//   fetch 失败 → 返 0；get_ubyte 失败 → cMyTrace 0x226b/0x2272 返 0；
//   use_material_cnt → +0x139；give_reward_flag != 0 → +0x138 置 1。
// ============================================================================
#include "DB_LoadCharac/_LoadEventBringUpInfo.inc"
}

// ============================================================================
// DB_LoadCharac::_sendResult @0x0840a4ce
// StreamPool::Acquire("DBThread.cpp",0x233a) → guard<<7<<slot →
// guard->GetInBuffer<SIG_LOAD_CHARAC>() → memcpy(sig,0x703) →
// MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)；随后 cMyTrace 0x2347
// "CharLastPlayTick char(%d) t(%d)"（charac_no=+4 / t=+0x59）。
// ============================================================================
#include "DB_LoadCharac/_sendResult.inc"
}

// ============================================================================
// DB_LoadCharac::dispatch @0x08408a3c
// DBDispatcher::dispatch 先决（失败返 0）→ GetOutBuffer<SIG_LOAD_CHARAC>()，
// +0x08(m_result) 清零。try{ _getCharacInfo 失败 throw 1；_getCharacStat
// 失败 throw 3；事件 0x19b is_eventing 且 _LoadEventBringUpInfo 失败
// throw 5 } catch(int line){ m_result=line }。_sendResult 后按
// IsSchoolPvPChannel && schoolNo(+0x682)!=0 调 _UpdateSchoolCharac。返 1。
// ============================================================================
#include "DB_LoadCharac/dispatch.inc"
}
