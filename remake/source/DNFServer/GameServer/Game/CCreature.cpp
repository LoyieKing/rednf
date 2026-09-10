// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCreature/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— user_creature::CCreature 专用实现 TU（TranslateCCreatureTU）。
//
// 承载（类声明权威宿主均为 CGameManager.h 的 user_creature 域）：
//   - user_creature::CStomach      （ORIG 0x8336bd0 域；5 方法，报告
//     docs/class_func_reports/user_creature__CStomach/）
//   - user_creature::COverSkillTime（ORIG checkTime 0x833d65e，报告
//     docs/class_func_reports/user_creature__COverSkillTime/）
//   - user_creature::CCreature     （ORIG 38 方法 + 访问器，报告
//     docs/class_func_reports/user_creature__CCreature/ 各 *.md）
//
// 字段全部具名访问（无裸偏移、无固定返回、无 asm 标签）；签名与 ORIG
// mangled 一致。Save/Load/Die/CheckDied/stomach 链此前由 CCreatureMgr.cpp
// 代管，本 TU 建立后为其唯一权威定义宿主；接入构建时需从 CCreatureMgr.cpp
// 移除同名定义（清单见 /tmp/df_batches/TranslateCCreatureTU.md）。
// ============================================================================
#include <string.h>

#include <map>
#include <algorithm>   // 与 CCreatureMgr.cpp 同前缀，保持共享头解析环境一致

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CCreatureScriptMgr.h"  // GetInstanceCreatureScriptMgr / FindCreatureScript
#include "CPacketRespondent.h"
#include "CCreatureScript.h"     // CCreatureScript 访问器（GetMaxLevel 等）
#include "TimerQueue.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "GameTypes.h"           // cMyTrace / ENUM_ITEMSPACE


namespace user_creature
{

// ORIG _ZN13user_creature8CStomach13IsSaveStomachENS_14CREATURE_STATEE T 0x8336dee
//（IsSaveStomach.md 全汇编转写）：默认可保存；state==1（战斗）时饱食度<=0
// → 不可保存；state∈[2,3] 时饱食度与 .rodata 0x8c2f450（100.0f）相等
//（fucompp+sahf，含 NaN 判定）→ 不可保存；其余 state 恒真。
#include "CCreature/IsSaveStomach.inc"
}

// ORIG _ZN13user_creature8CStomach6ResumeEv T 0x8336e4c：m_lastTime = 当前秒。
#include "CCreature/Resume.inc"
}

// CCreature ctor/dtor/GetOverSkillTime 定义于 CCreatureMgr.cpp（去重保留）。

// ORIG _ZN13user_creature9CCreature3DieEP5CUser T 0x8338536（Die.md）：
// CPacketRespondent(user,0,0).MakeNotipacketDiedCreature() + SendAll()，恒返 1。
#include "CCreature/Die.inc"
}

// ORIG _ZN13user_creature9CCreature17CheckDiedCreatureEP5CUser T 0x83384b8
//（CheckDiedCreature.md）：IsDieCreature 为真时先 Die(user)，返回判定结果。
#include "CCreature/CheckDiedCreature.inc"
}

// ============================================================================
// Save/Load 落库链（TranslateCCreatureMgrTU 批次；逐函数对照
// docs/class_func_reports/user_creature__CCreature/ 的 ## 汇编 + ## 反编译 C）
// ============================================================================

// ORIG _ZN13user_creature9CCreature16SaveCreatureItemEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x83387f6（SaveCreatureItem.md 全汇编转写）。param user 未读（保留形参以匹配
// ORIG 签名）；item 仅被写 idx 字段：*(int*)(item+2) = GetItemId()。
// 注意顺序：先写饱食度/经验/uid，再经 IsSaveStomach 刷新 m_saveFlag(+0x40)。
#include "CCreature/SaveCreatureItem.inc"
}

// ORIG _ZN13user_creature9CCreature16LoadCreatureItemEPNS_12CCreatureMgrEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x83385aa（LoadCreatureItem.md 全汇编转写）：DB 载入侧逆操作。
// +0x27 非 0 → Inven_Item::reset(item) 并返回 0（条目已删除）；成功路径把
// 成长宠/满级标志写回条目 +0x30/+0x31；slotNo==0xee 视作装备中并调 Equip；
// 注册后回填 CREATURE 区槽物 add_info 并按 IsSaveStomach 重算 m_saveFlag。
// [N96 清绑 2026-09-09] 原 `register unsigned char isNotGrow asm("eax")` 全域禁用
// （P0②/N96/P248 尾条），还原 P168a④ 具名 uchar 局部自然形（`v = 字段; v ^= 1; if (v)`）。
// 4.4 对该两语句形发 `movzbl; mov %al,-N; xorb $1,-N; cmpb $0,-N; je`（栈槽物化），
// ORIG 为 `movzbl 0x4c(%eax),%eax; xor $1,%eax; test %al,%al; je`（寄存器物化）⇒
// 自然形不可达，残差 = N96+N18a 封顶（P168a④ 补充，四形实测同）。
// 读数：清绑前 our_insns=178（TU 口径 DIFF）；清绑后 our_insns=179 / orig=177
//（TU 口径 DIFF）。
#include "CCreature/LoadCreatureItem.inc"
}

// ============================================================================
// 以下为待头声明落地后的追加实现（TranslateCCreatureTU；已按各 .md 转写完毕）
// ============================================================================

// ORIG _ZN13user_creature9CCreature14RenameCreatureEPKc T 0x83373aa：
// strncpy(m_name, name, 0xc) 后恒返 1。
#include "CCreature/RenameCreature.inc"
}

// ORIG _ZN13user_creature9CCreature19IsEvolutionCreatureEi T 0x83388ec。
// [N96 清绑 2026-09-09] 原 `register bool eq asm("al")` 全域禁用（P0②/N96/P248 尾条），
// 还原 P168a② 具名 bool 局部自然形（`bool eq = (call()==arg); if (eq) return 1;`）。
// 4.4 发 `sete %al; mov %al,-N; cmpb $0,-N; je`（栈槽物化），ORIG 为
// `sete %al; test %al,%al; je`（寄存器物化）⇒ 自然形不可达，残差 = N96+N18a 封顶。
// 读数：清绑前 our_insns=20（TU 口径 AE）；清绑后 our_insns=22 / orig=20（TU 口径 DIFF）。
#include "CCreature/IsEvolutionCreature.inc"
}

// ORIG _ZN13user_creature9CCreature24IsEvolutionCreatureMultyEi T 0x8338922。
#include "CCreature/IsEvolutionCreatureMulty.inc"
}

// ORIG _ZN13user_creature9CCreature24IsEventEvolutionCreatureEi T 0x8338bfa。
#include "CCreature/IsEventEvolutionCreature.inc"
}

// ORIG _ZN13user_creature9CCreature22GetLearnOverSkillLevelEv T 0x8338c78。
#include "CCreature/GetLearnOverSkillLevel.inc"
}

// ORIG _ZN13user_creature9CCreature24GetOverSkillRecoveryTimeEv T 0x8338c50。
#include "CCreature/GetOverSkillRecoveryTime.inc"
}

// ORIG _ZN13user_creature9CCreature20GetSkillRecoveryTimeEv T 0x8338c28。
#include "CCreature/GetSkillRecoveryTime.inc"
}

// ORIG _ZN13user_creature9CCreature12EvoluteMultyEP5CUseri T 0x8338950
//（EvoluteMulty.md 全转写）：进化目标由调用方给定 creatureId（isEvoluationCreature 校验）。
#include "CCreature/EvoluteMulty.inc"
}

// ORIG _ZN13user_creature9CCreature11ItemEvoluteEP5CUseri T 0x8338ca0
//（ItemEvolute.md 全转写）：目标取 GetItemEvolutionCreatureId(itemIdx)（0x833cad8）。
// 返回 int：ORIG 尾 `mov %ebx,%eax` 直通 + caller CCreatureMgr::ItemEvolute
// 0x833d6c2 call 后无 movzbl 归一（P159/P9 bool→int 判据）。
#include "CCreature/ItemEvolute.inc"
}

// ORIG _ZN13user_creature9CCreature12EventEvoluteEP5CUserii T 0x833db2e
//（EventEvolute.md 全转写）：活动进化。成长门（xor 链实证）：仅 next 脚本
// IsItemGrowCreature()==0 且 自身 m_isGrow!=1（即成长宠）进入经验折算；
// newExp = get_creature_level_exp((short)(oldExp>>16)/step + 1)，step 取
// GetGrowItemCountforLevelUp()（0→0x19）。包通知携带 param_3(slot)。
#include "CCreature/EventEvolute.inc"
}

}  // namespace user_creature
// ORIG _ZN13user_creature12CDBMsgSender20EventEvoluteCreatureEiii W 0x833fddc
//（objdump 逐条转写）：stream 空→0；buf memset 0x74（ORIG 无判空，如实保留）；
// m_hdr|=0x248；compat.m_itemIdx(+0x08)=itemIdx；m_addInfo2(+0x30)=uid；
// exp 写 +0x4c；find_item(itemIdx)（ORIG 不判空）→ m_days(+0x54)=
// getUsablePeriod()、m_expireTick(+0x58)=getExpirationDate()；返 1。

#include "CCreature/EventEvoluteCreature.inc"
}
 
 // ORIG _ZN13user_creature12CDBMsgSender4InitE19ENUM_INTERNALPACKETP5CUser W 0x833f7a8

// ORIG _ZN13user_creature12CDBMsgSender4InitE19ENUM_INTERNALPACKETP5CUser W 0x833f7a8
//（objdump 逐条转写）：type==0x89 走 BigStreamPool::Acquire("Creature.cpp",0xa7)，
// 其余走 StreamPool::Acquire("Creature.cpp",0xa9)；stream 空→false；
// m_user=user；CStreamGuard(m_stream,false) 后经 operator*()（de 0x80c8c46）链式
// 依次 << (int)type、<< (int)GetUID()；return true 落在 guard 作用域内
//（ORIG：mov $1,%ebx 先物化 → guard D1 → mov %ebx,%eax 共享出口）。
// 池记录名取 ORIG .rodata 0x8c2d76a 字面量 "Creature.cpp"（AE 口径归 <A>，语义等价）。
#include "CCreature/Init.inc"
}
