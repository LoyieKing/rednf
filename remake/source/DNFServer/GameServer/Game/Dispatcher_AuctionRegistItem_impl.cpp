#include "Dispatchers.h"

// ============================================================================
// Dispatcher_AuctionRegistItem::dispatch_sig —— 还原 ORIG 0x08213e8a T（约 792 insn）
//
// 语义（依据 docs/class_func_reports/Dispatcher_AuctionRegistItem/dispatch_sig.md）：
//   客户端请求「拍卖行登记物品」。CMDPACKET=0xba。包首字节为支付方式 pay_type：
//     0 = 金币(gold)拍卖，走 PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA + s_auction_proxy
//     1 = 点券(cera)拍卖，走 PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP + s_cera_auction_proxy
//   校验链：get_byte 成功 → pay==1 且角色等级<0x1f 直接拒(0x75) → AuctionPreCheck(0xba,pay)
//           （内部：对应代理 IsRunning、state、Secu(0x35)；返回<0 才继续）
//           → ServiceRestrict(1,5) → Secu(0x32)
//   gold 分支：解析 inven_type/slot_no/item_id/add_info/price/instant_price/unit_price
//              + 3×ROI(short,byte)；find_item；GetRandomOptionGrade!=0 时把空 ROI high 置 0x7530；
//              遍历 CDataManager::m_serverParameter.m_auction480 限购表（m_itemId==-1 通配或
//              等于请求 itemId 且 curSec∈[m_startSec,m_endSec)）→ 结余包 header(1,0xba)|byte0|byte7|3×int，
//              CUser::Send 后 return 0；否则 GetInvenTypeFromItemSpace + CheckItemLock(0xd5) →
//              价格范围 + stackable 乘积校验 → 组 GA 包并 s_auction_proxy 发送。
//   cera 分支：解析并校验（item_id∈[0x28d288,0x28d299]、price 范围、add_info≤1、price==-1、
//              instant_price∈[0x32,0x895440]）→ 组 GP 包（header+uid+characno）并发送。
//   收尾：遍历本地 gmList[]（5 个 GM characNo）命中则 cMyTrace("Trace Auction Delay,...")。
//   全部失败路径即 SendCmdErrorPacket(0xba, errcode, pay) 后 return 0。
//
// 说明：CAuctionServerProxy/CCeraAuctionServerProxy 是 CAuction.cpp/GlobalData.cpp 的
// 局部类（跨 TU 类型不完整），此处经其唯一真实传输通道 BaseServerProxy::SendTcpPacket
// （ORIG 0x082fd830，SendPacket 内联等价）发送，行为与 ORIG 一致。
// ============================================================================

#include "AuctionHelpers.h"          // IsPriceError / IsValidMultiplyResult（ORIG T）
#include "BaseServerProxy.h"         // BaseServerProxy::SendTcpPacket（传输通道）
#include "PacketDesign.h"            // nsl::PACKET_HEADER（0x12）+ setCategory/setPacketID/setSize
#include "CDataManager.h"            // G_CDataManager、find_item、m_serverParameter
#include "ServerParameterScript.h"   // stAuctionLimitItemParam_t（m_auction480，12B）
#include "CInventory.h"              // CSecu_ProtectionField、GetInvenTypeFromItemSpace
#include "GlobalData.h"              // s_auction_proxy / s_cera_auction_proxy / s_pSecuProtectionField
#include "ServiceRestrictManager.h"  // ServiceRestrictManager
#include "Arad_DataManager.h"        // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "PacketGuard.h"             // PacketGuard（限购结余包）
#include "InterfacePacketBuf.h"      // InterfacePacketBuf::put_header / put_byte / put_int / finalize
#include "GameTypes.h"               // cMyTrace
#include "CompoundEmblemDeps.h"      // LineFunc(int, char const*, int, unsigned)



// ---- ROI 类别（ORIG 12B，ServerLab AuctionItem.h 同布局；Game 侧无此类型故本地定义） ----
class RoiCategory
{
public:
    union
    {
        struct
        {
            short _high[3];
            short _reserved;
        } _hcv;
        long long _high_key;
    } m_field0;

    union
    {
        struct
        {
            char _low[3];
        } _lcv;
        int _low_key;
    } m_field1;

    RoiCategory()
    {
        m_field0._high_key = 0;
        m_field1._low_key = 0;
    }

    void _sort()
    {
        short th;
        char tl;
        if (m_field0._hcv._high[0] > m_field0._hcv._high[1])
        {
            th = m_field0._hcv._high[0]; m_field0._hcv._high[0] = m_field0._hcv._high[1];
            m_field0._hcv._high[1] = th;
            tl = m_field1._lcv._low[0]; m_field1._lcv._low[0] = m_field1._lcv._low[1];
            m_field1._lcv._low[1] = tl;
        }
        if (m_field0._hcv._high[1] > m_field0._hcv._high[2])
        {
            th = m_field0._hcv._high[1]; m_field0._hcv._high[1] = m_field0._hcv._high[2];
            m_field0._hcv._high[2] = th;
            tl = m_field1._lcv._low[1]; m_field1._lcv._low[1] = m_field1._lcv._low[2];
            m_field1._lcv._low[2] = tl;
        }
        if (m_field0._hcv._high[0] > m_field0._hcv._high[1])
        {
            th = m_field0._hcv._high[0]; m_field0._hcv._high[0] = m_field0._hcv._high[1];
            m_field0._hcv._high[1] = th;
            tl = m_field1._lcv._low[0]; m_field1._lcv._low[0] = m_field1._lcv._low[1];
            m_field1._lcv._low[1] = tl;
        }
    }
};

// ---- 拍卖请求报文（ORIG PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA / _GP 同布局复用） ----
// ORIG 字段偏移见 ServerLab/Auction/AuctionServer/auction_header/AuctionPacket.h，
// GA 类别 0 / pkid 2；GP(点券) 类别 0x13 / pkid 2。gold+cera 处理器均按此布局强转读取。
struct PACK_AUCTION_ASK_REGISTED_ITEM_NUM : public nsl::PACKET_HEADER
{
    int char_idx;                    // +0x12
    int character_id;                // +0x16
    unsigned char inven_type;        // +0x1a
    unsigned short slot_no;          // +0x1b
    unsigned long item_id;           // +0x1d
    int add_info;                    // +0x21
    int price;                       // +0x25
    int instant_price;               // +0x29
    int unit_price;                  // +0x2d
    RoiCategory roi_key;             // +0x31（12B，总 0x3d）
};

// 拍卖 GM 角色号表（ORIG 静态数据，供延迟追踪比对）
static const int g_auctionGmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

namespace
{
const char* kPrettyFunc =
    "virtual int Dispatcher_AuctionRegistItem::dispatch_sig(CUser*, PacketBuf&)";
}

int Dispatcher_AuctionRegistItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = 0;

    unsigned char payTypeByte = 0;
    if (packet.get_byte(payTypeByte) != 1)
    {
        // ORIG get_byte 失败 → LineFunc(0xc294, ...)
        return LineFunc(0xc294, kPrettyFunc, 0, 0);
    }

    if (payTypeByte >= 2)
    {
        return LineFunc(0xc297, kPrettyFunc, 0, 0);
    }

    const PAY_TYPE pay = (PAY_TYPE)payTypeByte;   // 0=gold, 1=cera

    // ---- 支付方式 == cera 需角色等级达到 0x1f（31） ----
    if (pay == 1 && (user->get_charac_level() < 0x1f))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x75, pay);
        return 0;
    }

    // ---- 拍卖前置校验（代理是否运行 / state / Secu 0x35）；返回<0 才继续 ----
    result = user->AuctionPreCheck((ENUM_CMDPACKET)0xba, pay);
    if (result >= 0)
        return result;

    // ---- 服务限制 / 安全保护（0x32） ----
    ServiceRestrictManager* restrictMgr =
        ARAD::Singleton<ServiceRestrictManager>::Get();
    if (restrictMgr->isRestricted(user, (RestrictCategory::Enum)1, 5))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0xd1, pay);
        return 0;
    }
    unsigned int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x32);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, (unsigned char)(secuErr & 0xff), pay);
        return 0;
    }

    if (pay == 1)
    {
        // ================= cera（点券）分支：PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP =================
        PACK_AUCTION_ASK_REGISTED_ITEM_NUM pck;
        memset(&pck, 0, sizeof(pck));
        pck.setCategory(0x13);
        pck.setPacketID(2);
        pck.setSize((unsigned int)sizeof(pck));

        pck.char_idx = (int)user->GetUID();
        pck.character_id = user->getCurCharacNo();

        unsigned char invenType = 0;
        if (packet.get_byte(invenType) != 1)
            return LineFunc(0xc38a, kPrettyFunc, 0, 0);
        unsigned short slotNo = 0;
        if (packet.get_short(slotNo) != 1)
            return LineFunc(0xc38c, kPrettyFunc, 0, 0);

        unsigned int itemId = 0;
        if (packet.get_int(itemId) != 1)
            return LineFunc(0xc392, kPrettyFunc, 0, 0);
        int addInfo = 0;
        if (packet.get_int(addInfo) != 1)
            return LineFunc(0xc395, kPrettyFunc, 0, 0);
        int price = 0;
        if (packet.get_int(price) != 1)
            return LineFunc(0xc396, kPrettyFunc, 0, 0);
        int instantPrice = 0;
        if (packet.get_int(instantPrice) != 1)
            return LineFunc(0xc397, kPrettyFunc, 0, 0);
        int unitPrice = 0;
        if (packet.get_int(unitPrice) != 1)
            return LineFunc(0xc398, kPrettyFunc, 0, 0);

        // ---- cera 限价 / 物品段校验 ----
        if (IsPriceError(price, instantPrice) || instantPrice >= 0x895441)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
            return 0;
        }
        if (itemId < 0x28d288 || itemId > 0x28d299)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (addInfo > 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (price != -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return LineFunc(0xc322, kPrettyFunc, 0, 0);
        }
        if (instantPrice < 0x32 || instantPrice > 0x895440)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
            return 0;
        }

        ((BaseServerProxy*)GlobalData::s_cera_auction_proxy)
            ->SendTcpPacket((char*)&pck, pck.getSize());
    }
    else
    {
        // ================= gold 分支：PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA =================
        PACK_AUCTION_ASK_REGISTED_ITEM_NUM pck;
        memset(&pck, 0, sizeof(pck));
        pck.setCategory(0);
        pck.setPacketID(2);
        pck.setSize((unsigned int)sizeof(pck));

        pck.char_idx = (int)user->GetUID();
        pck.character_id = user->getCurCharacNo();

        unsigned char invenType = 0;
        if (packet.get_byte(invenType) != 1)
            return LineFunc(0xc2fa, kPrettyFunc, 0, 0);
        pck.inven_type = invenType;
        unsigned short slotNo = 0;
        if (packet.get_short(slotNo) != 1)
            return LineFunc(0xc2fc, kPrettyFunc, 0, 0);
        pck.slot_no = slotNo;

        unsigned int itemId = 0;
        if (packet.get_int(itemId) != 1)
            return LineFunc(0xc302, kPrettyFunc, 0, 0);
        pck.item_id = itemId;
        int addInfo = 0;
        if (packet.get_int(addInfo) != 1)
            return LineFunc(0xc305, kPrettyFunc, 0, 0);
        pck.add_info = addInfo;
        int price = 0;
        if (packet.get_int(price) != 1)
            return LineFunc(0xc306, kPrettyFunc, 0, 0);
        pck.price = price;
        int instantPrice = 0;
        if (packet.get_int(instantPrice) != 1)
            return LineFunc(0xc307, kPrettyFunc, 0, 0);
        pck.instant_price = instantPrice;
        int unitPrice = 0;
        if (packet.get_int(unitPrice) != 1)
            return LineFunc(0xc308, kPrettyFunc, 0, 0);
        pck.unit_price = unitPrice;

        // ---- ROI 类别（3 × (short high, byte low)） ----
        RoiCategory roi;
        for (int i = 0; i < 3; ++i)
        {
            short high = 0;
            if (packet.get_short(high) != 1)
                return LineFunc(0xc3a2, kPrettyFunc, 0, 0);
            char low = 0;
            if (packet.get_byte(low) != 1)
                return LineFunc(0xc3a5, kPrettyFunc, 0, 0);
            roi.m_field0._hcv._high[i] = high;
            roi.m_field1._lcv._low[i] = low;
        }

        CDataManager* dm = G_CDataManager();
        CItem* item = dm->find_item((int)itemId);
        if (item == 0)
            return LineFunc(0xc3aa, kPrettyFunc, 0, 0);

        // 装备带随机选项（GetRandomOptionGrade!=0）：空 ROI high 槽置 0x7530（30000）后排序
        if (item->GetRandomOptionGrade() != 0)
        {
            for (int i = 0; i < 3; ++i)
            {
                if (roi.m_field0._hcv._high[i] == 0)
                    roi.m_field0._hcv._high[i] = 0x7530;
            }
        }
        roi._sort();
        pck.roi_key = roi;

        // ---- 拍卖限购检查（m_serverParameter.m_auction480） ----
        const std::vector<stAuctionLimitItemParam_t>& limitVec =
            dm->m_serverParameter.m_auction480;
        const int curSec = GlobalData::s_systemTime_.getCurSec();
        for (std::vector<stAuctionLimitItemParam_t>::const_iterator it = limitVec.begin();
             it != limitVec.end(); ++it)
        {
            if (it->m_itemId == -1 || (unsigned int)it->m_itemId == itemId)
            {
                if (it->m_startSec < curSec && curSec < it->m_endSec)
                {
                    // 该物品当前处于限购窗口：结余包 header(1,0xba)|byte0|byte7|3×int
                    PacketGuard pg;
                    ((InterfacePacketBuf*)&pg)->clear();
                    ((InterfacePacketBuf*)&pg)->put_header(1, 0xba);
                    ((InterfacePacketBuf*)&pg)->put_byte(0);
                    ((InterfacePacketBuf*)&pg)->put_byte(7);
                    ((InterfacePacketBuf*)&pg)->put_int(it->m_itemId);
                    ((InterfacePacketBuf*)&pg)->put_int(it->m_startSec);
                    ((InterfacePacketBuf*)&pg)->put_int(it->m_endSec);
                    ((InterfacePacketBuf*)&pg)->finalize(true);
                    user->Send(pg);
                    return 0;
                }
            }
        }

        // ---- 物品锁定 / 价格范围 / 可堆叠乘积校验 ----
        int invenType2 = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)invenType);
        if (user->CheckItemLock(invenType2, (int)slotNo))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0xd5, (PAY_TYPE)0);
            return 0;
        }
        if (price < -1 || instantPrice < -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, (PAY_TYPE)0);
            return 0;
        }
        if (instantPrice != -1 && instantPrice <= price)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, (PAY_TYPE)0);
            return 0;
        }
        CItem* item2 = dm->find_item((int)itemId);
        if (item2 == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (item2->is_stackable())
        {
            if (!IsValidMultiplyResult(price, addInfo) ||
                !IsValidMultiplyResult(instantPrice, addInfo))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
                return 0;
            }
        }

        ((BaseServerProxy*)GlobalData::s_auction_proxy)
            ->SendTcpPacket((char*)&pck, pck.getSize());
    }

    // ---- 收尾：GM 角色号延迟追踪 ----
    for (int i = 0; i < 5; ++i)
    {
        if (g_auctionGmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(kPrettyFunc, 0xc472, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionRegistItem",
                  user->getCurCharacNo());
            break;
        }
    }

    return 0;
}

int Dispatcher_AuctionRegistItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { (void)user; (void)msg; (void)param; return 0; }
int Dispatcher_AuctionRegistItem::read(PacketBuf& packet, MSG_BASE& msg) { (void)packet; (void)msg; return 0; }