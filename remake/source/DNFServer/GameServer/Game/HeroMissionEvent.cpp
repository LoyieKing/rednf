// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 HeroMissionEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— HeroMissionEvent（typo 批次，替换 GameStubs 桥接）。
// processMission @ 0x081672be / findMissionValue @ 0x08168010
// 忠实转写 docs/class_func_reports/HeroMissionEvent/{processMission,findMissionValue}.md。
// 判活：ORIG 经 vptr+0x34 调用 CEventBase::IsEventing(NULL)（即读取 +0x08 标志）。
// ============================================================================
// [b28d-solo-HeroMissionEvent 集中判词 · 单文件泳道 7 件收口]
//   （fresh check_tu_routed：COMPILE OK 0 错误 / identical=186 ae=362 near=0 diff=5；
//    本轮基线 identical=184 ae=350 near=1 diff=9）。
//   · HeroMissionValue::getStringToken@0x08164bca —— AE（fresh clean ndiff=0；
//     清单 NEAR1 系旧读数）。
//   · ConvertString2EnumMissionType@0x089613d7 —— AE（fresh clean ndiff=0；旧 NEAR19）。
//   · loadEventData@0x0816709e —— AE【本波翻正】P211 具名成员访问：
//     `sig->m_field0 = user->get_charac_no(-1);` 复现 ORIG RHS-first
//     `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`；此前
//     `*reinterpret_cast<unsigned int*>(sig)=…` 令 4.4 把 sig 升格 %ebx 跨 call
//     保活（LHS-first + `mov %eax,(%ebx)`）。gap 0 / ndiff 0。
//   · saveEventData@0x08166d38 —— N95 封顶（insn 225=225、p167 CLEAR、帧 0x60 相同；
//     全部局部槽整体深 4：ORIG -0xc/-0x20/-0x24/-0x2c/-0x30 ↔ OURS -0x10/-0x24/
//     -0x28/-0x30/-0x34。P259 删 stream 局部实测 gap 0→2 反劣已回退；data/characNo
//     两声明序均试不闭合）。
//   · LevelUpMission::processMission@0x08165c1e —— N18a 封顶（唯一残差
//     `mov (%eax),%eax; mov %eax,%ebx`。P254 enum 化实测净零：字段枚举化使
//     0x08165d3b 两段翻正，但同字段 0x08165e3a 单段被拖成两段（gap 1→-1），已回退；
//     P265 实参 cast 形无效。⇒ 同字段多使用点方向相反，源形不可同时满足）。
//   · loadScript@0x08167ac8 —— N104 封顶（insn 364=364、帧 0x6c 相同、p167 仅缺
//     1 次 _Unwind_Resume：4.4 把 new-cleanup 汇入 catch 尾块，4.1 走 _Unwind_Resume）。
//   · importHeroMissionConditionsData@0x0896171b —— N89/N82 封顶（732 vs 717、
//     gap 15、p167 CLEAR；剩余 ORIG-only 18 条 = movl 6（dead 局部初始化）+ jmp 8
//     （决策树块排布）+ xor/jg/movw/add 各 1；槽基差 4 属 N95）。
//     配套：RewardData 用户空析构删除（P317，Main 代落 HeroMissionCondition.h +
//     W9HeroCondition.cpp）后 EXTRA +2 RewardData D1 消失，本 TU 净 +2 identical/+11 ae。
//   前波其余 12 件（findMissionValue/processMission/rewardMission 族/setEventData 等）
//   维持原 AE/N 判词；语义均按报告逐指令转写，无真语义缺口。
//   P0/N96：本文件 0 处 `register asm` 绑定（grep 实证）。
// ============================================================================

#include "HeroMissionEvent.h"

#include "CUser.h"
#include "LogManager.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "TaiwanInternal.h"      // Taiwan::internal_stream / TaiwanInternalPack::T
#include "TaiwanQueries.h"      // Taiwan::SigLoadHeroMissionData（init/GetInBuffer）
#include "GameTypes.h"          // cMyTrace（StartEvent/EndEvent trace）
#include "ClearDungeonMission.h"
#include "UseFatigueMission.h"
#include "UseCubeMission.h"
#include "LevelUpMission.h"
#include "AttendanceMission.h"
#include <boost/tokenizer.hpp> // char_separator / tokenizer（setEventData 切分）
#include "TotalMission.h"
#include <sstream>             // stringstream（getStringToken 拼串）
#include <string.h>            // strncpy（saveEventData 写回 SigSaveHeroMissionData）

#include "CItem.h"             // CItem::make_item（LevelUpMission 发奖虚调用 vptr+0x08）
#include "Inven_Item.h"        // Inven_Item / set_add_info
#include "CDataManager.h"      // G_CDataManager / find_item
#include "CMailBoxHelper.h"    // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "DNFLexWrapper.h"     // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "CEnvironment.h"      // less_TCHAR / SpinLock / G_CEnvironment
#include "CSystemTime.h"       // CSystemTime::getCurSec（AttendanceMission 判活时刻）

#include "HeroMissionEvent/importHeroMissionConditionsData.inc"
namespace ARAD
{
template HeroMissionEvent* Singleton<HeroMissionEvent>::Get();
}

#include "HeroMissionEvent/HeroMissionEvent.inc"
}

// ============================================================================
// findMissionValue @ 0x08168010
//   bool findMissionValue(CUser*, MissionNo::T, HeroMissionValue*&)
//   判活 → mission ∈ [1,6] → m_values[charac_no] 的 vector 中匹配 mission。
// ============================================================================
#include "HeroMissionEvent/findMissionValue.inc"
}

// ============================================================================
// processMission @ 0x081672be
//   判活 → user 判空 → findMissionValue → m_conditions[mission] 条件对象
//   处理（虚表 +0x08）→ mission==4 且进度刚置位则递归触发 mission 6。
// ============================================================================
#include "HeroMissionEvent/processMission.inc"
}

// ============================================================================
// ORIG T 0x0816709e sz 0x177（loadEventData.md 全文已读）：
// vptr+0x34 IsEventing(0) 为假直接返回；user 空 → log；否则
//   Acquire("localtaiwan/Event/EventHeroMission.cpp",0x291) → CStreamGuard(true)
//   → Taiwan::internal_stream(guard, TaiwanInternalPack::T 0x12, user->GetUID())
//   → guard->GetInBuffer<Taiwan::SigLoadHeroMissionData>()；空 → log；
//   否则 sig->init()；*(int*)sig = user->get_charac_no(-1)；
//   s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard)；栈 guard 析构。
// （ORIG try/catch 形态由 C++ 栈对象自动析构等价承载。）
// ============================================================================
#include "HeroMissionEvent/loadEventData.inc"
}

// ---------------------------------------------------------------------------
// setMissionValue —— ORIG T 0x08167216
// _ZN16HeroMissionEvent15setMissionValueEP5CUserN20HeroMissionCondition9MissionNo1TEj
// （LinkE2.JpnExImpl 批次：objdump 0x8167216..0x81672bc 逐指令转写）。
// 判活：ORIG vptr+0x34 = CEventBase::IsEventing(NULL)，实现即读 +0x08 标志
//（本头布局镜像注释同证）。
// ---------------------------------------------------------------------------
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
#include "HeroMissionEvent/setMissionValue.inc"
}
// ============================================================================
// ORIG 0x0816664a（StartEvent.md 全文已读）：loadScript() 失败（==0，P22
// 取反形态 xor $0x1;test;je）→ cMyTrace(__PRETTY_FUNCTION__,0x1fe,5)
// ("[HeroMission] Script load fail!!!")；成功 → SetEventFlag(true) + trace
// ("[HeroMission] Start!")。镜像布局以 m_isEventing=1 等价 SetEventFlag(true)。
// ============================================================================
#include "HeroMissionEvent/StartEvent.inc"
}

// ============================================================================
// ORIG 0x081666e6（StartEvent_081666e6.md 全文已读）：thunk——经 vptr slot 2
// 虚调用 StartEvent()。镜像布局无 vtable，直接调用等价。
// ============================================================================
#include "HeroMissionEvent/StartEvent_1.inc"
}

// ============================================================================
// ORIG 0x08166700（EndEvent.md 全文已读）：先 cMyTrace("virtual void
// HeroMissionEvent::EndEvent()",0x20d,0)("[HeroMission] End!")，再
// SetEventFlag(false)。镜像布局以 m_isEventing=0 等价。
// ============================================================================
#include "HeroMissionEvent/EndEvent.inc"
}

#include "HeroMissionEvent/TotalMission.inc"
}
#include "HeroMissionEvent/dtor_TotalMission.inc"
#include "HeroMissionEvent/processMission_1.inc"
}


#include "HeroMissionEvent/ClearDungeonMission.inc"
}
#include "HeroMissionEvent/dtor_ClearDungeonMission.inc"
#include "HeroMissionEvent/processMission_2.inc"
}

// ============================================================================
// ClearDungeonMission::rewardMission —— ORIG T 0x08165484（rewardMission.md 全文）。
// 判活顺序：已奖励（非重复且 fieldA!=0）→ 任务条件不满足（type==2 且
// field8!=7）→ 计数未达上限 → rewardProcess 发奖并 fieldA++/field6=1，
// 重复任务 reset。
// ============================================================================
#include "HeroMissionEvent/rewardMission.inc"
}

#include "HeroMissionEvent/UseFatigueMission.inc"
}
#include "HeroMissionEvent/dtor_UseFatigueMission.inc"
#include "HeroMissionEvent/processMission_3.inc"
}

// ============================================================================
// UseFatigueMission::rewardMission —— ORIG T 0x081657a2（rewardMission.md）。
// 同 ClearDungeonMission::rewardMission 骨架（无 missionType 屏蔽分支）。
// ============================================================================
#include "HeroMissionEvent/rewardMission_1.inc"
}

#include "HeroMissionEvent/UseCubeMission.inc"
}
#include "HeroMissionEvent/dtor_UseCubeMission.inc"
#include "HeroMissionEvent/processMission_4.inc"
}

// ============================================================================
// UseCubeMission::rewardMission —— ORIG T 0x08165a2e（rewardMission.md）。
// 同 UseFatigueMission::rewardMission 骨架（行号 0x117/0x11e）。
// [b28c-L26] 残余 4 行判词：
//  ① 首判 `call isRepeatMission; xor $1,%eax; test %al; je`——callee 为 char 返回
//     （ORIG 0x08164fc2 仅 movzbl+ret），bool 化会毁 callee 本体（P168a③b/N 跨版本
//     bool ABI）；X1 `uchar ^=1` 形又会被 4.4 jump-thread 折叠掉紧随的
//     `value.m_fieldA!=0` bool 物化岛（实测 11→13 回退）⇒ 保持短路裸形，缺 1 xor。
//  ② 尾段 `call; test %al,%al; je`（char callee 直判、无 setne）——4.4 对 char 返回
//     在 if() 里恒补 setne 归一，实测本形 0 条差改裸形反增；维持。
//  ③ `mov (%eax),%eax; mov %eax,%ebx` 两段：ORIG 4.1 冗余 regmove（N18a 反向，
//     we-smaller 1 条），4.4 无源形可造。
//  ④ 其余 diff 全为 call/jmp 目标与字符串/行号地址的 .o 重定位占位（§6.3）。
// ============================================================================
#include "HeroMissionEvent/rewardMission_2.inc"
}

#include "HeroMissionEvent/LevelUpMission.inc"
}
#include "HeroMissionEvent/dtor_LevelUpMission.inc"
#include "HeroMissionEvent/processMission_5.inc"
}

// ============================================================================
// LevelUpMission::rewardMission —— ORIG T 0x08165f86（rewardMission.md）：
// 恒返回 true（升级奖励在 processMission 路径发放，无独立 reward 流程）。
// ============================================================================
#include "HeroMissionEvent/rewardMission_3.inc"
}

#include "HeroMissionEvent/AttendanceMission.inc"
}
#include "HeroMissionEvent/dtor_AttendanceMission.inc"
#include "HeroMissionEvent/processMission_6.inc"
}

// ============================================================================
// AttendanceMission::rewardMission —— ORIG T 0x081661d6（rewardMission.md）。
// 骨架同 UseFatigueMission::rewardMission；已奖励分支无日志，条件不满足分支
// 用 cMyTrace（0x1a4, lvl 5）而非 logFormat。
// ============================================================================
#include "HeroMissionEvent/rewardMission_4.inc"
}

// ============================================================================
// ConvertString2EnumMissionType —— ORIG t 0x89613d7（无报告，objdump 0x 转写）。
// [type] 标签的字符串枚举 → 整型（"normal"→1 / "payment"→2 / "final"→3）。
// 函数级 static 惰性表（lookup @0x94f7598 / SpinLock @0x94f75b0 / 就绪字节
// @0x94f75b4，双检锁 + 首轮 fallthrough 建表后重查，P21 范式）。
// [b28d-solo-HeroMissionEvent] 判词 AE：fresh clean ndiff=0（orig=213 our=213，
// gap=0）；清单 NEAR19 系旧读数。
// ============================================================================
static int ConvertString2EnumMissionType(std::string& s)
{
loop_top:
    static std::map<const char*, int, less_TCHAR> lookup;
    static SpinLock lock;
    static bool inited;
    int code;
    if (inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            lookup.find(s.c_str());
        code = (it != lookup.end()) ? it->second : -1;
    }
    else
    {
        lock.enter();
        if (inited)
        {
            lock.leave();
            goto loop_top;
        }
        code = 0;
    }

    switch (code)
    {
    case 0:
    case 0x3d1:   // "normal"
        if (!inited)
            lookup.insert(std::pair<const char* const, int>("normal", 0x3d1));
        else
            return 1;
    case 0x3d3:   // "payment"
        if (!inited)
            lookup.insert(std::pair<const char* const, int>("payment", 0x3d3));
        else
            return 2;
    case 0x3d5:   // "final"
        if (!inited)
            lookup.insert(std::pair<const char* const, int>("final", 0x3d5));
        else
            return 3;
    }

    if (!inited)
    {
        lock.leave();
        inited = true;
        goto loop_top;
    }
    return 0;
}

// ============================================================================
// importHeroMissionConditionsData —— ORIG T 0x896171b（无报告，nm+objdump 0x
// 逐指令转写）。RDA 脚本 "Event/HeroMissionEvent.evt" 解析：
//   [/hero event] [mission] [type] [repeat] [reset] [item] [condition]
//   [mail title] [mail content] [/mission]
// 返回：0=成功；非 0=出错脚本行号（loadScript 以 0 判活）。
// 首轮经 switch fallthrough 链惰性填充标签表（lookup @0x94f7578 / SpinLock
// @0x94f7590 / 就绪字节 @0x94f7594，P21 范式），随后重查当前行分派。
// ============================================================================
// importHeroMissionConditionsData @ 0x0896171b —— [b28d-solo-HeroMissionEvent ·
//   N89/N82 封顶] 732 vs 717、gap 15、p167 CLEAR（0 missing/0 extra）。
//   剩余 ORIG-only 18 条 = movl 6（dead 局部初始化）+ jmp 8（决策树块排布）
//   + xor/jg/movw/add 各 1；槽基差 4（ORIG cond -0x16c ↔ OURS -0x170）属 N95；
//   4.1 对 vector/map 模板 out-of-line 实例（N89）+ 4.4 槽分配序（N82）不可翻。
//   配套：RewardData 用户空析构删除（P317）后 EXTRA +2 D1 消失。
#include "HeroMissionEvent/importHeroMissionConditionsData_1.inc"
}

// loadScript @ 0x08167ac8 —— [b28d-solo-HeroMissionEvent · N104 封顶]
//   insn 364=364、帧 0x6c 相同、p167 仅缺 1 次 _Unwind_Resume：4.4 把
//   new-cleanup 汇入 catch 尾块，4.1 走 _Unwind_Resume（N104 EH 尾块相对序）。
//   语义完成（new×6 + import + 条件遍历 + logFormat 调用点全数对齐）。
#include "HeroMissionEvent/loadScript.inc"
}
// ============================================================================
// setEventData @ 0x08166752（setEventData.md 全文转写；定义归本 TU，
// 与 HeroMissionLink4.cpp 旧定义同符号，由 Main 整合去重）。
// 判活 → charac_no 非 0 → 空数据按条件表生成初始单（level==70 时任务 4/6
// 置完成态）；非空按 "," 切分逐 token setStringToken；最后
// find 已存在 → log(0x24b)+erase，再 insert(make_pair(charac_no, values))。
// ============================================================================
#include "HeroMissionEvent/setEventData.inc"
}
// ============================================================================
// HeroMissionValue::getStringToken —— ORIG T 0x08164bca（getStringToken.md
// 全文转写；saveEventData 依赖，定义归本 TU）。
// 若 out 非空先补 ','，再按 "mission f4 f6 fA f8" 空格分隔追加（字段序与
// setStringToken 的解析偏移一一对应：tok0→m_mission、tok1→+0x04、tok2→+0x06、
// tok3→+0x0a、tok4→+0x08）。
// [b28d-solo-HeroMissionEvent] 判词 AE：fresh clean ndiff=0（orig=111 our=111，
// gap=0）；清单 NEAR1 系旧读数。修法=枚举视图选 `_ZNSolsEi` + 链拆两语句。
// ============================================================================
#include "HeroMissionEvent/getStringToken.inc"
}
// ============================================================================
// setEventData(std::string const&) —— ORIG T 0x08166d0e（setEventData_08166d0e.md
// 全文转写）：thunk——c_str() 后转调 char const* 版本。
// ============================================================================
#include "HeroMissionEvent/setEventData_1.inc"
}
// ============================================================================
// setMissionForGm —— ORIG T 0x081679ac（setMissionForGm.md 全文转写）。
// findMissionValue 命中且 m_conditions 中存在该 mission 时，
// 写 HeroMissionValue +0x04（m_field4）为 ushort 截断的 GM 设定值。
// ============================================================================
#include "HeroMissionEvent/setMissionForGm.inc"
}
// ============================================================================
// resetMissionForGm —— ORIG T 0x08167a3a（resetMissionForGm.md 全文转写）。
// findMissionValue 命中且 m_conditions 中存在该 mission 时，调
// HeroMissionValue::reset()（+0x04..+0x0a 清零）。
// ============================================================================
#include "HeroMissionEvent/resetMissionForGm.inc"
}
// ============================================================================
// saveEventData —— ORIG T 0x08166d38（saveEventData.md 全文转写）。
// 判活 → user 判空 → charac_no 非 0 → m_values.find(charac_no) 不存在则 log
// 0x269 返回；存在则逐 HeroMissionValue getStringToken 拼 "," 分隔串；
// Acquire(0x271) → CStreamGuard(true) → Taiwan::internal_stream(guard, T 0x11,
// user->GetUID()) → GetInBuffer<SigSaveHeroMissionData>()；空 → log 0x276；
// 否则 sig->init()；*(int*)sig = charac_no；strncpy(sig+0x04, data, len)；
// s_msgq_mgr->put(QUEUE_IDX(2), guard)；removeData 为真再 erase(charac_no)。
// （ORIG try/catch 形态由栈对象自动析构等价承载。）
// ============================================================================
// saveEventData @ 0x08166d38 —— [b28d-solo-HeroMissionEvent · N95 封顶]
//   insn 225=225、帧 0x60 相同、p167 CLEAR；全部局部槽整体深 4
//   （ORIG -0xc/-0x20/-0x24/-0x2c/-0x30 ↔ OURS -0x10/-0x24/-0x28/-0x30/-0x34），
//   属 4.4 局部槽基/分配序跨版本（N95）。P259 删 stream 局部实测 gap 0→2 反劣
//   （已回退）；data/characNo 两声明序均试不闭合。
#include "HeroMissionEvent/saveEventData.inc"
}

