#include "Dispatchers.h"

// ============================================================================
// Dispatcher_AuctionRegistItem::dispatch_sig —— 还原 ORIG 0x08213e8a T
// （range [0x08213e8a, 0x08214b95]，792 insn；报告
//   docs/class_func_reports/Dispatcher_AuctionRegistItem/dispatch_sig.md
//   汇编段 + Ghidra C 段均已通读）
//
// 语义（CMDPACKET=0xba「拍卖行登记物品」）：包首字节 pay_type。
//   pay==1（cera/点券）→ PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP（ctor ORIG W 0x0822e9b4，
//           sizeof 0x2d，category 0x12/pkid 2）→ CCeraAuctionServerProxy::SendPacket。
//   pay==0（gold/金币）→ PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA（ctor ORIG W 0x0822e54e，
//           sizeof 0x3d，category 0/pkid 2，含 ROI_Category@+0x31）→
//           CAuctionServerProxy::SendPacket。
//   校验链：get_byte 成功 → pay>1 拒 → pay==1 且角色等级<0x1f 拒(0x75) →
//           AuctionPreCheck(0xba,pay)（>=0 直接返回）→ ServiceRestrict(1,5) → Secu(0x32)。
//   GP 分支：读 inven_type/slot_no/item_id/add_info/price/instant_price（无 unit_price、
//           无 ROI）→ IsPriceError/上限、item_id∈[0x28d288,0x28d299]、add_info<=1、
//           price==-1、instant∈[0x32,0x895440] → 发包。
//   GA 分支：多读 unit_price + 3×ROI(short,byte)；find_item；IsRandomOption() 时把空 ROI
//           high 置 0x7530；ROI_Category::_sort()；遍历
//           CDataManager::m_serverParameter.m_auction480 限购表（m_itemId==-1 通配或等于
//           请求 itemId，且 curSec∈(m_startSec,m_endSec)）→ 结余包 header(1,0xba)|0|7|3×int
//           经 CUser::Send 后 return 0；否则 GetInvenTypeFromItemSpace + CheckItemLock(0xd5)
//           → 价格范围 + stackable 乘积校验 → 发包。
//   收尾：遍历文件级静态 gmList[5]（ORIG d _ZL6gmList）命中则
//         cMyTrace("Trace Auction Delay, %s(%d)", "REQ AuctionRegistItem", characNo)。
//
// 说明（2 类型 1 ctor 建模）：ORIG 在两个分支分别构造 *不同* 的包类型，各一次 ctor
// 调用（0x08214042 → GP ctor；0x082143a7 → GA ctor）；ORIG 中不存在独立的
// ROI_Category 局部对象（ROI 直接读进 GA 包 +0x31 字段，ROI_Category ctor 只出现在
// GA ctor 内部），故本 TU 不再自建 RoiCategory / 合并型 PACK_ 结构。
// ============================================================================

#include "AuctionHelpers.h"          // IsPriceError / IsValidMultiplyResult（ORIG T）
#include "AuctionProxy.h"            // CAuctionServerProxy / CCeraAuctionServerProxy::SendPacket
#include "PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA.h"   // ORIG ctor W 0x0822e54e（sizeof 0x3d）
#include "PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP.h"   // ORIG ctor W 0x0822e9b4（sizeof 0x2d）
#include "ROI_Category.h"            // ROI_Category::_sort（ORIG W 0x0822ac30）
#include "CDataManager.h"            // G_CDataManager、find_item、m_serverParameter
#include "ServerParameterScript.h"   // stAuctionLimitItemParam_t（m_auction480，12B）
#include "CInventory.h"              // CSecu_ProtectionField、GetInvenTypeFromItemSpace
#include "CSystemTime.h"             // getCurSec（ORIG 0x080cbc9e）
#include "GlobalData.h"              // s_auction_proxy / s_cera_auction_proxy / s_pSecuProtectionField
#include "ServiceRestrictManager.h"  // ServiceRestrictManager
#include "Arad_DataManager.h"        // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "PacketGuard.h"             // PacketGuard（限购结余包）
#include "InterfacePacketBuf.h"      // put_header / put_byte / put_int / finalize
#include "GameTypes.h"               // cMyTrace
#include "CompoundEmblemDeps.h"      // LineFunc(int, char const*, int, unsigned)

// 拍卖 GM 角色号表（ORIG d _ZL6gmList @0x093701d8，文件级静态，5×int）
static int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

int Dispatcher_AuctionRegistItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char payType;                                   // -0x45
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc294, __PRETTY_FUNCTION__, 0, 0);

    if (payType > 1)
        return LineFunc(0xc297, __PRETTY_FUNCTION__, 0, 0);

    PAY_TYPE pay = (PAY_TYPE)payType;                        // -0x30（0=gold，1=cera）

    // ---- cera 拍卖需角色等级达到 0x1f（31） ----
    if (pay == 1)
    {
        if (user->get_charac_level() < 0x1f)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x75, pay);
            return 0;
        }
    }

    // ---- 拍卖前置校验（代理是否运行 / state / Secu 0x35）；返回 <0 才继续 ----
    int preCheck = user->AuctionPreCheck((ENUM_CMDPACKET)0xba, pay);   // -0x2c
    if (preCheck >= 0)
        return preCheck;

    // ---- 服务限制 / 安全保护（0x32） ----
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 5))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0xd1, pay);
        return 0;
    }

    unsigned int secuErr =                                   // -0x28
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x32);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, (unsigned char)secuErr, pay);
        return 0;
    }

    if (pay == 1)
    {
        // ============ cera（点券）分支：PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP ============
        PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP pck;            // -0x8d（ctor 0x0822e9b4）
        pck.char_idx = user->GetUID();
        pck.character_id = user->getCurCharacNo();

        if (packet.get_byte(pck.inven_type) != 1)
            return LineFunc(0xc2fa, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(pck.slot_no) != 1)
            return LineFunc(0xc2fc, __PRETTY_FUNCTION__, 0, 0);

        unsigned int itemId;                                 // -0x4c
        if (packet.get_int(itemId) != 1)
            return LineFunc(0xc302, __PRETTY_FUNCTION__, 0, 0);
        pck.item_id = itemId;

        if (packet.get_int(pck.add_info) != 1)
            return LineFunc(0xc305, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.price) != 1)
            return LineFunc(0xc306, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.instant_price) != 1)
            return LineFunc(0xc307, __PRETTY_FUNCTION__, 0, 0);

        // ---- cera 限价 / 物品段校验 ----
        if (IsPriceError(pck.price, pck.instant_price) || pck.instant_price > 0x895440)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
            return 0;
        }
        if (itemId < 0x28d288 || itemId > 0x28d299)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (pck.add_info > 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (pck.price != -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return LineFunc(0xc322, __PRETTY_FUNCTION__, 0, 0);
        }
        if (pck.instant_price < 0x32 || pck.instant_price > 0x895440)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
            return 0;
        }

        GlobalData::s_cera_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pck);
    }
    else
    {
        // ============ gold（金币）分支：PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA ============
        PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA pck;            // -0xca（ctor 0x0822e54e）
        pck.char_idx = user->GetUID();
        pck.character_id = user->getCurCharacNo();

        if (packet.get_byte(pck.inven_type) != 1)
            return LineFunc(0xc38a, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(pck.slot_no) != 1)
            return LineFunc(0xc38c, __PRETTY_FUNCTION__, 0, 0);

        unsigned int itemId;                                 // -0x50
        if (packet.get_int(itemId) != 1)
            return LineFunc(0xc392, __PRETTY_FUNCTION__, 0, 0);
        pck.item_id = itemId;

        if (packet.get_int(pck.add_info) != 1)
            return LineFunc(0xc395, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.price) != 1)
            return LineFunc(0xc396, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.instant_price) != 1)
            return LineFunc(0xc397, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.unit_price) != 1)
            return LineFunc(0xc398, __PRETTY_FUNCTION__, 0, 0);

        // ---- ROI 类别：3 × (short high, char low)，直接读入包 +0x31 ----
        // ORIG 调 PacketBuf::get_short(short&) / get_byte(char&)：ROI_Category.h 的
        // m_keys/m_marks 为 unsigned，故以引用转型选中与 ORIG 相同的重载（地址计算不变）。
        for (int i = 0; i < 3; ++i)                          // -0x14
        {
            if (packet.get_short(*(short*)&pck.m_roiCategory.m_keys[i]) != 1)
                return LineFunc(0xc3a2, __PRETTY_FUNCTION__, 0, 0);
            if (packet.get_byte(*(char*)&pck.m_roiCategory.m_marks[i]) != 1)
                return LineFunc(0xc3a5, __PRETTY_FUNCTION__, 0, 0);
        }

        CItem* item = G_CDataManager()->find_item(itemId);   // -0x24
        if (item == 0)
            return LineFunc(0xc3aa, __PRETTY_FUNCTION__, 0, 0);

        // 随机选项装备（虚 vptr+0x4c IsRandomOption）：空 ROI high 槽置 0x7530（30000）
        if (item->IsRandomOption())
        {
            for (int i = 0; i < 3; ++i)                      // -0x10
            {
                if (pck.m_roiCategory.m_keys[i] == 0)
                    pck.m_roiCategory.m_keys[i] = 0x7530;
            }
        }
        pck.m_roiCategory._sort();

        // ---- 拍卖限购检查（CDataManager+0x4e8 = m_serverParameter.m_auction480） ----
        std::vector<stAuctionLimitItemParam_t>& limitVec =                     // -0x20
            G_CDataManager()->m_serverParameter.m_auction480;
        unsigned int curSec = GlobalData::s_systemTime_.getCurSec();           // -0x1c
        for (std::vector<stAuctionLimitItemParam_t>::iterator it = limitVec.begin();
             it != limitVec.end(); ++it)                                       // -0x54 / -0x44
        {
            if (it->m_itemId == -1 || it->m_itemId == itemId)
            {
                if (it->m_startSec < curSec && curSec < it->m_endSec)
                {
                    // 限购窗口内：结余包 header(1,0xba)|byte0|byte7|3×int
                    PacketGuard pg;                                            // -0x60
                    pg.put_header(1, 0xba);
                    pg.put_byte(0);
                    pg.put_byte(7);
                    pg.put_int(it->m_itemId);
                    pg.put_int(it->m_startSec);
                    pg.put_int(it->m_endSec);
                    pg.finalize(true);
                    user->Send(pg);
                    return 0;
                }
            }
        }

        // ---- 物品锁定 / 价格范围 / 可堆叠乘积校验 ----
        if (user->CheckItemLock(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)pck.inven_type), pck.slot_no))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0xd5, (PAY_TYPE)0);
            return 0;
        }
        if (pck.price < -1 || pck.instant_price < -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, (PAY_TYPE)0);
            return 0;
        }
        if (pck.instant_price != -1 && pck.price >= pck.instant_price)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, (PAY_TYPE)0);
            return 0;
        }

        CItem* item2 = G_CDataManager()->find_item(itemId);  // -0x18
        if (item2 == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (item2->is_stackable())
        {
            // ORIG：call → xor $0x1,%eax → test %al,%al（bool 与 1 比较）
            if (IsValidMultiplyResult(pck.price, pck.add_info) != 1 ||
                IsValidMultiplyResult(pck.instant_price, pck.add_info) != 1)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
                return 0;
            }
        }

        GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pck);
    }

    // ---- 收尾：GM 角色号延迟追踪 ----
    for (int i = 0; i < 5; ++i)                              // -0xc
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            // ORIG：getCurCharacNo() 先于 cMyTrace ctor 求值（临时对象直接调用形态）
            cMyTrace(__PRETTY_FUNCTION__, 0xc472, 0)(
                "Trace Auction Delay, %s(%d)", "REQ AuctionRegistItem",
                user->getCurCharacNo());
            break;
        }
    }

    return 0;
}

int Dispatcher_AuctionRegistItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { (void)user; (void)msg; (void)param; return 0; }
int Dispatcher_AuctionRegistItem::read(PacketBuf& packet, MSG_BASE& msg) { (void)packet; (void)msg; return 0; }
