// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W8_CU_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W8_CU_A.cpp —— CUser Wave8 CUserOnlyOrig 残余批 A（抽奖/时序判定族，7 符号）
//
// 来源：docs/class_func_reports/CUser/{IsOldLotteryItem,lottery_item,
//       multibox_lottery_item}.md（## 汇编 + ## 反编译 C 全文）
//       + objdump 直转：
//         CUser::IsOldLotteryItem      T 0x08673342 (0x60)
//         CUser::inc_multiboxLotteryItemFailCnt W 0x080fdf56 (0x23)
//         CUser::SellItemTimeChecker   W 0x08230256 (0x55)
//         CUser::GetItemTimeChecker    W 0x082302ac (0x1f0)
//         CUser::GetItemSerialChecker  W 0x0823066a (0x14f)
//         CUser::lottery_item          T 0x086733a2 (0x15c6)
//         CUser::multibox_lottery_item T 0x08674968 (0x115d)
//
// NOTE(NEEDS_FIX, owner=CDataManager 域批次)：CDataManager::find_lottery_use_cost
// 当前声明/实现返回 bool（isNeedMoney 转发），ORIG mangled
// _ZNK12CDataManager21find_lottery_use_costEj 返回 int（花费数值——返回类型不编码
// 于 mangled）。本 TU 按正确语义以 int 使用其结果；宿主把签名改回 int 后无需再动
// 本文件（bool→int 整型提升编译可通过，值路径待修复）。
//
// NOTE(NEEDS_HEADER, owner=宿主裁决)：CParty.h 以 char 占位承载 +0xb24

// CBattle_Field(0xcd0) 子对象区（见 CParty.h 注释），ORIG ctor 0x082a5516 已实证。
// GetItemSerialChecker 的 party 分支需要读取该子对象的 m_visitedGotoFlag/
// m_hellMapIdx/get_cur_map()。本 TU 采用仓库既有成熟范式（DisPatcherCore.cpp:3333、
// ExpertJobOnlyMgr.cpp:86 同型 reinterpret_cast 视图）；待宿主把 CParty.h 战场区
// 正式嵌入 CBattle_Field 后统一 cutover 为具名子对象访问。
//
// 本 TU 依赖符号（owner 注记）：
//   CInGameAdvertisementManager::CheckFailMultiBoxLottery(CUser*,
//          std::vector<std::pair<Inven_Item,bool>>&)  ORIG T 0x080fcb3e：
//     定义在本 TU（见文件末尾）；ORIG 返回 void（0x080fcb3e 尾无 mov $0,%eax、
//     Ghidra void），权威头 CInGameAdvertisementManager.h:88 声明 bool —— 已上报
//     Main 待头修正（消费点 CUserOnlyOrig.cpp:4364 为注释、本 TU 调用忽略返回值）。
//   CInGameAdvertisementManager* InGameADManagerInstance()
//     ORIG T _Z23InGameADManagerInstancev @0x080fc9e9，owner 同上。
//   （二者的类最小视图在本 TU 末尾声明区给出，仅供指针上成员调用，
//     不产生任何静态数据/定义，无 ODR 风险。）
// ============================================================================

#include <cstring>

#include "CUser.h"
#include "CMCAPManager.h"
#include "CBoosterGage.h"
#include "CAvatarItemMgr.h"
#include "CCeraShop.h"
#include "CGenUniqueNo.h"
#include "Inven_Item.h"
#include "CItemList.h"
#include "CInventory.h"
#include "CItemGenerator.h"       // random_option::CRandomOptionItemHandle
#include "GamePODTypes.h"         // stAvatarExpansionInfo_t
#include "CSystemTime.h"
#include "GlobalData.h"           // s_pCeraShop / g_randomOptionHandle / s_systemTime_
#include "CBattle_Field.h"
#include "CParty.h"
// ORIG 全局空槽物品（B 0x943ddc0；唯一 owner 定义于 CAccountCargo.cpp）
extern Inven_Item g_emptySlot;
#include "GameWorld.h"

// ---------------------------------------------------------------------------
// 最小外部声明区（owner=W8_LongTail.LT08_AdvertisementMisc；本 TU 无定义）
// ---------------------------------------------------------------------------
class CInGameAdvertisementManager
{
public:
    void CheckFailMultiBoxLottery(
        CUser* user,
        std::vector<std::pair<Inven_Item, bool> >& rewardList);
};
extern CInGameAdvertisementManager* InGameADManagerInstance();

// ===========================================================================
// 1) CUser::IsOldLotteryItem(unsigned int) @0x08673342 —— 报告全文直转
// ===========================================================================
#include "W8_CU_A/IsOldLotteryItem.inc"
}


// ===========================================================================
// 3) CUser::SellItemTimeChecker(unsigned int nowSec, unsigned int interval)
//    @0x08230256。字段：m_mcap.m_sellTimeChkBaseSec(+0x3c)/
//    m_mcap.m_sellTimeHitCount(+0x40)（本批于 CMCAPManager.h 拆 pad 具名化，
//    已广播默许）。
// ===========================================================================
#include "W8_CU_A/SellItemTimeChecker.inc"
}

// ===========================================================================
// 3b) CUser::inc_multiboxLotteryItemFailCnt() @0x080fdf56
// ===========================================================================
#include "W8_CU_A/inc_multiboxLotteryItemFailCnt.inc"
}

// ===========================================================================
// 4) CUser::GetItemTimeChecker(unsigned int nowSec, unsigned short val1,
//                              unsigned short val2, unsigned int rangeSec)
//    @0x082302ac。双通道时序差窗（hack 时序检测）：
//    超 rangeSec → 重置；窗口内更新 min/max 并写 |max-min| 绝对值到 diff 字段。
// ===========================================================================
#include "W8_CU_A/GetItemTimeChecker.inc"
}

// ===========================================================================
// 5) CUser::GetItemSerialChecker(unsigned short serialIdx,
//                                unsigned short serialVal, int monsterIdx)
//    @0x0823066a。序列号差窗连续命中计数：idx 差∈[-23,23] 且 val 差∈[-17,17]
//    才保持计数；越窗重置。命中条件 = party 存在且 party->isMonsterDrop_item
//    且战场未做 visited-goto 校验（bf.m_visitedGotoFlag==0 或
//    bf.m_hellMapIdx != bf.get_cur_map()）。
// ===========================================================================
#include "W8_CU_A/GetItemSerialChecker.inc"
}

// ===========================================================================
// 6) CUser::lottery_item(int slot) @0x086733a2 —— 使用槽上 lottery 物品开奖。
//    金币奖励 / avatar / 生物蛋 / 强化三档概率（ServerParameterScript 内嵌表）/ 
//    useCost 扣款通知 / 主响应包 / 全服强化播报。全程照报告转写。
//    §6.2 巨件核销：P167 200/200 [0缺0多]；gap 1315→1330；残差归因 N 类
//    （N82 全帧槽平移 0x18/0x10 + 槽复用差、N-88/setne 物化假象（ORIG
//    cmpl/setne/test vs 4.4 cmp 直跳，P197④ 探针同型）、N22 比较方向反演
//    （get_rand_int 三段 seta↔setb 同语义折向）、bool 聚合变量槽物化
//    （movb/-0x51 槽 vs ORIG 寄存器 test）、short 形参槽中转（movswl 经
//    -0x22 中转）——语义与调用序列逐块比对零缺口，byte 层 N 封顶。
// ===========================================================================
// [round2-L30 复核·N95/N82] shift.py: frame ORIG=780 OURS=780，MIXED（相对序差，含 8-dword
//   结构拷贝展开位）⇒ 大件未改造，判 N；ORIG 0x086733a2（0x15c6）。
// [E04 2026-09-10 复核] p167 200/200 CLEAR（差集全为 C1/C2·D1/D2 同址别名 P93）⇒ 无真缺口；
//   gap +15 维持 N82/N95 槽序族。
// [F2a-C05② 复测·不适用] 件级 F2 旋钮逐条核对报告 5 处 ORIG `cmp -0xNN(%ebp),%eax`：
//   ① ORIG 0x086735... `cmp -0x40(%ebp),%eax; jle`（(prev+delta) vs 局部）我方对应点
//      已是同形 mem 直比（`cmp -0x4c(%ebp),%eax; setg`，仅槽号整体差 0xc，属 N82）；
//   ② `cmp -0x44(%ebp),%eax`（ORIG 0x086736xx）与 `cmp -0x30(%ebp),%eax; seta`
//      （ORIG 0x08673xx）的 mem 操作数是 **call 返回值局部**（gain_money / get_rand_int
//      的结果槽）⇒ C05② 边界（跨调用返回值不适用），且后者为无符号序 seta；
//   ③ 无一处属「两个具名局部比较」命中形 ⇒ 维持 N82 封顶，无改动、无回滚。
#include "W8_CU_A/lottery_item.inc"
}

// ===========================================================================
// 7) CUser::multibox_lottery_item(int boxSlot, int keySlot) @0x08674968
// ===========================================================================
// [round2-L30 复核·N126] 残余=整帧槽号常数平移 0x10（ORIG -0x14d… vs 我方 -0x15d…；帧
//   0x25c vs 0x24c）⇒ 判据⑪ (b) 类，声明重排无效；ORIG 0x08674968（0x115d）。
// [E04 2026-09-10 复核] p167 141/141 CLEAR（差集仅 C1/C2·D1/D2 别名）；另比较点
//   ORIG `mov -0x14b(%ebp),%eax; cmp $0x2061,%eax` vs 我方 `cmpl $0x2061,-0x58(%ebp)`
//   = N18b 比较形族（形参/局部 mem 直比）。N126+N18b 维持。
#include "W8_CU_A/multibox_lottery_item.inc"
}

// ---------------------------------------------------------------------------
// CInGameAdvertisementManager::CheckFailMultiBoxLottery —— ORIG 0x080fcb3e
// _ZN27CInGameAdvertisementManager24CheckFailMultiBoxLotteryEP5CUserRSt6vectorISt4pairI10Inven_ItembESaIS5_EE
// 报告 docs/class_func_reports/CInGameAdvertisementManager/CheckFailMultiBoxLottery.md：
//   若 rewardList 全部为 multibox 广告箱物品(idx ∈ {0x1e3c,0x1e3d,0x1e3e})，
//   inc_multiboxLotteryItemFailCnt；连续失败 >2 时发送 put_header(0,0x1c6) 空通知包；
//   任一非广告物品 → set_multiboxLotteryItemFailCnt(0)。单例 owner=GameMain.cpp。
// 定义置于本 TU 末尾（曾试 P210 后置以翻 multibox 帧：实测帧 0x24c 不变，
//   故非「已见定义 callee」成因；见 multibox_lottery_item 判词 N95）。
// ---------------------------------------------------------------------------
#include "W8_CU_A/CheckFailMultiBoxLottery.inc"
}
