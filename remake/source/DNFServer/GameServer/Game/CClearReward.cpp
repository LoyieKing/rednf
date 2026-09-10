// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CClearReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CClearReward.cpp — WongWork::CClearReward 清关奖励生成器实现
// （ORIG 报告 docs/class_func_reports/WongWork__CClearReward/ 13 方法逐段转写）
//
// 覆盖符号（唯一定义点；ctor/loadScript 归 DropLayoutDeps.cpp）：
//   setDropRatio            T 0x085385c8
//   getGoldCardCommission   T 0x085385b2
//   getGoldCardCreateRate   W 0x085bf000
//   getRewardItemRatePerMapMaxCount W 0x085bf018
//   _getGenItemProbIdx      T 0x08538694
//   generateRandomItem      T 0x0853873a（虚槽 +0x08）
//   _generateRandomItem     T 0x085387c0
//   _generateSpecificItem   T 0x08538bf8
//   generateSpecificItem    T 0x08538c84（虚槽 +0x0c）
//   generatePremiumGoldCardItem T 0x08538a92
//   generateGoldCardBlankItem   T 0x08539166
//   generatePcRoomCardBlankItem T 0x0853900c
//   power_war_dungeon_gold_clear_reward T 0x08538e64
// 依赖符号（ORIG T 无所属 TU；一并落于此避免双定义）：
//   WongWork::CItemGenRateTable::chooseItem     T 0x085349a4
//   WongWork::CItemGeneratorMgr::onTryGenerateRandomItem T 0x085342c6
//   ::CLuckPoint::GetItemRarity                 T 0x08550be4
//   ::CLuckPoint::GetLuckPoint                  T 0x085509da
//   ::roundFloat                                W 0x0853abe5
// ============================================================================
#include "CItemGenerator.h"
#include "CItemGeneratorMgr.h"
#include "GameRand.h"

#include "CDataManager.h"
#include "CDungeon.h"
#include "ServerParameterScript.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "GameMasterCmd.h"   // CGameMasterCharacter::sendServerState（sendItemDropRate 虚发）
#include <math.h>

// 全局 ::CLuckPoint ctor —— ORIG T 0x08550884 含 m_users.clear()。
#include "CClearReward/CLuckPoint.inc"
}

namespace WongWork
{
// ============================================================================
// stGenerateRefClearReward_t —— generateRandomItem 调用方写入外层 ref+0x34 起
// 的清关奖励上下文视图。ORIG mangled _ZN8WongWork26stGenerateRefClearReward_tE；
// 成员偏移全部来自 _generateRandomItem/_generateSpecificItem 反汇编访问：
//   +0x00 u8 命中基准值 / +0x01 u8 等级档【推断命名，布局实证】
//   +0x04 float 加成率 / +0x08 int dropProb 组 / +0x0c CDungeon*
// sizeof == 0x10（m_dungeon 止，ORIG 访问不超过 +0xf 实证）
// ============================================================================
struct stGenerateRefClearReward_t
{
    unsigned char m_value;      // +0x00
    unsigned char m_level;      // +0x01
    char m_pad02[2];            // +0x02..03
    float m_bonusRate;          // +0x04
    // +0x08：ORIG 0x085387d8 `mov 0x8(%eax),%edx` 单段直落（与
    // _getGenItemProbIdx 第二形参同型）⇒ 字段即 eClearRewardDropProb_t（P254）。
    eClearRewardDropProb_t m_dropProb;
    CDungeon* m_dungeon;        // +0x0c
};

// §4.3 裸偏移清零：调用方上下文区在 stGenerateRefData_t 内的起点由**具名字段**
// m_field34（CItemGenerator.h:210）经 offsetof 推导，不再是字面量 0x34；
// 视图尺寸 0x10 由下方断言锁定（本文件上方布局注释：ORIG 访问不超过 +0xf）。
// 取「常量相加」而非「直取字段地址」：ORIG 该处是基址 + 立即数两步物化，
// `&ref.m_field34` 会被 4.4 折成单条 lea，实测 generateRandomItem ndiff 2→14
// 倒退，故保留相加形（常量仍由具名字段推导，无裸偏移字面量）。
static const int kRefClearRewardCtx =
    static_cast<int>(offsetof(stGenerateRefData_t, m_field34));
static_assert(sizeof(stGenerateRefClearReward_t) == 0x10,
              "clear-reward ctx size drift");

// ORIG 0x08538ad8 段 `call G_CDataManager; mov -0xc(%ebp),%edx; add $0x1264,%edx;
// mov 0x14(%eax,%edx,4),%eax` 实证：六档稀有度概率表以 CDataManager +0x14 为基址、
// 索引 i+0x1264（scaled ×4）。以本域具名视图表达（§4.3：禁裸偏移字面量）；
// 基址字节偏移 0x14 按元素折算进索引（0x14/4=5）——4.4 会把成员 disp 强度归约成
// 额外 add，折进索引后与 ORIG 同为「add 常量 + scaled disp」两条（探针 p6 f1/f2）。
static const int kRarityProbIdx = 0x1264 + 0x14 / 4;   // = 0x1269
struct CDataManagerRarityProbView
{
    int m_probs[kRarityProbIdx + 6];   // 六档概率 @ dm+0x14，索引 i+0x1264
};

// power_war_dungeon_gold_clear_reward / generateSpecificItem 的 ref+0x34 上下文
// （ORIG 0x08538e77 / 0x08538c9x 段访问实证：+1 金币档位、+2 队伍档位、
// +8/+0xa/+0xe 三个 ushort 分量）。与 _generateRandomItem 的
// stGenerateRefClearReward_t 同起点但消费面不同，故各立具名视图（§4.3）。
struct stPowerWarRefCtx_t
{
    unsigned char m_field00;       // +0x00（generateSpecificItem 的 getGoldGenTable 档位）
    unsigned char m_levelGold;     // +0x01（m_dungeonGoldBonus 档位）
    unsigned char m_levelParty;    // +0x02（m_partyBonus 档位）
    char m_pad03;                  // +0x03
    // +0x04：ORIG 0x08538eaa `mov 0x4(%eax),%ebx` 单段直落 ⇒ 字段与
    // GetExpBonusWeight 形参（eRankBonusIndex）同型（P211 档③）。
    eRankBonusIndex m_rankBonusIndex;
    unsigned short m_field08;      // +0x08
    unsigned short m_field0a;      // +0x0a
    char m_pad0c[2];               // +0x0c..0x0d
    unsigned short m_field0e;      // +0x0e
};
static_assert(sizeof(stPowerWarRefCtx_t) == 0x10,
              "power-war ref ctx size drift");

// m_common->getGoldGenTable() 返回的两元素表（ORIG 0x08538ee9 `mov (%eax),%eax`
// / 0x08538f90 `mov 0x4(%eax),%eax` 实证）。指针下标在 4.4 会被拆成
// `add $0x4; mov (%eax)` 两步，具名成员访问折单条 disp。
struct stGoldGenTable_t
{
    int m_base;    // +0x00（base = m_base*0xaf/1000）
    int m_range;   // +0x04（GetRandomRange 半径）
};

#include "CClearReward/setDropRatio.inc"
}

#include "CClearReward/getGoldCardCommission.inc"
}

#include "CClearReward/getGoldCardCreateRate.inc"
}

#include "CClearReward/getRewardItemRatePerMapMaxCount.inc"
}

// [b28d-solo-CClearReward] verdict=AE：循环体基址改数组指针形
// `(*(m_workProbs + drop))[i]`，复现 ORIG `lea 0x60(%eax),%ecx; …;
// lea (%ecx,%eax,1),%edx`（下标形 4.4 恒折 add 链；探针 /tmp/b28d_ccr/p5.cpp f1↔f2）。
#include "CClearReward/_getGenItemProbIdx.inc"
}

#include "CClearReward/generateRandomItem.inc"
}
// [b28d-solo-CClearReward] verdict=DIFF（N 封顶）：
// 已修 —— m_dropProb 枚举化（P254 单段直落）、数组指针形 (*(m_workProbs+drop))[idx]
// （P30/P183 钥匙，探针 p5 f1/f2）、dropRoll 内联（P0）、thr 改 A*(1.0f-dropRate)
// 书写序（P243）、find_item 具名结果（P115）。
// 残余：①ORIG 把 e.v[0]/v[1]/v[2] 三字段各存槽（-0x3c/-0x38/-0x34）而我方只存
// e 地址（-0x40）——「三具名 int 局部（v0/v1 后续未用）」不可由合法源形反推；
// ②帧 0xa4 vs 0x9c（push edi/esi）+ 全帧槽号平移（N82）；③itemIdx/found/循环变量
// 槽位连锁。
#include "CClearReward/_generateRandomItem.inc"
}

#include "CClearReward/_generateSpecificItem.inc"
}

#include "CClearReward/generateSpecificItem.inc"
}

#include "CClearReward/power_war_dungeon_gold_clear_reward.inc"
}

// [b28d-solo-CClearReward] verdict=NEAR（N1 封顶）：102=102。唯一残差 = 概率表
// 基址常量折叠——ORIG `add $0x1264,%edx; mov 0x14(%eax,%edx,4),%eax`（两步）vs
// 4.4 折 `add $0x1269,%edx; mov (%eax,%edx,4),%eax`（一步；探针 p6 f1/f2/f6 穷举）。
#include "CClearReward/generatePremiumGoldCardItem.inc"
}

// [b28d-solo-CClearReward] verdict=AE：stGoldCardBlankItemInfo_t::v 由 int[3]
// 改 unsigned int[3]（CItemGenerator.h:790）—— v[1]→Inven_Item::m_addInfo 单段
// 直落、v[2]→CItemList::create_item(...,int) 两段 eax→ebx 中转（P211 档③）。
#include "CClearReward/generateGoldCardBlankItem.inc"
}

// [b28d-solo-CClearReward] verdict=AE：同 generateGoldCardBlankItem（v 无符号化）。
#include "CClearReward/generatePcRoomCardBlankItem.inc"
}

// ---- 依赖：CItemGenRateTable::chooseItem（ORIG T 0x085349a4）----
// [b28d-solo-CClearReward] verdict=NEAR（N82 封顶）：182=182，助记符序列全等
// （`setne` 物化经 P225 while+return 形复现）；仅余槽号/寄存器分配差
// （ORIG 迭代器 -0x38 / result -0x10，我方 -0x28 / -0x18；帧同 0x60）。
#include "CClearReward/chooseItem.inc"
}

// ---- 依赖：CItemGeneratorMgr::onTryGenerateRandomItem（ORIG T 0x085342c6）----
#include "CClearReward/onTryGenerateRandomItem.inc"
}

}  // namespace WongWork

// ORIG T 0x08550954：m_users 按幸运点取最大者下标（空表 0）。
#include "CClearReward/_GetMaxLuckPointUserIndex.inc"
}

// ORIG T 0x08550b14：最大幸运点用户执行扣点，成功记 m_points[seq]=序号。
// [b28c-lane17] idx 由 unsigned int 改 int：①ORIG 0x08550b41 首判是 `cmpl $0x0,-0x10`
// 有符号内存直比（cast 形在 4.4 反产 mov+test）；②0x08550b70 的 `m_users[idx]`
// 点为「值先」装载序（P218：int 下标→size_t 实参转换中转即复现），
// 尾段 m_points[seq]=idx 的 int 直存不变。
#include "CClearReward/UseLuckPoint.inc"
}

// ORIG T 0x08550ef0：m_users 中 getCurCharacR 非空者经 GM 虚发文本。
#include "CClearReward/sendItemDropRate.inc"
}

// ============================================================================
// 全局 ::CLuckPoint 幸运点修正（ORIG T 0x085509da / 0x08550be4）
// ============================================================================
// ORIG T 0x08550e50：m_points[idx]（operator[]）得用户序号；越界返 0，
// 否则 m_users[序号]（ORIG 以 int 承载指针值，调用方 m_ownerId 为 int）。
#include "CClearReward/GetUserGainItem.inc"
}

#include "CClearReward/GetLuckPoint.inc"
}

// [b28e-solo-CClearReward] verdict=AE：200=200（norm；raw 201=201），fn_diff 0 差异行，
// p167 callsite 11/11 CLEAR，帧 0x74/槽号与 ORIG 全等。本轮修三处：
// ①acc[2]→acc[3]（真 bug：ORIG 0x08550d26/d71/d83 读写清均为 -0x34=acc[3]）；
// ②break 移出 if(party!=0)（真 bug：ORIG party==0 与 party!=0 两路同汇 0x08550d20 出循环）；
// ③补具名 diff（ORIG 0x08550c73 有 -0x24 槽往返，P0）+ 删具名 v（ROUND 结果就地内联，
// 具名 v 多占两槽致全帧 -8 漂移、200→197）。
// 历史已修 —— 拆嵌套 if 消 `&&` 常量岛（P225 推广反向）、luck/sp 内联（P0）、
// 0.01f 右结合复现 `fildl A; fildl B; flds 0.01; fmulp; fmulp`（P243）。
#include "CClearReward/GetItemRarity.inc"
}

// 全局 ::roundFloat（ORIG W 0x0853abe5：call round 后 fnstcw/fldcw(0xc 截断)+fistpl
// 取整为 int，直接 mov %eax 返回——返回类型为 int，调用点均为 call 后直接用 eax）
#include "CClearReward/roundFloat.inc"
}
