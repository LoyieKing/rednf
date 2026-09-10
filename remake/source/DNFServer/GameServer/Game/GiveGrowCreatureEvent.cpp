// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GiveGrowCreatureEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// GiveGrowCreatureEvent.cpp —— GiveGrowCreatureEvent /
// DB_EventSaveGiveGrowCreature / DB_EventUpdateGiveGrowCreature 全部 ORIG 符号真实实现
//（声明见 W8LT_EventTail.h）。
//
// [GI 拆分 2026-09-09] 自 W8LT_EventTail.cpp 拆出：该 .cpp 为 ONE2MANY（我方一个 TU
// 覆盖 ORIG 多 TU），plan_gi_swaps.py --arbitrate 两候选分数完全并列 ⇒ 必须拆 TU。
// 本文件首「强符号·外部链接·本 TU 定义」实体 = GiveGrowCreatureEvent::GiveGrowCreatureEvent()
// ⇒ _GLOBAL__I_ 名 = _ZN21GiveGrowCreatureEventC2Ev（ORIG TU = GiveGrowCreatureEvent.cpp）。
// 定义体逐字未改（仅移动 + 段序调整以固定 GI 选名实体）。
//
// 依据 docs/class_func_reports/{GiveGrowCreatureEvent,DB_EventSaveGiveGrowCreature,
//   DB_EventUpdateGiveGrowCreature}/ 对应报告 + ORIG objdump 逐函数转写；
// 地址注释逐函数标注。
// ============================================================================

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/foreach.hpp>       // ORIG：questClearLog 用 BOOST_FOREACH
#include <boost/lexical_cast.hpp>  // ORIG：buyItemLog/questClearLog 用 lexical_cast
#include "GlobalData.h"
#include "W8LT_EventTail.h"
#include "GameTypes.h"            // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "MoreTypes.h"            // SIG_GIVE_GROWTH_CREATURE_EVENT
#include "CUser.h"
#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy                // GrowthCreatureEvent / getGrowthCreatureEventdata
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "MySQL.h"            // MySQL::{set_query,exec,getAffectedRowCount}
#include "CEnvironment.h"     // DB_HANDLE_9 / E_SERVER_GROUP_NONE
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"

// ORIG W 0x081b763e _Z9ss_strcpyILj30EEiRAT__cPKc：头内联模板版（同
// OnlyOrigBatch12.cpp 口径），调用点以数组引用选择本重载。
template <size_t N>
#include "GiveGrowCreatureEvent/ss_strcpy.inc"
#include "GiveGrowCreatureEvent/GiveGrowCreatureEvent.inc"
#include "GiveGrowCreatureEvent/dtor_GiveGrowCreatureEvent.inc"

#include "GiveGrowCreatureEvent/StartEvent.inc"
}

#include "GiveGrowCreatureEvent/EndEvent.inc"
}

#include "GiveGrowCreatureEvent/StartEvent_1.inc"
}

#include "GiveGrowCreatureEvent/StartAction.inc"
}

// [b28d-solo-W8LT_EventTail] DIFF→N 封顶（215=215，仅装载序/对齐 nop） | ORIG 0x081b6b70
// | P167 CLEAR（40/40）。
// 修法①（真语义 bug）：内层循环 ORIG 仅在 nameLen!=0 时 break（nameLen==0 继续），
// 原码无条件 break；②P171 删 `mine == 0`；③P167/P96：map 迭代器非 const + `->`
// （ptEv）、vector 迭代器 `(*it).`（deEv）；④P13/P28 缓存 end + 后置 `it++`；
// ⑤P198/P0 具名 jobCount（-0x10 槽往返）；⑥nameLen 分支改
// `if(nameLen==0){put_byte(1);} else{put_byte(2);put_int;put_str;break;}`（P47/P191
// 块序：ORIG then（==0 块）直落 kt++、else 后置跳外层）⇒ 块序与 p167 差 1 全消。
// 残余（N 封顶）：①`mine->begin()/end()` 调用点 ORIG this-first（`mov -0xc,%edx` 先）
// vs 4.4.6 恒 sret-first（`lea -0x3c,%eax` 先）——探针 t1..t4（初始化式/(*p)./引用
// 接收/直接初始化括号）全 sret-first；②else 块尾 ORIG 的 1 条对齐 nop（§6.1 豁免）
// 与本函数尾 4.4 对齐 nop 的位置差（两侧各 1 条，数量相等）。
#include "GiveGrowCreatureEvent/SendGrowthCreatureEventInfo.inc"
}

// [b28d-solo-W8LT_EventTail] DIFF→N 封顶 | ORIG 0x081b6e7a | P167 CLEAR（11/11）。
// 修法：①P171 删 `mine == 0` 守卫（ORIG 只判 empty）；②P13 缓存 end() 具名局部；
// ③P28 增量段后置 `it++`（ppEi）；④P167 判据：`it->m_job` → `(*it).m_job`
// （ORIG 发 `__normal_iterator::deEv`；`->` 会误选 `ptEv`，p167 差集 MISSING deEv /
// EXTRA ptEv 一击命中）⇒ 73=73、槽集/块序全等。
// 残余 2 条（N 封顶）：`begin()/end()` 调用点 ORIG this-first（`mov -0xc,%edx` 先）
// vs 4.4.6 恒 sret-first（`lea -0x1c,%eax` 先）——P75 跨版本实参装载时刻；已穷举
// 5 形（初始化式/(*p)./引用接收/逗号声明/直接初始化括号）全部 sret-first。
#include "GiveGrowCreatureEvent/CheckEventInfo.inc"
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081b6f76 | P167 CLEAR（30/30）。
// 修法①（真语义 bug）：ORIG 0x081b6fb9 第二查表实参为 infinityIdx（-0x18），
// 原码误传 creatureItemId ⇒ 已改正。修法②（P0/P183）：删 mine 具名指针局部
// （结果直传 push_back）⇒ 全槽回移 4、135=135 逐条全等。
#include "GiveGrowCreatureEvent/processEvent.inc"
}

#include "GiveGrowCreatureEvent/isGrowCreatureMaxLevel.inc"
}

// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x081b717a | P167 CLEAR（18/18）。
// 修法①（真语义 bug）：ORIG 命中后置 found/foundIdx 并 break、循环后统一发 DB 请求
// （只处理首个命中项），原码在循环体内直接发请求；②比较处显式 `(short)` cast 复现
// ORIG `movzwl+cwtl` 两条；③P158 反向：found 声明 uchar + `==0` 复现 `cmpb $0; je`
// （bool 恒 xor 物化）；④P287 `size() > i` + unsigned i 复现 mem 直比 seta；
// ⑤P223 负例边界：found 守卫改正向包裹消第 3 个尾岛（ORIG 仅 2 岛）⇒ 130=130。
// 残余 24 行 = 6 处 `(*mine)[i]` 的**实参装载序**（N 封顶）：ORIG 4.1 恒
// `mov this,%eax; mov idx,%edx`（this-first，idx 走 edx 中转），4.4.6 恒 idx-first
// （`mov idx,%eax; mov %eax,0x4(%esp); mov this,%eax`）。探针 /tmp/probe_cn（路由同款
// 4.4.6 后端）穷举 8 形：uint/int/size_t/ushort 下标、`(*p)[i]`/`p->operator[](i)`/
// 显式 cast/局部引用 receiver 全部 idx-first ⇒ 源形不可择，记 N（P75 跨版本实参装载
// 时刻，T5：装载序非类型旋钮）。
#include "GiveGrowCreatureEvent/changeCharacName.inc"
}

#include "GiveGrowCreatureEvent/IsEventing.inc"
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x08449b32 | P167 CLEAR（16/16）。
// 修法①：P159 返回类型 bool→void（ORIG 正常路径尾块无 mov $1）；②P259 删
// stream 具名局部（Acquire 直传 guard ctor）。
#include "GiveGrowCreatureEvent/makeRequest.inc"
}

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x08449d1c | P167 CLEAR（16/16）。
// 修法同 DB_EventSave：P159 返回类型 bool→void + P259 删 stream 具名局部。
#include "GiveGrowCreatureEvent/makeRequest_1.inc"
}
