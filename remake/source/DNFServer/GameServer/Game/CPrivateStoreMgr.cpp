// ============================================================================
// [b28d-solo-CPrivateStoreMgr] 泳道判词（唯一写者：S413_CPrivateStoreMgr）
// 基线 identical=146 ae=289 near=1 diff=19 → 收口 identical=146 ae=294
// near=1 diff=14（diff 列表含 N123 噪音 _Z41__static_initialization...）。
// 本轮 20 件清单：完全修复 5 件（MoveToGate / RemovePrivateStore /
// SavePrivateStore / put_object_info / MoveItems，均为 ae——残差仅
// __PRETTY_FUNCTION__/memset@plt 重定位显示形，AE 等价）；部分修复 6 件
// （CreatePrivateStore / DeleteItem / RecallPrivateStoreItem /
// RecallPrivateStoreItemToMail / IsAreaNearEntranceDungeon / AddItem）；
// 余 8 件为 N 族封顶（N82 帧槽 / N93 块内对象槽向 / N98 首次使用序 /
// N99 死比较 DCE / P75 实参装载序），逐件证据见各函数头注释与
// /tmp/b28d_solo_CPrivateStoreMgr_result.tsv。
// P167：清单内全部函数调用点差集 CLEAR（0 missing / 0 extra）。
// 无子子代理；本轮无 .h 改动落盘（CheckTotalItemPrice int→bool 已按 R2/S2
// 申报 Main 代落，依据 ORIG 0x085c73e2 `xor eax,0x1`，T3 直证）。
// ----------------------------------------------------------------------------
// [BigStore 巨型件轮] ndiff>30 前 8 件复核（基线 identical=171 ae=362 near=1
// diff=14 → 收口 identical=172 ae=362 near=2 diff=13）：
//  · REAL-BUG×3（BuyPrivateStoreItem/VerifyPrivateStoreItem）：
//    IsAvailableCurCharacTradeGoldDaily 实参两处传 dealGoldA（outBuyGold 恒 0）
//    ⇒ 非可信用户每日金币上限校验被短路；ORIG 0x085c8186/0x085c8ae8 实证传
//    GetItemPrice out1(-0x44/-0x28=price)。BuyPSI 反外挂倾销公式两处同错
//    （0x085c8526/0x085c8558 用 price，旧传恒 0 ⇒ 检测整体失效），并按 ORIG
//    x87 栈形补 (float) 窄化（0x085c8542 fstps/flds）。BuyItem 第 5 实参
//    改 out3（ORIG 0x085c8261 段，行为无差、忠实性）。
//  · 语义重构 2 件：DeleteItem 340→306（switch 跳转表→ORIG if-else 有符号链 +
//    每臂延迟重读 +0x48）；RecallPrivateStoreItem 375→188 转 near/N82（i 改
//    int+双初始化+第二循环复用 i 槽+==3/==4 正向包裹+两 invenW 具名缓存删）。
//  · P329 去具名提升 2 件：CreatePrivateStore 214→90（inventory 链式内联 +
//    dollError P225 while 形）、BuyPSI 363→322。
//  · 封顶（残差=已登记 N 族，不再抠旋钮）：Recall 188=N82 槽镜像；BuyPSI
//    322=N85(x87 fucompp)+N82(帧 0xbc)；DeleteItem 306=N82+P227 trace 位；
//    BuyItem 110=N18a 寄存器直落+N82；Create 90=N82 槽漂移；Verify 77=N82；
//    MakeCmdpacketEnter 58=N89(STL out-of-line, ORIG W 0x085cc160)；IsAreaNear
//    50=N111(0x085c5273..0x085c5299 无入口死块)+N82。
//  P167 本批 8 件：除 IsAreaNear(已豁免)外全 CLEAR。头文件未动（CPrivateStoreMgr.h
//  归 CMisVtable 泳道，虚槽保持现状）。
// ============================================================================
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
#include "PCK_AUCTION_OPEN_PRIVATE_STORE_GA.h"  // 权威 PCK_AUCTION_OPEN_PRIVATE_STORE_GA（ORIG W 0x85cb374）
#include "PCK_AUCTION_CLOSE_PRIVATE_STORE_GA.h" // 权威 PCK_AUCTION_CLOSE_PRIVATE_STORE_GA（ORIG W 0x85cb3dc）


#include <algorithm>
#include <cstring>
#include <cerrno>   // IsAreaNearEntranceDungeon 异常垫（ORIG strerror(errno)）
// ORIG T _Z26IsTradeLimitAttachTypeItemRK10Inven_Item @0x081201dc（定义在 CMailBoxHelper TU）。
bool IsTradeLimitAttachTypeItem(Inven_Item const& item);

// PCK_AUCTION_OPEN/CLOSE_PRIVATE_STORE_GA 均归一化到权威共享头（eliminate 伪镜像
// OpenPrivateStoreGaMirror / ClosePrivateStoreGaMirror，统一 ORIG 符号
// /_ZN33PCK_AUCTION_OPEN_PRIVATE_STORE_GAC1Ev/、/_ZN34PCK_AUCTION_CLOSE_PRIVATE_STORE_GAC1Ev/）。

namespace private_store
{
// ORIG 0x085c4be8：函数内静态单例（__cxa_guard）。
CPrivateStoreMgr* GetInstancePrivateStoreMgr()
{
    static CPrivateStoreMgr instance;
    return &instance;
}

// ---- COperator<int>::Product 特化（ORIG W _ZN13private_store9COperatorIiE7ProductEiiRi @0x85cd22a）----
// 重复加法乘法：out 累加 a 共 b 次，任一步非正即失败返回 0。须先于首次隐式实例化。
template <>
bool COperator<int>::Product(int a, int b, int& out)
{
    out = 0;
    for (int i = 0; i < b; ++i)
    {
        out = out + a;
        if (!(out > 0))
            return false;
    }
    return true;
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
// [b28d-24] DIFF 残1: P167 已改后置 it++（P28 ppEi）；余 ORIG 死局部
// `movl $0x0,-0xc(%ebp)`（P171 照抄不写）。ORIG 0x085ca4fe
CPrivateStoreMgr::~CPrivateStoreMgr()
{
    for (std::map<PrivateStoreZone, std::list<CPrivateStore*>*>::iterator it =
             m_zoneStores.begin();
         it != m_zoneStores.end(); it++)   // ORIG 用 ppEi 后置形（P28）
    {
        std::list<CPrivateStore*>* pList = it->second;
        if (pList != 0)
        {
            delete pList;
            // P171 死写入照抄：ORIG 0x085ca524 `movl $0x0,-0xc(%ebp)`（后续不再读）。
            pList = 0;
        }
    }
}

// ---- PrivateStoreZone / PrivateStorePos（定义在 TU 尾 inline 镜像区；
//      ORIG C1/C2(h,h) W 0x85cb6ce、Pos C1 W 0x85cb6f6 / 0x85cb72c、
//      clear W 0x85cb784。后置定义复现 ORIG 调用点 conservative outgoing）----

// ---- PrivateStoreItem（定义在 TU 尾 inline 镜像区；ORIG C1 0x823457c /
//      Reset 0x823459a / op= 0x85cb66e）----

// ---- CPrivateStorePos（定义在 TU 尾 inline 镜像区；
//      ORIG C1 0x85cb7c8 / 带参 C1 0x85cb7fc）----

// ORIG T 0x85cadf6（报告 docs/class_func_reports/private_store__CPrivateStorePos/
// put_object_info.md）：校验宿主 store 状态与卖家位置后序列化私商对象信息：
//   byte(1) | short(seller unique id) | int+str(店名) | byte(state!=1)
//   | byte(pos.a) | byte(pos.b) | short(pos.field2) | short(pos.field4)
//   | byte(isDollCreated)
// [b28d-solo-CPrivateStoreMgr] P167 归零。残 154 行：寄存器分配（ORIG
// `movzbl %al,%ebx` vs 我们 `%eax`）+ 槽号整体偏移（trace 对象 ORIG -0x58 vs
// 我们 -0x28，N82 槽复用）。
bool CPrivateStorePos::put_object_info(PacketGuard& p)
{
    if (m_pStore == 0)
    {
        // [b28d-solo] P221 全表达式临时链：ORIG 0x85cadf6 段 trace 对象槽
        // -0x58/-0x48/-0x38/-0x28（先写块占深槽），具名对象令 4.4 反向。
        cMyTrace(__PRETTY_FUNCTION__, 0xd0b, 0)("NOTEXIST_ERROR : private_store(null)");
        return false;
    }

    // ORIG 0x85cadf6 P167 差集：GetPos×9 / GetSeller×6 / GetState×4 /
    // GetSubject×4 / isDollCreated×3 / getCurCharacVill×2 / get_area×2 ——
    // 原码逐使用点重读 getter（P157/P183 禁缓存），日志变参亦直接读 getter
    // 表达式（P92）；缓存 pos/seller/state/subject 局部丢 20 个调用点。
    if (m_pStore->GetState() != 1 && m_pStore->GetState() != 2)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xd11, 0)(
            "STATE_ERROR : private_store_state(%d)", m_pStore->GetState());
        return false;
    }

    if (m_pStore->GetSeller() == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xd17, 0)(
            "NOTEXIST_ERROR : private_store(%d, %d) user(null)",
            (int)m_pStore->GetPos()->m_a, (int)m_pStore->GetPos()->m_b);
        return false;
    }

    // 未生成 doll 时才校验 store 登记位置与卖家当前位置一致。
    if (!m_pStore->isDollCreated())
    {
        if ((int)m_pStore->GetPos()->m_a !=
                (int)(signed char)m_pStore->GetSeller()->getCurCharacVill() ||
            (int)m_pStore->GetPos()->m_b !=
                m_pStore->GetSeller()->get_area(false))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xd23, 0)(
                "DIFFERENT_ERROR : private_store(%d,%d), user(%d,%d), uid(%d)",
                (int)m_pStore->GetPos()->m_a, (int)m_pStore->GetPos()->m_b,
                (int)(signed char)m_pStore->GetSeller()->getCurCharacVill(),
                m_pStore->GetSeller()->get_area(false),
                m_pStore->GetSeller()->get_unique_id() & 0xffff);
            return false;
        }
    }

    p.put_byte(1);
    p.put_short((unsigned short)m_pStore->GetSellerIndex());

    p.put_int((int)m_pStore->GetSubject()->length());
    p.put_str(m_pStore->GetSubject()->c_str(),
              (int)m_pStore->GetSubject()->length());

    // [b28d-solo] ORIG 0x85cb1xx `setne al; movzx edx,al` ⇒ 直接传 bool 值
    // （三目形 4.4 生 mov$1/jmp/mov$0 岛）。
    p.put_byte(m_pStore->GetState() != 1);

    p.put_byte((int)m_pStore->GetPos()->m_a);
    p.put_byte((int)m_pStore->GetPos()->m_b);
    p.put_short((short)m_pStore->GetPos()->m_field2);
    p.put_short((short)m_pStore->GetPos()->m_field4);

    // ORIG 0x85cb21c 尾：`call isDollCreated; test; je else; put_byte(1); jmp;
    // else: call isDollCreated; xor $1; test; je end; put_byte(0)` = 两独立守卫
    // 各重读一次 getter（P48/P184 死块照抄；三目形只出 1 次调用）。
    if (m_pStore->isDollCreated())
        p.put_byte(1);
    else if (!m_pStore->isDollCreated())
        p.put_byte(0);
    return true;
}

// ORIG W 0x85cb85c：对双方 +0x04 处的 Rectangle 做 include 判定
// （村庄对象矩形统一位于 IObject 头之后，与 ORIG lea 0x4(%eax) 一致）。
bool CPrivateStorePos::include(village_object::IObject const& obj) const
{
    // ORIG：无具名中间量（不 spill），(obj+4) 先求值入 %eax、lea 0x4(%this)
    // 入 %edx 后直落参数槽 ⇒ 表达式内联。
    return m_rect.include(
        *reinterpret_cast<collision_detection::Rectangle const*>(
            reinterpret_cast<char const*>(&obj) + sizeof(village_object::IObject)));
}

// ORIG W 0x85ccae4：基类子对象赋值 + Rectangle 4 个 int 直拷 + 宿主指针。
CPrivateStorePos& CPrivateStorePos::operator=(CPrivateStorePos const& other)
{
    village_object::IObject::operator=(other);  // ORIG W 0x85ccadc：空操作返回自身
    // ORIG 0x85ccae4：单基址四 int 顺序拷贝（mov 0x4/8/c/10(%edx),%ecx → %eax）
    // ⇒ Rectangle 结构体整体赋值（POD 隐式逐字拷贝），非逐字段（逐字段每行双 reload）。
    m_rect = other.m_rect;
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
unsigned short CPrivateStore::GetSellerIndex()
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
// [b28d-24] DIFF 残147: P167 EXTRA vector::operator[]×2 + memset×2。ORIG 0x085c5cec
// [b28d-solo-CPrivateStoreMgr] P167 差集归零（op[] 一次后缓存到 -0x20 槽；
// memset→初始化式）。残 85 行 = ORIG 内联 memset 慢路展开（title@-0x14d 非 4
// 对齐 → 「对齐剥离+rep stos+尾字节」vs 4.4 简单 rep stos；P312 pad 实测无效）
// + 槽基差 3（N95）+ N4/P30 寻址。语义已对齐（无缺调用）。
bool CPrivateStore::RecallPrivateStoreItemToMail()
{
    Inven_Item items[14];
    int count = 0;
    for (unsigned int i = 0; i < 0xe; ++i)
    {
        // [b28d-solo-CPrivateStoreMgr] ORIG 0x085c5d2f：operator[] 一次后把
        // 返回引用缓存到 -0x20 槽（`mov %eax,-0x20(%ebp)`），GetState/+2 读/
        // setCopy 实参全用该缓存 ⇒ 旧逐次下标形 P167 EXTRA op[]×2。
        PrivateStoreItem& storeItem = m_items[i];
        if (storeItem.GetState() == 3 && storeItem.m_item.m_addInfo != 0)
            items[count++].setCopy(storeItem.m_item);
    }
    // [b28d-solo] ORIG 0x085c5d9x `cmp $0,-0x10; sete al; test al,al; je`
    // （count<=0 跳过）⇒ 4.4 的 `0 < count` 产三目岛，改正写 `count > 0`。
    if (count != 0)
    {
        // [b28d-solo-CPrivateStoreMgr] P214③/P253：ORIG 0x085c5da3/0x085c5e04 为
        // 局部数组初始化内联清零（对齐剥离+rep stos），显式 memset 生 call
        // ⇒ P167 EXTRA memset×2。改初始化式。
        char sender[0x15] = {0};
        // [b28d-solo] P312：ORIG title@-0x14d 非 4 对齐 ⇒ 4.1 内联清零走
        // 「对齐剥离+rep stos+尾字节」慢路；4.4 的 -0x150 对齐槽只发简单 rep stos。
        char pad[1];
        char title[0x100] = {0};
        (void)pad;
        std::strcpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_183", 0));
        std::strcpy(title, g_scriptStringManager_.findString(4, "game_server_msg_186", 0));
        int ret = WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, items, (unsigned int)count, 0u,
            (unsigned int)m_pSeller->getCurCharacNo(), title,
            (int)std::strlen(title), 0u, m_pSeller->GetServerGroup(), false);
        if (ret != 0)
        {
            // [b28d-solo] P221：ORIG trace 槽 -0x38（深于 items/sender 区），
            // 具名对象令 4.4 排到 -0x50 ⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x3c1, 5)(
                "CPrivateStore::RecallPrivateStoreItemToMail Error - Mail Send Fail "
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
// [b28d-24] DIFF 残54: P167 MISSING CStreamGuard::deEv×2 + isDollCreated×1；EXTRA ptEv×2（`*guard` vs `guard->` 链，P103/P132）。ORIG 0x085ca116
// [b28d-solo-CPrivateStoreMgr] 残 3 行：P259 去 stream 具名局部（Acquire 直落
// CStreamGuard ctor 实参槽）。余 `&sig->m_items`（+0x8 常量偏移取址）ORIG
// `lea 0x8(%eax),%ebx` vs 4.4 `add $0x8,%eax; mov %eax,%ebx`——数组/取址/数组引用/
// 引用形 4 形实测均不可择（N107/N18a 族：4.4 成员常量偏移取址恒 add）。
void CPrivateStore::SavePrivateStore()
{
    // P259：ORIG 0x085ca116 无 stream 具名局部，Acquire 结果直落 ctor 实参槽。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PrivateStore.cpp", 0xbc6),
                       true);
    // ORIG P167：两次 `operator<<` 走 CStreamGuard::operator*（deEv）而非
    // operator->（ptEv），GetInBuffer 仍走 operator->（P103/P132 分工）。
    (*guard)->operator<<(0x77);
    (*guard)->operator<<((int)m_pSeller->GetUID());
    SIG_PRIVATE_STORE* sig = guard.operator->()->GetInBuffer<SIG_PRIVATE_STORE>();
    std::memset(static_cast<void*>(sig), 0, 0x434);
    sig->m_characNo = m_pSeller->get_charac_no(-1);
    // [b28d-solo] P311 成员数组视图：ORIG 0x85ca1e3 `lea 0x8(%eax),%ebx`（成员
    // 数组名退化取址单条 lea）；字节数组 + cast 形 4.4 恒 `add $0x8,%eax;
    // mov %eax,%ebx`（N107 六形探针之一）。
    struct SigItemsView
    {
        char m_pad[8];
        PrivateStoreItem m_items[14];
    };
    std::copy(m_items.begin(), m_items.begin() + 0xe,
              reinterpret_cast<SigItemsView*>(sig)->m_items);
    // ORIG 双守卫各重读一次 getter（P48/P184；三目形只出 1 次调用）。
    if (isDollCreated())
        sig->m_dollCreated = 1;
    else if (!isDollCreated())
        sig->m_dollCreated = 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

}
// ---- RecreatePrivateStore（ORIG T 0x85c5b60，报告 RecreatePrivateStore.md）----
// LoadPrivateStoreResponse 通知链经 ISeller 虚表槽 +0x14 调用。前置校验
// （m_pParent 非空 / 状态为 0 / 卖家非空）后：重算开店起点、清围观列表、
// ResetItems 后从 SIG 恢复 14 条目；拍卖代理在线时广播 OPEN_PRIVATE_STORE_GA
// (category 0 / packetID 0xb / size 0xaa)，最后置状态 2 返回 1。
// [b28d-24] DIFF 残29: P167 EXTRA __cxa_guard_abort+_Unwind_Resume（静态局部 guard 段差异）。ORIG 0x085c5b60
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
        PCK_AUCTION_OPEN_PRIVATE_STORE_GA packet;
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
// [b28d-24] DIFF 残69: P167 EXTRA getCurCharacName×1；余帧/槽分配（480=480）。ORIG 0x085c53ea
// [b28d-solo-CPrivateStoreMgr] P167 归零（删孤立 getCurCharacName 调用）。残 234
// 行 = 块序（ORIG 在 IsPermissionPrivateStore 之后插 G_GameWorld/IsPVPChannel/
// Do(0x3e) 段）+ 条件极性（ORIG xor$1 形，P292）+ 帧/槽（N82）。指令数 476 vs 480。
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
    // [b28d-solo] ORIG 0x085c5512 `xor eax,0x1; test al,al; je 0x159`（取反 +
    // 两臂互换的块序）⇒ 写否定式 + 交换两臂（P222/P59）。
    if (!m_pSeller->IsPermissionPrivateStore())
    {
        if (dollSlot == 0xffff)
        {
            m_errorHandler.Do(0x3f);
            return false;
        }
    }
    else if (dollSlot != 0xffff)
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
        // [BigStore P225] ORIG 0x085c5708 `cmpl $0,-0x20; setne %al; test; je`
        // 物化形 ⇒ while 复现（4.4 的 `!=` 直跳无 setne）。
        while (dollError != 0)
        {
            m_errorHandler.Do(dollError);
            return false;
        }
        SetDollCreated(true);
        // [BigStore P329] ORIG 0x085c5755 getCurCharacInvenR 结果 eax 于 0x085c576c
        // 直落 GetInvenSlot esp+0x04 实参槽，无 inventory 具名缓存槽 ⇒ 链式内联。
        Inven_Item dollItem =
            m_pSeller->getCurCharacInvenR()->GetInvenSlot(1, (int)dollSlot);
        SetDollItemID((unsigned long)dollItem.m_addInfo);  // ORIG 读隐藏返回副本 +2（物品索引）
    }

    if (!isDollCreated())
        m_pSeller->set_position(posX, posY, 0);

    CPrivateStorePos newPos(this, (int)posX, (int)posY);
    m_storePos = newPos;

    // [b28d-solo] 同 MoveToGate/RemovePrivateStore：ORIG 0x085c58xx 在 Zone ctor
    // 前 `lea ebx,[eax+0x4c]` 预取 &m_storePos（匿名 sret 临时；具名 zone 令
    // 4.4 排到 packet 之下并丢失 ebx 保活）。
    if (!GlobalData::s_villageObjectMgr->register_object(
            village_object::Zone(zoneA, zoneB), &m_storePos))
    {
        m_errorHandler.Do(0x38);
        return false;
    }

    // ORIG 0x085c5879-0x085c58bd：4 参 ctor（W 0x85cb72c 调用发射）构造临时，
    // 再 6 字节拷入 m_pos(+0x24 int 段/+0x28 short 段)——恢复原构造步形并复原
    // 历轮因定义未使用而丢失的 W 符号。
    PrivateStorePos regPos(zoneA, zoneB, posX, posY);
    m_pos = regPos;

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
        // [b28d-solo] P227：ORIG 在 ctor 前求 get_unique_id 并跨 ctor 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x335, 0)(
            "CreatePrivateStore() uid:%d\n",
            (unsigned int)m_pSeller->get_unique_id() & 0xffff);
    }
    // [b28d-solo-CPrivateStoreMgr] P167 EXTRA getCurCharacName×1：删孤立调用，
    // 仅保留 historyLog 实参位的一次（ORIG 调用点唯一）。
    m_pSeller->m_historyLog.CreatePrivateStore(          // CUser 内嵌 +0x79700（CUser.h:1596）
        (int)zoneA, (int)zoneB, m_pSeller->getCurCharacName());

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        PCK_AUCTION_OPEN_PRIVATE_STORE_GA auctionPacket;
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
// [b28d-solo-CPrivateStoreMgr] 已修真语义 bug：sendFail 守卫反转（ORIG
// 0x085c616c xor$1 ⇒ `if(!sendFail) return false;`）；trace 改 P227 全表达式
// 临时链（get_unique_id 跨 ctor %ebx 保活）。残 46 行 = 块级对象槽方向镜像
// （ORIG packet -0x28/zone -0x1a/trace -0x18 vs 我们 -0x24/-0x26/-0x3c）⇒ N93
// + ORIG Send 后预取 &m_storePos 入 %ebx（P52 物化时机）⇒ N 封顶。
bool CPrivateStore::RemovePrivateStore(bool sendFail)
{
    // ORIG(0x085c60xx)：判空先于 Set，失败返路径不经过 Set；仅通过后才 Set。
    // 两真语义：sendFail 为 false 返回 false（非 true）；锁列表用 R 版（const 链）。
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
            if (RecallPrivateStoreItem())
            {
                SavePrivateStore();
                m_pSeller->UpdateData();
                m_pSeller->send_itemspace(0);
                m_pSeller->send_itemspace(1);
                m_pSeller->getCurCharacInvenR()->SendItemLockListInven();
                m_pSeller->getCurCharacInvenR()->SendItemLockListAvatar();
            }
            else
            {
                SavePrivateStore();
                m_pSeller->UpdateData();
                m_errorHandler.Do(0x16);
                // 真语义 bug 修复：ORIG 0x085c616c `movzbl sendFail; xor $1;
                // test %al,%al; je 0x1b1` ⇒ 条件为 !sendFail（sendFail==1 时
                // 继续，==0 时 return false）；旧重建写成 `if (sendFail)` 反了。
                if (!sendFail)
                    return false;
            }
        }

        if ((int)m_users.size() != 0)
        {
            packet.MakeNotipacketForceRemovePrivateStore();
            SendUsers(&packet, 0, 0);
        }
    }

    // ORIG P167：getCurCharacName 仅 1 次（原先多一条返回值丢弃的死调用）。
    m_pSeller->m_historyLog.RemovePrivateStore(
        (int)m_pos.m_a, (int)m_pos.m_b, m_pSeller->getCurCharacName());

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        PCK_AUCTION_CLOSE_PRIVATE_STORE_GA auctionPacket;
        auctionPacket.m_id = (unsigned int)m_pSeller->get_acc_id();
        GlobalData::s_auction_proxy->SendPacket(&auctionPacket.m_header);
    }

    packet.MakeCmdpacketRemovePrivateStore();
    m_pSeller->Send(packet);

    packet.MakeNotipacketRemovePrivateStore((unsigned short)GetSellerIndex());
    CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

    // [b28d-solo] 同 MoveToGate：匿名 Zone 临时 + &m_storePos 实参（ORIG
    // 0x85c62xx `lea ebx,[eax+0x4c]` 先物化跨 Zone ctor 保活；具名 zone 令
    // 4.4 把块内对象排到 packet 之下 -0x26，ORIG 为 -0x1a）。
    if (!GlobalData::s_villageObjectMgr->unregister_object(
            village_object::Zone(m_pos.m_a, m_pos.m_b), &m_storePos))
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x473,
                              "unregister_object error");
    }

    m_pParent->FreeStoreSeller(m_pSeller);
    // P227 全表达式临时链：ORIG 在 ctor 前把 get_unique_id 读入 %ebx 跨 ctor
    // 保活（0x085c62xx 段），具名 trace 对象会退化为 ctor 后求值。
    cMyTrace(__PRETTY_FUNCTION__, 0x478, 0)(
        "RemovePrivateStore() uid:%d\n",
        (unsigned int)m_pSeller->get_unique_id() & 0xffff);
    Clear();
    return true;
}

// ---- CompleteDisplay（ORIG T 0x85c7800，报告 CompleteDisplay.md）----
// ISeller 虚表槽 +0x08：确认开店告示。MoveItems 成功后保存、刷新物品锁视图，
// doll 存在时消耗对应 doll 槽道具；写入店名，广播 noti 并回发 cmd，置状态 2。
// [b28d-24] DIFF 残12: P167 归零；残差=帧/槽分配（P112/N82）。ORIG 0x085c7800
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
            // P227 全表达式临时链：令 4.4 把 dollSlot 读入 %ebx 跨 ctor 保活
            // （ORIG 0x1a5 movzwl→%ebx、0x1cc mov %ebx,0x8(%esp)）。
            cMyTrace(__PRETTY_FUNCTION__, 0x6c1, 5)(
                "CPrivateStore::CompleteDisplay() ERROR!!! : use_item(%d)", (int)dollSlot);
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
// [b28d-24] DIFF 残26: P167 归零；残差=帧/槽分配（P112/N82）。ORIG 0x085c751a
// [b28d-solo-CPrivateStoreMgr] 已修：packet 声明下移至双守卫后（ORIG 0x085c754a
// 才 C1）；unregister_object 返回类型 int→bool（P292 档①，调用点 xor$1 形）。
// 残 24 行 = ①块外 packet(-0x18)/块内 zone(-0xa) 槽方向镜像 vs 我们 -0x14/-0x16
// ⇒ N93；②ORIG 在 Send 后把 &m_storePos 预取 %ebx 跨 Zone ctor 保活、4.4 在
// 调用点才 lea（P52 实参物化时机，具名局部会引入槽）⇒ 跨版本寄存器分配。
bool CPrivateStore::MoveToGate()
{
    m_errorHandler.Set(m_pSeller, 1, 0x59);

    if (m_pSeller == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    // ORIG 0x085c754a 才构造 packet（前两个守卫失败路径无 C1/D1）⇒ 声明下移。
    CPacketBufPrivate packet;

    if (m_pos.m_a != 0 || m_pos.m_b != 0 ||
        m_pos.m_field2 != 0 || m_pos.m_field4 != 0)
    {
        packet.MakeNotipacketRemovePrivateStore((unsigned short)GetSellerIndex());
        CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

        // [b28d-solo] ORIG 0x85c7609：&m_storePos 先入 %ebx（跨 Zone ctor 保活），
        // Zone 以匿名 sret 临时落 -0xa；具名 zone 令 4.4 把块内对象排到 packet
        // 之下（-0x16）⇒ 改匿名临时实参（P52/P134）。
        if (!GlobalData::s_villageObjectMgr->unregister_object(
                village_object::Zone(m_pos.m_a, m_pos.m_b), &m_storePos))
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

    // ORIG 0x85c8dd6：find() 结果 sret=-0x18 作 operator!= 的 this，第二次
    // end() 结果 -0xc 作实参。裸 `find(...) != end()` 使 4.4 先求实参 end()
    // （P75）；具名迭代器固定 find 先求值（P74 补充②旋钮一），再以正极
    // `found != end()` 命中 this=find/arg=end 的 ORIG 形态。
    std::list<CUser*>::iterator found =
        std::find(m_users.begin(), m_users.end(), m_pBuyer);
    if (found != m_users.end())
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
    if (!m_users.empty())
    {
        if (found == m_users.end())
        {
            m_errorHandler.Do(0x15);
            return false;
        }
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
// [b28d-24] DIFF 残87: P167 MISSING vector::operator[]×2（访问方式差异）。ORIG 0x085c6d48
// [b28d-solo-CPrivateStoreMgr] P167 归零（尾部 op[]×3 + +0x3d 字段直读；
// invenType 内联消 -0x1c 槽）。残 41 行 = find_item/GetCashPrice 的 sete/setg
// 物化点位置（P225 while 形已应用）+ N7 寄存器角色互换（invenType eax↔edx）+
// 帧/槽（N82）。
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

    if (item.m_field44 >= 7)
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

    // [b28d-solo-CPrivateStoreMgr] P0：ORIG 0x085c6ff1 后 `mov %eax,%edx`（无槽）
    // ⇒ invenType 是内联右值，无具名局部（旧具名形多 -0x1c 槽并使 srcItem 槽位
    // 由 -0x59 漂到 -0x5d）。
    Inven_Item srcItem = inventory->GetInvenSlot(
        (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)item.m_field46),
        (int)item.m_field48);
    // [b28d-solo-CPrivateStoreMgr] P197③ 预清零两句式 + P225 while 形：
    // ORIG 0x085c7015 `movl $0x0,-0x18` 预清零；0x085c7035 `cmpl $0,-0x18;
    // sete %al; test; je` 槽比零物化 ⇒ while(==0) 复现。
    CItem* itemInfo = 0;
    itemInfo = G_CDataManager()->find_item((int)srcItem.m_addInfo);
    while (itemInfo == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x582,
                              "CPrivateStore::AddItem find_item() ERROR item_id(%d)",
                              (int)srcItem.m_addInfo);
        return false;
    }

    // ORIG：cash 价 >=1 的商城物品禁止上架（报告反编译 iVar6<1 才继续）。
    // ORIG 0x085c7088 `test %eax,%eax; setg %al; test %al,%al; je` ⇒ while 形。
    while ((int)itemInfo->GetCashPrice() >= 1)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x5b8,
                              "CPrivateStore::AddItem GetCashPrice() ERROR item_id(%d), cash_price(%d)",
                              (int)srcItem.m_addInfo, (int)itemInfo->GetCashPrice());
        return false;
    }
    // ORIG 经 CItem vtable 槽 +0x10 判定化身类道具，禁止私商上架。
    // [b28d-solo] ORIG 块序：IsAvatarItem（0x85c70dd 虚调段）在 IsTradeRestriction
    // （0x85c71xx lea 副本+0x33 段）之前 ⇒ 两块顺序照 ORIG 排列。
    if (itemInfo->IsAvatarItem())
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x5be,
                              "CPrivateStore::AddItem IsAvatarItem() ERROR item_id(%d), cash_price(%d)",
                              (int)srcItem.m_addInfo, (int)itemInfo->GetCashPrice());
        return false;
    }

    // [b28d-solo-CPrivateStoreMgr] P59 负向守卫形：ORIG `call IsTradeRestriction;
    // test %al,%al; setne %al; test %al,%al; je <主流程>`（restriction 为真直返）
    // ⇒ 旧 `if(!f()){主流程}` 形产 sete 且块序反。
    if (srcItem.m_upgradeSep.IsTradeRestriction())   // ORIG lea 副本+0x33（UpgradeSeparateInfo 子对象）
        return false;

    if (item.m_field44 < 0xe && item.m_field44 >= 0 &&
        prevCountSlot < 0xe && prevCountSlot >= 0)
    {
        if (prevCountSlot == 0)
            ResetItems();

        // [b28d-solo-CPrivateStoreMgr] P157/P183 + P167：ORIG 尾部逐次
        // operator[]×3（读 +0x3d 字段 / Reset / operator=），且 +0x3d 判定
        // 是**直接字段读**非 GetState() 调用（旧缓存引用 ⇒ MISSING op[]×2
        // + EXTRA GetState×1）。
        if (m_items[item.m_field44].m_flag3d != 0)
            return false;

        if (item.m_field46 == 3)
            return false;   // ORIG：item space 3 的条目不可上架

        item.m_flag3d = 1;
        m_items[item.m_field44].Reset();
        m_items[item.m_field44] = item;
        return true;
    }
    return false;
}

// ---- BuyPrivateStoreItem（ORIG T 0x85c7e74，报告 BuyPrivateStoreItem.md）----
// ISeller 槽 +0x18 / IBuyer 槽 +0x00。买家购买：校验围观/槽位/忙碌与物品可售性，
// 计价后经 BuyItem 扣买家入包、SellItem 结算卖家金币（扣手续费），广播条目变化，
// 反外挂检测（金额阈值公式常量 0.01/0.7，ORIG rodata 0x8cbead0/0x8cbead8），
// 最后写入流通统计。
// [b28d-24] DIFF 残177: 帧/槽分配 + 分支块序（N82/P112）。ORIG 0x085c7e74
// [b28d-solo-CPrivateStoreMgr] P167 归零。残 335 行 = 帧 0xbc vs 0xe0（N82 槽复用
// 差异）+ 块序/调用序（CErrorHandler::Do / GetAttachType / getCurCharacInvenR
// 的发射位置）。
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
    // [b28d-solo-CPrivateStoreMgr] P225：ORIG 0x085c8804 `test %eax,%eax; sete %al;
    // test; je`（寄存器比零物化）⇒ while(==0) 形复现。
    while (m_pBuyer->getCurCharacR() == 0)
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
    // ORIG 0x085c895e `cmpl $0,-0x1c; sete %al; test; je` ⇒ while 形。
    while (itemInfo == 0)
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
    // [BigStore REAL-BUG] ORIG 0x085c8186 实参 = GetItemPrice out1 槽 -0x44（=price，
    // 0x085c80f6 lea -0x44 → esp+0x10 实证），旧写成 dealGoldA（out2 恒 0）⇒ 上限
    // 校验恒见 0 而被短路。按 ORIG 传成交金额。
    if (!m_pBuyer->GetReliablePerson() &&
        !m_pBuyer->IsAvailableCurCharacTradeGoldDaily((unsigned int)price))
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

    // [BigStore 忠实性] ORIG 0x085c8261-0x085c8298：BuyItem 三 int 实参 =
    // (-0x44, -0x3c, -0x40) = (price, out2, out3)；第 5 参在 ORIG 体内不读，
    // 旧写 useCount 无行为差，按 ORIG 数据流改传 out3。
    buyResult = BuyItem(slot, copyItem, price, (int)dealGoldA, (int)dealGoldB, outSlot);
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
        // [BigStore P329] ORIG 0x085c8519：getCurCharacInvenR 结果 eax 于
        // 0x085c851e 直落 get_money (%esp) 实参槽，无具名缓存 ⇒ 链式内联。
        // 反外挂：低价倾销检测。ORIG 浮点常量 [0x8cbead0]=0x3f847ae147ae147b(0.01)、
        // [0x8cbead8]=0x3fe6666666666666(0.7)（均 double）。
        // [BigStore REAL-BUG] ORIG 0x085c8526 `mov -0x44,%edx; add %edx,%eax` 与
        // 0x085c8558 `mov -0x44,%eax` 实证两处用 out1(=price)，旧写 dealGoldA
        //（out2 恒 0）⇒ `money*scale < 0` 永假，倾销检测整体失效。
        // [BigStore P253-x87] ORIG 0x085c8542 `fstps -0x9c; flds -0x9c` 为 rand25×0.01
        // 后的 float 窄化回展 ⇒ 按 (float) 显式复现；比较在 double 栈上直做。
        double hackScale =
            (double)(float)((double)rand25 * 0.01) + 0.7;
        if (hackScale * (double)((int)m_pBuyer->getCurCharacInvenR()->get_money() + price) <
            (double)price)
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
// [b28d-24] DIFF 残47: 帧/槽分配（P112/N82）。ORIG 0x085c874c
// [b28d-solo-CPrivateStoreMgr] 残 76 行：P225 while 形已用于
// getCurCharacR()==0 / find_item==0 两处；余为 CErrorHandler::Do 调用序 +
// put_binary 段顺序 + jmp 偏移差（帧/槽 N82）。
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

    // [b23S3C] P48 两次独立 operator[]：ORIG 0x169 首次结果 eax 直读 +0x3d（无
    // 槽），0x1ee 二次 call 结果才落 -0x18 槽（后续全经 -0x18）⇒ 状态判定用完整
    // 下标表达式，绑定延后到 itemInfo 预清（0x1d7 movl $0,-0x1c 两段式）之后。
    // [b23S3C] ORIG 0x16e movzbl 0x3d(%eax) 直读字段（-O0 下 GetState call 是
    // 本仓 out-of-line 化的产物；ORIG 此处无 GetState call，p167 EXTRA 实证）。
    if ((int)m_items[slot].m_flag3d != 3)
    {
        m_errorHandler.Do(1);
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
    // [BigStore REAL-BUG] ORIG 0x085c8ae8 实参 = GetItemPrice out1 槽 -0x28（=price，
    // 0x085c8a58 lea -0x28 → esp+0x10 实证），旧写 dealGoldA（out2 恒 0）⇒ 校验被短路。
    if (!m_pBuyer->GetReliablePerson() &&
        !m_pBuyer->IsAvailableCurCharacTradeGoldDaily((unsigned int)price))
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
// [b28d-24] DIFF 残23: P167 EXTRA Inven_Item::get_add_info×1。ORIG 0x085c532c
int CPrivateStore::CheckPrivateStoreDoll(unsigned short dollSlot)
{
    if (m_pSeller == 0)
        return 0x3a;

    // P52/P183：ORIG 0x085c533a 先把 (short)dollSlot 物化 %ebx 跨 receiver call
    // 保活，receiver 结果直落实参槽，无 inventory/dataMgr 具名缓存槽。
    Inven_Item item = m_pSeller->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1,
                                                                    (int)dollSlot);
    CItem* pItem = G_CDataManager()->find_item((int)item.m_addInfo);  // 副本 +2 = 物品索引
    if (pItem == 0)
        return 0x16;
    if (!pItem->is_stackable())
        return 0x16;
    // +0x32c 属派生类 CStackableItem（CItem 尺寸 0x148，CStackableItem 0x3f0）；
    // is_stackable 通过后必为 CStackableItem，字段 m_int32c（CStackableItem.h:365）。
    if (static_cast<CStackableItem*>(pItem)->m_int32c == -1)
        return 0x16;
    // ORIG 0x085c53d2 `mov -0x42(%ebp),%eax; test %eax,%eax; jg`：直读副本
    // +0x07 字段（m_addInfo2），不调 get_add_info（P167 EXTRA ×1）。
    if (item.m_addInfo2 <= 0)                                  // 副本 +7 = 数量
        return 0x16;
    return 0;
}

// ---- IsAreaNearEntranceDungeon（ORIG T 0x85c5082，报告 IsAreaNearEntranceDungeon.md L15-269 全文）----
// 开店区域校验：村庄 2 区 0 即入口区允许开店；区域类型 4 禁止；异常走 errno 日志垫。
// [b28d-24] DIFF 残53: P167 MISSING cMyTrace::operator()×1（少一条日志调用，真缺口）。ORIG 0x085c5082
// [b28d-solo-CPrivateStoreMgr] 残 79 行 = N111（ORIG 的 area==0 trace 块
// `0x085c5273..0x085c5299` 被 `jmp 0x298` 跳过、字节级复核无任何跳转指向该区
// ⇒ 4.1 无入口死错误块；补 trace 会让 area==0 时多打日志、改变行为，故不补，
// P167 该 MISSING 项按 N111 豁免）+ N82（5 个互斥 cMyTrace 对象 4.1 各占独立槽，
// 帧 0x90 vs 我们 0x70）。
bool CPrivateStore::IsAreaNearEntranceDungeon(CUser* user)
{
    // [b28d-solo] ORIG 0x85c508d..0x85c50a9 五条 movl $0 的槽序：-0x1c 死局部
    // （P171 只初始化从不读取）、area@-0x18、gameWorld@-0x14、village@-0x10、
    // areaNo@-0xc；声明序按 ORIG 槽深→浅排列复现 4.4 分配。
    int deadLocal = 0;         // -0x1c（ORIG 只写不读）
    Area* area = 0;            // -0x18
    GameWorld* gameWorld = 0;  // -0x14
    Village* village = 0;      // -0x10
    int areaNo = 0;            // -0xc
    (void)deadLocal;

    try
    {
        if (user == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x233, 5)("pUser == NULL");
            return false;
        }
        gameWorld = G_GameWorld();
        // [b28d-solo] P225：ORIG 0x85c50fe `cmp -0x14,0; sete al; test al,al;
        // je` 为 4.1 bool 物化形，4.4 的 if 恒直跳 ⇒ while 形复现。
        while (gameWorld == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x239, 5)("G_GameWorld call failed");
            return false;
        }
        // [b23S3C] P48 去缓存：ORIG get_area 结果 eax→%ebx 跨 getCurCharacVill
        // call 直落 0x8(%esp) 实参槽（0x0d5-0x0f8），无 curArea 槽 ⇒ 右值直传。
        if (!gameWorld->check_valid_area((int)user->getCurCharacVill(),
                                         user->get_area(false)))
        {
            // ORIG 复用 "G_GameWorld call failed" 文案（报告行 84 字面实证）。
            cMyTrace(__PRETTY_FUNCTION__, 0x23f, 5)("G_GameWorld call failed");
            return false;
        }
        village = gameWorld->getUserVillage(user);
        // [b28d-solo] P225：ORIG 0x85c51db `cmp -0x10,0; sete al; test; je`。
        while (village == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x246, 5)("getUserVillage call failed");
            return false;
        }
        areaNo = user->get_area(false);
        area = village->getArea(areaNo);
        // [b28d-solo] ORIG 0x85c5251 `cmp -0x18,0; sete al; test; jne 继续`。
        while (area == 0)
            return false;

        if (area->m_villageIndex == 2 && area->m_areaIndex == 0)
            return true;
        // [DualStore] ORIG 0x085c5274..0x085c529e 的 `getArea call failed (area no:%d)`
        // cMyTrace 块在 ORIG 侧无任何跳转入口（0x85c526e `jmp 0x85c531a` 直接跳过），
        // 但其 P167 调用点确实存在（新版 p167 MISSING cMyTrace::operator() ×1）。
        // 按 ORIG 形复现该块：上方 while 已拦 area==0 ⇒ 此条件恒假，运行时行为不变。
        if (area == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x24c, 5)("getArea call failed (area no:%d)", areaNo);
            return false;
        }
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
// [b28d-24] DIFF 残191: P167 MISSING vector::operator[]×2（访问方式差异）。ORIG 0x085c7246
// [b28d-solo-CPrivateStoreMgr] P167 归零（第二循环逐次 op[]×3）；P191 双初始化 +
// P292 档① 正向包裹已应用（两循环）。残 202 行 = N4/P30 寻址（ORIG `mov this,%edx;
// add $0x3c,%edx` vs 4.4 `mov this,%eax; lea 0x3c(%eax),%edx`）+ i 槽差 4
//（N98/N95）+ trace 具名对象槽（P227）。
bool CPrivateStore::MoveItems()
{
    // [b28d-solo-CPrivateStoreMgr] P191 双初始化 + P292 档① 正向包裹：
    // ORIG 0x085c724d/0x085c7254 两条 `movl $0x0,-0x10`（i 声明+for 初始化）；
    // 0x085c727d `cmp $1,%al; sete %al; test; je <循环尾>` ⇒ `==1` 正向包裹。
    // [b28d-solo] ORIG 0x085c727d `cmp $0xd,-0x10; setle` 有符号 + 每处
    // `mov i,%eax; mov this,%edx; add $0x3c,%edx`（值先）⇒ i 须为 int（P2/P218）。
    int i = 0;
    for (i = 0; i < 0xe; ++i)
    {
        if (m_items[i].GetState() == 1)
        {

        int prevAddInfo2 = (int)m_items[i].m_item.m_addInfo2;
        // [b28d-solo] 正极性包裹复现 ORIG 0x85c72d3 `call; test al,al;
        // je 0x85c7399`（4.1 对 !bool 直判不物化；4.4 的 if(!f()) 恒 xor $1
        // 并把失败块落到 fall-through，块序全反）。
        if (GetItemInfo(m_pSeller, m_items[i]))
        {
            m_items[i].m_flag3d = 2;

            // [b28d-solo] ORIG 0x85c730d `cmp al,8; setne al; test al,al;
            // je 0x85c7336` ⇒ 源码是否定式 if (field1 != AVATAR) 回写、else
            // 走 avatar 分支（P133 补充：否定式才产 setne 物化）。
            if ((int)m_items[i].m_item.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
            {
                m_items[i].m_item.m_addInfo2 = prevAddInfo2;
            }
            else
            {
                // P0：ORIG 0x85c7370 `test eax,eax` 直判（expire 无槽往返）
                // ⇒ 不声明具名 expire 局部，表达式内联。
                if (m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()
                        ->GetExpireDate((int)m_items[i].m_item.m_addInfo2) != 0)
                {
                    // 此失败路径不 ResetItems（ORIG 如此）。
                    m_errorHandler.Do(0x11);
                    return false;
                }
            }
        }
        else
        {
            ResetItems();
            m_errorHandler.Do(0x11);
            return false;
        }
        }
    }

    if (CheckTotalItemPrice() != 1)
    {
        ResetItems();
        m_errorHandler.Do(0x11);
        return false;
    }

    // ORIG 0x085c7414 `movl $0x0,-0x10` 单条（复用 i 槽）+ `cmp $2; sete; je` 正向。
    for (i = 0; i < 0xe; ++i)
    {
        // [b28d-solo-CPrivateStoreMgr] P157/P183：ORIG 逐次 operator[]（3 次：
        // GetState / DeleteItem 实参 / 写 +0x3d），旧缓存引用只 1 次 ⇒ P167
        // MISSING operator[]×2。改为每使用点重新下标。
        if (m_items[i].GetState() == 2)
        {
            if (DeleteItem(m_items[i]))
            {
                m_items[i].m_flag3d = 3;
            }
            else
            {
                // [b28d-solo] P221：ORIG trace 槽 -0x20，具名对象排到 -0x24。
                cMyTrace(__PRETTY_FUNCTION__, 0x637, 5)(
                    "CPrivateStore [%s][%d]", __PRETTY_FUNCTION__, 0x637);
                ResetItems();
                m_errorHandler.Do(0x11);
                return false;
            }
        }
    }
    return true;
}

// ---- RecallPrivateStoreItem（ORIG T 0x85c63ca，报告 RecallPrivateStoreItem.md L15-513 全文）----
// 第一轮把 state==3 条目按类型插回 avatar 背包(reason 0xa)/普通背包(reason 0x12)，
// 回填槽号后置 4；任一插入负值中断转第二轮回退 state==4→3；全部成功 ResetItems。
// [b28d-24] DIFF 残173: P167 MISSING getCurCharacName×1（真缺口）。ORIG 0x085c63ca
// [b28d-solo-CPrivateStoreMgr] P167 归零（getCurCharacName 两次独立调用，P48）。
// 残 281 行 = 帧 0x140 vs 0xd0（N82，4.1 不复用槽）+ N4 寻址（add $0x3c vs lea）
// + P292 极性（sete vs setne）。P191 双初始化+P292 正向包裹实测 281→285 劣化
//（已回退）⇒ 封顶。
bool CPrivateStore::RecallPrivateStoreItem()
{
    // [BigStore P191/P2] ORIG 0x085c63d5/0x085c63dc 双 `movl $0x0,-0x20`，尾测
    // `addl $1; cmpl $0xd; setle`（有符号）⇒ i 为 int、声明+for 各初始化一次。
    int i = 0;
    for (i = 0; i < 0xe; ++i)
    {
        PrivateStoreItem& item = m_items[i];
        // [BigStore P292] ORIG 0x085c6423 `cmp $0x3,%al; sete %al; test; je 循环尾`
        // ⇒ `==3` 正向包裹（旧 !=3+continue 反形产 setne/jne）。
        if (item.GetState() == 3)
        {
            int slot = -1;
            if ((int)item.m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)
            {
                // [b28d-solo] ORIG 0x85c642e 先把 item.m_item 逐字段拷到栈副本 -0xaa，
                // 再按值拷入 outgoing 槽（两次拷贝，4.1 对按值实参的 lowering）⇒
                // 显式具名副本复现。
                Inven_Item itemCopy = item.m_item;
                // [BigStore P329] ORIG 0x085c64bb getCurCharacInvenW 结果直落
                // insertAvatarIntoInventory this 实参槽（16+2 参），无 invenW 缓存槽。
                slot = m_pSeller->getCurCharacInvenW()->insertAvatarIntoInventory(
                    itemCopy, (eAvatarItemAddReason)0xa, false);
                if (slot < 0)
                    break;
                m_pSeller->getCurCharacInvenR()->SendAvatarEvent(
                    0x36, (int)item.m_item.m_addInfo2, 0, slot + 0xa, 0);
            }
            else
            {
                // [b28d-solo] 同上：ORIG 0x85c6xxx 两次逐字段拷贝（-0x6d 副本）。
                Inven_Item itemCopy = item.m_item;
                slot = m_pSeller->getCurCharacInvenW()->insertItemIntoInventory(
                    itemCopy, (eItemAddReason)0x12, false, true);
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
            // [b28d-solo-CPrivateStoreMgr] P48/P157：ORIG 对同一 getter 连调两次
            // （P167 差集 getCurCharacName×2，旧缓存形只 1 次）⇒ 实参处各写一次。
            m_pSeller->m_historyLog.PrivateStoreItemAdd(
                (INVEN_TYPE)1, item.m_item, addNew, addOld, (eItemAddReason)0x12,
                m_pSeller->getCurCharacName(), m_pSeller->getCurCharacName());

            item.m_field48 = (unsigned short)slot;
            item.m_flag3d = 4;
        }
    }

    if (i != 0xe)
    {
        // [BigStore P191] 第二循环复用同一 i 槽（ORIG 再发双初始化 -0x20）。
        for (i = 0; i < 0xe; ++i)
        {
            PrivateStoreItem& item = m_items[i];
            // [BigStore P292] ORIG 第二循环同形 `cmp $4; sete; je` ⇒ 正向包裹。
            if (item.GetState() == 4)
            {
                if (DeleteItem(item))
                {
                    // [b28d-solo] P221：ORIG trace 槽 -0x28，具名对象排到 -0x48。
                    cMyTrace(__PRETTY_FUNCTION__, 0x4c9, 5)(
                        "[%s][%d]", __PRETTY_FUNCTION__, 0x4c9);
                }
                item.m_flag3d = 3;   // DeleteItem 成败皆置
            }
        }
        return false;
    }

    ResetItems();
    return true;
}

// ---- DeleteItem（ORIG T 0x85c68b4，报告 DeleteItem.md L15-440 全文）----
// 按 item space 四路删除来源槽：0=背包(INVEN_TYPE 1, count=+0x07)、1=avatar(reason 4，
// 成功补发事件)、2=cargo、3=装备栏(INVEN_TYPE 0，成功 send_equip)；其余直接成功。
// [b28d-24] DIFF 残282: 大函数帧/槽分配 + 分支块序（N82/P112）。ORIG 0x085c68b4
// [b28d-solo-CPrivateStoreMgr] P167 归零。残 357 行 = cMyTrace 具名对象发射位置
//（ORIG C1 在 getCurCharacNo/Name 之后，P227）+ 帧 0x9c vs 0xac（N82）+ N18a
// 单段直落（`mov 0x2(%eax),%edi` vs 我们两段）。
bool CPrivateStore::DeleteItem(PrivateStoreItem& item)
{
    int sellPrice = item.m_field40;
    int resealCount = (int)item.m_item.GetReSealCount();
    int upgrade = (int)item.m_item.GetUpgrade();
    int addInfo2 = (int)item.m_item.m_addInfo2;
    int itemIdx = (int)item.m_item.m_addInfo;
    // [BigStore] ORIG 0x085c68xx：space 一次性 movsbl（有符号字节），随后按
    // if(==1)/else if((char)<2){if(==0)}/else if(==2)/else if(==3) 链展开；
    // 每臂独立重读 (short)+0x48（成功臂 SendAvatarEvent/send_equip 也重读），
    // 无函数级 slot 缓存 ⇒ 旧 switch+提前绑定形改回链式+延迟加载。
    const signed char space = (signed char)item.m_field46;

    // [b28d-solo] P227 全表达式临时链：ORIG 0x085c68f2 段先把 getCurCharacName/
    // getCurCharacNo/… 求值入 edi/esi/ebx，再 call cMyTrace C1；具名对象令 4.4
    // 先构造再求值（C1 在 -0x40，ORIG 在 -0x68）。
    cMyTrace(__PRETTY_FUNCTION__, 0x4d7, 0)(
        "PrivateStore del charac_name(%s)(%d), item_id(%d), add_info(%d), "
        "upgrade(%d), reseal_count(%d), sell_price(%d)",
        m_pSeller->getCurCharacName(), m_pSeller->getCurCharacNo(),
        itemIdx, addInfo2, upgrade & 0xff, resealCount & 0xff, sellPrice);

    if (space == 1)
    {
    int slot = (short)item.m_field48;
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)2, slot, 1, (eItemDelReason)4, true) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x515, 5)(
                "Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                m_pSeller->getCurCharacNo(), (short)item.m_field48);
            m_errorHandler.Do(0x11);
            return false;
        }
        m_pSeller->getCurCharacInvenR()->SendAvatarEvent(
            0x36, (int)item.m_item.m_addInfo2, 3, (short)item.m_field48 + 0xa, 0);
    }
    else if (space < 2)
    {
        if (space == 0)
        {
    int slot = (short)item.m_field48;
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, (int)item.m_item.m_addInfo2,
                (eItemDelReason)0xb, true) != 1)
{
            cMyTrace(__PRETTY_FUNCTION__, 0x4e0, 5)(
                "Store::user_sell_item, delete_item failed , User ch=%d , %d %d",
                    m_pSeller->getCurCharacNo(), (short)item.m_field48,
                    (int)item.m_item.m_addInfo2);
            m_errorHandler.Do(0x11);
            return false;
        }
        }
    }
    else if (space == 2)
    {
    int slot = (short)item.m_field48;
        if (m_pSeller->getCurCharacCargoW()->delete_item(
                slot, (int)item.m_item.m_addInfo2, (eItemDelReason)0xb) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4ec, 5)(
                "Store::user_sell_item, delete_item failed 2, User ch=%d , %d %d",
                m_pSeller->getCurCharacNo(), (short)item.m_field48,
                (int)item.m_item.m_addInfo2);
            m_errorHandler.Do(0x11);
            return false;
        }
    }
    else if (space == 3)
    {
    int slot = (short)item.m_field48;
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)0, slot, 1, (eItemDelReason)0xb, true) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4f9, 5)(
                "Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                m_pSeller->getCurCharacNo(), (short)item.m_field48);
            m_errorHandler.Do(0x11);
            return false;
        }
        m_pSeller->send_equip((short)item.m_field48);
    }
    return true;
}

// ---- GetItemInfo（ORIG T 0x85c9f22，报告 GetItemInfo.md L15-218 全文；跳转表 .rodata 0x8cbd6e4 实读）----
// 按 item space 分派来源槽读取：[0]invR type1 [1]invR type2 [2]cargo [3]invR type0
// [7]invR type3；4/5/6 与越界同走默认失败。读回后置 +2 非零并过 CheckValidItem。
// [b28d-24] DIFF 残84: 帧/槽分配（P112/N82）。ORIG 0x085c9f22
// [b28d-solo-CPrivateStoreMgr] 已修：case 叶块序 0,2,3,1,7（P46，跳转表
// @0x8cbd6e4 实证）+ 尾部 if(!CheckValidItem) return 0; return 1;（P214②）。
// 残 16 行 = 4 个 case 块 Inven_Item 局部槽方向镜像（ORIG -0x13d/-0x100/-0x86/
// -0x49 vs 我们 -0x49/-0x86/-0x100/-0x13d）⇒ N93 封顶（互斥块对象槽方向
// 跨版本相反，4.1 先写块占更深槽、4.4 占更浅；提至函数头会引入 5 次默认构造）。
bool CPrivateStore::GetItemInfo(CUser* user, PrivateStoreItem& item)
{
    int prevAddInfo2 = (int)item.m_item.m_addInfo2;

    // ORIG 跳转表 @0x8cbd6e4：[0]=0x85c9f51 [2]=0x85c9fa1 [3]=0x85c9fe9
    // [1]=0x85ca039 [7]=0x85ca086 [4/5/6]=0x85ca0cd ⇒ 叶块物理序 = case
    // 书写序 0,2,3,1,7（P46），case→体映射与源码一致，仅需按 ORIG 重排。
    switch ((signed char)item.m_field46)
    {
    case 0:
    {
        Inven_Item slotData =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (short)item.m_field48);
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
    case 1:
    {
        Inven_Item slotData =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)2, (short)item.m_field48);
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
    // P214②：ORIG 0x85ca0b6 xor$1;test;je;mov$0;jmp;mov$1 ⇒ if(!f())return 0;
    // 成功路径 fall-through 到单一 return 1。
    if (!CheckValidItem(item, prevAddInfo2))
        return false;
    return true;
}

// ---- SendUsers（ORIG T 0x85c7adc，报告 SendUsers.md L15-130 全文）----
// 围观列表广播，跳过 excludeA 与空指针；尾部对 excludeB 非空补发一次。返回发送数。
int CPrivateStore::SendUsers(PacketGuard* packet, CUser* excludeA, CUser* excludeB)
{
    int sentCount = 0;
    // ORIG 0x85c7adc 槽集：sentCount@-0x10、it@-0x1c（**默认构造 C1Ev + 后赋值**，
    // begin() 经 sret 临时 -0x2c 中转拷贝）、user@-0xc（函数头预置 0，循环内复用）。
    std::list<CUser*>::iterator it;
    CUser* user = 0;
    it = m_users.begin();
    while (it != m_users.end())
    {
        user = *it;
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
// [b28d-24] DIFF 残24: 迭代器临时槽阶梯/构造顺序（P28/P50）。ORIG 0x085c91b6
bool CPrivateStore::IsExistBuyerList(CUser* user)
{
    // P56：ORIG 尾部 test%al;je;mov$1;jmp;mov$0 为 if/return 分支形，勿直返表达式。
    // 具名迭代器固定 find 先求值（P74 补充②旋钮一），复现 ORIG 的
    // end(-0x14)/begin(-0x10)/find(-0x18)/end(-0xc) 槽阶梯。
    std::list<CUser*>::iterator found = std::find(m_users.begin(), m_users.end(), user);
    if (found != m_users.end())
        return true;
    return false;
}

// ---- CheckValidItem 三重载（ORIG T 0x85c9d4e / 0x85c9bb8 / 0x85c9cc2，三份报告全文）----

// CheckValidItem(PrivateStoreItem&, int prevAddInfo2) @0x85c9d4e：
// 物品存在/可售类型/生物类排除（vtable+0x14=IsCreatureItem）后按 item space
// 分派来源槽校验；可堆叠条目要求现库存 >= 上架前数量。
// [b28d-24] AE（P167 归零，clean 后 0 残差）。ORIG 0x085c9d4e
bool CPrivateStore::CheckValidItem(PrivateStoreItem& item, int prevAddInfo2)
{
    CItem* info = G_CDataManager()->find_item((int)item.m_item.m_addInfo);
    if (info == 0)
        return false;

    int attachType = info->GetAttachType();
    if (attachType == 0)
        ;    // ORIG je +0x92：放行继续
    else if (attachType == 3)
    {
        if ((int)item.m_item.m_field0 != 1)
            return false;
    }
    else if (attachType == 7)
    {
        if (!item.m_item.GetTradeLimitCount())
            return false;
    }
    else
        return false;    // ORIG default 失败岛（85c9dd6）在 7 分支体之后

    if (IsCreatureItemType((unsigned int)(unsigned char)item.m_item.m_field1))
        return false;

    // ORIG 经 CItem vtable+0x14 虚调（= IsCreatureItem），命中即拒。
    if (info->IsCreatureItem())
        return false;

    // ORIG 短路直跳形（test;je cont; cmp;jge cont; mov$0）——勿用 && 汇聚形
    if (info->is_stackable())
    {
        if ((int)item.m_item.m_addInfo2 < prevAddInfo2)
            return false;
    }

    // 真语义 bug 修复：ORIG 决策树 0x85c9e5c `test%eax;je 0x126`（case0）、
    // 0x85c9e51 `je 0x19d`（case1）、0x85c9e68 `je 0x17a`（case2）、
    // 0x85c9e6d `je 0x151`（case3）与各叶块实参联立 ⇒ case0=type1、
    // case1=type2、case2=int 版、case3=type0/count1。旧重建把 0/1/3 三标签
    // 的体循环错位。叶块物理序 = case 书写序（P46）⇒ 按 0,3,2,1 书写。
    switch ((signed char)item.m_field46)
    {
    case 0:
        return CheckValidItem((INVEN_TYPE)1, (short)item.m_field48,
                              (int)item.m_item.m_addInfo2);
    case 3:
        return CheckValidItem((INVEN_TYPE)0, (short)item.m_field48, 1);
    case 2:
        return CheckValidItem((short)item.m_field48, (int)item.m_item.m_addInfo2);
    case 1:
        return CheckValidItem((INVEN_TYPE)2, (short)item.m_field48,
                              (int)item.m_item.m_addInfo2);
    default:
        return false;
    }
}

// CheckValidItem(INVEN_TYPE, int slot, int count) @0x85c9bb8：
// 槽位有效且非空；type==1 校验装备外数量余量；type==2 仅 avatar 且非临时键、未过期。
// [b28d-24] DIFF 残24: P167 MISSING CInventory::GetAvatarItemMgrR×1（真缺口待补）。ORIG 0x085c9bb8
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
        // P225 推广：ORIG 两条件各发独立 je 直跳成功出口（0x85c9c1a/0x85c9c27），
        // 合并 `&&` 会被 4.4 物化成 setcc 岛 ⇒ 拆嵌套 if。
        if (!item.isEquipableItemType())
        {
            if ((int)item.m_addInfo2 - count < 0)
                return false;
        }
    }
    else if ((int)type == 2)
    {
        if ((int)item.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
            return false;
        // P48：ORIG 两次独立 call GetAvatarItemMgrR（0x85c9c8a/0x85c9cb4），
        // 4.4 无 CSE ⇒ 不设 mgr 缓存局部，逐次写完整链（P52 实参先物化）。
        if (inventory->GetAvatarItemMgrR()->IsTempKey((int)item.m_addInfo2))
            return false;
        if (inventory->GetAvatarItemMgrR()->GetExpireDate((int)item.m_addInfo2) != 0)
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

    // ORIG 0x085c9cc2 尾部（报告 CheckValidItem.md L…）：
    // `if (!isEquipableItemType())`（bool xor$1 形）内嵌 `< 0` 判（jns 跳过 mov$0 岛），
    // 两路汇合 mov$1 —— 与「两 if return true」布局异，探针+diff 实证。
    if (!item.isEquipableItemType())
    {
        if ((int)item.m_addInfo2 - count < 0)
            return false;
    }
    return true;
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
// [b28d-24] AE（P167 归零，clean 后 0 残差）。ORIG 0x085c76f0
// [b28d-solo-CPrivateStoreMgr] **AE（clean 后 0 残差，83=83）**：P0 清绑（删
// `register int addInfo asm("ebx")` / `register bool nullp __asm__("al")`，N96）；
// 槽比零物化改 **P225 while(pItem==0){return 0;}** 复现 ORIG `cmpl $0,-0xc;
// sete %al; test %al,%al; je`（新范式：4.4 对 if 槽比恒直跳，while 形才出四段）。
bool CPrivateStore::CheckTotalItemPrice()
{
    int total = 0;
    for (int i = 0; i <= 0xd; ++i)
    {
        PrivateStoreItem& entry = m_items[i];
        if (entry.m_flag3d != 2)
            continue;
        if (entry.m_field40 == 0)
            return 0;
        CItem* pItem = 0;                          // ORIG +0x55 movl $0,-0xc
        // P0 清绑（原 register asm("ebx") 已删）：ORIG +0x5c..+0x62
        // `mov 0x2(%eax),%eax; mov %eax,%ebx` 是表达式右值经寄存器直传，
        // 原码无具名中转 ⇒ 实参内联。
        pItem = G_CDataManager()->find_item((int)entry.m_item.m_addInfo);
        // ORIG +0x78 `cmpl $0x0,-0xc(%ebp); sete %al; test %al,%al; je`：4.1 槽比零
        // 物化；4.4 对 if 槽比恒 cmp/jne 直跳（P197④）⇒ 试 P225 while 形复现四段。
        while (pItem == 0)
        {
            return 0;
        }
        if (pItem->is_stackable())
        {
            if (!COperator<int>::Product(entry.m_field40,
                                         (int)entry.m_item.m_addInfo2, total))
                return 0;
        }
        else
        {
            total += entry.m_field40;
            if (total <= 0)
                return 0;
        }
    }
    // ORIG +0xf6 岛序：jg 跳过 mov$0 岛、mov$1 位于尾部 ⇒ `<= 0 ? 0 : 1` 写法。
    // 探针 x13 f2：`if (x <= 0) return 0; return 1;` 复现 test/jg/mov0/jmp/mov1 岛序。
    if (total <= 0)
        return 0;
    return 1;
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
    // ORIG 0x085c7bbc：正向 `if (is_stackable())`（test%al;je 直判），
    // !stackable 的 Reset 位于尾部跳岛（+0x40），非 inline。
    if (item->is_stackable())
    {
        if ((int)storeItem.m_item.m_addInfo2 == count)
            storeItem.Reset();
        else
            storeItem.m_item.m_addInfo2 -= count;
    }
    else
    {
        storeItem.Reset();
    }
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
        // ORIG 无中转槽：arg1 直载 *outPrice 旧值（4.1 前端不落临时）
        if (COperator<int>::Product(outPrice, count, outPrice) != 1)
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

    // ORIG choose 双岛形：jg → mov$1；else Do(1) → mov$0
    if (outPrice <= 0)
    {
        m_errorHandler.Do(1);
        return false;
    }
    return true;
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
// [b28d-24] DIFF 残97: P167 MISSING getCurCharacName×2（真缺口）。ORIG 0x085c924c
// [b28d-solo-CPrivateStoreMgr] 已修真缺口：ORIG 共 4 次 getCurCharacName（trace 2
// 次 + historyLog 实参 2 次独立调用，P48）——删除 sellerName/buyerName 具名局部、
// trace 改全表达式临时链（P227，结果入 %esi/%ebx 无槽）、PrivateStoreItemAdd
// 实参改表达式直写。残 104 行 = N18a 单段 callee-saved 直落不可造 + 帧/槽（N82）。
int CPrivateStore::BuyItem(int /*slot*/, Inven_Item& item, int gold,
                           int /*commissionBase*/, int /*flag*/, int& outSlot)
{
    unsigned int resealCount = (unsigned int)item.GetReSealCount();
    unsigned int upgrade = (unsigned int)item.GetUpgrade();
    // P227/P0：ORIG 在 ctor 前把两次 getCurCharacName 读入 %esi/%ebx 跨 ctor
    // 保活（无具名局部槽）⇒ 全表达式临时链 + 实参内联。
    cMyTrace(__PRETTY_FUNCTION__, 0xa63, 0)(
        "PrivateStore buy seller_charac_name(%s), buyer_charac_name(%s), "
        "item_id(%d), add_info(%d), upgrade(%d), reseal_count(%d), sell_price(%d)",
        m_pSeller->getCurCharacName(), m_pBuyer->getCurCharacName(),
        (int)item.m_addInfo, (int)item.m_addInfo2, upgrade, resealCount, gold);

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
            // [BigStore P225] ORIG 0x085c94a0 `cmpl $-0x1,-0x24; sete %al;
            // test; je 续` 物化形 ⇒ while 复现（4.4 直跳无 sete）。
            while (slot == -1)
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

    // P48：ORIG 此处重新调用两次 getCurCharacName（0x085c93xx 段，共 4 次），
    // 不复用 trace 的局部；4.4 无 CSE ⇒ 实参表达式直写。
    m_pBuyer->m_historyLog.PrivateStoreItemAdd(
        (INVEN_TYPE)1, item, outSlot, historyAddInfo, (eItemAddReason)0x11,
        m_pSeller->getCurCharacName(), m_pBuyer->getCurCharacName());

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
// [b28d-24] DIFF 残109: P167 EXTRA memset×2（ORIG 无 memset 调用）。ORIG 0x085c97e2
// [b28d-solo-CPrivateStoreMgr] **AE（clean 后 0 残差，255=255）**：EXTRA memset×2
// → `char msgBuf[256]={0}; char title[21]={0};`（P214③/P202①）；三处条件极性/块序
// 按 ORIG 正向化；**真语义 bug 修正**：msg_184/msg_185 的 snprintf 变参错位
//（count 属 msg_185 分支，0x085c99a8/0x085c9a1f 压栈槽证据）。
int CPrivateStore::SellItem(int /*slot*/, Inven_Item& item, int gold,
                            int commission, int count, int field7)
{
    int netGold = gold - commission;
    // ORIG 0x085c97xx：`call isEquipableItemType; test %al,%al; je <else>` 正向形
    //（then = 装备分支），`!f()` 写法会多一条 xor 并反转块序。
    if (item.isEquipableItemType())
    {
        field7 = 0;
        count = 1;
    }
    else
    {
        field7 -= count;
    }

    m_pSeller->m_historyLog.PrivateStoreItemDel(
        (INVEN_TYPE)1, item, count, field7, (eItemDelReason)0x16,
        m_pSeller->getCurCharacName(), m_pBuyer->getCurCharacName());

    // ORIG 0x085c98xx：`call isDollCreated; test %al,%al; je <gain_money 块>`
    // 正向形（then = 邮件分支）；`!f()` 写法多 xor 且块序反。
    if (isDollCreated())
    {
        // P214③/P202①：ORIG 无 memset call（EXTRA×2），零初始化列表即 rep stos/
        // 双写形；0x100 / 0x15 字节。
        char msgBuf[256] = {0};
        char title[21] = {0};

        std::strncpy(title,
                     g_scriptStringManager_.findString(4, "game_server_msg_183", 0),
                     0x14);

        CItem* pItem = G_CDataManager()->find_item((int)item.m_addInfo);
        // ORIG 0x085c99xx `call is_stackable; xor $1; test; je` ⇒ 否定条件 + 非堆叠
        // 分支 fall-through（msg_184）；正向写法少 xor 且块序反。
        // [b28d-solo-CPrivateStoreMgr] 真语义 bug 修正：ORIG 0x085c99a8（!stackable
        // fall-through, msg_184）变参 5 个 `GetItemName,gold,commission,GetItemName,
        // netGold`；0x085c9a1f（stackable, msg_185）变参 6 个含 count（0x1c(%ebp)
        // 压 0x10(%esp)）。旧码把 count 放在 184 分支 ⇒ 邮件正文参数错位。
        if (!pItem->is_stackable())
        {
            OS_API::snprintf(msgBuf, 0xff,
                             g_scriptStringManager_.findString(4, "game_server_msg_184", 0),
                             pItem->GetItemName(), gold, commission,
                             pItem->GetItemName(), netGold);
        }
        else
        {
            // ORIG 对 GetItemName 连续两次独立调用，保形转写。
            OS_API::snprintf(msgBuf, 0xff,
                             g_scriptStringManager_.findString(4, "game_server_msg_185", 0),
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
    else
    {
        m_pSeller->getCurCharacInvenW()->gain_money(
            netGold, (eMoneyAddReason)0xb, true, 0);
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
CCharacterControllerPrivate::CCharacterControllerPrivate() throw() : m_startPos() {}

// ---- CCharacterControllerPrivate::GetStartPoint（ORIG W 0x85cc894）----
// 计算私商开店起始坐标：pos 先清零再写 village 号(a)；user/world/village 缺失
// 时仅 trace 并保持 b=0。注意 ORIG 后两个失败分支复用 "G_GameWorld call failed"
// 文案（0x8cbd0cf，原版即如此），逐字保留。
// 【b28 P225】world/village 判空的 `cmp $0; sete %al; test; je` 物化岛改
// `while (p == 0) { trace; break; }` 形复现（P225 修正 P197④「槽比恒不可翻」
// 边界；旧 if-return 形产 cmp/jne 直跳为残差主因）。
void CCharacterControllerPrivate::GetStartPoint(CUser* user, PrivateStorePos& pos)
{
    pos.m_field2 = 0x172;
    pos.m_field4 = 0xbe;
    pos.m_a = 0;
    pos.m_b = 0;
    GameWorld* world = 0;    // P197③ ORIG movl $0x0,-0x10/-0xc 双槽预置（位于 pos 写后）
    Village* village = 0;
    if (user == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x183, 5)("pUser == NULL");
        return;
    }
    pos.m_a = (unsigned char)user->getCurCharacVill();
    world = G_GameWorld();
    while (world == 0)   // P225 while 四段物化复现 sete 岛；goto 尾保持原语义
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x18b, 5)("G_GameWorld call failed");
        goto out;
    }
    village = world->getUserVillage(user);
    while (village == 0)   // P225 同上
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x190, 5)("G_GameWorld call failed");
        goto out;
    }
    pos.m_b = (unsigned char)village->get_gate_area();
out:
    ;
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
// [b28d-24] DIFF 残1: ORIG 遗留死 cmpl（P171 照抄）；P167 归零。ORIG 0x085ca7b8
// [b28d-solo-CPrivateStoreMgr] 残 1 行 = ORIG 0x085ca7b8 段 `cmpl $0x0,-0xc(%ebp)`
// 无 jcc 的死比较（4.1 对 `if(p==0){}` 删 je 留 cmp；4.4 前端整块 DCE）⇒ N99 封顶。
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
    CUser* sellerUser = 0;
    // ORIG：参数直落（mov %ax,-0x1c 单槽），无具名中转。
    sellerUser = G_GameWorld()->find_from_world(sellerIndex);
    // ORIG +0x2d 死 cmpl $0x0,-0xc(%ebp)（无 jcc）：4.1 对 `if(p==0){}` 生成
    // cmp+je、jump-opt 删掉目标即下一条的 je 只留 cmp；4.4 前端把空条件整块
    // DCE。实测 if(p==0){} / while(p==0)break;（后者多 sete+test 两条）均不可
    // 复现 ⇒ N 封顶（N99 死代码 DCE 族）。
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
    // ORIG 0x085cad5e：busy 段 &&/|| 物化岛（4.4 原生复现，探针 x15 b1）；
    // isDollCreated 正向 test%al 直判（==0 写法会产 xor$1）；
    // state==1 判定走 sete 物化 + mov$1/$0 岛（`if(...) return true; return false;`）。
    if (store != 0 && (store->GetState() == 1 || store->GetState() == 2))
    {
        if (store->isDollCreated())
        {
            if (store->GetState() == 1)
                return true;
            return false;
        }
        return true;
    }
    return false;
}

// ORIG 0x85cab24（报告 LoadPrivateStore.md 逐指令）：申请 Stream 构造 CStreamGuard，
// 写 0x78(type)、GetUID、get_charac_no(-1) 三项后放入 DB 消息队列(DB_Q=2)。
void CPrivateStoreMgr::LoadPrivateStore(CUser* user)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PrivateStore.cpp", 0xcb9), true);
    guard.operator*()->operator<<(0x78);                  // ORIG CStreamGuard::operator*
    guard.operator*()->operator<<((int)user->GetUID());
    guard.operator*()->operator<<(user->get_charac_no(-1));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- MakeNotipacketPrivateStoreInfo（ORIG W 0x85cc5bc，objdump 逐指令转写）----
// 通知包：header(0,0x55)、byte(sig+4)、byte(占位计数)后遍历 14 条目，
// state==3 者写 short(槽)/int(售价 +0x40)/int(item idx)/int(add_info2)/
// byte(GetItemAttr)/short(endurance)/int(+0xd)/byte+short(强化类型与值)/put_packet(item)，
// 回填计数占位并 finalize(true)。返回上柜条数。
// [b28d-24] DIFF 残54: 协议字段序列化槽序/顺序（P95/P99）。ORIG 0x085cc5bc
// [b28d-solo-CPrivateStoreMgr] 残 39 行：P311 成员数组视图（PrivateStoreItemsView
// 的 `m_items[14]` 成员数组）把寻址链从 6 条/处降到 ORIG 的 4 条形（102→39）；
// N93 标量例外调 i/count 槽序（i 零初始化提前）。余为寄存器角色互换 + 一处
// lea-vs-add（N7/N1）。P292 档① `== 3` 正向包裹已应用。
// [b28d-solo-CPrivateStoreMgr] P311 成员数组视图：ORIG 0x085cc5bc 每条条目访问
// 发 `mov i; imul $0x4c; add sig; add $8`（索引先算，4 条）；字节数组 + cast
// 形 4.4 恒基址先算（6 条）。用具名数组成员视图替代裸字节 cast。
struct PrivateStoreItemsView
{
    char m_pad[8];
    PrivateStoreItem m_items[14];
};
#define PS_ITEM(i) (reinterpret_cast<PrivateStoreItemsView*>(sig)->m_items[(i)])
int CPacketBufPrivate::MakeNotipacketPrivateStoreInfo(SIG_PRIVATE_STORE* sig)
{
    clear();
    put_header(0, 0x55);
    put_byte((int)sig->m_field04);
    int countIndex = get_index();
    put_byte(0);

    // [b28d-solo-CPrivateStoreMgr] N93 标量例外 + ORIG 槽序：ORIG i@-0x10（深）、
    // count@-0xc（浅）⇒ 把 i 的零初始化提到 count 之前（4.4 按首次使用序分配）。
    int i = 0;
    int count = 0;
    for (; i <= 0xd; ++i)
    {
        if (PS_ITEM(i).GetState() == 3)
        {
            put_short(i);
            put_int(PS_ITEM(i).m_field40);           // 售价（条目 +0x40）
            put_int(PS_ITEM(i).m_item.m_addInfo);    // 物品索引（Inven_Item +0x02）
            put_int(PS_ITEM(i).m_item.m_addInfo2);   // Inven_Item +0x07
            put_byte((int)PS_ITEM(i).m_item.GetItemAttr());
            put_short((int)PS_ITEM(i).m_item.m_fieldb);   // 耐久（Inven_Item +0x0b）
            put_int(PS_ITEM(i).m_item.m_fieldd);          // Inven_Item +0x0d
            put_byte((int)PS_ITEM(i).m_item.m_amp.getAbilityType());
            put_short((int)PS_ITEM(i).m_item.m_amp.getAbilityValue());
            put_packet(PS_ITEM(i).m_item);
            ++count;
        }
    }

    put_byte(countIndex, count);
    finalize(true);
    return count;
}
#undef PS_ITEM

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
// [b28d-24] DIFF 残32: 计数器局部槽/循环形状（P25/P31）。ORIG 0x085cc160
// [b28d-solo-CPrivateStoreMgr] 已修条件极性（`==3` 正向包裹，P292 档①）；
// 残 56 行 = nowTick/writtenCount/i 槽序（N98，4.4 按首次使用序分配；声明序互换
// 实测 66 劣化 ⇒ 封顶）+ 寻址链（N1/P30）+ put_binary 实参装载序（P75）。
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
        // P292 档①：ORIG `cmp $3,%al; sete %al; test; je` ⇒ 正向 ==3 包裹
        //（`!=3 continue` 会产 setne/jne 反向形）。
        if (items[i].GetState() == 3)
        {
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
    // ORIG 0x85cbce6-0x85cbd1a：首 if 对 bool 槽 cmpb 直判走 put_byte(1)
    // （块尾 jmp 跳过 else 块）；else if(!p) 段 movzbl+xor$1+test 走
    // put_byte(0) —— else-if 链形（非 if/else 直配，否则无二次测试段）。
    if (dollCreated)
        put_byte(1);
    else if (!dollCreated)
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
// [b28d-solo-CPrivateStoreMgr] 残 21 行 = 3 处 `put_int/put_binary(链式调用结果)`
// 的实参装载序：ORIG `call; mov %eax,%edx; mov this,%eax; mov %edx,0x4(%esp);
// mov %eax,(%esp)`（实参先算入 edx），4.4 恒 `mov this,%edx; mov %eax,0x4(%esp);
// mov %edx,(%esp)`（this 先算）。receiver 为 packet this 固定，实参为链式调用
// 结果，无源码旋钮（P75 跨版本实参装载序）⇒ N 封顶；余为由此 1 条差引起的
// 4 处 jmp 偏移连锁。
void CPacketBufPrivate::MakeCmdpacketBuyPrivateStoreItem(CUser* buyerUser, Inven_Item& item,
                                                         int a3, int a4, int a5, int a6)
{
    (void)a3;   // ORIG 未读参数占位
    (void)a4;
    clear();
    put_header(1, 0x55);
    put_byte(1);
    // ORIG `call 链; mov %eax,%edx; mov this,%eax; mov %edx,0x4(%esp);
    // mov %eax,(%esp)`（实参先于 this 装载）；P226 裸语句表达式实测劣化
    // （21→26）⇒ 保留直写，残差按 P75 跨版本实参装载序封顶。
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
// ============================================================================
// ORIG W（头内联）镜像区：以下定义置于 TU 尾——本 TU 内更早的调用点按
// conservative outgoing 预留（sub $0x18），与 ORIG 产物一致；inline 标记
// 使符号为 W，与 ORIG nm 一致。
// ============================================================================

// ORIG C1/C2(h,h) W 0x85cb6ce。
inline PrivateStoreZone::PrivateStoreZone(unsigned char a, unsigned char b)
    : m_a(a), m_b(b)
{
}

inline PrivateStorePos::PrivateStorePos() : PrivateStoreZone(0, 0), m_field2(0), m_field4(0) {}

// ORIG W 0x85cb72c：zone 基类构造（init 列表）后，+0x2/+0x4 每 store 重读
// 0x8(%ebp)（P29 函数体赋值形：`mov this→%eax; movzwl val→%edx; mov %dx,off`），
// init 列表会被 4.4 展成值先装载（RHS-first）与 ORIG LHS-first 相反。
inline PrivateStorePos::PrivateStorePos(unsigned char a, unsigned char b,
                                        short field2, short field4)
    : PrivateStoreZone(a, b)
{
    m_field2 = field2;
    m_field4 = field4;
}

// ORIG W 0x85cb784。
inline void PrivateStorePos::clear()
{
    m_a = 0;
    m_b = 0;
    m_field2 = 0;
    m_field4 = 0;
}

// ORIG C1 W 0x823457c。
inline PrivateStoreItem::PrivateStoreItem()
{
    Reset();
}

// ORIG W 0x823459a：清空物品区与全部私有字段。
inline void PrivateStoreItem::Reset()
{
    m_item.reset();
    m_flag3d = 0;
    m_field40 = 0;
    m_field44 = 0;
    m_field46 = 0;
    m_field48 = 0;
}

// ORIG W 0x85cb66e：Inven_Item::setCopy 拷物品区，随后逐字段直拷并返回 *this。
inline PrivateStoreItem& PrivateStoreItem::operator=(PrivateStoreItem const& other)
{
    m_item.setCopy(other.m_item);
    m_flag3d = other.m_flag3d;
    m_field40 = other.m_field40;
    m_field44 = other.m_field44;
    m_field46 = other.m_field46;
    m_field48 = other.m_field48;
    return *this;
}

inline CPrivateStorePos::CPrivateStorePos() : m_rect(), m_pStore(0) {}

// ORIG W 0x85cb7fc：写宿主 store 指针后，以中心点 (centerX, centerY) 构造
// 0xa0 x 0x64 的碰撞矩形（左上 = 中心 - 半宽/半高）。
inline CPrivateStorePos::CPrivateStorePos(CPrivateStore* store, int centerX, int centerY)
    : m_rect(), m_pStore(store)
{
    m_rect.m_pos.m_x = centerX - 0x50;
    m_rect.m_width = 0xa0;      // ORIG 先写 width 再 m_y（-O0 语句序）
    m_rect.m_pos.m_y = centerY - 0x32;
    m_rect.m_height = 0x64;
}

}

// ---- 暂落定义（三处 ORIG 真实符号，权威归属批次后续收编；本批唯一落点 CPrivateStoreMgr.cpp）----

// ORIG T _ZN14village_object17CVillageObjectMgr17unregister_objectENS_4ZoneEPNS_7IObjectE @0x86b70d0
// objdump 逐指令转写（无独立 md 报告，自行导出并标记）：map find→end 判定→std::find→erase。
// [b28d-solo-CPrivateStoreMgr] 返回类型 int→bool（P292 档①，mangled 不含返回类型）。
bool village_object::CVillageObjectMgr::unregister_object(village_object::Zone zone,
                                                          village_object::IObject* object)
{
    std::map<village_object::Zone, std::list<village_object::IObject*> >::iterator it =
        m_objects.find(zone);
    if (it == m_objects.end())
        return 0;
    std::list<village_object::IObject*>& objects = it->second;   // ORIG 用 operator->（pt）
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
        return (short)it->second;   // +0x04 mapped value（movzbl 零扩展；ORIG 用 operator->（pt））
    return -1;
}
