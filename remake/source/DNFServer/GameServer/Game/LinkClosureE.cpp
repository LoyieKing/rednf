// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureE/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// LinkClosureE.cpp — 全量链接收敛批 E 散件 producer（LinkClosureE 批次）
//
// 覆盖符号（ORIG nm -S --defined-only 核对；报告路径见各段注释）：
//   CMaxFatigueFactorEvent::GetMaxFatigueFactor()            W 0x0812ce54
//   CBuffGroup::CBuffGroup(CUser*) / IsRight(CUser*)         W 0x08697c3e / 0x08697c64
//   SendLV70Buff(CUser*, unsigned long)                      T 0x0865defc
//   CItemMakingSkill::Send_ItemMakingSkillList(CUser*) const T 0x08545a62
//   Arad_GiftItem_Set::send_mail(CUser*, int, int, int, int) T 0x081a329e
//
// 注：CMaxFatigueFactorEvent / CBuffGroup / CBelong / CItemMakingSkill 在
// CUserOnlyOrig.cpp 等消费 TU 内为最小声明视图；本 TU 提供权威完整视图 + 定义，
// mangled 与 ORIG 一致。CBelong 各 TU（Area.cpp 等）已有同型局部视图。

#include "CBoosterGage.h"         // charac_expand::CData 权威基类（pack(1) 0x5）
#include "PvPTypes.h"             // CMTRand（CItemMakingSkill +0x1c 字段）
#include "PacketGuard.h"
#include "GameWorld.h"            // G_GameWorld / send_to_area 两重载
#include "CUser.h"                // CUser::Send/get_area/get_unique_id/GetServerGroup
#include "CUserCharacInfo.h"      // getCurCharacVill / get_charac_level / getCurCharacNo
#include "CDataManager.h"         // G_CDataManager / find_item
#include "CItemList.h"            // Arad_GiftItem_Set（find / send_mail 声明）
#include "Arad_ItemBonus.h"       // ARAD::send_mail / ARAD_DATAS::AradMailInfo
#include "Inven_Item.h"
#include "CStackableItem.h"      // CStackableItem::m_vec168（兔盒列表 +0x168）
#include "CInventory.h"          // insertItemIntoInventory（Inven_Item 按值入包）
#include "CCharacList.h"         // CCharacList::m_characs[11]（CDataManager+0x14）
#include "CItem.h"                // CItem::make_item 虚槽（vtable slot +0x8）
#include "CMailBoxHelper.h"       // ReqDBSendNewMail
#include "LogManager.h"
#include "GameTypes.h"            // cMyTrace

class CUser;
class CStackableItem;
class CBelong;

// ---------------------------------------------------------------------------
// global::tools::InitializeCharacStat::initCharacStat(_Additioal_info&,
//     unsigned int, int, int, int, int)
// ORIG T _ZN6global5tools20InitializeCharacStat14initCharacStatER15_Additioal_infojiiii
// @0x08145e64 sz0x43f。
// 报告 docs/class_func_reports/global__tools__InitializeCharacStat/initCharacStat.md。
// info 全零守卫 → clear → job/level 校验（0x1f 行 trace）→ memcpy 职业基表
// （CDataManager::m_characList + job*0x7dc，0x52 字节）→ 三个成长态
// get_growtype_stat 校验（0x2d 行 trace）→ info = 基表 → level-1 次分段 add
// （<=0xe 态1 / <=0x31 态2 / 其余态3）→ 0x41 行结果 trace。
// ---------------------------------------------------------------------------
namespace global
{
namespace tools
{

class InitializeCharacStat
{
public:
    static void initCharacStat(_Additioal_info& info, unsigned int characNo,
                               int job, int level, int grow1, int grow2);
};

#include "LinkClosureE/initCharacStat.inc"


}  // namespace tools
}  // namespace global

// ---------------------------------------------------------------------------
// CMaxFatigueFactorEvent::GetMaxFatigueFactor()
// ORIG W _ZN22CMaxFatigueFactorEvent19GetMaxFatigueFactorEv @0x0812ce54 sz0xb：
//   push/mov; mov 0x8(%ebp),%eax; mov 0xc(%eax),%eax; pop; ret —— 返回 +0xc int。
// 报告缺失（objdump 转写）。消费方 CUserOnlyOrig.cpp（A-2 域，禁碰）。
// ---------------------------------------------------------------------------
class CMaxFatigueFactorEvent
{
public:
    int GetMaxFatigueFactor();   // ORIG W 0x0812ce54

    char m_pad0c[0xc];           // +0x00..+0x0b（布局未还原区）
    int m_fatigueFactor;         // +0x0c（GetMaxFatigueFactor 返回值）
};

#include "LinkClosureE/GetMaxFatigueFactor.inc"
{
    return m_fatigueFactor;
}

// ---------------------------------------------------------------------------
// CBuffGroup（CBelong 派生，ORIG vtable @0x08cf3998 slot0=IsRight）
// CBuffGroup::CBuffGroup(CUser*) W @0x08697c3e：call CBelongC1 → 置 vptr
//   → this+0x04 = user。IsRight W @0x08697c64（报告缺失，objdump 转写）：
//   user 等级 >0x45 → 0；|dx|>0x12c 或 |dy|>0x12c → 0；否则 1。
// ---------------------------------------------------------------------------
class CBelong
{
public:
    virtual char IsRight(CUser* user) = 0;   // 抽象虚槽（ORIG 基类槽 __cxa_pure_virtual）
};

class CBuffGroup : public CBelong
{
public:
    CBuffGroup(CUser* user);                 // ORIG W 0x08697c3e
    char IsRight(CUser* user);               // ORIG W 0x08697c64（虚槽 slot0）

    CUser* m_user;                           // +0x04
};

#include "LinkClosureE/CBuffGroup.inc"


#include "LinkClosureE/IsRight.inc"


#include "LinkClosureE/SendLV70Buff.inc"


// ---------------------------------------------------------------------------
// CItemMakingSkill（charac_expand::CData 派生）
// ORIG vtable @0x08c9c340：slot1=loadData 0x08545202、slot2=getData 0x08545b42、
//   slot4/5=D1/D0、slot6=_saveData 0x08545276、slot7=_reset 0x085453ba ——
//   与 charac_expand::CData 虚序一致（ResetDailyMidnight/ResetDaily 走基类空实现）。
// ctor 0x08545108（objdump）：CData ctor → vptr → +0x1c = new CMTRand(time(0))；
// dtor 0x08545192：delete +0x1c 后置 0 → CData dtor。字段 +0x06..+0x1b 为
// Send_ItemMakingSkillList 逐槽读取的 11 项技能等级表（movzwl 0x6(%eax,%edx,2)）。
// 其它方法（UpSkillLevel/GainItemMakingSkillPoint 等）实现由后续批次分派。
// ---------------------------------------------------------------------------
class CMTRand;

class CItemMakingSkill : public charac_expand::CData
{
public:
    int Send_ItemMakingSkillList(CUser* user) const;   // ORIG T 0x08545a62
    int GetItemMakingSkillLevel(const CStackableItem* item, CUser* user);   // ORIG T 0x08545cf4
    int GetSkillLevel(ENUM_ITEM_MAKING_TYPE type, CUser* user) const;       // ORIG T 0x08545c0c
    void UpSkillLevel(ENUM_ITEM_MAKING_TYPE type, int point, CUser* user);  // ORIG T 0x0854571c
    void GainItemMakingSkillPoint(const CStackableItem* item, CUser* user); // ORIG T 0x085453e0
    void SetItemMakingSkillPoint(int skillIdx, int point, CUser* user);     // ORIG T 0x08545d24
    char m_pad5;                // +0x05（1 字节；语义未还原 [推断]）
    unsigned short m_skillLevels[11];   // +0x06..+0x1b（11 项等级表）
    CMTRand* m_rand;            // +0x1c（ctor new CMTRand(time(0))，dtor delete）
};

#include "LinkClosureE/Send_ItemMakingSkillList.inc"


// ---------------------------------------------------------------------------
// CItemMakingSkill 技能成长链（报告 docs/class_func_reports/CItemMakingSkill/）。
// ---------------------------------------------------------------------------

#include "LinkClosureE/GetSkillLevel.inc"


#include "LinkClosureE/GetItemMakingSkillLevel.inc"


#include "LinkClosureE/UpSkillLevel.inc"


#include "LinkClosureE/GainItemMakingSkillPoint.inc"



#include "LinkClosureE/send_mail.inc"


#include "LinkClosureE/SetItemMakingSkillPoint.inc"

