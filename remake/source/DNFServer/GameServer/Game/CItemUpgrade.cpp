// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemUpgrade/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CItemUpgrade 实现。
// [2026-09-10 round5 复核（A25TailBatch3）] 本 TU 三件全部维持 N（p167 三件皆 CALLSITE CLEAR）：
//   · _CheckCondition 325/323：仅 2 处索引寻址形差 —— ORIG 08546603/0x08546635
//     `mov %eax,%edx; …; add $0x104,%edx; mov 0xc(%eax,%edx,4)`（两步）vs 4.4
//     `lea 0x104(%eax),%edx`（一条折叠）⇒ P30/N4 结构性伪影（P311 二维指针视角已实测反劣
//     323→329）；另 float 局部槽 -0x74/-0x70（ORIG）vs -0x14/-0x10 + Inven_Item 临时槽
//     ∓8 = N95/N126 帧族（判据⑪ MIXED）。
//     [C05② 复查 2026-09-10] 本件 cmp mem 仅 2 处（discountGroup `cmp -0x28(%ebp),%eax`、
//     predict `cmp -0xc(%ebp),%eax`），objdump 实测**均已是 mem 直比**，与 ORIG
//     `cmp -0x20(%ebp),%eax; setl` / `cmp -0xc(%ebp),%eax; jae` 同形 ⇒ **非**「两个具名
//     局部比较」命中形，未触发 int/unsigned 旋钮。边界留痕（Main 2026-09-10 定稿）：
//     `cmp $imm` 常量比较子型不适用（A01）；`test` 零比较子型可试（A12）；比较对象为
//     形参 / 跨调用返回值 / 该值被 setcc 物化 —— 均不适用（A07）。
//   · ProcUpgrade(MSG_ENCHANT) 437/437：p167 CLEAR 41/41；残余 = 帧 0x1ac/0x19c 与槽号平移
//     （N82/N95）+ 位域读 2 处多 `movzbl %al,%eax`（P168a③）+ cMyTrace 实参寄存器角色
//     `%ebx`↔`%eax`（N7）+ 数据地址伪差（N132）。
//   · _ProcPenalty 359/359：p167 CLEAR 54/54；残余 = 3 处 `lea` 与前邻指令的次序互换
//     （N7/N18a 装载序）+ 字符串地址伪差（N124/N132）。
// [C05 三条细化留痕 2026-09-10] 本 TU 三件均**未命中**：
//   ① C05③ 试前闸门（我方岛数 − ORIG 岛数 ≥ 2 才试合并）—— 三件各早退出口均为共享
//      epilogue、jcc 目标下标逐位一致、无 `jmp+nop` 岛残差 ⇒ 不满足闸门；
//   ② C05① 多层尾 fail 块 —— ORIG 无「两检查共享同一尾块」结构（失败出口一律
//      `mov $imm,%eax; jmp <共享 epilogue>`，与我方同形，故无需双层嵌套）；
//   ③ C07（字段读升格 callee-saved ⇒ 写具名 int 局部）—— 本 TU 无该族残件；其负例
//      边界（消费者为被调方按值实参时写具名局部反而落槽 +2，A14 实证）亦不适用。
//   ④ [C05④ 分诊 2026-09-10，按 Main「四步判定顺序」] `ProcUpgrade` 的两处 OUR-only
//      `movzbl`（我方 `movzbl -0x7a(%ebp),%eax; movzbl %al,%eax` 双段 vs ORIG 单段
//      `movzbl -0xba(%ebp),%eax`）逐条判：
//      ① 无消费者死比较？**否** —— ORIG 该段 `and $0x1,%eax; test %al,%al; je` 有真消费者；
//      ② 槽值/表达式物化 ⇒ 试 P225 原形？**不适用** —— 偏差方向是**我方多一条归一化**，
//        非「ORIG 有 setne 我方缺」（P225 治的是后者，D04 实证）；
//      ③ callee 返回类型与 ORIG 不一致？**否** —— 该 `movzbl` 与任何 call 返回无关，
//        两侧均从 1 字节槽读（ORIG -0xba / 我方 -0x7a 同族），属 4.4 前端 uchar 读冗余
//        归一化（P168a③）；
//      ⇒ 落第 4 档：**N-88 / P197④ 封顶**（勿动控制流、勿动 callee），与 A07 两例同族。
//        补充取证：ProcUpgrade 函数范围内**无**可见的多余 `(unsigned char)` cast
//        （内置 grep 已核），故该冗余归一化源自 4.4 前端对 1 字节槽值的使用点零扩展，
//        非源形可择；C05④ 四步中「body 终止性」（P225 命中条件）对本偏差亦不适用
//        （偏差方向为**我方多归一化**而非缺 setne）。
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CItemAmplifier.h"
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager / CEventBase::GetRepeatEvent
#include "CEnvironment.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "DB_ReqItemUpgradeLog.h"
#include "PvPTypes.h"            // CMTRand（+0x4e8 randInt）
#include "GameTypes.h"           // cMyTrace
#include "DisJoint.h"            // DisJoint::GetResult/getPredictResultItemTypeCount
#include "CCubeStatistic.h"      // CCubeStatistic::collectCubeStatistics
#include "STStackableScript.h"   // STEnchantSystemMapKey/MapData
#include "PacketTypes.h"         // MSG_ENCHANT 完整类型
#include "CReduceUpgradeItemPay.h"   // CReduceUpgradeItemPay（GetReduceRate 权威声明）

#include <cstring>
#include <cstdio>
#include <ctime>     // ctor 的 time(0)（CMTRand 种子）

// 类声明见 CDataManager.h（WongWork::CItemUpgrade）。
namespace
{
// [b28d-solo-CItemUpgrade] ORIG getUpgradeItemRepairCostRate 0x08549170/0x0854918d
// 为 `add $0x9c,%edx; mov 0x4(%eax,%edx,4)`：基址是表指针本身、数组成员位于
// 视图 +0x04（P311 成员数组视图）。表本体 +0x04 落在 vector 内部指针上，无法
// 直接声明成员，故以只读视图类型表达该区布局（仅 reinterpret_cast 视图，
// 不新增符号、不改任何 sizeof/偏移）。
struct UpgradeRateTableView
{
    char m_pad04[4];                  // +0x00..+0x03
    float m_rate[(0x404 - 4) / 4];    // +0x04 起 4 字节步长（同 m_bodyView.m_f32 区）
};

}  // namespace

namespace WongWork
{
// [2026-09-10 判据㉔/P327 跨 TU 定义迁移] ORIG 地址序：C1/C2 @0x085461c8 <
//   D1/D2 @0x085462c6（同一 CItemUpgrade.cpp TU 段），且本 TU `_GLOBAL__I_`
//   键名 = `_ZN8WongWork12CItemUpgradeC2Ev`（0x0854b178，P327 取本 TU 第一个
//   强符号·外链实体=C2 形）。原定义在 CtorProducers_2C.cpp 使本 TU 键名退化为
//   `...D2Ev` ⇒ 定义迁回本文件并置于 dtor 之前。函数体逐字节照搬（0xfe 字节）。
#include "CItemUpgrade/CItemUpgrade.inc"
}

#include "CItemUpgrade/dtor_CItemUpgrade.inc"
}

#include "CItemUpgrade/getUpgradeItemRepairCostRate.inc"
}

#include "CItemUpgrade/make3rdChroniclePacket.inc"
}

// ============================================================================
// enableAmplifyRandomUpgrade @ 0x854af38
//   校验随机增幅券并计算随机强化值写入 int& outCount。
//   返回错误码：0=通过, 4=找不到券/物品或非增幅券, 0xd=upgrade 越界,
//   0x13=物品不可增幅。
// [b28d-solo-CItemUpgrade 判词] DIFF（N 封顶，p167 CLEAR）。残余：
//   `_GetUpgradeInfo(...) != 1` ORIG `xor $0x1,%eax; test %al,%al; je`（3 条）
//   vs 4.4 `cmp $0x1,%eax; setne %al; test %al,%al; je`（4 条）——P292/P168a
//   定论：int callee 的调用点写法恒产 setcc，改返回 bool 则 _GetUpgradeInfo
//   尾部（ORIG 0x08546816 `call get_upgrade_info; add; pop; ret` 直返）多归一化
//   ⇒ N 封顶；另帧/槽号差（4.4 对 characLevel 等具名局部槽化）为 N82 族。
// ============================================================================
#include "CItemUpgrade/enableAmplifyRandomUpgrade.inc"
}

// ============================================================================
// ProcAmplifyRandomUpgrade @ 0x854ac8a
//   处理随机增幅结果：写升级值、统计、日志、广播。
//   返回错误码：0=成功, 4=失败。
// [b28d-solo-CItemUpgrade 判词] AE（P227 全表达式临时链复现 %esi/%ebx 跨 ctor
//   保活；真 bug 3 处：find_item/DB itemId 误用 ticket→item、IsRandomOption 两臂互换）。
//   原 DIFF 186/192：
//   本轮修：trace ctor 移到 characNo/NumberToString 之后（ORIG 0x0854ac93 求值序）、
//   itemIdx/dm 内联、oldUpgrade 改 int 槽。残余 = ORIG 4.1 把 characNo/idStr/
//   characName/characNo2/m_addInfo 保活于 %esi/%ebx/%edi（push edi/esi/ebx），
//   4.4 恒槽化（register 提示实测零效 192=192）⇒ N82/N18a 封顶。
// ============================================================================
#include "CItemUpgrade/ProcAmplifyRandomUpgrade.inc"
}

// ============================================================================
// _GetUpgradeInfo @ 0x854678a
//   按 upgrade_table_t 取 upgrade_info 到 out。
//   返回 get_upgrade_info 结果（0=越界/失败, 1=成功）。
// ============================================================================
#include "CItemUpgrade/_GetUpgradeInfo.inc"
}

// ============================================================================
// _CheckCondition @ 0x8546342
//   校验强化/增幅条件，返回错误码（0=通过）。
// [2026-09-10 判词] DIFF（gap=2、p167 CALLSITE CLEAR 32/32）。残余 = 2 处
//   `m_i32[rarity+0x104]`：ORIG 0x08546603/0x08546635 `add $0x104,%edx;
//   mov 0xc(%eax,%edx,4)` vs 4.4 `lea 0x104(%eax),%edx`（P30 结构性伪影；
//   已实测 P311 二维指针视角反劣 ourN 323→329 ⇒ 回滚）⇒ N4 封顶。
//   本轮修 P0 内联/P222 正向包裹/UPG_TBL_I32 视图/predict 两句式/upType if-else。
//   本轮已修：删 repeatEvent 判空（ORIG 0x0854639f 直接解引用）、内联 reduce
//   （无具名局部）。残余 = 4.4 对具名局部的槽化 vs ORIG 的 %ebx/%esi 寄存器
//   保活（N82/N95 帧差 0xa4 vs 0x84）+ 常量折叠形差 ⇒ N 封顶。
// ============================================================================
// [round3-L29 2026-09-10 复核] 判据⑪ MIXED（OURS-only=[-101,-99,-40,-36] vs
//   ORIG-only=[-116,-112,-93,-91]）⇒ 相对序差；观测 -0x20/-0x10(ORIG) vs -0x28/-0x18(我方)。
//   本轮未及逐对重排（轮次预算）；grep 本注释可避免重复排查方向。
#include "CItemUpgrade/_CheckCondition.inc"
}

// ============================================================================
// _NoticeUpgrade @ 0x8546824
//   广播强化通知包（header 0x56）。
// ============================================================================
#include "CItemUpgrade/_NoticeUpgrade.inc"
}

// ============================================================================
// isDoEnchant @ 0x0854977a（isDoEnchant.md）：randInt(100)（经 this+0x4e8
//   CMTRand）与 buf[(item+0x1f & 1) ? 1 : 0]（char 符号扩展）比较；rand<=bufVal
//   返回 true。
// [b28d-solo-CItemUpgrade 判词] AE（位域视图 m_bits.m_flag 复现
//   `movzbl; and $1; test %al`）。原 DIFF 35/36 残余 1 条：
//   位测试段 ORIG `and $0x1,%eax; test %al,%al` vs 4.4 `movzbl %al,%eax;
//   and $0x1,%eax; test %eax,%eax`（uchar 位与加宽，P168a③ 跨版本，探针
//   /tmp/probe_isd.cpp 五形 f1..f5 全败）⇒ N 封顶。
// ============================================================================
#include "CItemUpgrade/isDoEnchant.inc"
}

// ============================================================================
// ProcUpgrade(MSG_ENCHANT*) @ 0x0854a1ba（ProcUpgrade_0854a1ba.md）
//   三代强化（ENCHANT 消息）主流程。返回错误码；0 成功。
// [2026-09-10 判词] DIFF（gap 4→0；origN=ourN=437；jcc 逐位一致；imm 多重集一致；
//   p167 CLEAR 41/41）。
//   本轮真 bug 修：①**范围检查方向反**——ORIG 0x0854a39c `jl +0x204`/0x0854a3b0
//   `jg +0x204` 均跳「继续」，fall-through 才是 `mov $0x13`，即正极性
//   `if (m_0d >= start && m_0d <= end) return 0x13`（[3,8]=ITEM_TYPE_0 非装备栏
//   槽 ⇒ 报错）；旧写 `m_0d < start || m_0d > end` 方向全反（判据⑧/jcc 逐位）；
//   ②**GetSellItemPrice 第三实参来源错**——ORIG 0x0854a5f0 读 slotA+0x07
//   (m_addInfo2)，旧写 `(short)msg->m_19`（判据㉕）；③删 ORIG 无有的 `& 0xffff`
//   截断（判据⑮）。
//   本轮形修：P198① 预清三处（cond/penalty/doEnchant 两句式）、P225 while 形 ×2、
//   P0 去 equipType/ret 具名槽、P174 返回类型 int→ENUM_ERROR（__PRETTY_FUNCTION__
//   串 0x53 与 ORIG 全等）、P248① ProcUpgrade_3rdChronicle int→bool、P272 块序。
//   残余 = 帧 0x1ac/0x19c 与槽号平移（N82/N95）+ 位域读 2 处多 `movzbl %al,%eax`
//   （P168a③）⇒ N 封顶。
// ============================================================================
// [P174 串定返回类型] ORIG __PRETTY_FUNCTION__ 串 = "ENUM_ERROR WongWork::
//   CItemUpgrade::ProcUpgrade(CUser*, MSG_ENCHANT*, PacketGuard*)" ⇒ 返回
//   ENUM_ERROR；函数体 return 立即数与 ORIG 一致（枚举底层 int）。
#include "CItemUpgrade/ProcUpgrade.inc"
}

// ============================================================================
// _CheckCondition(ENCHANT 5 参) @ 0x085497ea（_CheckCondition_085497ea.md）
//   三代强化条件校验。返回错误码，0=通过。
// [b28d-solo-CItemUpgrade 判词] AE（位域视图复现 `and $1; test %al` 与
//   `shr $3,%al`/`shr %al; and $3`；need 改 unsigned 复现 `cmp mem,%eax; jae`
//   单段装载；`ret != 0` 用 P225 while 形复现 setne;test;je 四段物化）。
// ============================================================================
#include "CItemUpgrade/_CheckCondition_1.inc"
}

// ============================================================================
// ProcUpgrade_3rdChronicle @ 0x0854a006（ProcUpgrade_3rdChronicle.md）
//   三代强化落点：按 key(job/growType/equipType/param) 查 itemScript+0x2f4 附魔表；
//   命中 → item+0x1f 或 item+0x20（按首字节 &1 二选一）写入附魔类型/槽位，返回 1；
//   未命中 → 0。
// [b28d-solo-CItemUpgrade 判词] AE（P285 正向 if/else{return 0} 尾 return 1 +
//   val 作用域收窄 + 位域视图）。原 DIFF 134/133：
//   本轮修：非 const map/迭代器（P96/P270，消 p167 MISSING/EXTRA 各 4）、位域赋值
//   复现三段 RMW、内联 map2f4/stackItem/f 局部。残余 = N93附（return 1 的
//   `mov $1,%ebx` 在 vector 析构之后 vs 4.4 之前）+ %al/%dl 寄存器名 ⇒ N 封顶。
// ============================================================================
// [P248 档①/P292] 调用点 ORIG 0x0854a743 `call; xor $0x1,%eax; test %al,%al; je`
//   ⇒ 返回类型 bool（int 返回恒产 sete 三段）。
#include "CItemUpgrade/ProcUpgrade_3rdChronicle.inc"
}

// ============================================================================
// _ProcPenalty(ENCHANT 5 参) @ 0x08549ac8（_ProcPenalty_08549ac8.md）
//   三代强化惩罚：删材料 → 分解 → 附加物品入包+插入事件物品+统计。返回错误码。
// [2026-09-10 判词] DIFF（origN=ourN=357；jcc 逐位一致；p167 CLEAR 54/54）。
//   本轮真 bug 修：删 ORIG 0x08549d94 之后**无**的 `if (insertRet != 0)` 门禁
//   （判据⑮：put_byte 后直接 begin；旧包裹使 outSlots 在 insertRet==0 时被跳过）
//   + P223 if-join 岛（0x08549e99 `je 08549f7d(nop)` + body 尾 `jmp`）。
//   残余 = 实参装载调度（N7/P75）+ 槽号 ⇒ N 封顶。
//   本轮修**真语义 bug 4 处**：①djr 读索引 [4+i]→[5+i]（ORIG 0x08549c10 权威）；
//   ②make_pair(fi,fi)→make_pair(m_f[i],m_f[i+5]) 两不同字段；③第二次
//   _getAdditionalDisjointItem 漏传 hasAbility 第 5 实参（5 参重载）；
//   ④outSlots 循环整段缺失的 make_item + collectCubeStatistics + GetSellItemPrice
//   + AddValueStatistic(9) 逐物品统计已补。残余 = 帧/槽号与寄存器化 N 类 ⇒ N 封顶。
// ============================================================================
// [round3-L29 2026-09-10 复核] p167 CALLSITE CLEAR（54=54；差集仅 vector D1/D2、
//   DisJointResult C1/C2、Inven_Item C2 同址别名 P93）。判据⑪ PURE SHIFT c=0
//   （槽位直方图两侧逐值一致）⇒ 残余为块序/装载序，非槽与调用缺口。
#include "CItemUpgrade/_ProcPenalty.inc"
}
}  // namespace WongWork
