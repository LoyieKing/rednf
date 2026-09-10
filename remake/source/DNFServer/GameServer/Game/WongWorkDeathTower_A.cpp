// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkDeathTower_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// （见下方注释）须置于全部 include 前：首个带入 CItemGenerator.h 的链
//   （CDeathTower.h→CParty.h→…）展开时即需这两个名字。
class CMonster;
namespace WongWork { struct stGenerateRefClearReward_t; }

// ============================================================================
// df_game_r 还原 —— WongWork::CDeathTower 嵌套类（G4 批次，WongWorkDeathTower_A）
//
// 本 TU 承载：
//   · CPacketHandler 全部包构造族（ORIG T 0x0845fc6c..0x084605a1）及
//     C1/D1（ORIG W 0x08469a8c / 0x08469aa0，转发 PacketGuard）；
//   · CDungeonMgr C1/C2、D1/D2（ORIG W 0x082a74b0 / 0x082a74b6：空函数）；
//   · CPlayData D1/D2（ORIG W 0x82a7572：空函数）、generateRewardExp
//     （ORIG T 0x08462538）、generateRewardItem（ORIG T 0x08461fc0）；
//   · 生命周期闭环补件：CPlayData C1（W 0x82a7538）、stRewardItem_t C1
//     （W 0x82a7502，逐 Inven_Item() 构造）—— 显式定义以匹配 ORIG 弱符号。
// 类声明见 CDeathTower.h（唯一完整定义点）；CGameManager.h 仅 #include 该头。
// ============================================================================

#include "CDeathTower.h"

#include "CDungeon.h"
#include "CMap.h"

// CItemGenerator.h 自身顺序缺口的前置（其 _generateMonseterSpecificItem 参数用
// CMonster、_generateRandom/SpecificItem 私有签名在自身前置声明行之前引用
// stGenerateRefClearReward_t；完整定义在 CClearReward.cpp，见该头注释）。
class CMonster;
namespace WongWork { struct stGenerateRefClearReward_t; }
#include "CDataManager.h"          // G_CDataManager / GetChannelScript
#include "GlobalData.h"            // GlobalData::s_event_manager
#include "CGuildServerProxy.h"     // class CEventManager / class CEventBase
#include "CBurningFatigueEvent.h"
#include "CExpDoubleEvent.h"
#include "CPCRoomBurningEvent.h"
#include "CServerEvent.h"
#include "CEnvironment.h"          // G_CEnvironment / get_server_group / get_channel_no
#include "CItemGeneratorMgr.h"
#include "CInventory.h"
#include "CDeathTowerRanking.h"
#include "LogManager.h"

namespace {

static const char kFile_DeathTower[] = "DeathTower.cpp";
static const char kFunc_generateRewardExp[] =
    "void WongWork::CDeathTower::CPlayData::generateRewardExp(int, int, unsigned int, "
    "CParty*, WongWork::CDeathTower::CDungeonMgr&)";

} // namespace

namespace WongWork {

// ---- CDeathTower::stMapMonsterKillChecker_t（ORIG 嵌套版，布局 0x0c）----
#include "WongWorkDeathTower_A/init.inc"
}

#include "WongWorkDeathTower_A/setUseSkillMaterial.inc"
}

#include "WongWorkDeathTower_A/getUseSkillMaterialTime.inc"
}

#include "WongWorkDeathTower_A/initMonsterInfo.inc"
}

#include "WongWorkDeathTower_A/dieMonster.inc"
}

// ==============================================================================
// 生命周期补件（ORIG W）
// ==============================================================================

// ORIG 0x82a7502：编译器生成 do-while 依次构造 m_items[10]（步长 0x3d，
// 起点 this+4）。本类字段均为非平凡默认构造成员，空函数体即由编译器逐个调用。
#include "WongWorkDeathTower_A/stRewardItem_t.inc"
}

// 其余标量 ORIG 不初始化；仅 m_rewards[4] 调 stRewardItem_t C1（0x82a7538）。
// [b28d-Main·检查点14 A/B] throw()（异常规格不入 mangled；声明/定义须一致）。
#include "WongWorkDeathTower_A/CPlayData.inc"
}

// ORIG 0x82a7572：push ebp/mov esp,ebp/pop ebp/ret —— 空函数（Inven_Item 平凡
// 析构，编译器不发清理代码，与 ORIG 完全一致）。
// [b28d-Main·检查点14 A/B] throw()（同上）。
#include "WongWorkDeathTower_A/dtor_CPlayData.inc"
}

// ==============================================================================
// CDungeonMgr C1/C2 @0x082a74b0、D1/D2 @0x082a74b6（四符号同址：全空函数）
// ==============================================================================
#include "WongWorkDeathTower_A/CDungeonMgr.inc"
#include "WongWorkDeathTower_A/dtor_CDungeonMgr.inc"
#include "WongWorkDeathTower_A/CPacketHandler.inc"
#include "WongWorkDeathTower_A/dtor_CPacketHandler.inc"
#include "WongWorkDeathTower_A/appendPickupMoney.inc"
}

// ORIG T 0x0845fdc0：4 字节名 + ushort + short + byte + finalize(true)。
#include "WongWorkDeathTower_A/appendPickupItem.inc"
}

// ORIG T 0x0845fe4c：header(1,cmd)；byte(0)；byte(err)；finalize(true)。
#include "WongWorkDeathTower_A/makeCommonError.inc"
}

// ORIG T 0x0845fea6：header(1,cmd)；byte(0)；byte(err)；byte(slot)；finalize(true)。
#include "WongWorkDeathTower_A/makeDropItemError.inc"
}

// ORIG T 0x0845ff1a：塔内掉落通知包（字段偏移对应 map_item/Inven_Item 具名成员）。
// [round3-L29 2026-09-10 修复] 尾部 bool 条件物化：真因 = `map_item::m_count` 为 bool，
//   `put_byte(item.m_count == 0)` 被 4.4 折 `xor $1; movzbl`（块序亦反）。改为
//   `put_byte((int)item.m_count != 0 ? 0 : 1);`（int cast 阻断 bool 取反折叠）⇒
//   **AE 111/111 ndiff=0**（此前 DIFF 108/111 ndiff13）。ORIG 0x0845ff1a 证据形：
//   `movzbl (%eax),%eax; test %al,%al; je <mov $1>; mov $0,%eax; jmp; mov $1,%eax`。
#include "WongWorkDeathTower_A/makeDropItemNotify.inc"
}

// ORIG T 0x084600a6：header(1,0x32)；byte(1)；byte(order)；short(xy)；int(key)；
// finalize(true)。
#include "WongWorkDeathTower_A/makeDropItem.inc"
}

// ORIG T 0x0846012c：header(0,0x1e)；int(0)；finalize(true)。
#include "WongWorkDeathTower_A/makeFinishLoading.inc"
}

// ORIG T 0x08460176：header(0,0x20)；short(uid)；byte(0)；finalize(true)。
#include "WongWorkDeathTower_A/makeDieState.inc"
}

// ORIG T 0x084601da：排行榜展示包。个人最优/榜首记录段委托给
// CDeathTowerRanking 的两个 static 表访问 + 记录序列化函数。
#include "WongWorkDeathTower_A/makeDisplayRangking.inc"
}

// ORIG T 0x084602c4：header(0,0x91)；int(v1)；后接 4 组 [byte cnt；cnt×{int
// m_addInfo(Inven_Item+0x02); int get_add_info()}]，Inven_Item 步长 0x3d 遍历；
// finalize(true)。
#include "WongWorkDeathTower_A/makeDisplayReward.inc"
}

// ORIG T 0x084604b0：header(0,0x92)；byte(flag)；finalize(true)。
#include "WongWorkDeathTower_A/makeDisplayEPLP.inc"
}

// ORIG T 0x0846050e：header(0,0xe)；byte(space)；short(1)；
// inv.MakeItemPacket(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot, *this)；finalize(true)。
#include "WongWorkDeathTower_A/makeUpdateItemList.inc"
}

// ==============================================================================
// CPlayData::generateRewardExp（ORIG T 0x08462538）
//
// 权重表：ORIG `flds 0xc(dm + (stage+tier)*4)` = m_deathTower.m_rewardExpWeight
// （tier 0x1464）/ m_illusionRewardExpWeight（tier 0x15f4）——基址
// 0x5198+0x4/0x644 与 0xc+0x1464*4/0xc+0x15f4*4 精确吻合（float 读取，ORIG
// 报告 WongWork__CDeathTower__CPlayData/generateRewardExp.md）；所得 exp 与最终
// 结算结果 >0x10c8e0(1100000) 时写 BLOOD_LOG 并截断。各加成系数仅进日志参数流。
// ==============================================================================
// [b28d-24 历史] DIFF 残148（P282 候选）: vcall 偏移 52 ORIG 3 次 vs 我方 2 次。
// [2026-09-10 round5 复核] **p167 CALLSITE CLEAN 36/36**：残余仅 C1/C2 变体差
//   （_ZN23stClearRewardBonusExp_t C1==C2 同址 0x0814a800、_ZN27stClearRewardExpParameter_t
//   C1==C2 同址 0x0814a8b8 ⇒ P93 伪差，非缺口）；vcall +0x34（IsEventing）三处已齐。
//   本轮修：删 channelNo/dungeonIdx 具名局部（P0，值经 %ebx/%esi 跨 call 保活直传）、
//   G_CEnvironment 两处调用改复用 env。残余 = 我方 308 vs ORIG 291（帧 0x178 vs 0x130：
//   具名 float 局部槽基差，N82/N95 族；mn_cmp 残差全为「+mov|+mov」槽往返对）⇒ N 封顶。
// ORIG 0x08462538
#include "WongWorkDeathTower_A/generateRewardExp.inc"
}

// ==============================================================================
// CPlayData::generateRewardItem（ORIG T 0x08461fc0）
//
// 期望掉落条数写入 m_rewards[memberIdx].m_itemCnt；循环 count 次投掷
// get_rand_int(1000)：rnd>rate 高阶掉落分支的概率锚点为 DataManager+0x5e20 /
// +0x5e24（乘 param5 比较）= m_deathTower.m_rewardItemProb[0]/[1]（0x5198+0xc88
// 起 int 数组，ORIG `mov 0x5e20(%eax),%eax` int 宽实证）；计数率因子
// +0x5e2c = m_deathTower.m_rewardGoldWeight（ORIG `flds` float）。**受控分支**
// （roll>=prob0*param5）经生成器虚槽 +0x08 = generateRandomItem、**随机分支**经
// 虚槽 +0x0c = generateSpecificItem（P282 实证，见函数头 2026-09-10 判词；旧注释
// 把两者写反）。受控分支额外触发幸运点
// 扣减（::CLuckPoint::UseLuckPoint）。ref 对象由其 ctor 初始化，上下文区按
// 具名字段写入（宽度逐条对齐 ORIG：movb/movw/movl，见
// WongWork__CDeathTower__CPlayData/generateRewardItem.md 0846205d..099 /
// 084622bc..328）。
// ==============================================================================
// [b28d-24 历史] DIFF 残380: P167 迭代器写法差（MISSING begin/deEv×5；EXTRA front/C1/D1/G_CDataManager 等）。
// [2026-09-10 round5 定稿] 真 bug 修 + P167 修（P282 流程）：
//   ① **两分支虚调目标互换（真 bug）**：P282 五步核 _ZTVN8WongWork14CItemGeneratorE
//      @0x08c9a788 —— word[2]=0x0853a3f0 setDropRatio / word[3]=0x0853a3f6 loadScript /
//      word[4]=0x0853a400 generateRandomItem / word[5]=0x0853a406 generateSpecificItem。
//      ORIG 08462311 段 `mov (%eax),%edx; add $0x8,%edx; call *%ecx` ⇒ **受控分支
//      （ref 0x3f800000、roll>=prob0*param5）调 generateRandomItem**；ORIG 08462131 段
//      `add $0xc,%edx` ⇒ 随机分支（ref 0x10001 组）调 generateSpecificItem —— 旧重建
//      恰好相反（两分支掉落语义颠倒）。
//   ② 条件符号性：ORIG `cmp -0x20(%ebp),%eax; seta %al; test %al,%al; je` 为 **unsigned**
//      （prob0*param5 <= roll ⇒ 受控）；旧重建写 `(int)boundHigh` signed 比较 + 二段
//      `||` + boundHigh 具名槽。prob1 内层 ORIG 只求值一次 G_CDataManager（旧重建
//      `<= && - != 0` 两次 ⇒ EXTRA +1）。
//   ③ P167：`.front()`→`*v.begin()`（ORIG begin×4/deEv×5）；`probe` 由 setCopy 改
//      **拷贝构造**（ORIG 084623b5..0x43f = 15×dword+1byte 块拷贝到 -0xf5）；随机分支
//      具名迭代器复用（ORIG 只 begin 一次却 deEv 两次）；随机分支 ORIG 无 `empty()`
//      守卫、无 `RewardData_Result::clear`。
//   ⇒ p167 差集 16→12 项（begin/deEv/front/empty/setCopy/G_CDataManager 全清）、
//      missing_in_game 25→24。
//   ④ [配方 C05② 正向（A27 机理）+ P0] 删 6 处 ORIG 无的具名局部（dropEmpty 直判
//      `!empty()`、itemList/grade/rarity 内联、genMgr→gen 链内联）⇒ our **388→357**
//      （ORIG 347，gap −41→**−10**）。
//   残余 = C1/C2·D1/D2 变体（P93 同址伪差：0814ab06 C2==C1、0814ab4c D2==D1）
//      + setcc 物化（ORIG seta×2/test×2，4.4 恒直跳，P197④ 族，等价 −4 条）+ ref 末
//      两 short 经 lea+add（stGenerateRefData_t 字段建模 +4）+ 杂项 ⇒ N 封顶。
//   ⑤ [C05 边界留痕] 本件 grade/rarity 在 ORIG 为**寄存器保活**（0846247c `mov %eax,%ebx`
//      / 08462489 `mov %eax,%edi`，经 `084624a7 mov %ebx,0xc(%esp)` 直传 varargs）⇒ 与 C07
//      （ORIG 用槽则写具名 int 局部）方向**相反**：此处必须内联（已按此改）。C05③ 闸门
//      （岛差 ≥2）与 C05① 多层尾块均不适用（各早退为共享 epilogue，ORIG 同形）。
//   ORIG 0x08461fc0
#include "WongWorkDeathTower_A/generateRewardItem.inc"
}

} // namespace WongWork
