// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 UserMiscLink/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// UserMiscLink.cpp —— LinkHelper.UserMiscLink 批次独占 TU。
// 13 个 ORIG 符号 + 直接连带依赖（TotalMission::rewardMission、CRedeemItem
// GetItemPrice/GetRedeemInfoR/RedeemItem/_CheckVaildIndex_N_GetIter、
// LevelupSupportEventManger::GetItemInfoVector）。全部符号逐份读
// docs/class_func_reports/ 对应报告转写；无报告者 objdump 按地址转写并标注。
// 不得编辑：Dispatchers.cpp/DisPatcherCore.cpp/CUser.cpp/CUser.h/ChecksumAlgo.h。
// ============================================================================
#include <cstring>
#include <algorithm>

#include "CUser.h"                          // CUser / m_guildDB / m_levelupEvents / m_historyLog / ENUM_ERROR
#include "CUserCharacInfo.h"                // setCurCharChangingGrowthType/getCurCharFirstGrowType 等
#include "CInventory.h"                     // CInventory::delete_item/insertItemIntoInventory/use_money/update_item、GetInvenTypeFromItemSpace
#include "Inven_Item.h"                     // Inven_Item / RandomOption
#include "CItem.h"                          // CItem::make_item/is_stackable
#include "CDataManager.h"                   // G_CDataManager / find_item / get_GrowthWeaponEventReward / GuildParameterScript
#include "SkillSlot.h"                      // SkillSlot::clear_all_skills_both/set_parent、WongWork::CSkillChanger、addSkillOnCreateCharacter
#include "GameTypes.h"                      // cMyTrace
#include "LogManager.h"                     // LogManager::logFormat
#include "PacketGuard.h"                    // PacketGuard
#include "InterfacePacketBuf.h"             // put_header/put_byte/put_int/finalize
#include "Redeem_Item__CRedeemItem.h"       // Redeem_Item::CRedeemItem
#include "CPrivateStoreMgr.h"               // private_store::CPrivateStoreMgr::IsBusyPrivateStore / GetInstancePrivateStoreMgr
#include "CMailBoxHelper.h"                 // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"        // g_scriptStringManager_ / findString
#include "DB_SaveEventLevelupSupport.h"     // DB_SaveEventLevelupSupport::makeRequest
#include "DB_EventUpdateGrowthWeaponTimepiece.h"  // DB_EventUpdateGrowthWeaponTimepiece::makeRequest
#include "GrowthEquipEvent.h"               // GrowthEquipEvent
#include "HeroMissionEvent.h"               // HeroMissionEvent / HeroMissionValue
#include "HeroMissionCondition.h"           // BaseHeroMissionCondition
#include "TotalMission.h"                   // TotalMission
#include "LevelupSupportEventManger.h"      // LevelupSupportEventManger
#include "RandomOptionScript.h"             // RandomOptionScript / CRandomOptionGive / OptionGroup
#include "CItemGenerator.h"                 // random_option::CRandomOptionItemHandle
#include "PvPTypes.h"                       // CMTRand
#include "GlobalData.h"                     // GlobalData::s_auction_proxy / s_cera_auction_proxy
#include "AuctionProxy.h"                   // CAuctionServerProxy / CCeraAuctionServerProxy::IsRunning
#include "PacketTypes.h"                    // MSG_QUICK_CHAT

// CUser.cpp:140 同型声明（ORIG T 0x08668823，CUser.cpp 定义）
extern void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow);

// CRandomList<unsigned int> —— ORIG W functor（_ZN11CRandomListIjEclEj 0x085f5b6a，
// 16 字节；与 RandomRentalA2.cpp 同型定义，模板定义可跨 TU 重复）。
template <typename T>
class CRandomList
{
public:
    explicit CRandomList(unsigned int n)
        : m_value(0), m_n(n), m_sum(0), m_count(0)
    {
    }

    void operator()(unsigned int weight)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < weight)
        {
            m_value = m_count;
        }
        m_sum += weight;
        ++m_count;
    }
    // ORIG 形参按值（_ZN11CRandomListIjEclE11OptionGroup，无 RK 前缀）。
    // [L31 round2] ORIG 0x085f58d2（sz 0x58 = 34 条）**不是转发**，而是与
    // unsigned 版相同的体**在本函数内联展开**：`mov 0x8(%eax),%edx; mov 0x4(%eax),%eax;
    // cmp; ja` 起手，权重直接两次读形参槽 `0x10(%ebp)`（P0：无具名局部）。旧码
    // `(*this)(static_cast<unsigned int>(og.m_weight4));` 让 4.4 发 call（我方仅 10 条）。
    void operator()(OptionGroup og)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < (unsigned int)og.m_weight4)
        {
            m_value = m_count;
        }
        m_sum += (unsigned int)og.m_weight4;
        ++m_count;
    }

    // ORIG _ZN11CRandomListIjE9get_valueEv W @0x085f49a0（非 const，直读 +0）
    unsigned int get_value() { return m_value; }

private:
    unsigned int m_value;   // +0x00
    unsigned int m_n;       // +0x04
    unsigned int m_sum;     // +0x08
    unsigned int m_count;   // +0x0c
};

// ===========================================================================
// 1) CUser::ChangeGrowType_Item(int, short) —— ORIG T 0x0867b196
//    报告 docs/class_func_reports/CUser/ChangeGrowType_Item.md 全文转写。
// ===========================================================================
#include "UserMiscLink/ChangeGrowType_Item.inc"
}

// ===========================================================================
// 2) CUser::IsAddableGuildFund(int) —— ORIG T 0x0866c21e
//    报告 CUser/IsAddableGuildFund.md：fundLevel(+0x8d03b)<=0xf 时上限
//    20000000(0x1312d00)，否则 10000000(0x989680)。
// ===========================================================================
#include "UserMiscLink/IsAddableGuildFund.inc"
}

// ===========================================================================
// 3a) Redeem_Item::CRedeemItem 四方法 —— Redeem_Item 直接连带依赖
//     （报告 Redeem_Item__CRedeemItem/{GetItemPrice,GetRedeemInfoR,RedeemItem,
//      _CheckVaildIndex_N_GetIter}.md 全文转写）。
// ===========================================================================
namespace Redeem_Item
{

std::list<Inven_Item_Expand_RedeemInfo>::const_iterator
#include "UserMiscLink/_CheckVaildIndex_N_GetIter.inc"
}

#include "UserMiscLink/GetItemPrice.inc"
}

#include "UserMiscLink/GetRedeemInfoR.inc"
}

#include "UserMiscLink/RedeemItem.inc"
}

}  // namespace Redeem_Item
// ===========================================================================
// 3) CUser::Redeem_Item(int) —— ORIG T 0x08646df2
//    报告 CUser/Redeem_Item.md 全文转写。
// [SIGFIX:CUser::Redeem_Item] ORIG __PRETTY_FUNCTION__ 串为 bool（Main 117 件
// 清单）；返回值仅 0/1，CUser.h:3537 已声明 bool ⇒ 定义同步（mangled 不含返回类型）。
// ===========================================================================
#include "UserMiscLink/Redeem_Item.inc"
}

// ===========================================================================
// 4) CUser::get_pvp_exp_of_week() —— ORIG W 0x08230100（报告缺失，objdump 转写）
//    PvpResultType +0x08=m_expPoint、+0x10=m_field10（周重置基准）：
//    field10==-1 → expPoint，否则 expPoint - field10。
// ===========================================================================
#include "UserMiscLink/get_pvp_exp_of_week.inc"
}

// ===========================================================================
// 5) GrowthEquipEvent::SendInfinityGrowthWeapon —— ORIG T 0x080f74b2
//    报告 GrowthEquipEvent/SendInfinityGrowthWeapon.md 全文转写。
// ===========================================================================
#include "UserMiscLink/SendInfinityGrowthWeapon.inc"
}

// ===========================================================================
// 6) GuildParameterScript::getGuildCargoUpradeInfo(int) —— ORIG T 0x08979726
//    报告 GuildParameterScript/getGuildCargoUpradeInfo.md（拼写 Uprade 为
//    ORIG mangled 原样）：m_vec38 线性查找首字段==capacity。
// ===========================================================================
#include "UserMiscLink/getGuildCargoUpradeInfo.inc"
}

// ===========================================================================
// 7) HeroMissionEvent::rewardMission(CUser*, MissionNo::T) —— ORIG T 0x0816748e
//    报告 HeroMissionEvent/rewardMission.md 全文转写。
//    ORIG 入口经 vptr+0x34（CEventBase::IsEventing）判活；本头为布局镜像类，
//    等价读 m_isEventing（头注释口径）。
// ===========================================================================
#include "UserMiscLink/rewardMission.inc"
}

// ===========================================================================
// 7a) TotalMission::rewardMission —— 两参版 ORIG T 0x081663f4（报告全文：
//     函数体恒返回 1）；三参版 ORIG T 0x081663fe（rewardMission_081663fe.md）。
// ===========================================================================
#include "UserMiscLink/rewardMission_1.inc"
}

#include "UserMiscLink/rewardMission_2.inc"
}

// ===========================================================================
// 8/9/9a) LevelupSupportEventManger —— GetStepIndex 0x08147362 /
//         InsertItem 0x081473cc / GetItemInfoVector 0x08147724
// ===========================================================================
#include "UserMiscLink/GetStepIndex.inc"
}

std::vector<LevelupSupportSciprt::ItemInfo>*
#include "UserMiscLink/GetItemInfoVector.inc"
}

#include "UserMiscLink/InsertItem.inc"
}

// ===========================================================================
// 10) random_option::CRandomOptionItemHandle::set_random_option_give ——
//     ORIG T 0x085f0ea2（报告 3 行：m_optionSet(+0x14) = give）。
// ===========================================================================
namespace random_option
{

#include "UserMiscLink/set_random_option_give.inc"
}

// ===========================================================================
// 11) random_option::CRandomOptionItemHandle::change_option —— ORIG T 0x085f3340
//     报告 change_option.md 全文转写。参数语义（汇编栈序实证）：
//       itemIdx、rarity、level(→_get_option_type 第3参/_choose 的 optTypeByte)、
//       grade(→_choose 的 grade)、group(→_get_option_type 第4参/_choose 的 level)、
//       slot(0..2；排除第一组已占槽 + seed2 低 2 位)、opts。
//     读取第一组三槽（+0/+3/+6），把命中组写入第二组槽（+0xa..+0xc）与
//     seed2(+0xd)；随机池 m_rand[9]（+0x5808）/m_rand[10]（+0x61d0）。
// ===========================================================================
void* GetRandomOption();   // GlobalData.cpp 定义（T 0x085f0eb0：&G_CDataManager()->m_randomOption）

#include "UserMiscLink/change_option.inc"
}

}  // namespace random_option

// ===========================================================================
// 12) IsAuctionServerDisconnected(unsigned char) —— ORIG T 0x082138f2
//     （报告缺失，objdump 转写）：payType==0 检查 GlobalData::s_auction_proxy、
//     payType==1 检查 GlobalData::s_cera_auction_proxy；对应代理未运行 → true。
// ===========================================================================
#include "UserMiscLink/IsAuctionServerDisconnected.inc"
}

// ===========================================================================
// 13) MSG_QUICK_CHAT::isLastMessage() —— ORIG W 0x0822dc98（报告缺失，objdump：
//     返回 m_11 == 1）。声明 PacketTypes.h:1106（HdrFieldA 域），定义外联至此。
// ===========================================================================
#include "UserMiscLink/isLastMessage.inc"
}


// P147 显式实例化：发射 ORIG W _ZN11CRandomListIjE9get_valueEv @0x085f49a0。
template unsigned int CRandomList<unsigned int>::get_value();
