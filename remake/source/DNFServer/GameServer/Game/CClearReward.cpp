// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CClearReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CClearReward/CLuckPoint.inc"
{
    m_users.clear();
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


#include "CClearReward/getGoldCardCommission.inc"


#include "CClearReward/getGoldCardCreateRate.inc"


#include "CClearReward/getRewardItemRatePerMapMaxCount.inc"


#include "CClearReward/_getGenItemProbIdx.inc"


#include "CClearReward/generateRandomItem.inc"

#include "CClearReward/_generateRandomItem.inc"


#include "CClearReward/_generateSpecificItem.inc"


#include "CClearReward/generateSpecificItem.inc"


#include "CClearReward/power_war_dungeon_gold_clear_reward.inc"


#include "CClearReward/generatePremiumGoldCardItem.inc"


#include "CClearReward/generateGoldCardBlankItem.inc"


#include "CClearReward/generatePcRoomCardBlankItem.inc"


#include "CClearReward/chooseItem.inc"


#include "CClearReward/onTryGenerateRandomItem.inc"


}  // namespace WongWork

#include "CClearReward/_GetMaxLuckPointUserIndex.inc"


#include "CClearReward/UseLuckPoint.inc"


#include "CClearReward/sendItemDropRate.inc"


#include "CClearReward/GetUserGainItem.inc"


#include "CClearReward/GetLuckPoint.inc"


#include "CClearReward/GetItemRarity.inc"


#include "CClearReward/roundFloat.inc"

