// df_game_r Game/ AvatarRechargeC2 —— 独占恢复批次（AvatarRechargeC2）。
// 恢复以下 ORIG 真实符号（df_game_r nm T 实证）：
//   _ZN20AvatarRechargeServer20CalcDurabilityDetailEP5CUseriRSt6vectorIiSaIiEE   @0x081902e8
//   _ZN12advancealtar25AdvanceAltarShopParameter12getFieldTypeESs                @0x088a27a2
//   _ZN12advancealtar25AdvanceAltarShopParameter11getdataTypeENS_9FieldType1TE   @0x088a2b8e
// 依据 docs/class_func_reports/AvatarRechargeServer/CalcDurabilityDetail.md 与
// docs/class_func_reports/advancealtar__AdvanceAltarShopParameter/
// {getFieldType,getdataType}.md 的 ## 汇编 + ## 反编译 C 全文转写。
//
// 注：AvatarRoulette::swap 已由 StrikerWorldDropB2.cpp 提供（ORIG T 0x08911e4e），
// 本 TU 不重复定义。

#include <string>
#include <vector>

#include "AvatarRechargeServer.h"  // AvatarRechargeServer 布局（m_enduranceReduceRate +0x54 /
                                   // m_bUseEnduranceReduceRate +0x58）
#include "CAvatarItemMgr.h"        // WongWork::CAvatarItemMgr::GetEmblemReduceRate/UpdateEndurance
#include "CDataManager.h"          // advancealtar::AdvanceAltarShopParameter / FieldType::T
#include "CInventory.h"            // CInventory::GetInvenRef / GetAvatarItemMgrW
#include "CUser.h"                 // CUserCharacInfo::getCurCharacInvenW
#include "GameRand.h"              // get_rand_int
#include "Inven_Item.h"

// ============================================================================
// 1. AvatarRechargeServer::CalcDurabilityDetail —— ORIG T 0x081902e8
//
// 头像耐久掷点递减：取当前角色背包 0 号区槽位物品；空槽或耐久计数为 0 返 false。
// 否则取减免率：Gm 开关(+0x58)==1 用 GM 值(+0x54 低 16 位)，否则查徽章耐久表
// （CAvatarItemMgr::GetEmblemReduceRate，键=item m_addInfo2 即 avatar uid）；
// 率 >0 且 get_rand_int(1000) < 率 时耐久 -1、经 UpdateEndurance 上报 DB 保存流，
// 并把槽位号 push 进 rates。除空槽分支外恒返 true。
// ============================================================================
// [b28d-D43] N82/N89 大函数（90/92）（ORIG 0x081902e8）。
bool AvatarRechargeServer::CalcDurabilityDetail(CUser* user, int equipSlot,
                                                std::vector<int>& rates)
{
    // ORIG 调用序：getCurCharacInvenRefW(user) → GetInvenRef(inven, 0, equipSlot)。
    // ORIG 无 inven 具名槽（P0：结果 %eax 直传 GetInvenRef 首参；equipSlot 跨
    // 该 call 由 callee-saved 保活）⇒ 内联成员链，勿落局部（W6：旧形多 1 槽
    // ⇒ 全槽后移 + 帧 0x28→0x38）。
    Inven_Item* item = user->getCurCharacInvenRefW()->GetInvenRef(0, equipSlot);
    if (item->isEmpty() || item->m_fieldSplit.m_avatarEndur == 0)
        return false;

    // ORIG：getCurCharacInvenW(user) → GetAvatarItemMgrW(inven)。
    WongWork::CAvatarItemMgr* mgr =
        user->getCurCharacInvenW()->GetAvatarItemMgrW();

    // 减免率：GM 开关命中取 GM 值低 16 位，否则取徽章耐久表值低 16 位（未命中 0）。
    short rate;
    if (m_bUseEnduranceReduceRate)
        rate = static_cast<short>(m_enduranceReduceRate);
    else
        rate = static_cast<short>(mgr->GetEmblemReduceRate(item->m_addInfo2));

    if (rate <= 0)
        return true;
    if (item->m_fieldSplit.m_avatarEndur == 0)
        return true;

    // 掷点：get_rand_int(1000) 有符号比较 < rate 才递减。
    int rolled = get_rand_int(1000);
    if (rolled >= rate)
        return true;

    item->m_fieldSplit.m_avatarEndur -= 1;
    mgr->UpdateEndurance(item->m_addInfo2,
                         item->m_fieldSplit.m_avatarEndur);
    rates.push_back(equipSlot);
    return true;
}

// ============================================================================
// 2. advancealtar::AdvanceAltarShopParameter::getFieldType —— ORIG T 0x088a27a2
//
// 字段名 → FieldType::T 编号；逐个 string::compare 命中即返回，未命中返回 -1。
// 名称顺序与编号严格按报告（level=0 … etc2=24）。
// ============================================================================
// [b28d-D43][W6] 判据⑫ 命中：ORIG 有具名 int ret（-0xc，首条 `movl $0xffffffff`
// + 命中点 `movl $N,-0xc` + 尾部 `mov -0xc(%ebp),%eax`）⇒ if/else-if 链 +
// 单出口 return（我方旧形 25 个 early-return 缺该槽，帧 0x10 vs ORIG 0x28）。
// ORIG 0x088a27a2。
int advancealtar::AdvanceAltarShopParameter::getFieldType(std::string name)
{
    int fieldType = -1;
    if (name.compare("level") == 0)
        fieldType = FieldType::FT_LEVEL;          // 0x00
    else if (name.compare("index") == 0)
        fieldType = FieldType::FT_INDEX;          // 0x01
    else if (name.compare("price") == 0)
        fieldType = FieldType::FT_PRICE;          // 0x02
    else if (name.compare("name") == 0)
        fieldType = FieldType::FT_NAME;           // 0x03
    else if (name.compare("cooltime") == 0)
        fieldType = FieldType::FT_COOLTIME;       // 0x04
    else if (name.compare("need point") == 0)
        fieldType = FieldType::FT_NEED_POINT;     // 0x05
    else if (name.compare("explain") == 0)
        fieldType = FieldType::FT_EXPLAIN;        // 0x06
    else if (name.compare("type explain") == 0)
        fieldType = FieldType::FT_TYPE_EXPLAIN;   // 0x07
    else if (name.compare("set slot") == 0)
        fieldType = FieldType::FT_SET_SLOT;       // 0x08
    else if (name.compare("image index") == 0)
        fieldType = FieldType::FT_IMAGE_INDEX;    // 0x09
    else if (name.compare("hp") == 0)
        fieldType = FieldType::FT_HP;             // 0x0a
    else if (name.compare("dmg") == 0)
        fieldType = FieldType::FT_DMG;            // 0x0b
    else if (name.compare("pic") == 0)
        fieldType = FieldType::FT_PIC;            // 0x0c
    else if (name.compare("rcv") == 0)
        fieldType = FieldType::FT_RCV;            // 0x0d
    else if (name.compare("sum") == 0)
        fieldType = FieldType::FT_SUM;            // 0x0e
    else if (name.compare("mht") == 0)
        fieldType = FieldType::FT_MHT;            // 0x0f
    else if (name.compare("rgn") == 0)
        fieldType = FieldType::FT_RGN;            // 0x10
    else if (name.compare("msd") == 0)
        fieldType = FieldType::FT_MSD;            // 0x11
    else if (name.compare("asd") == 0)
        fieldType = FieldType::FT_ASD;            // 0x12
    else if (name.compare("sdt") == 0)
        fieldType = FieldType::FT_SDT;            // 0x13
    else if (name.compare("fdt") == 0)
        fieldType = FieldType::FT_FDT;            // 0x14
    else if (name.compare("etc") == 0)
        fieldType = FieldType::FT_ETC;            // 0x15
    else if (name.compare("lrg") == 0)
        fieldType = FieldType::FT_LRG;            // 0x16
    else if (name.compare("spl") == 0)
        fieldType = FieldType::FT_SPL;            // 0x17
    else if (name.compare("etc2") == 0)
        fieldType = FieldType::FT_ETC2;           // 0x18
    return fieldType;
}

// ============================================================================
// 3. advancealtar::AdvanceAltarShopParameter::getdataType —— ORIG T 0x088a2b8e
//
// 字段类型 → 数据形态：无符号比较 ft<8 且位集 0xc8（bit3/6/7 = name/explain/
// type explain，字符串型字段）命中返 0（字符串），其余返 1（整数）。
// ============================================================================
// [b28d-D43][W6] N6 位折叠攻形：内联 `1u<<ft` 被 4.4 折 `(0xc8>>t)&1`；
// 改裸 register（P231，非绑定，不占槽）承载移位基值，4.4 保 `shl %cl` 形（ORIG 0x088a2b8e）。
int advancealtar::AdvanceAltarShopParameter::getdataType(FieldType::T ft)
{
    if (static_cast<unsigned int>(ft) < 8)
    {
        register unsigned int bit = 1u;
        bit <<= ft;
        if ((bit & 0xc8u) != 0)
            return 0;
    }
    return 1;
}
