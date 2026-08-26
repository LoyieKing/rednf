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
bool AvatarRechargeServer::CalcDurabilityDetail(CUser* user, int equipSlot,
                                                std::vector<int>& rates)
{
    // ORIG 调用序：getCurCharacInvenRefW(user) → GetInvenRef(inven, 0, equipSlot)。
    // repo 中 Ref/W 两口径同返背包基址（均 enableSaveInven），按类型化 W 口径调用。
    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item* item = inven->GetInvenRef(0, equipSlot);
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
int advancealtar::AdvanceAltarShopParameter::getFieldType(std::string name)
{
    if (name.compare("level") == 0)
        return FieldType::FT_LEVEL;          // 0x00
    if (name.compare("index") == 0)
        return FieldType::FT_INDEX;          // 0x01
    if (name.compare("price") == 0)
        return FieldType::FT_PRICE;          // 0x02
    if (name.compare("name") == 0)
        return FieldType::FT_NAME;           // 0x03
    if (name.compare("cooltime") == 0)
        return FieldType::FT_COOLTIME;       // 0x04
    if (name.compare("need point") == 0)
        return FieldType::FT_NEED_POINT;     // 0x05
    if (name.compare("explain") == 0)
        return FieldType::FT_EXPLAIN;        // 0x06
    if (name.compare("type explain") == 0)
        return FieldType::FT_TYPE_EXPLAIN;   // 0x07
    if (name.compare("set slot") == 0)
        return FieldType::FT_SET_SLOT;       // 0x08
    if (name.compare("image index") == 0)
        return FieldType::FT_IMAGE_INDEX;    // 0x09
    if (name.compare("hp") == 0)
        return FieldType::FT_HP;             // 0x0a
    if (name.compare("dmg") == 0)
        return FieldType::FT_DMG;            // 0x0b
    if (name.compare("pic") == 0)
        return FieldType::FT_PIC;            // 0x0c
    if (name.compare("rcv") == 0)
        return FieldType::FT_RCV;            // 0x0d
    if (name.compare("sum") == 0)
        return FieldType::FT_SUM;            // 0x0e
    if (name.compare("mht") == 0)
        return FieldType::FT_MHT;            // 0x0f
    if (name.compare("rgn") == 0)
        return FieldType::FT_RGN;            // 0x10
    if (name.compare("msd") == 0)
        return FieldType::FT_MSD;            // 0x11
    if (name.compare("asd") == 0)
        return FieldType::FT_ASD;            // 0x12
    if (name.compare("sdt") == 0)
        return FieldType::FT_SDT;            // 0x13
    if (name.compare("fdt") == 0)
        return FieldType::FT_FDT;            // 0x14
    if (name.compare("etc") == 0)
        return FieldType::FT_ETC;            // 0x15
    if (name.compare("lrg") == 0)
        return FieldType::FT_LRG;            // 0x16
    if (name.compare("spl") == 0)
        return FieldType::FT_SPL;            // 0x17
    if (name.compare("etc2") == 0)
        return FieldType::FT_ETC2;           // 0x18
    return -1;
}

// ============================================================================
// 3. advancealtar::AdvanceAltarShopParameter::getdataType —— ORIG T 0x088a2b8e
//
// 字段类型 → 数据形态：无符号比较 ft<8 且位集 0xc8（bit3/6/7 = name/explain/
// type explain，字符串型字段）命中返 0（字符串），其余返 1（整数）。
// ============================================================================
int advancealtar::AdvanceAltarShopParameter::getdataType(FieldType::T ft)
{
    if (static_cast<unsigned int>(ft) < 8 &&
        ((1u << ft) & 0xc8u) != 0)
        return 0;
    return 1;
}
