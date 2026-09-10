// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_BuyCeraShopLimitItem_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
// _ZNK…18_checkBuyCondition），已在 Dispatchers.h 私有区声明（const 成员），本 TU 定义产出 T 强符号。
// ============================================================================

// _checkUserState —— ORIG 0x0821f72a：CSecu Check(0x39)→IsBusyPrivateStore→CheckInTrade→
//   getCurSec()-lastBuyTime<0x1e→9。返回 0=通过，否则错误码。
// [N96 清绑 2026-09-09] 删除历史 `register bool isInInterval asm("al")`（P0 限定② /
// N96 全域禁用；P248 不得恢复）。报告 docs/class_func_reports/Dispatcher_BuyCeraShopLimitItem/
// _checkUserState.md 两段已读（Ghidra C 的 iVar2 - iVar3 < 0x1e ⇒ 我方三目条件）。
// ORIG 0x0821f7b2 `setle %al; test %al,%al; je`——isInInterval 值在 %al、无栈槽
// ⇒ 按 P0 双推律原码无该具名局部，还原为三目表达式直传。
// 残差 = N96+N18a 封顶（4.4 对整数比较恒直跳，不物化 setCC）。
// 读数：清绑前 [identical=6 ae=6 合计12 near=0 diff=2]，本函数 identical/ae；
//       清绑后 [identical=6 ae=5 合计11 near=0 diff=3]，verdict=DIFF
//       orig51/our49（缺 setle+test 两条）。
#include "Dispatcher_BuyCeraShopLimitItem_impl/_checkUserState.inc"
{
    int ret = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x39);
    if (ret != 0)
        return ret;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0x13;
    if (user->CheckInTrade())
        return 0x13;
    // [N96 清绑] 原 `register bool isInInterval asm("al")` 已删（P0/N96 全域禁绑）。
    // ORIG 0x0821f7b2 `setle %al; test %al,%al; je`——isInInterval 值在 %al、无
    // 栈槽 ⇒ 按 P0 双推律原码无该具名局部，还原为三目表达式直传（残差见函数头判词）。
    // [L31 round3 试形 · P225] 三目形被 4.4 折成 `jg` 直跳（旧记录 N96/N18a）；改用
    // `while (cond) { return X; }` 形试探 4.1 的 `setle %al; test %al,%al; je` 物化。
    while (GlobalData::s_systemTime_.getCurSec()
           - user->getLastItemLimitEditionBuyTime() < 0x1e)
    {
        return 9;
    }
    return 0;
}

// _checkBuyCondition —— ORIG 0x0821f7cc：逐条件校验职业/等级/建号时间/账号注册时间区间，
//   任一不符返回 7，全过返回 0。
#include "Dispatcher_BuyCeraShopLimitItem_impl/_checkBuyCondition.inc"
{
    // 职业条件（ORIG：getCondCharacJob 两次独立调用——P48 无 CSE；<0xb 走 setle 物化，
    // 内层 get_charac_job 先入 ebx 再与第二次 getCondCharacJob 比较）。
    if (edition->getCondCharacJob() < 0xb) {
        if (user->get_charac_job() != edition->getCondCharacJob())
            return 7;
    }

    // 等级区间（ORIG：begin/end 各两次调用；getter 结果入 ebx 跨另一 getter 调用存活，
    // 比较在 int 域带符号，故方向按 ORIG 汇编右操作数先入 ebx 的求值序书写）。
    if (edition->getCondCharacLevelBegin() != 0
        && edition->getCondCharacLevelBegin() > user->get_charac_level())
        return 7;

    if (edition->getCondCharacLevelEnd() != 0
        && edition->getCondCharacLevelEnd() < user->get_charac_level())
        return 7;

    // 建号时间区间。
    if (edition->getCondCharacCreateTimeBegin() != 0
        && edition->getCondCharacCreateTimeBegin() > user->getCurCharacCreateTime())
        return 7;

    if (edition->getCondCharacCreateTimeEnd() != 0
        && edition->getCondCharacCreateTimeEnd() < user->getCurCharacCreateTime())
        return 7;

    // 账号注册时间区间。
    if (edition->getCondAccCreateTimeBegin() != 0
        && edition->getCondAccCreateTimeBegin() > user->getRegTime())
        return 7;

    if (edition->getCondAccCreateTimeEnd() != 0
        && edition->getCondAccCreateTimeEnd() < user->getRegTime())
        return 7;

    return 0;
}

// ============================================================================
// dispatch_sig
// ============================================================================
#include "Dispatcher_BuyCeraShopLimitItem_impl/dispatch_sig.inc"
{
    // 用户状态必须 >=3（ORIG: get_state < 3 → LineFunc(0xd887) 断言路径）。
    if (user->get_state() < 3)
        return LineFunc(0xd887,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // ---------------- 报文解析（ORIG 依次读字段） ----------------
    // 第一条记录区：{ int; uchar subCount; uchar count(<0x24); [item_idx, uchar]*count }
    unsigned int ipgNo = 0;          // 首个 int（IPG/商品编号，再 a 传 checkBuyingCondition）
    unsigned char subCount = 0;      // 首个 byte（再 d 传 checkBuyingCondition）
    unsigned char entryCnt1 = 0;     // 第一条记录数（<0x24）
    if (!packet.get_int(ipgNo))
        return LineFunc(0xd88c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(subCount))
        return LineFunc(0xd88d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(entryCnt1) || entryCnt1 >= 0x24)
        return LineFunc(0xd890,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // 第一条记录：每条 { unsigned long itemIdx; unsigned char cnt }（ORIG 栈数组，上限 0x23）。
    // [P6S6 2026-09-11] ORIG 0x080d5xxx 寻址 `mov -0x40(%ebp),%edx; shl $0x2,%eax;
    //   add %edx,%eax; add $0x10,%eax; lea (%ecx,%eax,1); add $0x4` ⇒ 元素
    //   **stride = 5**（4i+i+0x10+4）、起点 buf+0x14；我方非 packed ⇒ stride 8 ⇒
    //   帧 0x4cc vs ORIG 0x26c。补 `pack(1)` 实测。
#pragma pack(push, 1)
    struct RawEntry1 { unsigned long itemIdx; unsigned char cnt; };
#pragma pack(pop)
    RawEntry1 rec1[0x24];
    for (int i = 0; i < entryCnt1; ++i) {
        if (!packet.get_item_idx(rec1[i].itemIdx))
            return LineFunc(0xd894,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(rec1[i].cnt))
            return LineFunc(0xd895,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }

    // 第二条记录区：count(<0x24)；每条 { unsigned long itemIdx; ushort; ushort }（stride 8）。
    //   ORIG 同时校验两条 item_idx+ushort 组合不重复（重复 → LineFunc(0xd8ac)）。
    unsigned char entryCnt2 = 0;
    if (!packet.get_byte(entryCnt2) || entryCnt2 >= 0x24)
        return LineFunc(0xd89d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    struct RawEntry2 { unsigned long itemIdx; unsigned short v1; unsigned short v2; };
    RawEntry2 rec2[0x24];
    for (int i = 0; i < entryCnt2; ++i) {
        if (!packet.get_item_idx(rec2[i].itemIdx))
            return LineFunc(0xd8a1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(rec2[i].v1))
            return LineFunc(0xd8a2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(rec2[i].v2))
            return LineFunc(0xd8a3,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }
    for (int i = 0; i < entryCnt2; ++i) {
        for (int j = i + 1; j < entryCnt2; ++j) {
            if (rec2[i].itemIdx == rec2[j].itemIdx && rec2[i].v1 == rec2[j].v1)
                return LineFunc(0xd8ac,
                                __PRETTY_FUNCTION__,
                                0, 0);
        }
    }

    // ---------------- 业务（ORIG 整体包在 try 中，失败 throw ENUM_ERROR 载荷 int） ----------------
    CItemLimitEdition* edition = 0;
    CItem* relatedItem = 0;      // relateditem!=1 时 find_item 得到的相关道具
    try
    {
        int stateRet = _checkUserState(user);
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

        // ORIG 0x082200be：ret=_checkBuyCondition(...)，ret!=0 时 throw ret（异常码=返回值）。
        int condRet = _checkBuyCondition(user, edition);
        if (condRet != 0) throw condRet;

        // ---- avatar 类型选取 ----
        unsigned short avatarTypeSel = (unsigned short)edition->getAvatarTypeSelect();
        // ORIG 0x08220132：先 relatedItem->IsCreatureItem()（vtable+0x10 虚调用）通过后才 dynamic_cast。
        if (relatedItem != 0 && relatedItem->IsCreatureItem()) {
            CEquipItem* equip = dynamic_cast<CEquipItem*>(relatedItem);
            if (equip != 0 && edition->getAvatarTypeSelect() == (unsigned char)0xff) {
                // b28d-lane25：P167 差集实证 ORIG 0x082201a1 调
                // CEquipItem::getAvatarTypeSelect()（返回 m_avatarSelect 视图）再 begin/end，
                // 非直读成员。
                // 遍历 relatedItem 的 m_avatarSelect，跳过 m_field10(小孔数)<=0 的项，
                // 停在首个小孔数>0 的项，avatarTypeSel 随之递增。
                const std::vector<stAvatarTypeSelect_t>& sel =
                    *static_cast<const std::vector<stAvatarTypeSelect_t>*>(
                        equip->getAvatarTypeSelect());
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
        if (!ok) throw 7;

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
            // b28d-lane25：P167 差集实证 ORIG 直读 CDataManager+0x0c（m_itemList）字段
            // 再调 GetRestrictCode（无 getItemList 调用）。
            restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(edition->getRelateditem());
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
        // b28d-lane25：P167 差集实证 ORIG 0x082203d4..0x08220412 无 m_payload 填充
        // （无 memcpy/循环，栈对象按 ctor 状态直发）⇒ 删除本重建段的 memcpy（P171 照抄）。

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

#include "Dispatcher_BuyCeraShopLimitItem_impl/process.inc"
#include "Dispatcher_BuyCeraShopLimitItem_impl/read.inc"
