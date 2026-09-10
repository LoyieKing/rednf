// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBossDungeonEntranceLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CBossDungeonEntranceLog（BossTower 入场统计日志）
// 逐函数依据 docs/class_func_reports/CBossDungeonEntranceLog/ 各报告汇编转写；
// ctor/dtor/reset 依据 ORIG nm（W 符号）与 objdump 0x081456c2/0x08145704/0x08145746。
//
// 三个 Increment* 的 key 打包（ORIG -0x40 局部零初始化 → byte0=a →
// get_charac_level→byte1 → getCurCharacGrowType→byte2 →
// (get_charac_job&0xf)|((b&0xf)<<4)→byte3）在各函数内联展开，语句顺序与
// ORIG 汇编一致，不引入 ORIG 不存在的局部辅助函数。
// ============================================================================
#include "CBossDungeonEntranceLog.h"

#include <cstring>

#include "CUser.h"          // CUserCharacInfo get_charac_level/getCurCharacGrowType/get_charac_job
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"

// ORIG 0x081564f0：命中 → ++stageStartCount(+0)；未命中 → memset0 后 insert
// {stageStartCount=1, 其余 0}。
#include "CBossDungeonEntranceLog/IncrementBossStageStart.inc"
}

#include "CBossDungeonEntranceLog/CBossDungeonEntranceLog.inc"
}

#include "CBossDungeonEntranceLog/dtor_CBossDungeonEntranceLog.inc"
}

#include "CBossDungeonEntranceLog/reset.inc"
}

// ORIG 0x0815662e：命中 → ++stageEndCount(+4)、playTimeSum(+0xc) += playTimeSec；
// 未命中 → insert {stageStartCount=1, stageEndCount=1, playTimeSum=playTimeSec}。
#include "CBossDungeonEntranceLog/IncrementBossStageEnd.inc"
}

// ORIG 0x0815678e：命中 → ++pickUpItemCount(+8)；未命中 → insert {pickUpItemCount=1}。
#include "CBossDungeonEntranceLog/IncrementBossPickUpItem.inc"
}

// ORIG 0x081568cc：size>0 时打包 header(0x2e0,-1,size) + 逐项(key + 0x10 字节
// value)，随后 clear 并投递 DB 队列（QUEUE_IDX 4）。
#include "CBossDungeonEntranceLog/BossTowerLogDBUpdate.inc"
}
