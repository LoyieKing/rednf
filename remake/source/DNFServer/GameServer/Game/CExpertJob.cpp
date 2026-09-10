// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CExpertJob/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ expert_job 核心类型实现（地址证据见 CExpertJob.h 头注释）。
#include "CExpertJob.h"

#include "CDataManager.h"
#include "CExpertJobList.h"
#include "CInventory.h"
#include "CItem.h"
#include "CGuildServerProxy.h"   // WongWork::CGMAccounts::isGM
#include "CStreamGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include <cstring>

#include "GameRand.h"
#include "SigTypes.h"
#include "SkillSlot.h"

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN10expert_job16CCharacExpertJob6_resetEv`（P327）。全限定定义可写在
// 命名空间块之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// _reset 0x0832b064：清 +0x06(giveup)/+0x08(endurance)/+0x0c(enchant) 与 30 字节 recipe flag。
#include "CExpertJob/_reset.inc"
}

namespace collision_detection
{

#include "CExpertJob/Rectangle.inc"
#include "CExpertJob/include.inc"
}

}  // namespace collision_detection

namespace expert_job
{

#include "CExpertJob/ExpertJobMaterial.inc"
#include "CExpertJob/Reset.inc"
}

// ORIG 0x849f2b4：仅置 vptr。
#include "CExpertJob/CExpertJob.inc"

#include "CExpertJob/dtor_CExpertJob.inc"
#include "CExpertJob/GetType.inc"
#include "CExpertJob/SetType.inc"
#include "CExpertJob/CDisjointMachine.inc"
}

#include "CExpertJob/dtor_CDisjointMachine.inc"
#include "CExpertJob/reset.inc"
}

// ORIG 0x85d1cc2：m_user 为空时走 cMyTrace 断言并返回 0；
// 否则 put_byte(0) + put_disjoint_machine_info 后返回 1。
#include "CExpertJob/put_object_info.inc"
}

// ORIG 0x85d1bc0：short(unique_id 低16位)、int(name.length)、str(name)、
// byte(Vill)、byte(get_area(false))、short(+0x24)、short(+0x28)、int(+0x1c)。
#include "CExpertJob/put_disjoint_machine_info.inc"
}

// ORIG 0x85d1d3c：基类 CExpertJob -> CDisjointMachine@+0x8。
#include "CExpertJob/CDisjointer.inc"

#include "CExpertJob/dtor_CDisjointer.inc"
#include "CExpertJob/CEnchanter.inc"

#include "CExpertJob/dtor_CEnchanter.inc"
#include "CExpertJob/GetSuccessRate.inc"
}

// IsSuccess @ 0x849b462：base 成功率 = rate->m_successRate + variation；
// 随机 100 < base 则成功：resultExp ∈ [expMin, expMax]，再加
// GetExpertJobAddtionalExp 的额外经验（概率 m_field0，量 m_field4）。
// [b28d-solo-CExpertJob] 书写序旋钮：`base > get_rand_int(100)` 复现 ORIG
//   0x0849b49c 的 %ebx 免槽跨 call 保活（lea/cmp/setg 段逐条全等）。
// [SweepCExpertJob 2026-09-09] 剩余 1 条 = x87 比较 lowering（4.1
//   `fucompp; fnstsw %ax; test $0x45,%ah; sete` vs 4.4.7 默认
//   `fucomip %st(1),%st; fstp %st(0); seta`），非源码可择；`-march=i486
//   -mtune=generic` 下 4.4.7 直出 ORIG 形 ⇒ AE（93=93 尺寸全等）。
//   p167 CLEAR。ORIG 0x0849b462
#include "CExpertJob/IsSuccess.inc"
}

// IsLearnRecipe @ 0x849b590：GetRecipeInfoIndex(itemIndex) 找到 recipe 且
// 已学习（get_recipe）或 GM → true；否则对应 cMyTrace + false。
// [b28d-62] AE：P227 trace 全表达式链 + P0/P134 expand 免槽内联 + P266 字节
//   收窄 + P198① 预清 + get_recipe bool 化；105=105，p167 CLEAR。ORIG 0x0849b590
#include "CExpertJob/IsLearnRecipe.inc"
}

// IncreaseExpertJobExp @ 0x849ae40：clamp exp 至 m_expVec 末项上限；跨级/边界
// 处理 isBoundaryExpValue + isExpertJobLimitLevel；SetCurCharacExpertJobExp +
// make_basic_info 包；非边界本地 Send，边界则 LogHistory + send_all + 升级技能
// m_growSkillVec 循环 + send_skill_info + quest 包。
// [b28d-solo-CExpertJob] N 封顶（语义完成、p167 CLEAR 60/60）：本件 341 条。
//   已复现：P0 去 lvlMap/gw/sk/lv2 具名局部（ORIG 0x0849af10/0x0849b127/0x0849b14e
//   均寄存器免槽）、P225 推广嵌套 if 复现 setle 物化（0x0849af76 逐条全等）、
//   P226 扩展语句表达式复现 b 短路物化岛零槽（0x0849af90 段）、P196 比较方向
//   `exp > prevExp`（0x0849aff8 jle 段全等）、块序 `if (isBoundary){长}else{Send}`
//   复现 0x0849b0c6 `cmpb;je 尾部` 布局。剩余差异（结构性、语义等价）：
//   ① 开头 getCurCharacR()==0 的 4.1 取反物化 `setne;je` vs 4.4 `sete;jne`
//      （setcc-fold 跨版本极性，两形实测均折）；② 尾部 4.4 epilogue-threading
//      `jmp;nop;jmp;nop` 岛（N121 三形不可消）。ORIG 0x0849ae40
#include "CExpertJob/IncreaseExpertJobExp.inc"
}

// ============================================================================
// CCharacExpertJob（ORIG，见 expert_job__CCharacExpertJob/*.md）
// ============================================================================

// ctor 0x0832af52：仅基类 CData 构造（vptr），成员未初始化（loadData 填充）。
#include "CExpertJob/CCharacExpertJob.inc"
}

#include "CExpertJob/dtor_CCharacExpertJob.inc"
}

// InitScript 0x0832b36c：m_endurance = script->m_enduranceInit（+0x274，
// [endurance initial value]）；m_enchantCount = 0；清 30 字节 recipe flag；
// alter()。
#include "CExpertJob/InitScript.inc"
}

// get_recipe 0x0832b306：idx = index/8（带符号除），越界(<0 || >0x1d)返回 0；
// 返回 bitmap 第 (index%8) 位。
// [b28d-62] 返回类型改 bool 后调用点 xor $1 形归零。ORIG 0x0832b306
// [b28d-solo-CExpertJob] N97 定位：残差仅 idx 段（ORIG 4.1 `mov %eax,%edx;
//   sar/shr; lea (%edx,%eax,1); sar $3` 6 条 vs 默认旗标 4.4.7 `sar/shr %eax;
//   add 0xc(%ebp),%eax` 5 条）。
// [SweepCExpertJob 2026-09-09] 真因=旗标而非源码形：`-march=i486 -mtune=generic`
//   下 4.4.7 对**自然形** `index/8` 直出 ORIG 逐条 6 条形（函数尺寸 0x65=ORIG，
//   逐条相同 ⇒ AE）。四组实测（源码形×旗标两变量，逐函数 verdict 全表比对）：
//     手写 sign 形 + 默认旗标 = 本注释历史形（DIFF）；
//     `index/8` + 默认旗标 = 与前者逐函数读数完全相同（零回退，TU 36/45/0/5）；
//     手写 sign 形 + i486 = 仍 DIFF（4.4 覆盖 %eax 回读形不变）；
//     `index/8` + i486 = **AE**（TU 36/45/0/5 → 37/47/0/2，无任一函数劣化）。
//   ⇒ 源码保持自然形 `index/8`，旗标由 Main 路由表落（同 CUserCharacInfo/
//   PvP_Room 的 i486 先例）。p167 CLEAR。
#include "CExpertJob/get_recipe.inc"
}

// set_recipe 0x0832b264：未置位则置位并返回 1；已置位返回 0。
// [b28d-solo-CExpertJob] N97 定位：残差仅 idx 段 5 vs 6 条（同 get_recipe），
//   其余 61 条全等（含 P222 负极早返 set 岛形）。ORIG 0x0832b264
// [SweepCExpertJob 2026-09-09] 同 get_recipe：真因=旗标。`-march=i486
//   -mtune=generic` 下 `index/8` 直出 ORIG 逐条 6 条形 ⇒ **IDENTICAL**
//   （函数尺寸 0xa2=ORIG，全 62 条逐条相同）；默认旗标下 `index/8` 与手写
//   sign 形读数逐函数相同（零回退）。旗标由 Main 路由表落。p167 CLEAR。
#include "CExpertJob/set_recipe.inc"
}


// learnRecipe 0x0832b3c0：GetRecipeInfoIndex(recipeIndex)；index<0 → 0；
// GetRecipeInfo(index) 为空 → 0；recipeInfo->m_field8 <= param+2 才 set_recipe + alter。
// b28c-1 N: 符号位测试 4.4 恒折 cmpl/jns vs ORIG mov/shr/test（跨版本 lowering）。
// [b28d-62] AE：P266 字节收窄符号测试（P198① 预清）后 46=46，p167 CLEAR。
//   ORIG 0x0832b3c0
#include "CExpertJob/learnRecipe.inc"
}

// loadData 0x0832b1fe：pData 即 DB 侧 SIG_EXPERT_JOB 载荷（wire 布局见
// SigTypes.h），按具名字段拷入。
#include "CExpertJob/loadData.inc"
}

// _saveData 0x0832b0ac：StreamGuard 写 0x181 头 + UID + SIG_EXPERT_JOB 载荷
// （DB wire 格式，结构定义见 SigTypes.h），MsgQueueMgr::put(DB_Q)。
// [b28d-solo-CExpertJob] AE（92=92，DIFF→AE）：P211 档③/P235 匿名 union 双视角
//   （SigTypes_B.h SIG_EXPERT_JOB::m_characNoU，与 getCurCharacNo() 同型）复现
//   ORIG 0x0832b163 `mov -0xc(%ebp),%edx; mov %eax,(%edx)` LHS-first 直存；
//   消费点 W8LT_DBTask.cpp 读 int 视角零回归（属主 S340 fresh 复验确认）。ORIG 0x0832b0ac
#include "CExpertJob/_saveData.inc"
}

// ORIG 0x849f5ac：先 CData::alter() 再写 +0x06。
#include "CExpertJob/set_giveup_count.inc"
}

#include "CExpertJob/get_giveup_count.inc"

}  // namespace expert_job
