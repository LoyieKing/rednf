// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 UserCoreDepsB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>
#include <stdlib.h>

#include "CUser.h"                    // CUser/_Charac_info/Secu_HackLogCheck/
                                      // Secu_MonsterCheck/WongWork::CHackAnalyzer/
                                      // ENUM_HACKTYPE/WongWork::CUserPremium(m_premium)
#include "CUserCharacInfo.h"          // getCurCharacR/getTagCharacR/getTagCharacSkillW/
                                      // getTagCharacInvenR/getCurCharacInvenW/
                                      // getCurCharacNo/get_charac_10_level_section/
                                      // getCurCharacStrikerSkillIndex/
                                      // online_preliminary::COnlinePreliminary
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "AvatarRechargeServer.h"     // CalcDurabilityChangeEquip/CalcDurabilityDetail 声明
#include "CInventory.h"               // MakeEquipList/GetInvenSlot/insertItemIntoInventory/
                                      // GetInvenTypeFromItemSpace/_checkItemMovable 声明
#include "CExpandEquipslot.h"         // GetCharacExpandDataR(9) 返回类型与 GetInvenSlot
#include "CEquipItem.h"               // CEquipItem 视图（GetUsableEquipmentType/m_field1bc）
#include "CItem.h"                    // is_stackable/get_index/getUsableLevel/GetItemType
#include "CDataManager.h"             // G_CDataManager/find_item/m_serverParameter(+0x68)
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "CCharacter.h"               // _Additioal_info（GetAdditionalInfo 返回值）
#include "CBattle_Field.h"            // check_grid_clear（CheckMapClear 转发目标）
#include "CDungeon.h"                 // isNoFatigueDungeon/m_villageAttackEventFlag/m_bloodDungeonType
#include "CParty.h"                   // 路由 API/IsUseFatigueDungeon/CheckMapClear/
                                      // DeleteDungeonDropItem/send_to_party
#include "CSpecialItemRoutingManager.h"
#include "CBossTower.h"               // WongWork::CBossTower（getBossTower 返回值）
#include "WarField.h"                 // MapInfo::m_itemMap（+0xa18 = CBossStage+0x9f4+0x24）
#include "CGenUniqueNo.h"             // WongWork::CGenUniqueNo::genIPGNo
#include "GameRand.h"                 // get_rand_int
#include "CStatisticServerProxy.h"    // statistc_proxy::add
#include "CSystemTime.h"              // getCurDate
#include "GlobalData.h"               // s_systemTime_
#include "LogManager.h"               // logFormat

#include "UserCoreDepsB2/Init.inc"


// ---------------------------------------------------------------------------
// 1. CUser::MakeTagCharacInfo —— ORIG T 0x08690776（报告 CUser/MakeTagCharacInfo.md）
// tag 角色不存在 → 0；否则写入：uid 低 16 位、名字、等级/job/+0x29、striker 技能
// 序号、附加信息 0x52 字节（premium 时叠加 CUserPremium::GetAdditionalInfo）、
// 装备列表、技能强化循环、求购单包。成功返回 1。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] 236→289（ORIG 276），P167 40=40 CLEAR。本轮修入：
// ①去 tag 具名局部（ORIG 每字段独立 `call getTagCharacR`，无槽；P0）；
// ②nameLen 改 unsigned char（ORIG `mov %al,-0x29` + movzbl 回读，int 槽不符）；
// ③去 inven 具名局部改内联 receiver（ORIG 无槽）；
// ④**P167 重载修复**：`put_binary(infoBuf,…)` 原选 `put_binary(char*,int)`（ORIG 为
//   `put_binary(char const*,int)`）⇒ 加 `(const char*)` cast 后 MISSING/EXTRA 清零；
// ⑤**消 memcpy EXTRA**：ORIG 无 memcpy call（0x52 块搬移内联 rep movsl）⇒ 改
//   `*(_Additioal_info*)infoBuf = *(const _Additioal_info*)src;`（P83 结构整体赋值），
//   隐式 operator= 被 4.4 内联展开、零新增符号（nm 仅 U operator+=）；
// ⑥premium 分支改正极性（ORIG `call; test; je` 直判 ⇒ `if (IsHavePremiumAdvantage())`
//   then=premium）；⑦循环体 `base[i*2]` 下标形。
// 残差（byte 层封顶）：N78（ORIG memcpy 内联对齐+rep movsl 40 条 vs 4.4 结构赋值逐字段
// 展开）、N82/N89（帧 0xec/0xec±、槽号与 STL/日志内联布局）、N4/P30（`i*2+base` add 链
// vs lea 折叠）。ORIG 0x08690776，语义等价。
// [b28d-PoolGlobalData 二压 P329] baseSmall 具名 bool 内联回 if 条件（消 -0x1d
// 槽与其 `mov %al,-slot; cmpb $0,-slot` 往返）⇒ ndiff 91→46，-0x29/-0x20/-0x24
// 标量槽与 ORIG 逐位复齐；余 46=N78 结构展开 10 行 + 双 0x52 缓冲槽向（见函数
// 体内实测回退记录）+ idxPos -0x80/-0x30 随动，封顶。
// [D01 · 闸3 A/B 2026-09-10] 0x52 块搬移改 pack(2) 局部镜像 AddInfoMove2（定义见下）：
// peel 1→2 字节与 ORIG 0x086908ce 段逐形一致；our_insns 288→279（ORIG 276，gap
// +12→+3），四数 39/64/0/6 零回退、nm 零符号泄漏；余 3 条 = getTagCharacR/
// getTagCharacSkillW 重调序列次数差（P48/P183 族）。
// ---------------------------------------------------------------------------
// [D01 · P142 派生判据 · Main 批准闸3 A/B] `_Additioal_info` 为 pack(1)
// （alignof=1）⇒ 其 0x52 块整体赋值在 4.4 下只能发 1 字节 peel
// （`and $0x1,%ecx`）；ORIG 为 **2 字节 peel**（`mov $0x52,%ebx; and $0x2,%ecx;
// … rep movsl`，ORIG 0x086908ce 段直证）⇒ 原类型 alignof ≥ 2。
// 「改头 alignof 1→2」已判**结构性不可行**（Main 登记）：CUserPremium 无 pack
// 外层于 +0x8bd/+0x90f（奇数偏移）内嵌该类型，插 1B padding 会连锁改
// m_pad961/m_returnItemMap/m_mailBox 偏移（闸1 嵌入点清单 7 点）。
// 本视图只用于**块拷贝点**：同字段序/同宽 POD + pack(2)（alignof=2、布局与
// _Additioal_info 逐字段等价、sizeof 0x52），使 4.4 复现 2 字节 peel；
// 纯 0x52 块搬移、无指针/引用成员 ⇒ 无别名/生命周期风险。
#pragma pack(push, 2)
struct AddInfoMove2
{
    int            m_field00;      // +0x00
    int            m_field04;      // +0x04
    unsigned short m_field08;      // +0x08
    unsigned short m_field0a;      // +0x0a
    unsigned short m_field0c;      // +0x0c
    unsigned short m_field0e;      // +0x0e
    unsigned short m_field10[4];   // +0x10..+0x17（union 8B 视图同宽）
    unsigned short m_field18[17];  // +0x18..+0x39（union 0x22B 视图同宽）
    int            m_field3a;      // +0x3a（2 对齐、非 4 对齐）
    unsigned short m_field3e;      // +0x3e
    unsigned short m_field40;      // +0x40
    int            m_field42;      // +0x42（2 对齐）
    unsigned short m_field46;      // +0x46
    unsigned short m_field48;      // +0x48
    unsigned short m_field4a;      // +0x4a
    unsigned short m_field4c;      // +0x4c
    int            m_field4e;      // +0x4e（2 对齐）→ 共 0x52
};
#pragma pack(pop)
typedef char AddInfoMove2_size_0x52[(sizeof(AddInfoMove2) == 0x52) ? 1 : -1];
typedef char AddInfoMove2_align_2[(__alignof__(AddInfoMove2) == 2) ? 1 : -1];
typedef char AddInfoMove2_off_10[(__builtin_offsetof(AddInfoMove2, m_field10) == 0x10) ? 1 : -1];
typedef char AddInfoMove2_off_18[(__builtin_offsetof(AddInfoMove2, m_field18) == 0x18) ? 1 : -1];
typedef char AddInfoMove2_off_3a[(__builtin_offsetof(AddInfoMove2, m_field3a) == 0x3a) ? 1 : -1];
typedef char AddInfoMove2_off_42[(__builtin_offsetof(AddInfoMove2, m_field42) == 0x42) ? 1 : -1];
typedef char AddInfoMove2_off_4e[(__builtin_offsetof(AddInfoMove2, m_field4e) == 0x4e) ? 1 : -1];

#include "UserCoreDepsB2/MakeTagCharacInfo.inc"


#include "UserCoreDepsB2/CalcDurabilityChangeEquip.inc"


#include "UserCoreDepsB2/_checkItemMovable.inc"


#include "UserCoreDepsB2/MoveMap.inc"


#include "UserCoreDepsB2/Reset.inc"


#include "UserCoreDepsB2/ProcessFailEtc.inc"


#include "UserCoreDepsB2/processRouting.inc"


// ---------------------------------------------------------------------------
// 依赖叶子（无报告，objdump 直转；地址见各注释）。
// ---------------------------------------------------------------------------

#include "UserCoreDepsB2/IsUseFatigueDungeon.inc"


#include "UserCoreDepsB2/CheckMapClear.inc"


#include "UserCoreDepsB2/check_grid_clear.inc"


#include "UserCoreDepsB2/GetUsableEquipmentType.inc"


#include "UserCoreDepsB2/getServerHackAccumulatedCnt.inc"


#include "UserCoreDepsB2/SetUserPtr.inc"

