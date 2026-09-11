#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8_CU_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "W8_CU_A/IsOldLotteryItem.inc"



#include "W8_CU_A/SellItemTimeChecker.inc"


#include "W8_CU_A/inc_multiboxLotteryItemFailCnt.inc"


#include "W8_CU_A/GetItemTimeChecker.inc"


#include "W8_CU_A/GetItemSerialChecker.inc"


#include "W8_CU_A/lottery_item.inc"


#include "W8_CU_A/multibox_lottery_item.inc"


#include "W8_CU_A/CheckFailMultiBoxLottery.inc"

