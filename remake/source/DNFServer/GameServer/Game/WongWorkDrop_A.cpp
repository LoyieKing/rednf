// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkDrop_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// WongWorkDrop_A.cpp — G1 Drop 家族（WongWorkA 批次）逐报告忠实转写
//
// 实现 16 个 ORIG 符号（唯一 owner = 本 TU）：
//   CWorldDrop::generateRandomItem              T 0x085383b8
//   CDungeonDrop::generateSpecificItem          T 0x08539c3e
//   CObjectDrop::generateRandomItem             T 0x0853818a
//   CObjectDrop::generateSpecificItem           T 0x08538234
//   CMonsterDrop::setDropRatio                  T 0x08535cf8
//   CMonsterDrop::_getBossGoldDrop              T 0x08535dc8
//   CMonsterDrop::_getGenItemProbIdx            T 0x085372ee
//   CMonsterDrop::_generateRandomItem           T 0x08535ed2
//   CMonsterDrop::_generateMonseterSpecificItem T 0x08536fce（ORIG 拼写如此）
//   CMonsterDrop::generateRandomItem            T 0x08536da8
//   CMonsterDrop::generateSpecificItem          T 0x08537070
//   stFirstMobHuntingWeight_t::stFirstMobHuntingWeight_t W 0x0853a6ae
//
// 跨 TU 依赖（P6 清桥后全部为真实成员/函数调用，本 TU 无 __asm__ 桥）：
//   CMonsterDrop_Hell::{setDropRatio,generateSpecificItem,hellPartySpecificItem}
//   CSpecialMonsterDrop::{setDropRatio,_generateRandomItem}
//   CAutoMarketConditionsControl::{GetItemDropRate,ChangeItemRarity}
//   ::CLuckPoint::{UseLuckPoint(成员4int),sendItemDropRate}
//   GenerateRandomUpgrade::{generateEquipGrade,generateUpgrade}
//   CMonster::{drop_item_index(ji),generateDropAvatarItem,generateCatchMonsterItem}
//   CAICharacter::generateDropAvatarItem
//   ::roundFloat 0x0853abe5（定义于 CClearReward.cpp）
//
// stGenerateRefData_t +0x34..+0x47 为调用方上下文 blob（头注释实证）；本文件
// 按字节偏移读取并注明语义【推断】，不改任何共享布局。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include <algorithm>

#include "CItemGenerator.h"
#include "Inven_Item.h"
#include "GameRand.h"
#include "CDataManager.h"
#include "CMonster.h"
#include "STMonsterScript.h"
#include "MonsterIndependentDrop.h"
#include "CDungeon.h"

namespace WongWork
{


}  // namespace WongWork

// ORIG W 0x0853abe5 ::roundFloat —— 定义于 CClearReward.cpp（drops 域公共依赖，
// 全局命名空间符号 _Z10roundFloatf；声明须置于全局域保证调用 mangled 匹配）
extern int roundFloat(float v);

namespace WongWork
{

// ---------------------------------------------------------------------------
// 文件局部 asm 桥（只引用不定义）
// ---------------------------------------------------------------------------
namespace {

// b28d-solo-WongWorkDrop_A：CWorldDrop ORIG 具名指针局部 ctx = &ref+0x34
// 的字节视图（ctx[0]=type/ctx[1]=level）。结构体指针视图使 4.4 出
// `movzbl 0x1(%eax)` 单条 disp（裸 ctx[1] 恒产 add $1 + disp 0，P30/N4）。
struct StRefCtxView
{
    unsigned char m_type;    // +0x34
    unsigned char m_level;   // +0x35
};

// CMonsterDrop::generateSpecificItem 低难度块 ORIG 具名指针 ctx = &ref+0x34
//（-0x20 槽）：ctx[4]=+0x38 int / ctx[8]=+0x3c / ctx[9]=+0x3d。
struct StRefCtx2
{
    unsigned char m_pad[4];      // +0x34..+0x37
    int m_rankBonusIndex;        // +0x38
    unsigned char m_difficulty;  // +0x3c
    unsigned char m_count;       // +0x3d
    unsigned char m_field3e;     // +0x3e
    unsigned char m_field3f;     // +0x3f
};

}  // namespace

// ===========================================================================
// CWorldDrop::generateRandomItem — ORIG T 0x085383b8
// b21 同族：+0x34/+0x35 改成员直读（m_field34/m_field35，语义同 u8_at）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=DIFF(N121)：p167 CLEAR；残余仅 4.4
// epilogue-threading 尾岛 2×`jmp+nop`（ORIG 无岛），break/goto 尾标签/return
// 三形实测不消 ⇒ N121 封顶。已修：ctx 具名指针局部（-0x20）、prod/100 去负数
// 特判、pick 复用 roll 槽、push 内联、int i + size() 有符号比较、slot 预清 0。
// [round4 L3CUser 复核 2026-09-10] ae_insn 残差仅 2 处：①`movl $&_ZL14gUnicodeBuffer
//   +0xe174,0x8(%esp)` 我方为 .o 未解析重定位（AE/N132 等价）；②尾 2×`jmp+nop`
//   为我方多出（ORIG 无岛）⇒ N121 维持，指令数 123/127 与判词一致。
// [A28 round5 复核 2026-09-10] 重跑 clean 口径：orig=123 / ours=125，残差仍仅尾部
//   2×`jmp`（ORIG `jne <T>; leave; ret` 直落 ⇒ 无岛）⇒ N121（4.4 epilogue-threading
//   尾岛不可消）维持封顶；p167 调用点 CLEAR。
// [A28 round5 · C05③ P223 实测 2026-09-10 19:42] 按 Main 转发的「单尾标签串消早返
//   中继岛」范式，把三处早返改为 `goto end;` + 尾 `end:;`（跨 goto 的 slot/threshold/acc
//   一律声明与赋值分两句）：**实测无效**，orig=123/ours=125 与残差 2×`jmp` 逐字不变
//   （CWorldDrop 的岛来自 4.4 自身 epilogue-threading，非早返平铺产物）⇒ 已回退为
//   `return;` 形，N121 维持。C05①② 对本 TU 其余件亦无命中（残差无 `cmp mem` 类、
//   无「失败出口落函数尾」布局差）。
#include "WongWorkDrop_A/generateRandomItem.inc"
}

// ===========================================================================
// CObjectDrop::generateRandomItem — ORIG T 0x0853818a
// b21 同族：u8_at/i8_at → m_field34/m_field35/m_rateSlot/m_pad37 成员直读（语义不变）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=AE：去 type 具名局部（P0，ORIG 两次独立
// `movzbl 0x34` 直读；帧 -4、done/n 槽 -0x21/-0x20 → -0x1d/-0x1c 全等）。
#include "WongWorkDrop_A/generateRandomItem_1.inc"
}

// ===========================================================================
// CObjectDrop::generateSpecificItem — ORIG T 0x08538234
// b21 同族：i8_at(0x39) → static_cast<signed char>(m_field39)（语义不变）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=AE：去 props 指针缓存（P183，ORIG 每次
// begin/end 重读 ref+0x34）+ pushCreatedItem 内联 + m_field34l 具名 list 视图。
#include "WongWorkDrop_A/generateSpecificItem.inc"
}

// ===========================================================================
// CDungeonDrop::generateSpecificItem — ORIG T 0x08539c3e
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=AE：去 props 缓存（P183）+ acc 先声明
// （-0x10 深）/pick 后（-0xc）+ pushCreatedItem 内联 + m_field34v 具名 vector
// 视图（复现 ORIG 单条 `mov 0x34(%eax),%edx`）。
#include "WongWorkDrop_A/generateSpecificItem_1.inc"
}

// ===========================================================================
// CMonsterDrop
// ===========================================================================

// ---- _getGenItemProbIdx — ORIG T 0x085372ee ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：循环内 return 改「命中 break +
// 循环外 size()<=i 复核」（消 4.4 mov$1/mov$0 bool 岛）；元素具名引用 e；
// unsigned i 使 size() 比较恒内存直比（P225/P194 族）。
// [b28d-solo-WongWorkDrop_A] 修法：循环内 return i 改为「命中 break + 循环外
// size()<=i 复核」两段（消 4.4 的 mov$1/mov$0 bool 岛，复现 ORIG
// setbe;test;je 尾块）；元素经具名引用 e 单次 operator[]（ORIG -0xc 槽往返）；
// i 用 unsigned int 使 size() 比较恒内存直比（P225/P194 族）。
#include "WongWorkDrop_A/_getGenItemProbIdx.inc"
}

// ---- _getBossGoldDrop — ORIG T 0x08535dc8 ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：真语义 bug 修（首参 ORIG 为
// m_conditionRate[type].value + .rate，旧码误取 m_goldQuantity[2*type]）；
// m_conditionRateW[4][2] 二维视图复现 disp32 折叠、m_goldQuantityS[2] 结构体
// 数组视图复现 `add $K` + scaled4 折叠；正向 if(type<4) 包裹（P222）。
// [b28d-solo-WongWorkDrop_A] 真语义 bug 修正：ORIG 首参用
// m_conditionRate[type].value + m_conditionRate[type].rate（0x25938a/0x259388
// 两半字），旧码误取 m_goldQuantity[2*type]；且 ORIG 无 probSlot/baseSlot/
// goldLo/goldHi 具名局部（P0 寄存器直传），tier 是唯一局部（-0xc）。
// 分支形：`ja <尾部 mov $0>` 失败块在函数尾 ⇒ 正向 if(type<4){…return true}
// return false（P222 正向包裹）。
#include "WongWorkDrop_A/_getBossGoldDrop.inc"
}

// ---- setDropRatio — ORIG T 0x08535cf8 ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：外层单次 operator[] 取具名元素引用 e
// （P157 例外）+ e.w[r]（+0x08 起五档视图）复现 ORIG `mov 0x8(%eax,%edx,4)`。
#include "WongWorkDrop_A/setDropRatio.inc"
}

// ---- _generateRandomItem — ORIG T 0x08535ed2 ----
// [b28d-solo-WongWorkDrop_A] verdict=DIFF(N82/N78 封顶)：p167 CLEAR 50=50
//（0 missing/0 extra）；gap=19 全为跨版本 codegen——N78（ORIG altTable 清零
// rep stos 对齐剥离慢路 vs 4.4 简单形）、N82（帧 0x24c vs 0x25c：ORIG buf
// 与 goldItem 同址复用，全帧槽号 +0x10 平移）、N85（x87 fdivs/fucompp-fnsahf
// vs fdivp/fucomip）。真语义 bug 修 6 处（见下）。
// [round4 L3CUser 复核 2026-09-10] 帧 0x24c vs 我方 0x25c（+0x10）、槽号
//   -0x21c/-0x1fc/-0x204/-0x208 vs -0x22c/-0x20c/-0x214/-0x218 恒定平移 ⇒
//   判据⑪ B-FRAME（整帧平移）⇒ N126/N82，声明重排无效；gap -19 为此族固有。
// [A28 round5 复核 2026-09-10] 重跑 p167：CLEAR 50=50（仅 Inven_Item C1↔C2 同址
//   别名，P93 良性）；orig 665 / ours 646（raw 670/652）；帧 0x24c vs 0x25c。
//   ⇒ 维持 N78（altTable `rep stos` 对齐剥离慢路）+ N82/N126（整帧 +0x10）封顶。
// 参数（自 wrapper）【推断命名】：
//   type       = ref+0x34 怪物等级档
//   role       = ref+0x35（signed char；==3 走首领金币路径）
//   dungeonLv  = ref+0x36 地下城等级档
//   difficulty = ref+0x37 难度档
//   partyPos   = ref+0x38 组队档位（-1 后作加成列下标）
//   context3c  = *(int*)&ref[0x3c]（人数上下文，clip [0,100]）
//   dropRate   = ref.m_dropRate（×value/100）
#include "WongWorkDrop_A/_generateRandomItem.inc"
}

// ---- _generateMonseterSpecificItem — ORIG T 0x08536fce ----
#include "WongWorkDrop_A/_generateMonseterSpecificItem.inc"
}

// ---- generateRandomItem — ORIG T 0x08536da8（虚槽 +0x08）----
// b21 verdict=DIFF（N 残余）：158/158 指令数全等；u8_at/i8_at helper 在 O0 产 EXTRA call
//   （每个读调用 ~+6 insn，共 ~45），改用 stGenerateRefData_t 具名字节成员直读（m_field34..
//   m_field41，CItemGenerator.h union 字节视图）后消除；memcpy(&m_firstMobWeight,...) 小常数
//   复制产 call → m_firstMobHunt.m_weight/m_field04 直赋。残余仅 1) `cmp $0x3; jg` vs
//   `ja`（movzbl 已知非负，GCC 选无符号跳转，语义全等）2) 跳转/调用目标地址。语义完成。
#include "WongWorkDrop_A/generateRandomItem_2.inc"
}

// ---- generateSpecificItem — ORIG T 0x08537070（虚槽 +0x0c）----
// [b28d-solo-WongWorkDrop_A] verdict=AE：ORIG 布局重写（difficulty>4 高难度块
// 直落、jle 跳后部低难度块；旧早返形相反）+ 低难度块具名 ctx 指针（-0x20，
// +0x38 find_monster / +0x3d 循环上界 / +0x3c 末次判定）+ m_field43/
// m_field44_byte bool 化（P248 档② xor $1 形）。真语义 bug：find_monster
// 旧码取 ref.m_field35，ORIG 取 +0x38（m_rankBonusIndex）。
// b21 同族：u8_at/i8_at 改成员直读；+0x43/+0x44 改为 ORIG 语义 (byte!=1)&&(byte!=1)（原 ==0 误）。
//   残余：字节比较 codegen（ORIG xor$0x1 vs cmpb，N-86b 全同）+ AE。
#include "WongWorkDrop_A/generateSpecificItem_2.inc"
}

// ---- CMonsterDrop::stFirstMobHuntingWeight_t ctor — ORIG W 0x0853a6ae ----
#include "WongWorkDrop_A/stFirstMobHuntingWeight_t.inc"
}

}  // namespace WongWork
