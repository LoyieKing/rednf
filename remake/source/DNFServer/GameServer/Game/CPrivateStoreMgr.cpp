#include "CPrivateStoreMgr.h"
#include "CErrorGeneratorPrivate.h"   // private_store::CErrorGeneratorPrivate / g_ErrorGenerator
#include "MoreTypes.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "CUser.h"
#include "PacketGuard.h"
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString（RecallToMail 文案）
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail
#include "AuctionProxy.h"             // CAuctionServerProxy::IsRunning/SendPacket
#include "PacketDesign.h"             // nsl::PACKET_HEADER / PACKET_CTOR_BODY
#include "Village.h"                  // Village::get_gate_area / getArea（Area 字段）
#include "CStackableItem.h"           // CheckPrivateStoreDoll 的 +0x32c 字段
#include "CInventory.h"              // CInventory 完整类型（GetInvenSlot/use_money/gain_money 等）
#include "AvatarItem.h"              // 全局 Avatar_Item（BuyItem 所有权转移链）
#include "CAvatarItemMgr.h"           // WongWork::CAvatarItemMgr（avatar 剩余期/宝石槽/扩展）
#include "CCirculationStatistic.h"   // GetInstanceCirculationStatistic/CIRCULATION_STATISTIC_FIELD
#include "HistoryLog.h"               // HistoryLog::WriteBuyItem/WriteSellItem
#include "DB_AvatarChangeOwner.h"     // avatar 过户 DB 请求
#include "DisPatcherCore.h"           // DisPatcher_LeaveParty（CreatePrivateStore 离队）


#include <algorithm>
#include <cstring>
#include <cerrno>   // IsAreaNearEntranceDungeon 异常垫（ORIG strerror(errno)）
// ORIG T _Z26IsTradeLimitAttachTypeItemRK10Inven_Item @0x081201dc（定义在 CMailBoxHelper TU）。
bool IsTradeLimitAttachTypeItem(Inven_Item const& item);

// AuctionPacket.h 与 Game 侧 Inven_Item.h 存在同名结构（RandomOption* 等），
// 无法在同一 TU 共存；此处按 ORIG 布局镜像 PCK_AUCTION_OPEN_PRIVATE_STORE_GA
// （nsl::PACKET_HEADER 18B + m_id@18/character_num@22/character_ids[36]@26，
// sizeof 0xaa；ctor 等价 AuctionPacket.h 权威版 PACKET_CTOR_BODY(0,0xb,0xaa)，
// ORIG W 0x85cb374）。权威定义见 ServerLab/Auction/.../AuctionPacket.h。
namespace
{
struct OpenPrivateStoreGaMirror : public nsl::PACKET_HEADER
{
    unsigned int m_id;        // @18
    int character_num;        // @22
    int character_ids[36];    // @26

    OpenPrivateStoreGaMirror()
    {
        PACKET_CTOR_BODY(0, 0xb, 0xaa);
    }
};
}  // namespace

// PCK_AUCTION_CLOSE_PRIVATE_STORE_GA 镜像：ORIG W 0x85cb3dc
// PACKET_CTOR_BODY(0, 0xc, 0x16)（memset 0x16→setCategory(0)→setPacketID(0xc)→setSize(0x16)），
// m_id（memberIdentificationNumber_t）@18；权威定义见 ServerLab/Auction AuctionPacket.h。
struct ClosePrivateStoreGaMirror : public nsl::PACKET_HEADER
{
    unsigned int m_id;   // @18（setSize 0x16 = 头 0x12 + 4）

    ClosePrivateStoreGaMirror()
    {
        PACKET_CTOR_BODY(0, 0xc, 0x16);
    }
};

namespace private_store
{
// ---- COperator<int>::Product 特化（ORIG W _ZN13private_store9COperatorIiE7ProductEiiRi @0x85cd22a）----
// 重复加法乘法：out 累加 a 共 b 次，任一步非正即失败返回 0。须先于首次隐式实例化。
template <>
char COperator<int>::Product(int a, int b, int& out)
{
    out = 0;
    for (int i = 0; i < b; ++i)
    {
        out = out + a;
        if (!(out > 0))
            return 0;
    }
    return 1;
}


// ORIG 0x085ca3d6（报告 private_store__CPrivateStoreMgr/CPrivateStoreMgr.md 逐指令）：
// IUserPos 基类构造（写基类 vptr）→ 派生 vptr → 构造 +0x04 store、+0x68 池、
// +0x98/+0xb0/+0xc8 三张表，函数体再对三表 clear()。析构序与成员声明逆序一致。
CPrivateStoreMgr::CPrivateStoreMgr()
{
    m_storeSellers.clear();
    m_storeBuyers.clear();
    m_zoneStores.clear();
}

// ORIG 0x085ca4fe（报告 ~CPrivateStoreMgr.md 逐指令）：遍历 zone 表 delete 每个
// list<CPrivateStore*>；随后按声明逆序自动析构 zone 表/买家表/卖家表/+0x68 池/
// +0x04 嵌入 store。
CPrivateStoreMgr::~CPrivateStoreMgr()
{
    for (std::map<PrivateStoreZone, std::list<CPrivateStore*>*>::iterator it =
             m_zoneStores.begin();
         it != m_zoneStores.end(); ++it)
    {
        std::list<CPrivateStore*>* pList = it->second;
        if (pList != 0)
            delete pList;
    }
}

// ORIG 0x085c4be8：函数内静态单例（__cxa_guard）。
CPrivateStoreMgr* GetInstancePrivateStoreMgr()
{
    static CPrivateStoreMgr instance;
    return &instance;
}

// ---- PrivateStoreZone（ORIG C1/C2(h,h) W 0x85cb6ce，out-of-line；派生
//      PrivateStorePos 构造经 C2 初始化基类子对象）----
PrivateStoreZone::PrivateStoreZone(unsigned char a, unsigned char b)
    : m_a(a), m_b(b)
{
}

PrivateStorePos::PrivateStorePos() : PrivateStoreZone(0, 0), m_field2(0), m_field4(0) {}

// ORIG W 0x85cb72c：zone 基类构造后写 +0x2/+0x4。
PrivateStorePos::PrivateStorePos(unsigned char a, unsigned char b,
                                 short field2, short field4)
    : PrivateStoreZone(a, b), m_field2(field2), m_field4(field4)
{
}

void PrivateStorePos::clear()
{
    m_a = 0;
    m_b = 0;
    m_field2 = 0;
    m_field4 = 0;
}

// ---- PrivateStoreItem（ORIG C1 0x823457c / Reset 0x823459a）----
PrivateStoreItem::PrivateStoreItem()
{
    Reset();
}

// ORIG W 0x823459a：清空物品区与全部私有字段。
void PrivateStoreItem::Reset()
{
    m_item.reset();
    m_flag3d = 0;
    m_field40 = 0;
    m_field44 = 0;
    m_field46 = 0;
    m_field48 = 0;
}

// ---- PrivateStoreItem::operator=（ORIG W 0x85cb66e，报告缺失，objdump 转写）----
// Inven_Item::setCopy 拷物品区，随后逐字段直拷并返回 *this。
PrivateStoreItem& PrivateStoreItem::operator=(PrivateStoreItem const& other)
{
    m_item.setCopy(other.m_item);
    m_flag3d = other.m_flag3d;
    m_field40 = other.m_field40;
    m_field44 = other.m_field44;
    m_field46 = other.m_field46;
    m_field48 = other.m_field48;
    return *this;
}

// ---- CPrivateStorePos（ORIG C1 0x85cb7c8 / 带参 C1 0x85cb7fc）----
CPrivateStorePos::CPrivateStorePos() : m_rect(), m_pStore(0) {}

// ORIG W 0x85cb7fc：写宿主 store 指针后，以中心点 (centerX, centerY) 构造
// 0xa0 x 0x64 的碰撞矩形（左上 = 中心 - 半宽/半高）。
CPrivateStorePos::CPrivateStorePos(CPrivateStore* store, int centerX, int centerY)
    : m_rect(), m_pStore(store)
{
    m_rect.m_pos.m_x = centerX - 0x50;
    m_rect.m_pos.m_y = centerY - 0x32;
    m_rect.m_width = 0xa0;
    m_rect.m_height = 0x64;
}

// ORIG T 0x85cadf6（报告 docs/class_func_reports/private_store__CPrivateStorePos/
// put_object_info.md）：校验宿主 store 状态与卖家位置后序列化私商对象信息：
//   byte(1) | short(seller unique id) | int+str(店名) | byte(state!=1)
//   | byte(pos.a) | byte(pos.b) | short(pos.field2) | short(pos.field4)
//   | byte(isDollCreated)
bool CPrivateStorePos::put_object_info(PacketGuard& p)
{
    if (m_pStore == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xd0b, 0);
        trace("NOTEXIST_ERROR : private_store(null)");
        return false;
    }

    int state = m_pStore->GetState();
    if (state != 1 && state != 2)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xd11, 0);
        trace("STATE_ERROR : private_store_state(%d)", state);
        return false;
    }

    CUser* seller = m_pStore->GetSeller();
    if (seller == 0)
    {
        PrivateStorePos* pos = m_pStore->GetPos();
        cMyTrace trace(__PRETTY_FUNCTION__, 0xd17, 0);
        trace("NOTEXIST_ERROR : private_store(%d, %d) user(null)",
              (int)pos->m_a, (int)pos->m_b);
        return false;
    }

    // 未生成 doll 时才校验 store 登记位置与卖家当前位置一致。
    if (!m_pStore->isDollCreated())
    {
        PrivateStorePos* pos = m_pStore->GetPos();
        signed char vill = (signed char)seller->getCurCharacVill();
        int area = seller->get_area(false);
        if ((int)pos->m_a != (int)vill || (int)pos->m_b != area)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xd23, 0);
            trace("DIFFERENT_ERROR : private_store(%d,%d), user(%d,%d), uid(%d)",
                  (int)pos->m_a, (int)pos->m_b, (int)vill, area,
                  seller->get_unique_id() & 0xffff);
            return false;
        }
    }

    p.put_byte(1);
    p.put_short((unsigned short)m_pStore->GetSellerIndex());

    std::string* subject = m_pStore->GetSubject();
    p.put_int((int)subject->length());
    p.put_str(subject->c_str(), (int)subject->length());

    p.put_byte(m_pStore->GetState() != 1 ? 1 : 0);

    PrivateStorePos* pos = m_pStore->GetPos();
    p.put_byte((int)pos->m_a);
    p.put_byte((int)pos->m_b);
    p.put_short((short)pos->m_field2);
    p.put_short((short)pos->m_field4);

    p.put_byte(m_pStore->isDollCreated() ? 1 : 0);
    return true;
}

// ORIG W 0x85cb85c：对双方 +0x04 处的 Rectangle 做 include 判定
// （村庄对象矩形统一位于 IObject 头之后，与 ORIG lea 0x4(%eax) 一致）。
bool CPrivateStorePos::include(village_object::IObject const& obj) const
{
    collision_detection::Rectangle const* other =
        reinterpret_cast<collision_detection::Rectangle const*>(
            reinterpret_cast<char const*>(&obj) + sizeof(village_object::IObject));
    return m_rect.include(*other);
}

// ORIG W 0x85ccae4：基类子对象赋值 + Rectangle 4 个 int 直拷 + 宿主指针。
CPrivateStorePos& CPrivateStorePos::operator=(CPrivateStorePos const& other)
{
    village_object::IObject::operator=(other);  // ORIG W 0x85ccadc：空操作返回自身
    m_rect.m_pos.m_x = other.m_rect.m_pos.m_x;
    m_rect.m_pos.m_y = other.m_rect.m_pos.m_y;
    m_rect.m_width = other.m_rect.m_width;
    m_rect.m_height = other.m_rect.m_height;
    m_pStore = other.m_pStore;
    return *this;
}

// ---- CPrivateStore（ORIG C1 0x85c4d10 / D1 0x85c4ecc）----
CPrivateStore::CPrivateStore()
{
    // ORIG 语句顺序：m_pParent → m_state → items reserve/resize → name reserve
    // → m_dollCreated → m_dollItemID（O0 下按源码顺序生成指令；异常路径只含
    // string/vector/list 析构 + Unwind_Resume，无基类析构——基类 trivial）。
    m_pParent = 0;
    m_state = 0;
    // ORIG ctor 不初始化 m_pSeller/m_pBuyer（开店/购买流程经 SetSeller/SetBuyer 写入）。
    m_items.reserve(0xe);
    m_items.resize(0xe, PrivateStoreItem());
    m_name.reserve(0x28);
    m_dollCreated = 0;
    m_dollItemID = 0;
}

CPrivateStore::~CPrivateStore() {}

// ORIG W 0x82345e4：返回 +0x08（m_dollCreated）是否非 0。
// dispatch_sig 购买链中用于判定卖家 store 已创建 doll（私商店铺实体）。
bool CPrivateStore::isDollCreated()
{
    return m_dollCreated;  // ORIG：movzbl +0x08 直返，不做 0/1 归一化
}

// ORIG W 0x85cb894：卖家 world unique id（GetSeller()->get_unique_id()）。
int CPrivateStore::GetSellerIndex()
{
    return m_pSeller->get_unique_id();
}

// ---- ResetItems（ORIG T 0x85c4f52，报告 private_store__CPrivateStore/ResetItems.md）----
// 逐项调用 PrivateStoreItem::Reset（m_items 恒为 14 项，ctor resize(0xe)）。
void CPrivateStore::ResetItems()
{
    for (int i = 0; i < (int)m_items.size(); ++i)
        m_items[i].Reset();
}

// ---- IsLimitedSpace（ORIG T 0x85c4ff2，报告 private_store__CPrivateStore/IsLimitedSpace.md 全文）----
// 开店起点 m_pos 与目标 pos 区域(a/b)一致，且目标坐标落在
// [m_field2-0x50, m_field2+0x50) × [m_field4-0x32, m_field4+0x32) 矩形内。
// ORIG 为 && 短路链 + 单出口（mov $1/jmp/mov $0），按 P56 写 if 分支。
bool CPrivateStore::IsLimitedSpace(PrivateStorePos const& pos)
{
    if (m_pos.m_a == pos.m_a && m_pos.m_b == pos.m_b &&
        (int)(short)m_pos.m_field4 - 0x32 < (int)(short)pos.m_field4 &&
        (int)(short)m_pos.m_field4 + 0x32 > (int)(short)pos.m_field4 &&
        (int)(short)m_pos.m_field2 - 0x50 < (int)(short)pos.m_field2 &&
        (int)(short)m_pos.m_field2 + 0x50 > (int)(short)pos.m_field2)
        return true;
    return false;
}

// ---- CopyItems（ORIG T 0x85c4f9c，报告 CopyItems.md）----
// 先 ResetItems，再把 items[0..count) 拷到 m_items 头部（std::copy，步长 0x4c）。
void CPrivateStore::CopyItems(PrivateStoreItem* items, int count)
{
    ResetItems();
    std::copy(items, items + count, m_items.begin());
}

// ---- Clear（ORIG T 0x85c4e70，报告 Clear.md）----
// 清卖家/买家指针、状态、位置、围观列表并 ResetItems；随后清 doll 标志与 doll 物品。
// ORIG 不清 m_pParent 与店名。
void CPrivateStore::Clear()
{
    m_pSeller = 0;
    m_pBuyer = 0;
    m_state = 0;
    m_pos.clear();
    m_users.clear();
    ResetItems();
    m_dollCreated = 0;
    m_dollItemID = 0;
}

// ---- RecallPrivateStoreItemToMail（ORIG T 0x85c5cec，报告 RecallPrivateStoreItemToMail.md）----
// 收集 state==3 且物品索引非 0 的条目，经 ReqDBSendNewSystemMultiMail 以系统邮件退还；
// 发送失败仅 trace；成功或无条目时 ResetItems。恒返回 1。
bool CPrivateStore::RecallPrivateStoreItemToMail()
{
    Inven_Item items[14];
    int count = 0;
    for (unsigned int i = 0; i < 0xe; ++i)
    {
        if (m_items[i].GetState() == 3 && m_items[i].m_item.m_addInfo != 0)
            items[count++].setCopy(m_items[i].m_item);
    }
    if (0 < count)
    {
        char sender[0x15];
        std::memset(sender, 0, sizeof(sender));
        char title[0x100];
        std::memset(title, 0, sizeof(title));
        std::strcpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_183", 0));
        std::strcpy(title, g_scriptStringManager_.findString(4, "game_server_msg_186", 0));
        int ret = WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, items, (unsigned int)count, 0u,
            (unsigned int)m_pSeller->getCurCharacNo(), title,
            (int)std::strlen(title), 0u, m_pSeller->GetServerGroup(), false);
        if (ret != 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x3c1, 5);
            trace("CPrivateStore::RecallPrivateStoreItemToMail Error - Mail Send Fail "
                  "User(%d), Charac_no(%d), item_count(%d)",
                  m_pSeller->get_acc_id(), m_pSeller->getCurCharacNo(), count);
            return 1;
        }
    }
    ResetItems();
    return 1;
}

// ---- SavePrivateStore（ORIG T 0x85ca116，报告 SavePrivateStore.md）----
// Stream(0x77)+GetUID 后取 SIG_PRIVATE_STORE：清零 0x434、写 charac_no、
// 拷贝 14 条目至 +8、按 isDollCreated 写 +0x430 标志，入 DB 队列(DB_Q=2)。
void CPrivateStore::SavePrivateStore()
{
    Stream* s = GlobalData::s_stream_pool->Acquire("PrivateStore.cpp", 0xbc6);
    CStreamGuard guard(s, true);
    guard->operator<<(0x77);
    guard->operator<<((int)m_pSeller->GetUID());
    SIG_PRIVATE_STORE* sig = guard->GetInBuffer<SIG_PRIVATE_STORE>();
    std::memset(static_cast<void*>(sig), 0, 0x434);
    sig->m_characNo = m_pSeller->get_charac_no(-1);
    std::copy(m_items.begin(), m_items.begin() + 0xe,
              reinterpret_cast<PrivateStoreItem*>(sig->m_items));
    sig->m_dollCreated = isDollCreated() ? 1 : 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

}
// ---- RecreatePrivateStore（ORIG T 0x85c5b60，报告 RecreatePrivateStore.md）----
// LoadPrivateStoreResponse 通知链经 ISeller 虚表槽 +0x14 调用。前置校验
// （m_pParent 非空 / 状态为 0 / 卖家非空）后：重算开店起点、清围观列表、
// ResetItems 后从 SIG 恢复 14 条目；拍卖代理在线时广播 OPEN_PRIVATE_STORE_GA
// (category 0 / packetID 0xb / size 0xaa)，最后置状态 2 返回 1。
int CPrivateStore::RecreatePrivateStore(SIG_PRIVATE_STORE* sig)
{
    if (m_pParent == 0)
        return 0;
    if (m_state != 0)
        return 0;
    if (m_pSeller == 0)
        return 0;

    // ORIG：函数内 static（__cxa_guard 守护的 characterControllerPrivate 单例）。
    static CCharacterControllerPrivate characterControllerPrivate;
    characterControllerPrivate.GetStartPoint(m_pSeller, m_pos);

    m_users.clear();
    ResetItems();
    CopyItems(reinterpret_cast<PrivateStoreItem*>(sig->m_items), 0xe);

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        OpenPrivateStoreGaMirror packet;
        packet.m_id = (unsigned int)m_pSeller->get_acc_id();
        packet.character_num = m_pSeller->get_charac_count();
        for (int i = 0; i < packet.character_num; ++i)
            packet.character_ids[i] = m_pSeller->get_charac_no(i);
        GlobalData::s_auction_proxy->SendPacket(&packet);
    }

    m_state = 2;
    return 1;
}

// ---- CreatePrivateStore（ORIG T 0x85c53ea，报告 private_store__CPrivateStore/CreatePrivateStore.md）----
// ISeller 虚表槽 +0x00。开店主流程：权限/区域/频道校验，可选 doll 物品登记，
// 构造碰撞位并注册村庄对象，置状态 1，命令包与广播包，有队伍则离队分发，
// 历史/拍卖代理上报。
bool CPrivateStore::CreatePrivateStore(unsigned char zoneA, unsigned char zoneB,
                                       short posX, short posY, unsigned short dollSlot)
{
    m_errorHandler.Set(m_pSeller, 1, 0x58);

    if (m_pSeller == 0)
        return false;
    if (m_pSeller->IsBusyDisjointStore())
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if ((signed char)m_pSeller->getCurCharacVill() == 7)
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    // 权限与 doll 槽互斥：有私商权限必须传 0xffff；无权限禁止传 0xffff。
    if (m_pSeller->IsPermissionPrivateStore())
    {
        if (dollSlot != 0xffff)
        {
            m_errorHandler.Do(0x3f);
            return false;
        }
    }
    else if (dollSlot == 0xffff)
    {
        m_errorHandler.Do(0x3f);
        return false;
    }

    if (m_pSeller->get_state() == 6)
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (G_GameWorld()->GetChannelType() == 2)
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (m_state != 0)
    {
        m_errorHandler.Do(0x14);
        return false;
    }

    int restrictResult =
        G_GameWorld()->check_restrictive_commercial_transaction_zone(
            (int)zoneA, (int)zoneB, (int)posX, (int)posY);
    if (restrictResult != 0)
    {
        m_errorHandler.Do(restrictResult);
        return false;
    }
    if (G_GameWorld()->IsPVPChannel())
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (!IsAreaNearEntranceDungeon(m_pSeller))
    {
        m_errorHandler.Do(0x3e);
        return false;
    }

    if (dollSlot != 0xffff)
    {
        int dollError = CheckPrivateStoreDoll(dollSlot);
        if (dollError != 0)
        {
            m_errorHandler.Do(dollError);
            return false;
        }
        SetDollCreated(true);
        const CInventory* inventory = m_pSeller->getCurCharacInvenR();
        Inven_Item dollItem = inventory->GetInvenSlot(1, (int)dollSlot);
        SetDollItemID((unsigned long)dollItem.m_addInfo);  // ORIG 读隐藏返回副本 +2（物品索引）
    }

    if (!isDollCreated())
        m_pSeller->set_position(posX, posY, 0);

    CPrivateStorePos newPos(this, (int)posX, (int)posY);
    m_storePos = newPos;

    village_object::Zone zone(zoneA, zoneB);
    if (!GlobalData::s_villageObjectMgr->register_object(zone, &m_storePos))
    {
        m_errorHandler.Do(0x38);
        return false;
    }

    PrivateStorePos registeredPos;
    registeredPos.m_a = zoneA;
    registeredPos.m_b = zoneB;
    registeredPos.m_field2 = (unsigned short)posX;
    registeredPos.m_field4 = (unsigned short)posY;
    m_pos = registeredPos;

    m_users.clear();
    ResetItems();
    m_state = 1;

    CPacketBufPrivate packet;
    packet.MakeCmdpacketCreatePrivateStore();
    m_pSeller->Send(packet);

    packet.MakeNotipacketCreatePrivateStore((unsigned short)GetSellerIndex(), m_pos,
                                            isDollCreated() ? true : false);
    CBroadcastPrivate::Send((int)zoneA, (int)zoneB, packet);

    // 有队伍时经函数内静态 DisPatcher_LeaveParty 分发离队（ORIG leave_party 局部静态）。
    if (m_pSeller->GetParty() != 0)
    {
        static DisPatcher_LeaveParty leave_party;  // ORIG 函数内静态（guard 0x09472e28/本体 0x09472e40）
        // ORIG call 0x0822b766 = InterfacePacketBuf::get()（PacketBuf* 无参 getter）
        leave_party.dispatch_template(m_pSeller, *packet.get());
    }

    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x335, 0);
        trace("CreatePrivateStore() uid:%d\n",
              (unsigned int)m_pSeller->get_unique_id() & 0xffff);
    }
    m_pSeller->getCurCharacName();
    m_pSeller->m_historyLog.CreatePrivateStore(          // CUser 内嵌 +0x79700（CUser.h:1596）
        (int)zoneA, (int)zoneB, m_pSeller->getCurCharacName());

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        OpenPrivateStoreGaMirror auctionPacket;
        auctionPacket.m_id = (unsigned int)m_pSeller->get_acc_id();
        auctionPacket.character_num = m_pSeller->get_charac_count();
        for (int i = 0; i < auctionPacket.character_num; ++i)
            auctionPacket.character_ids[i] = m_pSeller->get_charac_no(i);
        GlobalData::s_auction_proxy->SendPacket(&auctionPacket);
    }
    return true;
}

// ---- RemovePrivateStore（ORIG T 0x85c5fd0，报告 RemovePrivateStore.md）----
// ISeller 虚表槽 +0x04。拆店主流程：按状态退还条目（doll 走邮件，非 doll 走
// RecallPrivateStoreItem），围观广播强拆包，历史/拍卖上报，注销村庄对象，
// 释放卖家表项并 Clear。sendFail=false 且条目退还失败时中止。
bool CPrivateStore::RemovePrivateStore(bool sendFail)
{
    m_errorHandler.Set(m_pSeller, 1, 0x59);

    if (m_pSeller == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }
    if (m_state == 0)
    {
        m_errorHandler.Do(0x3b);
        return false;
    }

    CPacketBufPrivate packet;
    if (m_state == 2)
    {
        if (isDollCreated())
        {
            if (RecallPrivateStoreItemToMail())
                SavePrivateStore();
        }
        else
        {
            if (!RecallPrivateStoreItem())
            {
                SavePrivateStore();
                m_pSeller->UpdateData();
                m_errorHandler.Do(0x16);
                if (!sendFail)
                    return false;
            }
            else
            {
                SavePrivateStore();
                m_pSeller->UpdateData();
                m_pSeller->send_itemspace(0);
                m_pSeller->send_itemspace(1);
                m_pSeller->getCurCharacInvenR()->SendItemLockListInven();
                m_pSeller->getCurCharacInvenR()->SendItemLockListAvatar();
            }
        }

        if ((int)m_users.size() != 0)
        {
            packet.MakeNotipacketForceRemovePrivateStore();
            SendUsers(&packet, 0, 0);
        }
    }

    m_pSeller->getCurCharacName();
    m_pSeller->m_historyLog.RemovePrivateStore(
        (int)m_pos.m_a, (int)m_pos.m_b, m_pSeller->getCurCharacName());

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        ClosePrivateStoreGaMirror auctionPacket;
        auctionPacket.m_id = (unsigned int)m_pSeller->get_acc_id();
        GlobalData::s_auction_proxy->SendPacket(&auctionPacket);
    }

    packet.MakeCmdpacketRemovePrivateStore();
    m_pSeller->Send(packet);

    packet.MakeNotipacketRemovePrivateStore((unsigned short)GetSellerIndex());
    CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

    village_object::Zone zone(m_pos.m_a, m_pos.m_b);
    if (!GlobalData::s_villageObjectMgr->unregister_object(zone, &m_storePos))
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x473,
                              "unregister_object error");
    }

    m_pParent->FreeStoreSeller(m_pSeller);
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x478, 0);
        trace("RemovePrivateStore() uid:%d\n",
              (unsigned int)m_pSeller->get_unique_id() & 0xffff);
    }
    Clear();
    return true;
}

// ---- CompleteDisplay（ORIG T 0x85c7800，报告 CompleteDisplay.md）----
// ISeller 虚表槽 +0x08：确认开店告示。MoveItems 成功后保存、刷新物品锁视图，
// doll 存在时消耗对应 doll 槽道具；写入店名，广播 noti 并回发 cmd，置状态 2。
bool CPrivateStore::CompleteDisplay(char const* subject, unsigned short dollSlot)
{
    m_errorHandler.Set(m_pSeller, 1, 0x5a);

    if (m_pSeller == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    if (!MoveItems())
        return false;

    SavePrivateStore();
    m_pSeller->UpdateData();
    m_pSeller->send_itemspace(0);
    m_pSeller->send_itemspace(1);
    m_pSeller->getCurCharacInvenR()->SendItemLockListInven();
    m_pSeller->getCurCharacInvenR()->SendItemLockListAvatar();

    // 无 doll 却指定了 doll 槽道具则拒绝（ORIG：isDollCreated!=0 或 slot==0xffff 才放行）。
    if (!(isDollCreated() != 0) && dollSlot != 0xffff)
        return false;

    if (isDollCreated())
    {
        int useResult = m_pSeller->getCurCharacInvenW()->use_item((int)dollSlot, 1, 1);
        m_pSeller->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, (int)dollSlot);
        if (useResult != 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x6c1, 5);
            trace("CPrivateStore::CompleteDisplay() ERROR!!! : use_item(%d)", (int)dollSlot);
            return false;
        }
    }

    m_name = subject;

    CPacketBufPrivate packet;
    packet.MakeNotipacketCompleteDisplay((unsigned short)GetSellerIndex(), m_name,
                                         isDollCreated() ? true : false);
    CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

    packet.MakeCmdpacketCompleteDisplay(isDollCreated() ? true : false);
    m_pSeller->Send(packet);

    m_state = 2;
    return true;
}

// ---- MoveToGate（ORIG T 0x85c751a，报告 MoveToGate.md）----
// ISeller 虚表槽 +0x0c：收店回城门。已登记位置时先广播拆除通知并注销村庄对象，
// 再把卖家移回开店起点；随后发送 MoveToGate 命令包。
bool CPrivateStore::MoveToGate()
{
    m_errorHandler.Set(m_pSeller, 1, 0x59);

    CPacketBufPrivate packet;

    if (m_pSeller == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    if (m_pos.m_a != 0 || m_pos.m_b != 0 ||
        m_pos.m_field2 != 0 || m_pos.m_field4 != 0)
    {
        packet.MakeNotipacketRemovePrivateStore((unsigned short)GetSellerIndex());
        CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

        village_object::Zone zone(m_pos.m_a, m_pos.m_b);
        if (!GlobalData::s_villageObjectMgr->unregister_object(zone, &m_storePos))
        {
            LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x658,
                                  "unregister_object error");
        }

        g_CharacterController.MoveCharacterStartPoint(m_pSeller);
    }

    packet.MakeCmdpacketMoveToGate();
    m_pSeller->Send(packet);
    return true;
}

// ---- EnterPrivateStore（ORIG T 0x85c8dd6，报告 EnterPrivateStore.md）----
// ISeller 槽 +0x20 / IBuyer 槽 +0x08。买家围观入店：状态必须为 2，未在
// 围观列表则登记并接收店铺信息包。
bool CPrivateStore::EnterPrivateStore()
{
    m_errorHandler.Set(m_pBuyer, 1, 0x56);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    if (m_state != 2)
    {
        m_pParent->FreeStoreBuyer(m_pBuyer);
        m_errorHandler.Do(0x3b);
        return false;
    }

    if (std::find(m_users.begin(), m_users.end(), m_pBuyer) != m_users.end())
    {
        m_errorHandler.Do(0x14);
        return false;
    }

    m_users.push_back(m_pBuyer);

    CPacketBufPrivate packet;
    packet.MakeCmdpacketEnterPrivateStore(m_pSeller, (unsigned short)GetSellerIndex(),
                                          m_items);
    m_pBuyer->Send(packet);
    return true;
}

// ---- ExitPrivateStore（ORIG T 0x85c8fd4，报告 ExitPrivateStore.md）----
// ISeller 槽 +0x24 / IBuyer 槽 +0x0c。买家离店：发退出命令包、释放买家表项、
// 从围观列表摘除并清空买家指针。列表非空且找不到该买家时报 0x15。
bool CPrivateStore::ExitPrivateStore()
{
    m_errorHandler.Set(m_pBuyer, 1, 0x57);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    std::list<CUser*>::iterator found =
        std::find(m_users.begin(), m_users.end(), m_pBuyer);
    if (!m_users.empty() && found == m_users.end())
    {
        m_errorHandler.Do(0x15);
        return false;
    }

    CPacketBufPrivate packet;
    packet.MakeCmdpacketExitPrivateStore();
    m_pBuyer->Send(packet);

    m_pParent->FreeStoreBuyer(m_pBuyer);

    if (!m_users.empty())
        m_users.erase(found);

    m_pBuyer = 0;
    return true;
}

// ---- AddItem（ORIG T 0x85c6d48，报告 AddItem.md）----
// ISeller 虚表槽 +0x10。卖家上架：doll 存在时校验 doll 物品与槽位上限；
// 校验背包来源物品（非现金价、非化身、可交易限制），prevCountSlot==0 时重置
// 全部条目后按 slot_no 落位。
bool CPrivateStore::AddItem(PrivateStoreItem& item, int prevCountSlot)
{
    if (isDollCreated())
    {
        CItem* dollInfo =
            G_CDataManager()->find_item((int)GetDollItemID());
        if (dollInfo == 0)
        {
            LogManager::logFormat(
                1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x550,
                "CPrivateStore::AddItem ERROR : shop_doll_item_id(%d)",
                (unsigned int)GetDollItemID());
            return false;
        }
        STExpertJobScript* expertScript = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(4);  // 头返回 void*
        int maxSlot = (int)expertScript->GetShopDollSlot(
            (int)static_cast<CStackableItem*>(dollInfo)->m_int32c);   // ORIG 读 doll 物品 +0x32c
        if (maxSlot < 1)
        {
            LogManager::logFormat(
                1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x559,
                "CPrivateStore::AddItem ERROR : shop_doll_item_index(%d)",
                (int)static_cast<CStackableItem*>(dollInfo)->m_int32c);
            return false;
        }
        if (maxSlot <= (int)item.m_field44)
        {
            LogManager::logFormat(
                1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x55f,
                "CPrivateStore::AddItem ERROR : slot_no(%d), shop_doll_item_index(%d), maxSlot(%d)",
                (int)item.m_field44, (int)static_cast<CStackableItem*>(dollInfo)->m_int32c, maxSlot);
            return false;
        }
    }

    if ((short)item.m_field44 >= 7)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x566,
                              "CPrivateStore::AddItem slot_no(%d) ERROR",
                              (int)item.m_field44);
        return false;
    }

    if (m_pSeller == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x56c,
                              "CPrivateStore::AddItem seller_ == NULL ERROR");
        return false;
    }
    if (m_pSeller->getCurCharacR() == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x572,
                              "CPrivateStore::AddItem getCurCharacR() == NULL ERROR");
        return false;
    }
    const CInventory* inventory = m_pSeller->getCurCharacInvenR();
    if (inventory == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x579,
                              "CPrivateStore::AddItem inventory == NULL ERROR");
        return false;
    }

    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)item.m_field46);  // ORIG 返回 int
    Inven_Item srcItem = inventory->GetInvenSlot((INVEN_TYPE)invenType, (int)item.m_field48);
    CItem* itemInfo = G_CDataManager()->find_item((int)srcItem.m_addInfo);
    if (itemInfo == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x582,
                              "CPrivateStore::AddItem find_item() ERROR item_id(%d)",
                              (int)srcItem.m_addInfo);
        return false;
    }

    // ORIG：cash 价 >=1 的商城物品禁止上架（报告反编译 iVar6<1 才继续）。
    if ((int)itemInfo->GetCashPrice() >= 1)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x5b8,
                              "CPrivateStore::AddItem GetCashPrice() ERROR item_id(%d), cash_price(%d)",
                              (int)srcItem.m_addInfo, (int)itemInfo->GetCashPrice());
        return false;
    }
    if (!srcItem.m_upgradeSep.IsTradeRestriction())   // ORIG lea 副本+0x33（UpgradeSeparateInfo 子对象）
    {
        // ORIG 经 CItem vtable 槽 +0x10 判定化身类道具，禁止私商上架。
        if (itemInfo->IsAvatarItem())
        {
            LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x5be,
                                  "CPrivateStore::AddItem IsAvatarItem() ERROR item_id(%d), cash_price(%d)",
                                  (int)srcItem.m_addInfo, (int)itemInfo->GetCashPrice());
            return false;
        }

        if ((short)item.m_field44 < 0xe && (short)item.m_field44 >= 0 &&
            prevCountSlot < 0xe && prevCountSlot >= 0)
        {
            if (prevCountSlot == 0)
                ResetItems();

            PrivateStoreItem& target = m_items[(unsigned short)item.m_field44];
            if (target.GetState() != 0)
                return false;

            if (item.m_field46 == 3)
                return false;   // ORIG：item space 3 的条目不可上架

            item.m_flag3d = 1;
            target.Reset();
            target = item;
            return true;
        }
        return false;
    }
    return false;
}

// ---- BuyPrivateStoreItem（ORIG T 0x85c7e74，报告 BuyPrivateStoreItem.md）----
// ISeller 槽 +0x18 / IBuyer 槽 +0x00。买家购买：校验围观/槽位/忙碌与物品可售性，
// 计价后经 BuyItem 扣买家入包、SellItem 结算卖家金币（扣手续费），广播条目变化，
// 反外挂检测（金额阈值公式常量 0.01/0.7，ORIG rodata 0x8cbead0/0x8cbead8），
// 最后写入流通统计。
bool CPrivateStore::BuyPrivateStoreItem(int slot, int count)
{
    m_errorHandler.Set(m_pBuyer, 1, 0x55);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }
    if (!IsExistBuyerList(m_pBuyer))
    {
        m_errorHandler.Do(0x15);
        return false;
    }
    if (m_pBuyer->getCurCharacR() == 0)
    {
        m_errorHandler.Do(1);
        return false;
    }
    if (slot < 0 || 0xd < slot)
    {
        m_errorHandler.Do(0x39);
        return false;
    }
    if (m_pParent->IsBusyPrivateStore(m_pBuyer))
    {
        m_errorHandler.Do(0x3c);
        return false;
    }

    CItem* itemInfo = 0;
    PrivateStoreItem& storeItem = m_items[slot];
    itemInfo = G_CDataManager()->find_item((int)storeItem.m_item.m_addInfo);
    if (itemInfo == 0)
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    int attachType = itemInfo->GetAttachType();
    if (attachType != 0)
    {
        if (attachType == 3)
        {
            if ((int)storeItem.m_item.m_field0 != 1)
            {
                m_errorHandler.Do(0x11);
                return false;
            }
        }
        else
        {
            if (attachType != 7)
            {
                m_errorHandler.Do(0x11);
                return false;
            }
            // 绑定类：无剩余交易次数时静默拒绝。
            if (!storeItem.m_item.GetTradeLimitCount())
                return false;
        }
    }
    // 生物类道具禁止私商交易。
    if (IsCreatureItemType((unsigned int)(unsigned char)storeItem.m_item.m_field1))
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    int price = 0;
    unsigned int dealGoldA = 0;
    unsigned int dealGoldB = 0;
    if (!GetItemPrice(itemInfo, storeItem, count, price, dealGoldA, dealGoldB))
        return false;
    if (!IsPossibleDeal(price, dealGoldA, dealGoldB))
        return false;
    // 非可信用户受每日交易金币上限约束。
    if (!m_pBuyer->GetReliablePerson() &&
        !m_pBuyer->IsAvailableCurCharacTradeGoldDaily(dealGoldA))
    {
        m_errorHandler.Do(0x46);
        return false;
    }

    Inven_Item copyItem;
    copyItem.setCopy(storeItem.m_item);
    int useCount = count;                       // ORIG local_8e
    int buyResult = -1;                          // ORIG local_2c
    int outSlot = 0;                             // ORIG local_4c
    if ((int)copyItem.m_field1 == 8)             // avatar：数量实参替换为 avatar uid
        useCount = (int)storeItem.m_item.m_addInfo2;

    if (IsTradeLimitAttachTypeItem(copyItem))
    {
        DecreaseTradeLimitCount(copyItem);
        m_pSeller->m_historyLog.ChangeItemLimitCount(-1, -1,
                                                     (int)copyItem.m_addInfo, -1, 1);
    }

    buyResult = BuyItem(slot, copyItem, price, (int)dealGoldA, useCount, outSlot);
    if (buyResult >= 0)
    {
        int commission = GetCommissionPercent() * price / 100;
        SellItem(slot, copyItem, price, commission, useCount,
                 (int)storeItem.m_item.m_addInfo2);
        SubtractItemCount(itemInfo, storeItem, count);
        SavePrivateStore();
        m_pSeller->UpdateData();
        m_pBuyer->UpdateData();

        CPacketBufPrivate packet;
        packet.MakeNotipacketPrivateStoreItem(slot, count);
        SendUsers(&packet, 0, 0);

        packet.MakeNotipacketBuyPrivateStoreItem(
            m_pSeller, (int)copyItem.m_addInfo, slot, useCount,
            m_pBuyer->getCurCharacName(), price, commission);
        m_pSeller->Send(packet);

        packet.MakeCmdpacketBuyPrivateStoreItem(m_pBuyer, copyItem,
                                                (int)dealGoldB, (int)dealGoldA,
                                                buyResult, useCount);
        m_pBuyer->Send(packet);

        unsigned char flagByte = 0;              // ORIG local_27
        if (!itemInfo->IsAvatarItem() &&
            (int)itemInfo->get_rarity() <= 1)
        {
            flagByte = 1;
        }
        // CUser::IncrementTradeGold(uint,int) ORIG _ZN5CUser18IncrementTradeGoldEji @0x0864658a
        // （定义在 CUserOnlyOrig.cpp；CUser.h 两参声明，直接调用与 ORIG 字节一致。）
        m_pBuyer->IncrementTradeGold((unsigned int)price, (int)flagByte);

        unsigned short usedFatigue =
            (unsigned short)m_pBuyer->getCurCharacUsedFatigue();   // ORIG local_26
        int rand25 = get_rand_int(0x19);         // ORIG local_24
        int rand20 = get_rand_int(0x14);         // ORIG local_20
        const CInventory* buyerInven = m_pBuyer->getCurCharacInvenR();
        // 反外挂：低价倾销检测。ORIG 浮点常量 [0x8cbead0]=0.01、[0x8cbead8]=0.7。
        double hackScale = (double)rand25 * 0.01 + 0.7;
        if ((float)((double)((long long)buyerInven->get_money() + (int)dealGoldA) * hackScale) <
            (float)(int)dealGoldA)
        {
            unsigned int etcResult = 0;
            etcResult = m_pBuyer->SetETC(0x2c0, m_pSeller->get_acc_id());
            m_pBuyer->getHackAnalyzer()->addServerHackCnt(
                m_pBuyer, (WongWork::ENUM_HACKTYPE)0x2c0, 1u, etcResult, 0u);
            if (rand20 + 0x78 <= (int)usedFatigue)
            {
                etcResult = m_pBuyer->SetETC(0x2bd, m_pSeller->get_acc_id());
                m_pBuyer->getHackAnalyzer()->addServerHackCnt(
                    m_pBuyer, (WongWork::ENUM_HACKTYPE)0x2bd, 1u, etcResult, 0u);
            }
            etcResult = m_pSeller->SetETC(0x2c1, m_pBuyer->get_acc_id());
            m_pSeller->getHackAnalyzer()->addServerHackCnt(
                m_pSeller, (WongWork::ENUM_HACKTYPE)0x2c1, 1u, etcResult, 0u);
        }

        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x12, m_pSeller, (unsigned int)commission);
        GetInstanceCirculationStatistic()->AddCirculationStatistic(
            (CIRCULATION_STATISTIC_FIELD)0x15, m_pSeller, (unsigned int)commission);
        return true;
    }

    m_errorHandler.Do(4);
    return false;
}

// ---- VerifyPrivateStoreItem（ORIG T 0x85c874c，报告 VerifyPrivateStoreItem.md）----
// ISeller 槽 +0x1c / IBuyer 槽 +0x04。买家购买前校验并回发 0x195 详情包：
// 计价/交易限制/每日金额校验通过后，按条目类型序列化（avatar 附剩余时间、
// 宝石槽 30B 与扩展信息 4B），put_packet 后发买家。
bool CPrivateStore::VerifyPrivateStoreItem(int slot, int count)
{
    m_errorHandler.Set(m_pBuyer, 1, 0x195);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }
    if (!IsExistBuyerList(m_pBuyer))
    {
        m_errorHandler.Do(0x15);
        return false;
    }
    if (m_pBuyer->getCurCharacR() == 0)
    {
        m_errorHandler.Do(1);
        return false;
    }
    if (slot < 0 || 0xd < slot)
    {
        m_errorHandler.Do(0x39);
        return false;
    }
    // 卖家侧角色必须有效。
    if (m_pSeller == 0 || m_pSeller->getCurCharacR() == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    PrivateStoreItem& storeItem = m_items[slot];
    if ((int)storeItem.GetState() != 3)
    {
        m_errorHandler.Do(1);
        return false;
    }
    if (m_pParent->IsBusyPrivateStore(m_pBuyer))
    {
        m_errorHandler.Do(0x3c);
        return false;
    }

    CItem* itemInfo = G_CDataManager()->find_item((int)storeItem.m_item.m_addInfo);
    if (itemInfo == 0)
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    int attachType = itemInfo->GetAttachType();
    if (attachType != 0)
    {
        if (attachType == 3)
        {
            if ((int)storeItem.m_item.m_field0 != 1)
            {
                m_errorHandler.Do(0x11);
                return false;
            }
        }
        else
        {
            if (attachType != 7)
            {
                m_errorHandler.Do(0x11);
                return false;
            }
            if (!storeItem.m_item.GetTradeLimitCount())
                return false;   // ORIG：绑定类无次数静默拒绝
        }
    }
    if (IsCreatureItemType((unsigned int)(unsigned char)storeItem.m_item.m_field1))
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    int price = 0;
    unsigned int dealGoldA = 0;
    unsigned int dealGoldB = 0;
    if (!GetItemPrice(itemInfo, storeItem, count, price, dealGoldA, dealGoldB))
        return false;
    if (!IsPossibleDeal(price, dealGoldA, dealGoldB))
        return false;
    if (!m_pBuyer->GetReliablePerson() &&
        !m_pBuyer->IsAvailableCurCharacTradeGoldDaily(dealGoldA))
    {
        m_errorHandler.Do(0x46);
        return false;
    }

    PacketGuard packet;
    packet.put_header(1, 0x195);
    packet.put_byte(1);
    packet.put_short(slot);
    packet.put_int(storeItem.m_field40);                       // +0x40 售价
    packet.put_int((int)storeItem.m_item.m_addInfo);           // +0x02 物品索引
    int nowTick = OS_API::GetDateTimeTick();
    if ((int)storeItem.m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
    {
        packet.put_int(
            m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                (int)storeItem.m_item.m_addInfo2, nowTick));
    }
    else
    {
        packet.put_int(count);
    }
    packet.put_byte((int)storeItem.m_item.GetItemAttr());
    packet.put_short((int)storeItem.m_item.m_fieldb);          // +0x0b 耐久
    packet.put_int((int)storeItem.m_item.m_fieldd);            // +0x0d
    packet.put_byte((int)storeItem.m_item.m_amp.getAbilityType());
    packet.put_short((int)storeItem.m_item.m_amp.getAbilityValue());
    if ((int)storeItem.m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
    {
        packet.put_int(0x1e);
        packet.put_binary(
            (char const*)m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                (int)storeItem.m_item.m_addInfo2),
            0x1e);
        void* expansion =
            m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                (int)storeItem.m_item.m_addInfo2);
        packet.put_int(4);
        if (expansion == 0)
        {
            stAvatarExpansionInfo_t defaultExpansion;
            defaultExpansion.init();
            packet.put_binary((char const*)&defaultExpansion, 4);
        }
        else
        {
            packet.put_binary((char const*)expansion, 4);
        }
    }
    packet.put_packet(storeItem.m_item);
    packet.finalize(true);
    m_pBuyer->Send(packet);
    return true;
}

// ---- CheckPrivateStoreDoll（ORIG T 0x85c532c，报告 CheckPrivateStoreDoll.md L15-129 全文）----
// 校验 doll 槽物品可作开店娃娃：来源背包槽存在且为可堆叠、脚本索引有效、数量>0。
// 返回错误码：0x3a 卖家缺失 / 0x16 物品无效 / 0 可用。
int CPrivateStore::CheckPrivateStoreDoll(unsigned short dollSlot)
{
    if (m_pSeller == 0)
        return 0x3a;

    const CInventory* inventory = m_pSeller->getCurCharacInvenR();
    Inven_Item item = inventory->GetInvenSlot((INVEN_TYPE)1, (int)dollSlot);

    CDataManager* dataMgr = G_CDataManager();                  // ORIG _Z14G_CDataManagerv
    CItem* pItem = dataMgr->find_item((int)item.m_addInfo);    // 副本 +2 = 物品索引
    if (pItem == 0)
        return 0x16;
    if (!pItem->is_stackable())
        return 0x16;
    // +0x32c 属派生类 CStackableItem（CItem 尺寸 0x148，CStackableItem 0x3f0）；
    // is_stackable 通过后必为 CStackableItem，字段 m_int32c（CStackableItem.h:365）。
    if (static_cast<CStackableItem*>(pItem)->m_int32c == -1)
        return 0x16;
    if ((int)item.get_add_info() <= 0)                         // 副本 +7 = 数量
        return 0x16;
    return 0;
}

// ---- IsAreaNearEntranceDungeon（ORIG T 0x85c5082，报告 IsAreaNearEntranceDungeon.md L15-269 全文）----
// 开店区域校验：村庄 2 区 0 即入口区允许开店；区域类型 4 禁止；异常走 errno 日志垫。
bool CPrivateStore::IsAreaNearEntranceDungeon(CUser* user)
{
    GameWorld* gameWorld = 0;
    Village* village = 0;
    Area* area = 0;
    int areaNo = 0;

    try
    {
        if (user == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x233, 5);
            trace("pUser == NULL");
            return false;
        }
        gameWorld = G_GameWorld();
        if (gameWorld == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x239, 5);
            trace("G_GameWorld call failed");
            return false;
        }
        int curArea = user->get_area(false);
        if (!gameWorld->check_valid_area((int)user->getCurCharacVill(), curArea))
        {
            // ORIG 复用 "G_GameWorld call failed" 文案（报告行 84 字面实证）。
            cMyTrace trace(__PRETTY_FUNCTION__, 0x23f, 5);
            trace("G_GameWorld call failed");
            return false;
        }
        village = gameWorld->getUserVillage(user);
        if (village == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x246, 5);
            trace("getUserVillage call failed");
            return false;
        }
        areaNo = user->get_area(false);
        area = village->getArea(areaNo);
        if (area == 0)
            return false;

        if (area->m_villageIndex == 2 && area->m_areaIndex == 0)
            return true;
        if (area->m_areaType == 4)
            return false;
        return true;
    }
    catch (...)
    {
        // ORIG 落地垫 0x085c52b2-0x085c5303：errno → strerror(errno) 作 %s。
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x25c,
                              "[EXCEPTION IsAreaNearEntranceDungeon] - code = %s",
                              strerror(errno));
        return false;
    }
}

// ---- MoveItems（ORIG T 0x85c7246，报告 MoveItems.md L15-333 全文）----
// 两轮扫描：state==1 条目经 GetItemInfo 刷新后置 2 并校验（avatar 校验剩余期，
// 其余回写 +0x07）；CheckTotalItemPrice 通过后把 state==2 条目 DeleteItem 置 3。
bool CPrivateStore::MoveItems()
{
    for (unsigned int i = 0; i < 0xe; ++i)
    {
        PrivateStoreItem& item = m_items[i];
        if (item.GetState() != 1)
            continue;

        int prevAddInfo2 = (int)item.m_item.m_addInfo2;
        if (!GetItemInfo(m_pSeller, item))
        {
            ResetItems();
            m_errorHandler.Do(0x11);
            return false;
        }
        item.m_flag3d = 2;

        if ((int)item.m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
        {
            int expire =
                m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpireDate(
                    (int)item.m_item.m_addInfo2);
            if (expire != 0)
            {
                // 此失败路径不 ResetItems（ORIG 如此）。
                m_errorHandler.Do(0x11);
                return false;
            }
        }
        else
        {
            item.m_item.m_addInfo2 = prevAddInfo2;
        }
    }

    if (CheckTotalItemPrice() != 1)
    {
        ResetItems();
        m_errorHandler.Do(0x11);
        return false;
    }

    for (unsigned int i = 0; i < 0xe; ++i)
    {
        PrivateStoreItem& item = m_items[i];
        if (item.GetState() != 2)
            continue;
        if (DeleteItem(item))
        {
            item.m_flag3d = 3;
        }
        else
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x637, 5);
            trace("CPrivateStore [%s][%d]", __PRETTY_FUNCTION__, 0x637);
            ResetItems();
            m_errorHandler.Do(0x11);
            return false;
        }
    }
    return true;
}

// ---- RecallPrivateStoreItem（ORIG T 0x85c63ca，报告 RecallPrivateStoreItem.md L15-513 全文）----
// 第一轮把 state==3 条目按类型插回 avatar 背包(reason 0xa)/普通背包(reason 0x12)，
// 回填槽号后置 4；任一插入负值中断转第二轮回退 state==4→3；全部成功 ResetItems。
bool CPrivateStore::RecallPrivateStoreItem()
{
    unsigned int i = 0;
    for (; i < 0xe; ++i)
    {
        PrivateStoreItem& item = m_items[i];
        if (item.GetState() != 3)
            continue;

        int slot = -1;
        if ((int)item.m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
        {
            CInventory* invenW = m_pSeller->getCurCharacInvenW();
            slot = invenW->insertAvatarIntoInventory(item.m_item,
                                                     (eAvatarItemAddReason)0xa, false);
            if (slot < 0)
                break;
            m_pSeller->getCurCharacInvenR()->SendAvatarEvent(
                0x36, (int)item.m_item.m_addInfo2, 0, slot + 0xa, 0);
        }
        else
        {
            CInventory* invenW = m_pSeller->getCurCharacInvenW();
            slot = invenW->insertItemIntoInventory(item.m_item,
                                                   (eItemAddReason)0x12, false, true);
        }
        if (slot < 0)
            break;

        PrivateStoreItem info;
        info.m_field48 = (unsigned short)slot;   // 先写槽号 +0x48 再从背包回填数据
        GetItemInfo(m_pSeller, info);

        int addNew = 0;
        int addOld = 0;
        if (item.m_item.isEquipableItemType())
        {
            addNew = 1;
            addOld = 1;
        }
        else
        {
            addNew = (int)info.m_item.get_add_info();
            addOld = (int)item.m_item.get_add_info();
        }
        char const* characName = m_pSeller->getCurCharacName();
        // ORIG 对同一 getter 连调两次取名字（纯 getter，此处复用一次）。
        m_pSeller->m_historyLog.PrivateStoreItemAdd(
            (INVEN_TYPE)1, item.m_item, addNew, addOld, (eItemAddReason)0x12,
            characName, characName);

        item.m_field48 = (unsigned short)slot;
        item.m_flag3d = 4;
    }

    if (i != 0xe)
    {
        for (unsigned int j = 0; j < 0xe; ++j)
        {
            PrivateStoreItem& item = m_items[j];
            if (item.GetState() != 4)
                continue;
            if (DeleteItem(item))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x4c9, 5);
                trace("[%s][%d]", __PRETTY_FUNCTION__, 0x4c9);
            }
            item.m_flag3d = 3;   // DeleteItem 成败皆置
        }
        return false;
    }

    ResetItems();
    return true;
}

// ---- DeleteItem（ORIG T 0x85c68b4，报告 DeleteItem.md L15-440 全文）----
// 按 item space 四路删除来源槽：0=背包(INVEN_TYPE 1, count=+0x07)、1=avatar(reason 4，
// 成功补发事件)、2=cargo、3=装备栏(INVEN_TYPE 0，成功 send_equip)；其余直接成功。
bool CPrivateStore::DeleteItem(PrivateStoreItem& item)
{
    int sellPrice = item.m_field40;
    int resealCount = (int)item.m_item.GetReSealCount();
    int upgrade = (int)item.m_item.GetUpgrade();
    int addInfo2 = (int)item.m_item.m_addInfo2;
    int itemIdx = (int)item.m_item.m_addInfo;

    cMyTrace trace(__PRETTY_FUNCTION__, 0x4d7, 0);
    trace("PrivateStore del charac_name(%s)(%d), item_id(%d), add_info(%d), "
          "upgrade(%d), reseal_count(%d), sell_price(%d)",
          m_pSeller->getCurCharacName(), m_pSeller->getCurCharacNo(),
          itemIdx, addInfo2, upgrade & 0xff, resealCount & 0xff, sellPrice);

    int slot = (short)item.m_field48;
    switch (item.m_field46)
    {
    case 0:
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, (int)item.m_item.m_addInfo2,
                (eItemDelReason)0xb, true) != 1)
        {
            cMyTrace traceFail(__PRETTY_FUNCTION__, 0x4e0, 5);
            traceFail("Store::user_sell_item, delete_item failed , User ch=%d , %d %d",
                      m_pSeller->getCurCharacNo(), slot,
                      (int)item.m_item.m_addInfo2);
            m_errorHandler.Do(0x11);
            return false;
        }
        break;
    case 1:
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)2, slot, 1, (eItemDelReason)4, true) != 1)
        {
            cMyTrace traceFail(__PRETTY_FUNCTION__, 0x515, 5);
            traceFail("Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                      m_pSeller->getCurCharacNo(), slot);
            m_errorHandler.Do(0x11);
            return false;
        }
        m_pSeller->getCurCharacInvenR()->SendAvatarEvent(
            0x36, (int)item.m_item.m_addInfo2, 3, slot + 0xa, 0);
        break;
    case 2:
        if (m_pSeller->getCurCharacCargoW()->delete_item(
                slot, (int)item.m_item.m_addInfo2, (eItemDelReason)0xb) != 1)
        {
            cMyTrace traceFail(__PRETTY_FUNCTION__, 0x4ec, 5);
            traceFail("Store::user_sell_item, delete_item failed 2, User ch=%d , %d %d",
                      m_pSeller->getCurCharacNo(), slot,
                      (int)item.m_item.m_addInfo2);
            m_errorHandler.Do(0x11);
            return false;
        }
        break;
    case 3:
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)0, slot, 1, (eItemDelReason)0xb, true) != 1)
        {
            cMyTrace traceFail(__PRETTY_FUNCTION__, 0x4f9, 5);
            traceFail("Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                      m_pSeller->getCurCharacNo(), slot);
            m_errorHandler.Do(0x11);
            return false;
        }
        m_pSeller->send_equip(slot);
        break;
    default:
        break;
    }
    return true;
}

// ---- GetItemInfo（ORIG T 0x85c9f22，报告 GetItemInfo.md L15-218 全文；跳转表 .rodata 0x8cbd6e4 实读）----
// 按 item space 分派来源槽读取：[0]invR type1 [1]invR type2 [2]cargo [3]invR type0
// [7]invR type3；4/5/6 与越界同走默认失败。读回后置 +2 非零并过 CheckValidItem。
bool CPrivateStore::GetItemInfo(CUser* user, PrivateStoreItem& item)
{
    int prevAddInfo2 = (int)item.m_item.m_addInfo2;

    switch ((signed char)item.m_field46)
    {
    case 0:
    {
        Inven_Item slotData =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (short)item.m_field48);
        item.m_item.setCopy(slotData);
        break;
    }
    case 1:
    {
        Inven_Item slotData =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)2, (short)item.m_field48);
        item.m_item.setCopy(slotData);
        break;
    }
    case 2:
    {
        Inven_Item slotData =
            user->getCurCharacCargoR()->get_cargo_slot((short)item.m_field48);
        item.m_item.setCopy(slotData);
        break;
    }
    case 3:
    {
        Inven_Item slotData =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, (short)item.m_field48);
        item.m_item.setCopy(slotData);
        break;
    }
    case 7:
    {
        Inven_Item slotData =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, (short)item.m_field48);
        item.m_item.setCopy(slotData);
        break;
    }
    default:
        return false;   // 表项 4/5/6 与 ja 默认路径同址 0x85ca0cd
    }

    if ((int)item.m_item.m_addInfo == 0)
        return false;
    return CheckValidItem(item, prevAddInfo2);
}

// ---- SendUsers（ORIG T 0x85c7adc，报告 SendUsers.md L15-130 全文）----
// 围观列表广播，跳过 excludeA 与空指针；尾部对 excludeB 非空补发一次。返回发送数。
int CPrivateStore::SendUsers(PacketGuard* packet, CUser* excludeA, CUser* excludeB)
{
    int sentCount = 0;
    std::list<CUser*>::iterator it = m_users.begin();
    while (it != m_users.end())
    {
        CUser* user = *it;
        if (user != excludeA && user != 0)
        {
            user->Send(*packet);
            ++sentCount;
        }
        it++;
    }
    if (excludeB != 0)
    {
        excludeB->Send(*packet);
        ++sentCount;
    }
    return sentCount;
}

// ---- IsExistBuyerList（ORIG T 0x85c91b6，报告 IsExistBuyerList.md L15-91 全文）----
bool CPrivateStore::IsExistBuyerList(CUser* user)
{
    return std::find(m_users.begin(), m_users.end(), user) != m_users.end();
}

// ---- CheckValidItem 三重载（ORIG T 0x85c9d4e / 0x85c9bb8 / 0x85c9cc2，三份报告全文）----

// CheckValidItem(PrivateStoreItem&, int prevAddInfo2) @0x85c9d4e：
// 物品存在/可售类型/生物类排除（vtable+0x14=IsCreatureItem）后按 item space
// 分派来源槽校验；可堆叠条目要求现库存 >= 上架前数量。
bool CPrivateStore::CheckValidItem(PrivateStoreItem& item, int prevAddInfo2)
{
    CItem* info = G_CDataManager()->find_item((int)item.m_item.m_addInfo);
    if (info == 0)
        return false;

    int attachType = info->GetAttachType();
    if (attachType != 0)
    {
        if (attachType == 3)
        {
            if ((int)item.m_item.m_field0 != 1)
                return false;
        }
        else
        {
            if (attachType != 7)
                return false;
            if (!item.m_item.GetTradeLimitCount())
                return false;
        }
    }

    if (IsCreatureItemType((unsigned int)(unsigned char)item.m_item.m_field1))
        return false;

    // ORIG 经 CItem vtable+0x14 虚调（= IsCreatureItem），命中即拒。
    if (info->IsCreatureItem())
        return false;

    if (info->is_stackable() && (int)item.m_item.m_addInfo2 < prevAddInfo2)
        return false;   // asm：cur>=prev 才继续

    switch ((signed char)item.m_field46)
    {
    case 1:
        return CheckValidItem((INVEN_TYPE)1, (short)item.m_field48,
                              (int)item.m_item.m_addInfo2);
    case 0:
        return CheckValidItem((INVEN_TYPE)0, (short)item.m_field48, 1);
    case 2:
        return CheckValidItem((short)item.m_field48, (int)item.m_item.m_addInfo2);
    case 3:
        return CheckValidItem((INVEN_TYPE)2, (short)item.m_field48,
                              (int)item.m_item.m_addInfo2);
    default:
        return false;
    }
}

// CheckValidItem(INVEN_TYPE, int slot, int count) @0x85c9bb8：
// 槽位有效且非空；type==1 校验装备外数量余量；type==2 仅 avatar 且非临时键、未过期。
bool CPrivateStore::CheckValidItem(INVEN_TYPE type, int slot, int count)
{
    const CInventory* inventory = m_pSeller->getCurCharacInvenR();  // ORIG 读路径（InvenR）
    if (!inventory->CheckValidSlot((int)type, slot))
        return false;

    Inven_Item item = inventory->GetInvenSlot(type, slot);
    if ((int)item.m_addInfo == 0)
        return false;

    if ((int)type == 1)
    {
        if (!item.isEquipableItemType() && (int)item.m_addInfo2 - count < 0)
            return false;
    }
    else if ((int)type == 2)
    {
        if ((int)item.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
            return false;
        const WongWork::CAvatarItemMgr* mgr = inventory->GetAvatarItemMgrR();
        if (mgr->IsTempKey((int)item.m_addInfo2))
            return false;
        if (mgr->GetExpireDate((int)item.m_addInfo2) != 0)
            return false;
    }
    return true;
}

// CheckValidItem(int cargoSlot, int count) @0x85c9cc2：
// cargo 槽可操作且非空；装备放行，否则要求剩余数量 - count >= 0。
bool CPrivateStore::CheckValidItem(int cargoSlot, int count)
{
    const CCargo* cargo = m_pSeller->getCurCharacCargoR();
    if (!cargo->IsOperatorable(cargoSlot))
        return false;

    Inven_Item item = cargo->get_cargo_slot(cargoSlot);
    if ((int)item.m_addInfo == 0)
        return false;

    if (item.isEquipableItemType())
        return true;
    return (int)item.m_addInfo2 - count >= 0;
}

// ---- CErrorHandler::Set（ORIG W 0x85cb7a8，objdump 逐指令：三条直存无副作用）----
void CErrorHandler::Set(CUser* user, int sendError, int code)
{
    m_user = user;
    m_sendError = sendError;
    m_code = code;
}

// ---- CErrorHandler::Do（ORIG T 0x85c4c7c，报告 private_store__CErrorHandler/Do.md 全文）----
// 构造错误回执包 MakeErrorReturnPacket(m_sendError, m_code, errorCode) 发给目标用户，
// 返回目标是否有效。
bool CErrorHandler::Do(int errorCode)
{
    if (m_user == 0)
        return false;

    CPacketBufPrivate packet;
    packet.MakeErrorReturnPacket(m_sendError, m_code, errorCode);
    m_user->Send(packet);
    return true;
}

// ---- CheckTotalItemPrice（ORIG T 0x85c76f0，报告 CheckTotalItemPrice.md 汇编 L15-103/反编译C L105-160 全文）----
// 仅统计 state==2 在售条目；可堆叠经 COperator<int>::Product（ORIG 覆写 total 非累加），
// 不可堆叠累加且非正即失败。返回 total>0。
int CPrivateStore::CheckTotalItemPrice()
{
    int total = 0;
    for (int i = 0; i <= 0xd; ++i)
    {
        PrivateStoreItem& entry = m_items[i];
        if (entry.m_flag3d != 2)
            continue;
        if (entry.m_field40 == 0)
            return 0;
        CItem* pItem = G_CDataManager()->find_item((int)entry.m_item.m_addInfo);
        if (pItem == 0)
            return 0;
        if (pItem->is_stackable())
        {
            if (COperator<int>::Product(entry.m_field40,
                                        (int)entry.m_item.m_addInfo2, total) != 1)
                return 0;
        }
        else
        {
            total += entry.m_field40;
            if (total <= 0)
                return 0;
        }
    }
    return (total > 0) ? 1 : 0;
}

// ---- IsPossibleDeal（ORIG T 0x85c7c0a，报告 IsPossibleDeal.md 全文）----
// 卖家 CheckMoney 失败 → Do(0x16)+定向错误包；买家余额不足 → Do(10)。a3/a4 未用。
bool CPrivateStore::IsPossibleDeal(int gold, unsigned int /*a3*/, unsigned int /*a4*/)
{
    if (!m_pSeller->CheckMoney(gold))
    {
        m_errorHandler.Do(0x16);
        CPacketBufPrivate packet;
        packet.MakeErrorReturnPacket(0, 0x52, 0x16);
        m_pSeller->Send(packet);
        return false;
    }
    if ((int)m_pBuyer->getCurCharacInvenR()->get_money() < gold)
    {
        m_errorHandler.Do(10);
        return false;
    }
    return true;
}

// ---- SubtractItemCount（ORIG T 0x85c7bbc，报告 SubtractItemCount.md 全文）----
// 计数字段 = 条目 +0x07 = m_item.m_addInfo2。非堆叠或减至恰好相等时整条 Reset。
void CPrivateStore::SubtractItemCount(CItem const* item, PrivateStoreItem& storeItem,
                                      int count)
{
    if (!item->is_stackable())
        storeItem.Reset();
    else if ((int)storeItem.m_item.m_addInfo2 == count)
        storeItem.Reset();
    else
        storeItem.m_item.m_addInfo2 -= count;
}

// ---- GetItemPrice（ORIG T 0x85c7d04，报告 GetItemPrice.md 全文）----
// 引参写出顺序（ORIG 硬序）：outPrice←+0x40 先于一切校验，随后两金清零；
// 堆叠分支做数量乘并溢出检测，非堆叠仅放行化身/整条购买。
bool CPrivateStore::GetItemPrice(CItem const* item, PrivateStoreItem& storeItem,
                                 int count, int& outPrice,
                                 unsigned int& outBuyGold, unsigned int& outSellGold)
{
    outPrice = storeItem.m_field40;   // 校验前先写单价
    outBuyGold = 0;
    outSellGold = 0;

    if (item->is_stackable())
    {
        if (count < 1)
        {
            m_errorHandler.Do(1);
            return false;
        }
        if ((int)storeItem.m_item.m_addInfo2 < count)   // 库存数 < 请求数
        {
            m_errorHandler.Do(0x11);
            return false;
        }
        int priceSnapshot = outPrice;   // ORIG 先取旧值再 Product 覆写
        if (COperator<int>::Product(priceSnapshot, count, outPrice) != 1)
        {
            m_errorHandler.Do(1);
            return false;
        }
        outBuyGold = outBuyGold * count;   // 0*count（ORIG imul 保形）
        outSellGold = outSellGold * count;
    }
    else
    {
        // ORIG vtable+0x10 虚调 = CItem::IsAvatarItem（CCeraShop_Special.cpp:1024 同证）。
        // 仅当「非化身 且 库存数 != 请求数」报错。
        if (!item->IsAvatarItem() && (int)storeItem.m_item.m_addInfo2 != count)
        {
            m_errorHandler.Do(1);
            return false;
        }
    }

    if (outPrice <= 0)
        m_errorHandler.Do(1);
    return outPrice > 0;
}

// ---- GetCommissionPercent（ORIG T 0x85c9b8c，报告 GetCommissionPercent.md 全文）----
// 频道类型 3 → 3%，否则 4%。
int CPrivateStore::GetCommissionPercent()
{
    if (G_GameWorld()->GetChannelType() == 3)
        return 3;
    return 4;
}

// ---- BuyItem（ORIG T 0x85c924c，报告 BuyItem.md 汇编 L15-401/反编译C L403-545 全文）----
// 物品入买方背包（avatar 走 mgr 所有权转移链），扣买方货款、安全上报与历史；
// slot/commissionBase/flag 三参在 ORIG 函数体内未被读取。
int CPrivateStore::BuyItem(int /*slot*/, Inven_Item& item, int gold,
                           int /*commissionBase*/, int /*flag*/, int& outSlot)
{
    unsigned int resealCount = (unsigned int)item.GetReSealCount();
    unsigned int upgrade = (unsigned int)item.GetUpgrade();
    char const* sellerName = m_pSeller->getCurCharacName();
    char const* buyerName = m_pBuyer->getCurCharacName();

    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xa63, 0);
    trace("PrivateStore buy seller_charac_name(%s), buyer_charac_name(%s), "
          "item_id(%d), add_info(%d), upgrade(%d), reseal_count(%d), sell_price(%d)",
          sellerName, buyerName, (int)item.m_addInfo, (int)item.m_addInfo2,
          upgrade, resealCount, gold);

    int slot;
    if ((unsigned char)item.m_field1 == 8)   // ITEM_TYPE_AVATAR
    {
        slot = m_pBuyer->getCurCharacInvenW()->tryInsertAvatarIntoInventory(item);
        if (slot >= 0)
        {
            WongWork::CAvatarItemMgr* sellerMgr =
                m_pSeller->getCurCharacInvenW()->GetAvatarItemMgrW();
            WongWork::Avatar_Item* erased = sellerMgr->EraseAvatarItem((int)item.m_addInfo2);
            if (erased == 0)
                return -1;

            slot = m_pBuyer->getCurCharacInvenW()->insertAvatarIntoSpecificSlot(
                item, slot, (eAvatarItemAddReason)0xb, false);
            if (slot == -1)
                return -1;

            m_pBuyer->getCurCharacInvenW()->GetAvatarItemMgrW()->AddAvatarItem(
                (int)item.m_addInfo2, erased);

            DB_AvatarChangeOwner::makeRequest(
                (unsigned int)item.m_addInfo2,
                (unsigned int)m_pSeller->getCurCharacNo(),
                (unsigned int)m_pBuyer->getCurCharacNo(),
                (unsigned char)(slot + 10));
            m_pBuyer->getCurCharacInvenR()->SendAvatarEvent(
                0x36, (int)item.m_addInfo2, 0, slot + 10, 0);
        }
    }
    else
    {
        slot = m_pBuyer->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0x11, false, false);
    }

    if (slot < 0)
        return slot;

    // 本地条目：仅预写槽位号到 +0x48，再由 GetItemInfo 按 +0x46 区切回填完整数据。
    PrivateStoreItem storeEntry;
    storeEntry.m_field48 = (unsigned short)slot;
    GetItemInfo(m_pBuyer, storeEntry);

    int historyAddInfo;
    if (item.isEquipableItemType())
    {
        outSlot = 1;
        historyAddInfo = 1;
    }
    else
    {
        outSlot = (int)storeEntry.m_item.get_add_info();
        historyAddInfo = (int)item.get_add_info();
    }

    m_pBuyer->m_historyLog.PrivateStoreItemAdd(
        (INVEN_TYPE)1, item, outSlot, historyAddInfo, (eItemAddReason)0x11,
        sellerName, buyerName);

    // XNuclear::CHades::GetIP @0x84b9774（CUser.h 已声明，直接调用与 ORIG 字节一致）
    char ipBuf[16];
    m_pSeller->getHades()->GetIP(ipBuf);

    unsigned int buyerMoney =
        (unsigned int)m_pBuyer->getCurCharacInvenR()->get_money();
    // Secu_AccountHacking::sendGold @0x8278cce（CHackAnalyzer.h 已声明，
    // 直接调用与 ORIG 字节一致；参数 (0, sellerId, gold, buyerMoney, 1, ip)）。
    m_pBuyer->getAcccountHackInfo()->sendGold(
        0, (unsigned int)m_pSeller->get_acc_id(), (unsigned int)gold,
        buyerMoney, 1, ipBuf);

    m_pBuyer->getCurCharacInvenW()->use_money(gold, (eMoneySubReason)0xe, true);

    HistoryLog::WriteBuyItem((FILE*)m_pBuyer->m_field796f8,   // +0x796f8 句柄（int→FILE* 直转，先例 CGameManager.cpp:2727）
                             (int)item.m_addInfo, (int)item.m_addInfo2);
    return slot;
}

// ---- SellItem（ORIG T 0x85c97e2，报告 SellItem.md 汇编 L15-274/反编译C L276-419 全文）----
// 净额入卖方：无 doll 直接 gain_money；有 doll 组邮件文本走系统邮件。
// slot 参数未读；恒返回 1。
int CPrivateStore::SellItem(int /*slot*/, Inven_Item& item, int gold,
                            int commission, int count, int field7)
{
    int netGold = gold - commission;
    if (!item.isEquipableItemType())
        field7 -= count;
    else
    {
        field7 = 0;
        count = 1;
    }

    m_pSeller->m_historyLog.PrivateStoreItemDel(
        (INVEN_TYPE)1, item, count, field7, (eItemDelReason)0x16,
        m_pSeller->getCurCharacName(), m_pBuyer->getCurCharacName());

    if (!isDollCreated())
    {
        m_pSeller->getCurCharacInvenW()->gain_money(
            netGold, (eMoneyAddReason)0xb, true, 0);
    }
    else
    {
        char msgBuf[256];
        std::memset(msgBuf, 0, sizeof(msgBuf));       // ORIG 0x100 清零
        char title[21];
        std::memset(title, 0, sizeof(title));         // ORIG 0x15 清零

        std::strncpy(title,
                     g_scriptStringManager_.findString(4, "game_server_msg_183", 0),
                     0x14);

        CItem* pItem = G_CDataManager()->find_item((int)item.m_addInfo);
        if (pItem->is_stackable())
        {
            // ORIG 对 GetItemName 连续两次独立调用，保形转写。
            OS_API::snprintf(msgBuf, 0xff,
                             g_scriptStringManager_.findString(4, "game_server_msg_185", 0),
                             pItem->GetItemName(), gold, commission,
                             pItem->GetItemName(), netGold);
        }
        else
        {
            OS_API::snprintf(msgBuf, 0xff,
                             g_scriptStringManager_.findString(4, "game_server_msg_184", 0),
                             pItem->GetItemName(), count, gold, commission,
                             pItem->GetItemName(), netGold);
        }
        Inven_Item mailItem;   // ORIG 默认构造 W 0x80cb854
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, mailItem, (unsigned int)netGold,
            (unsigned int)m_pSeller->getCurCharacNo(),
            msgBuf, (int)std::strlen(msgBuf), 0u,
            m_pSeller->GetServerGroup(), false, false);
    }
    HistoryLog::WriteSellItem((FILE*)m_pSeller->m_field796f8,
                              (int)item.m_addInfo, (int)item.m_addInfo2);
    return 1;
}


// ---- CBroadcastPrivate::Send（ORIG W 0x85cca73，objdump 逐指令转写）----
// vill/area 双零直接返回；否则交 GameWorld 按村庄+区域广播。
void CBroadcastPrivate::Send(int vill, int area, PacketGuard& packet)
{
    if (vill == 0 && area == 0)
        return;
    G_GameWorld()->send_to_area(vill, area, packet);   // ORIG 0x86c8168
}

// ---- CCharacterControllerPrivate::MoveCharacterStartPoint（ORIG W 0x85cc9d2，objdump 逐指令）----
// 以 this 充当起点 Pos 重算后 move_area 回城门（z/flag/prev 全 0，返回值忽略）。
void CCharacterControllerPrivate::MoveCharacterStartPoint(CUser* user)
{
    GetStartPoint(user, m_startPos);
    G_GameWorld()->move_area(user,
                             (int)m_startPos.m_a,
                             (int)m_startPos.m_b,
                             (int)(short)m_startPos.m_field2,
                             (int)(short)m_startPos.m_field4,
                             0, false, 0, 0, 0);       // ORIG move_area 0x86c5a84
}

// ORIG B _ZN13private_store21g_CharacterControllerE @0x09472de0（BSS size 6）
CCharacterControllerPrivate g_CharacterController;

// ---- CErrorGeneratorPrivate（ORIG C1 W 0x85d0fa4 / D1 W 0x85d0f7c，报告缺失，objdump 转写）----
// 唯一成员 std::map<CUser*,int>：C1 调 map C1、D1 调 map D1（sizeof 0x18）。
CErrorGeneratorPrivate::CErrorGeneratorPrivate() : m_errors() {}

CErrorGeneratorPrivate::~CErrorGeneratorPrivate() {}

// ORIG B _ZN13private_store16g_ErrorGeneratorE @0x09472de8（BSS size 0x18）；
// 全局构造序列（_GLOBAL__I__...GetInstancePrivateStoreMgr）实证：C1 构造 + __cxa_atexit(D1)。
CErrorGeneratorPrivate g_ErrorGenerator;

// ---- CCharacterControllerPrivate（ORIG C1 W 0x85d0f90）----
CCharacterControllerPrivate::CCharacterControllerPrivate() : m_startPos() {}

// ---- CCharacterControllerPrivate::GetStartPoint（ORIG W 0x85cc894）----
// 计算私商开店起始坐标：pos 先清零再写 village 号(a)；user/world/village 缺失
// 时仅 trace 并保持 b=0。注意 ORIG 后两个失败分支复用 "G_GameWorld call failed"
// 文案（0x8cbd0cf，原版即如此），逐字保留。
void CCharacterControllerPrivate::GetStartPoint(CUser* user, PrivateStorePos& pos)
{
    pos.m_field2 = 0x172;
    pos.m_field4 = 0xbe;
    pos.m_a = 0;
    pos.m_b = 0;
    if (user == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x183, 5);
        trace("pUser == NULL");
        return;
    }
    pos.m_a = (unsigned char)user->getCurCharacVill();
    GameWorld* world = G_GameWorld();
    if (world == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x18b, 5);
        trace("G_GameWorld call failed");
        return;
    }
    Village* village = world->getUserVillage(user);
    if (village == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x190, 5);
        trace("G_GameWorld call failed");  // ORIG 原样复用该文案（0x8cbd0cf）
        return;
    }
    pos.m_b = (unsigned char)village->get_gate_area();
}

// ---- CPacketBufPrivate::MakeErrorReturnPacket（ORIG W 0x85cc830）----
void CPacketBufPrivate::MakeErrorReturnPacket(int code1, int code2, int code3)
{
    clear();
    put_header(code1, code2);
    put_byte(0);
    put_byte(code3);
    finalize(true);
}

// ORIG 0x085ca358（报告 FindStoreSeller.md 逐指令）：卖家表(+0x98) find，命中返 mapped。
CPrivateStore* CPrivateStoreMgr::FindStoreSeller(CUser* user)
{
    if (!user)
        return 0;
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeSellers.find(user);
    if (it != m_storeSellers.end())
        return it->second;
    return 0;
}

// ORIG 0x085ca2da（报告 FindStoreBuyer.md 逐指令）：买家表(+0xb0) find，命中返 mapped。
CPrivateStore* CPrivateStoreMgr::FindStoreBuyer(CUser* user)
{
    if (!user)
        return 0;
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeBuyers.find(user);
    if (it != m_storeBuyers.end())
        return it->second;
    return 0;
}

// ORIG 0x85ca914（报告 FreeStoreBuyer.md 逐指令）：买家表 find 命中则 erase，返回命中与否。
int CPrivateStoreMgr::FreeStoreBuyer(CUser* user)
{
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeBuyers.find(user);
    if (it != m_storeBuyers.end())
    {
        m_storeBuyers.erase(it);
        return 1;
    }
    return 0;
}

// ORIG 0x85ca874（报告 FreeStoreSeller.md 逐指令）：卖家表 find 命中则先经
// StaticPool(+0x68)::Free 归还 store 对象再 erase 表项，返回命中与否。
int CPrivateStoreMgr::FreeStoreSeller(CUser* user)
{
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeSellers.find(user);
    if (it != m_storeSellers.end())
    {
        m_storePool.Free(it->second);
        m_storeSellers.erase(it);
        return 1;
    }
    return 0;
}

// ORIG 0x085ca698（报告 GetStoreSeller.md 逐指令）：已开店则直取；否则从
// StaticPool(+0x68) 申请——池满退回 +0x04 嵌入 store（SetSeller 后返回 0），
// 成功则 Clear/SetParent/SetSeller 并登记卖家表，*out=store 返回 1。
int CPrivateStoreMgr::GetStoreSeller(ISeller** out, CUser* user)
{
    CPrivateStore* store;
    if ((store = FindStoreSeller(user)) == 0)
    {
        if ((store = m_storePool.Acquire()) == 0)
        {
            *out = &m_embeddedStore;
            m_embeddedStore.SetSeller(user);
            return 0;
        }
        store->Clear();
        store->SetParent(this);
        store->SetSeller(user);
        m_storeSellers[user] = store;
    }
    *out = store;
    return 1;
}

// ORIG 0x085ca7b8（报告 GetStoreBuyer_085ca7b8.md 逐指令）：买家未登记则按卖家
// 查其 store 并登记买家表；两者皆无时回退 +0x04 嵌入 store 的 IBuyer 子对象
// （this+8）并 SetBuyer 返回 0；命中时 *out 为 store+4 之 IBuyer 接口。
int CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser* user, CUser* sellerUser)
{
    CPrivateStore* store;
    if ((store = FindStoreBuyer(user)) == 0)
    {
        if ((store = FindStoreSeller(sellerUser)) == 0)
        {
            *out = &m_embeddedStore;   // 派生→IBuyer 隐式转换即 +4（ORIG this+8）
            m_embeddedStore.SetBuyer(user);
            return 0;
        }
        m_storeBuyers[user] = store;
    }
    *out = (store != 0) ? store : 0;   // 派生→IBuyer 隐式转换产生 store+4
    store->SetBuyer(user);
    return 1;
}

// ORIG 0x085ca760（报告 GetStoreBuyer.md 逐指令）：按 world unique id 解析卖家
// CUser（初始置空）后委托 GetStoreBuyer(3 参版)。
void CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser* user, unsigned short sellerIndex)
{
    unsigned short idx = sellerIndex;
    CUser* sellerUser = 0;
    sellerUser = G_GameWorld()->find_from_world(idx);
    GetStoreBuyer(out, user, sellerUser);
}

// ORIG T 0x85caa8a（报告 Enter.md 逐指令）：委托 LoadPrivateStore(user)，恒返 1。
int CPrivateStoreMgr::Enter(CUser* user)
{
    LoadPrivateStore(user);
    return 1;
}

// ORIG 0x85ca994（报告 Exit.md 逐指令）：买家身份在册则经 IBuyer 虚槽 +0xc
// （ExitPrivateStore）通知后 FreeStoreBuyer；卖家身份在册则经 ISeller 虚槽 +4
// （RemovePrivateStore(true)）通知后 FreeStoreSeller；user 空指针返 0。
int CPrivateStoreMgr::Exit(CUser* user)
{
    if (user == 0)
        return 0;
    CPrivateStore* store;
    if ((store = FindStoreBuyer(user)) != 0)
    {
        IBuyer* buyer = 0;
        GetStoreBuyer(&buyer, user, (CUser*)0);
        buyer->ExitPrivateStore();
        FreeStoreBuyer(user);
    }
    if ((store = FindStoreSeller(user)) != 0)
    {
        ISeller* seller = 0;
        GetStoreSeller(&seller, user);
        seller->RemovePrivateStore(true);
        FreeStoreSeller(user);
    }
    return 1;
}

// ORIG T 0x85caaaa（报告 HandleError.md 逐指令）：栈上 CPacketBufPrivate 构造
// MakeErrorReturnPacket(code1,code2,code3) 后经 CUser::Send 下发，析构，返回 0。
int CPrivateStoreMgr::HandleError(CUser* user, int code1, int code2, int code3)
{
    CPacketBufPrivate packet;
    packet.MakeErrorReturnPacket(code1, code2, code3);
    user->Send(packet);
    return 0;
}

// ORIG 0x85cad5e（报告 IsBusyPrivateStore.md 逐指令）：卖家在册且状态为 1/2 时，
// 未生成娃娃即忙碌；已生成娃娃仅状态 1 忙碌。
bool CPrivateStoreMgr::IsBusyPrivateStore(CUser* user)
{
    CPrivateStore* store = 0;
    store = GetInstancePrivateStoreMgr()->FindStoreSeller(user);
    if (store != 0 && (store->GetState() == 1 || store->GetState() == 2))
    {
        if (store->isDollCreated() == 0)
            return true;
        return store->GetState() == 1;
    }
    return false;
}

// ORIG 0x85cab24（报告 LoadPrivateStore.md 逐指令）：申请 Stream 构造 CStreamGuard，
// 写 0x78(type)、GetUID、get_charac_no(-1) 三项后放入 DB 消息队列(DB_Q=2)。
void CPrivateStoreMgr::LoadPrivateStore(CUser* user)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PrivateStore.cpp", 0xcb9), true);
    guard->operator<<(0x78);
    guard->operator<<((int)user->GetUID());
    guard->operator<<(user->get_charac_no(-1));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- MakeNotipacketPrivateStoreInfo（ORIG W 0x85cc5bc，objdump 逐指令转写）----
// 通知包：header(0,0x55)、byte(sig+4)、byte(占位计数)后遍历 14 条目，
// state==3 者写 short(槽)/int(售价 +0x40)/int(item idx)/int(add_info2)/
// byte(GetItemAttr)/short(endurance)/int(+0xd)/byte+short(强化类型与值)/put_packet(item)，
// 回填计数占位并 finalize(true)。返回上柜条数。
int CPacketBufPrivate::MakeNotipacketPrivateStoreInfo(SIG_PRIVATE_STORE* sig)
{
    clear();
    put_header(0, 0x55);
    put_byte((int)sig->m_field04);
    int countIndex = get_index();
    put_byte(0);

    int count = 0;
    PrivateStoreItem* items = reinterpret_cast<PrivateStoreItem*>(sig->m_items);
    for (int i = 0; i <= 0xd; ++i)
    {
        if (items[i].GetState() != 3)
            continue;
        put_short(i);
        put_int(items[i].m_field40);                 // 售价（条目 +0x40）
        put_int(items[i].m_item.m_addInfo);          // 物品索引（Inven_Item +0x02）
        put_int(items[i].m_item.m_addInfo2);         // Inven_Item +0x07
        put_byte((int)items[i].m_item.GetItemAttr());
        put_short((int)items[i].m_item.m_fieldb);    // 耐久（Inven_Item +0x0b）
        put_int(items[i].m_item.m_fieldd);           // Inven_Item +0x0d
        put_byte((int)items[i].m_item.m_amp.getAbilityType());
        put_short((int)items[i].m_item.m_amp.getAbilityValue());
        put_packet(items[i].m_item);
        ++count;
    }

    put_byte(countIndex, count);
    finalize(true);
    return count;
}

// ---- 9 槽全链包构造（ORIG W 内联符号，objdump 逐指令转写，PsBuildB 交付）----

// ORIG _ZN13private_store17CPacketBufPrivate31MakeCmdpacketCreatePrivateStoreEv @0x85cb986
void CPacketBufPrivate::MakeCmdpacketCreatePrivateStore()
{
    clear();
    put_header(1, 0x58);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate32MakeNotipacketCreatePrivateStoreEtRNS_15PrivateStorePosEb @0x85cb9da
void CPacketBufPrivate::MakeNotipacketCreatePrivateStore(unsigned short sellerIndex,
                                                         PrivateStorePos& pos, bool dollCreated)
{
    clear();
    put_header(0, 0x4e);
    put_short((int)sellerIndex);
    put_byte((int)pos.m_a);
    put_byte((int)pos.m_b);
    put_short((int)(short)pos.m_field2);
    put_short((int)(short)pos.m_field4);
    if (dollCreated)
        put_byte(1);
    else if (!dollCreated)
        put_byte(0);   // ORIG xor $1 二次测试的字面结构（净语义等价 else）
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate23MakeCmdpacketMoveToGateEv @0x85cc7dc
void CPacketBufPrivate::MakeCmdpacketMoveToGate()
{
    clear();
    put_header(1, 0x5b);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate32MakeNotipacketRemovePrivateStoreEt @0x85cbb2c
void CPacketBufPrivate::MakeNotipacketRemovePrivateStore(unsigned short sellerIndex)
{
    clear();
    put_header(0, 0x4f);
    put_short((int)sellerIndex);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate31MakeCmdpacketRemovePrivateStoreEv @0x85cbad8
void CPacketBufPrivate::MakeCmdpacketRemovePrivateStore()
{
    clear();
    put_header(1, 0x59);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate37MakeNotipacketForceRemovePrivateStoreEv @0x85cbb88
void CPacketBufPrivate::MakeNotipacketForceRemovePrivateStore()
{
    clear();
    put_header(0, 0x50);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate29MakeCmdpacketExitPrivateStoreEv @0x85cc568（包仅发买家）
void CPacketBufPrivate::MakeCmdpacketExitPrivateStore()
{
    clear();
    put_header(1, 0x57);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate30MakeCmdpacketEnterPrivateStoreEP5CUsertRSt6vectorINS_16PrivateStoreItemESaIS4_EE @0x85cc160
// 仅序列化 GetState()==3 的条目；条目数先写占位字节、循环后回填。
void CPacketBufPrivate::MakeCmdpacketEnterPrivateStore(CUser* sellerUser,
                                                       unsigned short sellerIndex,
                                                       std::vector<PrivateStoreItem>& items)
{
    clear();
    put_header(1, 0x56);
    put_byte(1);
    put_short((int)sellerIndex);

    int countIndex = get_index();               // ORIG 0x8110b4c
    put_byte(0);
    int nowTick = OS_API::GetDateTimeTick();
    int writtenCount = 0;
    for (int i = 0; i <= 13; ++i)
    {
        if (items[i].GetState() != 3)
            continue;
        put_short(i);
        put_int(items[i].m_field40);                              // +0x40 售价
        put_int((int)items[i].m_item.m_addInfo);                  // +0x02 物品索引
        if ((int)items[i].m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
        {
            // avatar：剩余时间按 uid 实时查询（tick 在循环外取一次）。
            put_int(sellerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                        (int)items[i].m_item.m_addInfo2, nowTick));
        }
        else
        {
            put_int((int)items[i].m_item.m_addInfo2);             // +0x07
        }
        put_byte((int)items[i].m_item.GetItemAttr());
        put_short((int)items[i].m_item.m_fieldb);                 // +0x0b 耐久
        put_int((int)items[i].m_item.m_fieldd);                   // +0x0d
        put_byte((int)items[i].m_item.m_amp.getAbilityType());
        put_short((int)items[i].m_item.m_amp.getAbilityValue());
        if ((int)items[i].m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
        {
            put_int(0x1e);
            put_binary((char const*)sellerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                           (int)items[i].m_item.m_addInfo2), 0x1e);
            put_int(4);
            void* expansion =
                sellerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    (int)items[i].m_item.m_addInfo2);
            if (expansion != 0)
            {
                put_binary((char const*)expansion, 4);
            }
            else
            {
                stAvatarExpansionInfo_t defaultExpansion;
                defaultExpansion.init();
                put_binary((char const*)&defaultExpansion, 4);
            }
        }
        put_packet(items[i].m_item);          // ORIG put_packetERK10Inven_Item（const&）
        ++writtenCount;
    }
    put_byte(countIndex, writtenCount);       // 回填占位字节（put_byteERii 0x8110b28）
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate29MakeNotipacketCompleteDisplayEtRSsb @0x85cbc52
void CPacketBufPrivate::MakeNotipacketCompleteDisplay(unsigned short sellerIndex,
                                                      std::string& subject,
                                                      bool dollCreated)
{
    clear();
    put_header(0, 0x51);
    put_short((int)sellerIndex);
    put_int((int)subject.length());
    put_str(subject.c_str(), (int)subject.length());
    if (dollCreated)
        put_byte(1);
    else
        put_byte(0);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate28MakeCmdpacketCompleteDisplayEb @0x85cbbca
void CPacketBufPrivate::MakeCmdpacketCompleteDisplay(bool dollCreated)
{
    clear();
    put_header(1, 0x5a);
    put_byte(1);
    if (dollCreated)
        put_byte(1);
    else
        put_byte(0);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate30MakeNotipacketPrivateStoreItemEii @0x85cc0fa（SendUsers 广播）
void CPacketBufPrivate::MakeNotipacketPrivateStoreItem(int slot, int count)
{
    clear();
    put_header(0, 0x53);
    put_short(slot);
    put_int(count);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate33MakeNotipacketBuyPrivateStoreItemEP5CUseriiiPKcii @0x85cbfea
// buyerUser=卖家 m_pSeller（收包人，序列化其金钱）；avatar 条目时 count 已由调用方换成 avatar uid。
void CPacketBufPrivate::MakeNotipacketBuyPrivateStoreItem(CUser* buyerUser, int itemId, int slot,
                                                          int count, char const* name,
                                                          int gold, int commission)
{
    clear();
    put_header(0, 0x52);
    put_byte(1);
    put_int(buyerUser->getCurCharacInvenR()->get_money());
    put_int(itemId);
    put_short(slot);
    put_int(count);
    put_int((int)std::strlen(name));
    put_str(name, (int)std::strlen(name));
    put_int(gold);
    put_int(commission);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate32MakeCmdpacketBuyPrivateStoreItemEP5CUserR10Inven_Itemiiii @0x85cbd38
// buyerUser=买家 m_pBuyer（收包人，序列化金钱/胜点/cera）；a3/a4 在 ORIG 函数体内从未读取；
// a5=BuyItem 返回的落位槽位；a6=购买数量（avatar 时为 uid）。
void CPacketBufPrivate::MakeCmdpacketBuyPrivateStoreItem(CUser* buyerUser, Inven_Item& item,
                                                         int a3, int a4, int a5, int a6)
{
    (void)a3;   // ORIG 未读参数占位
    (void)a4;
    clear();
    put_header(1, 0x55);
    put_byte(1);
    put_int(buyerUser->getCurCharacInvenR()->get_money());
    put_int(buyerUser->GetWinPoint());
    put_int(buyerUser->GetCera());
    put_short(a5);
    put_int((int)item.m_addInfo);
    int nowTick = OS_API::GetDateTimeTick();
    if ((int)item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
        put_int(buyerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(a6, nowTick));
    else
        put_int(a6);
    put_byte((int)item.GetItemAttr());
    put_short((int)item.m_fieldb);
    put_int((int)item.m_fieldd);
    put_byte((int)item.m_amp.getAbilityType());
    put_short((int)item.m_amp.getAbilityValue());
    if ((int)item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
    {
        put_int(0x1e);
        put_binary((char const*)buyerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(a6),
                   0x1e);
        put_int(4);
        void* expansion =
            buyerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(a6);
        if (expansion != 0)
        {
            put_binary((char const*)expansion, 4);
        }
        else
        {
            stAvatarExpansionInfo_t defaultExpansion;
            defaultExpansion.init();
            put_binary((char const*)&defaultExpansion, 4);
        }
    }
    put_packet(item);
    finalize(true);
}

// ORIG T 0x85cac16（报告 LoadPrivateStoreResponse.md 逐指令）。
//   sig->m_dollCreated==1（DB 恢复失败回执）：GetStoreSeller → ResetItems →
//     CopyItems(sig 条目,0xe) → RecallPrivateStoreItemToMail → SavePrivateStore →
//     Clear → FreeStoreSeller；
//   否则若 IsPermissionPrivateStore：MakeNotipacketPrivateStoreInfo(sig) → Send；
//     包内条数非 0 时再经 ISeller 虚槽 +0x14（RecreatePrivateStore）通知卖家。
void CPrivateStoreMgr::LoadPrivateStoreResponse(CUser* user, SIG_PRIVATE_STORE* sig)
{
    int made = 0;
    if (sig->m_dollCreated == 1)
    {
        ISeller* seller;
        GetStoreSeller(&seller, user);
        CPrivateStore* store = static_cast<CPrivateStore*>(seller);
        store->ResetItems();
        store->CopyItems(reinterpret_cast<PrivateStoreItem*>(sig->m_items), 0xe);
        store->RecallPrivateStoreItemToMail();
        store->SavePrivateStore();
        store->Clear();
        FreeStoreSeller(user);
    }
    else if (user->IsPermissionPrivateStore())
    {
        CPacketBufPrivate packet;
        made = packet.MakeNotipacketPrivateStoreInfo(sig);
        user->Send(packet);
        if (made != 0)
        {
            ISeller* seller;
            GetStoreSeller(&seller, user);
            seller->RecreatePrivateStore(sig);
        }
    }
}

// ORIG T 0x86d23a4（objdump 逐指令）：返回卖家表(+0x98) size()。
int CPrivateStoreMgr::GetPrivateStoreSize()
{
    return (int)m_storeSellers.size();
}
}

// ---- 暂落定义（三处 ORIG 真实符号，权威归属批次后续收编；本批唯一落点 CPrivateStoreMgr.cpp）----

// ORIG T _ZN14village_object17CVillageObjectMgr17unregister_objectENS_4ZoneEPNS_7IObjectE @0x86b70d0
// objdump 逐指令转写（无独立 md 报告，自行导出并标记）：map find→end 判定→std::find→erase。
int village_object::CVillageObjectMgr::unregister_object(village_object::Zone zone,
                                                         village_object::IObject* object)
{
    std::map<village_object::Zone, std::list<village_object::IObject*> >::iterator it =
        m_objects.find(zone);
    if (it == m_objects.end())
        return 0;
    std::list<village_object::IObject*>& objects = (*it).second;
    std::list<village_object::IObject*>::iterator found =
        std::find(objects.begin(), objects.end(), object);
    if (found == objects.end())
        return 0;
    objects.erase(found);
    return 1;
}

// ORIG W _ZNK5CItem12GetCashPriceEv @0x822c856：objdump 逐指令 mov 0x10(%eax) 直返。
int CItem::GetCashPrice() const
{
    return m_field10;   // +0x10
}

// ORIG W _ZN17STExpertJobScript15GetShopDollSlotEi @0x85cb306：objdump 逐指令转写。
// m_shopDollMap.find(index) 命中返 mapped 值，未命中哨兵 -1；唯一调用点 AddItem 0x85c6dfc。
short STExpertJobScript::GetShopDollSlot(int index)
{
    std::map<int, unsigned char>::iterator it = m_shopDollMap.find(index);
    if (it != m_shopDollMap.end())
        return (short)(*it).second;   // +0x04 mapped value（movzbl 零扩展）
    return -1;
}
