#include "Dispatchers.h"

#include <vector>
#include <cstring>

#include "CUser.h"                 // CUser 全套（get_state/GetCera/GetUID/GetServerGroup/
                                   //   IsRestrictedGoods/SetRestrictedGoods/SendCmdErrorPacket/CheckInTrade/getRegTime）
#include "CUserCharacInfo.h"       // getCurCharacInvenR/get_charac_job/level/getCurCharacCreateTime/
                                   //   GetWinPoint/getLastItemLimitEditionBuyTime/updateLastItemLimitEditionBuyTime
#include "CInventory.h"            // get_money + CSecu_ProtectionField::Check + SECURITY_PROTCTION
#include "CPrivateStoreMgr.h"      // private_store::GetInstancePrivateStoreMgr / IsBusyPrivateStore
#include "CDataManager.h"          // G_CDataManager / find_item / getItemList / getItemLimitEditionMgr
#include "CItemLimitEdition.h"     // CItemLimitEdition / CItemLimitEditionMgr / stItemLimitEditionItemInfo_t
#include "CCeraShop.h"             // WongWork::CCeraShop::checkBuyingCondition / IsSpecialItem
#include "GlobalData.h"            // s_systemTime_ / s_pCeraShop / s_monitor_proxy_mgr / s_pSecuProtectionField
#include "CMonitorServerProxy.h"   // CMonitorServerProxy::SendTcpPacket
#include "CServerProxyMgr.h"       // CServerProxyMgr<T>::GetServerProxy
#include "CEquipItem.h"            // stAvatarTypeSelect_t / CEquipItem::m_avatarSelect
#include "CItemList.h"             // CItemList::GetRestrictCode
#include "CSystemTime.h"           // CSystemTime::getCurSec
#include "Packet_Item_Limit_Edition_Buyable_Query.h"  // 权威头（shared/packet）：ORIG _ZN39Packet_Item_Limit_Edition_Buyable_QueryC1Ev @0x0822c064

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T。
// 权威声明见 CompoundEmblemDeps.h；本 TU 用普通 extern 声明（非 asm 桥），定义由 CompoundEmblemDeps.cpp 提供。
extern int LineFunc(int, const char*, int, unsigned int);

// ============================================================================
// Dispatcher_BuyCeraShopLimitItem::dispatch_sig 还原
//   ORIG _ZN31Dispatcher_BuyCeraShopLimitItem12dispatch_sigEP5CUserR9PacketBuf @ 0x0821f9ba T
// 报告：docs/class_func_reports/Dispatcher_BuyCeraShopLimitItem/{dispatch_sig,_checkUserState,_checkBuyCondition}.md
//
// 语义：瑟拉商城「限购版」购买。解析商品 IPG/数量/记录数组 → _checkUserState 校验用户状态 →
// 取 CItemLimitEdition（getItemLimitEditionMgr→getItemInfo）→ 校验售罄/售价期/余额/限购条件 →
// CCeraShop::checkBuyingCondition → 限制道具处理（GetRestrictCode/IsRestrictedGoods/SetRestrictedGoods）→
// 更新购买时间 → 组 Packet_Item_Limit_Edition_Buyable_Query 发监控代理。
//
// 异常约定同 ORIG：业务校验失败 throw 4 字节 int（ENUM_ERROR 仅类型标签），try 外层 catch 后
// SendCmdErrorPacket(0x100, err) 并 return 0。
//
// _checkUserState / _checkBuyCondition 是该类独立 ORIG 成员（_ZNK…15_checkUserState /
// _ZNK…18_checkBuyCondition），未在只读头 Dispatchers.h 声明，故本 TU 无法产出这两个成员符号；
// 依同域惯例以文件内 static 函数内联其逻辑供 dispatch_sig 调用。
// ============================================================================

// _checkUserState_inline —— ORIG 0x0821f72a：CSecu Check(0x39)→IsBusyPrivateStore→CheckInTrade→
//   getCurSec()-lastBuyTime<0x1e→9。返回 0=通过，否则错误码。
static int checkUserState_inline(CUser* user)
{
    int ret = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x39);
    if (ret == 0) {
        if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user)) {
            ret = 0x13;
        } else if (user->CheckInTrade()) {
            ret = 0x13;
        } else {
            int curSec = GlobalData::s_systemTime_.getCurSec();
            int lastBuy = user->getLastItemLimitEditionBuyTime();
            ret = (curSec - lastBuy < 0x1e) ? 9 : 0;
        }
    }
    return ret;
}

// _checkBuyCondition_inline —— ORIG 0x0821f7cc：逐条件校验职业/等级/建号时间/账号注册时间区间，
//   任一不符返回 7，全过返回 0。
static int checkBuyCondition_inline(CUser* user, const CItemLimitEdition* edition)
{
    int job = edition->getCondCharacJob();
    if (job < 0xb) {
        if (user->get_charac_job() != job) return 7;
    }

    unsigned short lvBegin = edition->getCondCharacLevelBegin();
    if (lvBegin != 0 && (unsigned short)lvBegin > (unsigned short)user->get_charac_level())
        return 7;

    unsigned short lvEnd = edition->getCondCharacLevelEnd();
    if (lvEnd != 0 && (unsigned short)user->get_charac_level() > (unsigned short)lvEnd)
        return 7;

    int ctBegin = edition->getCondCharacCreateTimeBegin();
    if (ctBegin != 0 && ctBegin > user->getCurCharacCreateTime())
        return 7;

    int ctEnd = edition->getCondCharacCreateTimeEnd();
    if (ctEnd != 0 && user->getCurCharacCreateTime() > ctEnd)
        return 7;

    int atBegin = edition->getCondAccCreateTimeBegin();
    if (atBegin != 0 && atBegin > user->getRegTime())
        return 7;

    int atEnd = edition->getCondAccCreateTimeEnd();
    if (atEnd != 0 && atEnd < user->getRegTime())
        return 7;

    return 0;
}

// ============================================================================
// dispatch_sig
// ============================================================================
int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // 用户状态必须 >=3（ORIG: get_state < 3 → LineFunc(0xd887) 断言路径）。
    if (user->get_state() < 3)
        return LineFunc(0xd887,
                        "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);

    // ---------------- 报文解析（ORIG 依次读字段） ----------------
    // 第一条记录区：{ int; uchar subCount; uchar count(<0x24); [item_idx, uchar]*count }
    unsigned int ipgNo = 0;          // 首个 int（IPG/商品编号，再 a 传 checkBuyingCondition）
    unsigned char subCount = 0;      // 首个 byte（再 d 传 checkBuyingCondition）
    unsigned char entryCnt1 = 0;     // 第一条记录数（<0x24）
    if (!packet.get_int(ipgNo))
        return LineFunc(0xd88c,
                        "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    if (!packet.get_byte(subCount))
        return LineFunc(0xd88d,
                        "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    if (!packet.get_byte(entryCnt1) || entryCnt1 >= 0x24)
        return LineFunc(0xd890,
                        "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);

    // 第一条记录：每条 { unsigned long itemIdx; unsigned char cnt }（ORIG 栈数组，上限 0x23）。
    struct RawEntry1 { unsigned long itemIdx; unsigned char cnt; };
    RawEntry1 rec1[0x24];
    for (unsigned int i = 0; i < entryCnt1; ++i) {
        if (!packet.get_item_idx(rec1[i].itemIdx))
            return LineFunc(0xd894,
                            "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                            0, 0);
        if (!packet.get_byte(rec1[i].cnt))
            return LineFunc(0xd895,
                            "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                            0, 0);
    }

    // 第二条记录区：count(<0x24)；每条 { unsigned long itemIdx; ushort; ushort }（stride 8）。
    //   ORIG 同时校验两条 item_idx+ushort 组合不重复（重复 → LineFunc(0xd8ac)）。
    unsigned char entryCnt2 = 0;
    if (!packet.get_byte(entryCnt2) || entryCnt2 >= 0x24)
        return LineFunc(0xd89d,
                        "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    struct RawEntry2 { unsigned long itemIdx; unsigned short v1; unsigned short v2; };
    RawEntry2 rec2[0x24];
    for (unsigned int i = 0; i < entryCnt2; ++i) {
        if (!packet.get_item_idx(rec2[i].itemIdx))
            return LineFunc(0xd8a1,
                            "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                            0, 0);
        if (!packet.get_short(rec2[i].v1))
            return LineFunc(0xd8a2,
                            "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                            0, 0);
        if (!packet.get_short(rec2[i].v2))
            return LineFunc(0xd8a3,
                            "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                            0, 0);
    }
    for (unsigned int i = 0; i < (unsigned int)entryCnt2; ++i) {
        for (unsigned int j = i + 1; j < (unsigned int)entryCnt2; ++j) {
            if (rec2[i].itemIdx == rec2[j].itemIdx && rec2[i].v1 == rec2[j].v1)
                return LineFunc(0xd8ac,
                                "virtual int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser*, PacketBuf&)",
                                0, 0);
        }
    }

    // ---------------- 业务（ORIG 整体包在 try 中，失败 throw ENUM_ERROR 载荷 int） ----------------
    CItemLimitEdition* edition = 0;
    CItem* relatedItem = 0;      // relateditem!=1 时 find_item 得到的相关道具
    try
    {
        int stateRet = checkUserState_inline(user);
        if (stateRet != 0) throw (int)stateRet;

        // ORIG 传参：getItemInfo 的 key 是首个 byte（subCount），见 ORIG 汇编 +0x4dd mov %ebx(=local_22a),0x4(%esp)。
        edition = G_CDataManager()->getItemLimitEditionMgr()->getItemInfo((unsigned int)subCount);
        if (edition == 0) throw 0x16;

        if (edition->isSellComplete()) throw 0x5f;
        if (edition->getSellEndTime() < GlobalData::s_systemTime_.getCurSec()) throw 0x16;

        if (edition->getRelateditem() != 1) {
            relatedItem = G_CDataManager()->find_item(edition->getRelateditem());
            if (relatedItem == 0) throw 0x16;
        }

        // 余额校验：瑟拉点 / 金币 / 胜利点。
        if (user->GetCera() < (unsigned int)edition->getPriceCera()) throw 0xb;
        if (user->getCurCharacInvenR()->get_money() < edition->getPriceGold()) throw 0xa;
        if ((unsigned int)user->GetWinPoint() < (unsigned int)edition->getPriceWinPoint()) throw 0xc;

        if (checkBuyCondition_inline(user, edition) != 0) throw 7;

        // ---- avatar 类型选取 ----
        unsigned short avatarTypeSel = (unsigned short)edition->getAvatarTypeSelect();
        if (relatedItem != 0) {
            CEquipItem* equip = dynamic_cast<CEquipItem*>(relatedItem);
            if (equip != 0 && edition->getAvatarTypeSelect() == (unsigned char)0xff) {
                // 遍历 relatedItem 的 m_avatarSelect，跳过 m_field10(小孔数)<=0 的项，
                // 停在首个小孔数>0 的项，avatarTypeSel 随之递增。
                const std::vector<stAvatarTypeSelect_t>& sel = equip->m_avatarSelect;
                for (std::vector<stAvatarTypeSelect_t>::const_iterator it = sel.begin();
                     it != sel.end(); ++it)
                {
                    ++avatarTypeSel;
                    if (it->m_field10 > 0) break;
                }
            }
        }

        // ---- CCeraShop 购买条件（ORIG 参数映射）----
        int ok = GlobalData::s_pCeraShop->checkBuyingCondition(
            user, ipgNo, (unsigned long)edition->getRelateditem(),
            (unsigned int)edition->getRelatedItemCnt(),
            (unsigned short)avatarTypeSel, subCount, (unsigned short)avatarTypeSel);
        if (ok != 1) throw 7;

        // ---- 限制/特供道具 ----
        // ORIG：local_2c(=restrictCode) 初始化为 featuredIdx；featuredIdx==0 时 IsSpecialItem
        //   将 outType/outValue 写入独立 scratch(local_58[1]/local_58[0])，不污染 restrictCode；
        //   若为特供，restrictCode 才由 GetRestrictCode(relateditem) 覆写。
        bool special = false;
        int restrictCode = edition->getFeaturedIdx();
        if (restrictCode == 0) {
            int outType = 0;
            int outValue = 0;
            bool flag = false;
            if (GlobalData::s_pCeraShop->IsSpecialItem(edition->getRelateditem(),
                                                       outType, outValue, flag))
                special = true;
        }
        if (special) {
            restrictCode = G_CDataManager()->getItemList()->GetRestrictCode(edition->getRelateditem());
        }
        if (restrictCode != 0) {
            if (user->IsRestrictedGoods(restrictCode)) throw 0x12;
            user->SetRestrictedGoods(restrictCode);
        }

        // ---- 更新限购购买时间 ----
        user->updateLastItemLimitEditionBuyTime(GlobalData::s_systemTime_.getCurSec());

        // ---- 组监控查询包并发送 ----
        Packet_Item_Limit_Edition_Buyable_Query query;
        query.m_seq   = user->GetUID();
        // ORIG 包布局：m_ipgno(+0xe) 与解析首个 byte(subCount, ebp-0x22a) 同址，故填 subCount。
        query.m_ipgno = (unsigned int)subCount;
        // m_payload 填入第二条记录（每条 itemIdx + short + short，stride 8）。
        if (entryCnt2 > 0) {
            std::memcpy(query.m_payload, &rec2[0], entryCnt2 * sizeof(RawEntry2));
        }

        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket((char*)&query, (int)sizeof(query));
    }
    catch (int err)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x100, (unsigned char)err);
    }

    return 0;
}

int Dispatcher_BuyCeraShopLimitItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { (void)user; (void)msg; (void)param; return 0; }
int Dispatcher_BuyCeraShopLimitItem::read(PacketBuf& packet, MSG_BASE& msg) { (void)packet; (void)msg; return 0; }