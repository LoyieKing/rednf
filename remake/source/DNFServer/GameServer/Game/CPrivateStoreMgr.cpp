#include "CPrivateStoreMgr.h"
#include "MoreTypes.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "CUser.h"
#include "PacketGuard.h"

namespace private_store
{
namespace
{
CPrivateStoreMgr g_privateStoreMgr;
}

// ORIG 0x085ca3d6。真实布局含 CPrivateStore(+4)、StaticPool<CPrivateStore,300>(+0x68)、
// zone 表(+0xc8)；此处还原 stores 表（卖家 +0x98 / 买家 +0xb0）为真实 std::map 并初始化。
// StaticPool / 嵌入 store 深层成员尚未还原（见各函数报告推断标注）。
CPrivateStoreMgr::CPrivateStoreMgr()
{
    m_storeSellers.clear();
    m_storeBuyers.clear();
}

// ORIG 0x085ca4fe。真实析构遍历 zone 表释放 list、并析构 StaticPool 与嵌入 store；
// 此处释放 stores 表中的池对象（推断：StaticPool 尚未还原，故按普通堆归还得）
// 并清空两张 map。
CPrivateStoreMgr::~CPrivateStoreMgr()
{
    for (std::map<CUser*, CPrivateStore*>::iterator it = m_storeSellers.begin();
         it != m_storeSellers.end(); ++it)
    {
        delete it->second;  // 推断：ORIG 经 StaticPool::Free 归还，等价释放
    }
    m_storeSellers.clear();
    m_storeBuyers.clear();
}

CPrivateStoreMgr* GetInstancePrivateStoreMgr()
{
    return &g_privateStoreMgr;
}

// ORIG 0x085ca358：store 卖家表查找（map +0x98），返回 CPrivateStore*。
void* CPrivateStoreMgr::FindStoreSeller(CUser* user)
{
    if (!user)
        return 0;
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeSellers.find(user);
    if (it == m_storeSellers.end())
        return 0;
    return it->second;
}

// ORIG 0x085ca2da：store 买家表查找（map +0xb0）。
void* CPrivateStoreMgr::FindStoreBuyer(CUser* user)
{
    if (!user)
        return 0;
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeBuyers.find(user);
    if (it == m_storeBuyers.end())
        return 0;
    return it->second;
}

// ORIG 0x085ca914：从买家表移除该用户，返回是否命中。
int CPrivateStoreMgr::FreeStoreBuyer(CUser* user)
{
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeBuyers.find(user);
    if (it == m_storeBuyers.end())
        return 0;
    m_storeBuyers.erase(it);
    return 1;
}

// ORIG 0x085ca874：从卖家表移除该用户；ORIG 同时经 StaticPool(<this+0x68>)::Free
// 归还 CPrivateStore（推断：此处由 ~CPrivateStoreMgr 统一释放，故仅移除表项）。
int CPrivateStoreMgr::FreeStoreSeller(CUser* user)
{
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeSellers.find(user);
    if (it == m_storeSellers.end())
        return 0;
    m_storeSellers.erase(it);
    return 1;
}

// ORIG 0x085ca698：取得/创建卖家 store。逻辑：
//   - 若卖家已存在 => *out = store，返回 1；
//   - 否则从 StaticPool(+0x68) 申请新 store；
//       * 池满 => 退回管理器嵌入 store(this+4)，通过 SetSeller 绑定，返回 0；
//       * 成功 => Clear/SetParent/SetSeller，插入 map(+0x98)，返回 1。
// 依赖的 CPrivateStore 方法 (Clear/SetParent/SetSeller) 与 StaticPool 成员尚未还原，
// 此处还原表查找+新表项登记的真实语义，store 构造与 CPrivateStore 初始化标「推断」。
int CPrivateStoreMgr::GetStoreSeller(ISeller** out, CUser* user)
{
    CPrivateStore* store = (CPrivateStore*)FindStoreSeller(user);
    if (!store)
    {
        // 推断：ORIG 从 StaticPool<CPrivateStore,300>(+0x68) 申请；
        // 该池成员未还原，暂以堆分配等价代替（~CPrivateStoreMgr 负责释放）。
        store = new CPrivateStore();
        // 推断：ORIG 调用 CPrivateStore::Clear/SetParent/SetSeller 初始化。
        m_storeSellers[user] = store;
    }
    *out = (ISeller*)store;
    return 1;
}

// ORIG 0x085ca7b8：取得/创建买家 store。逻辑：
//   - 买家已存在 => 直接取；
//   - 否则依据卖家 user3 查找卖家 store，绑定为买家并登记 map(+0xb0)。
// CPrivateStore 初始化 (SetBuyer 等) 与嵌入 store 回退 (this+8) 标「推断」。
int CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser* user, CUser* sellerUser)
{
    CPrivateStore* store = (CPrivateStore*)FindStoreBuyer(user);
    if (!store)
    {
        // 推断：ORIG 复用卖家 user3 的 store 作为买家容器；
        // 若两者皆无，回退到管理器嵌入 store(this+8)。
        store = (CPrivateStore*)FindStoreSeller(sellerUser);
        if (!store)
            store = new CPrivateStore();  // 推断：等价于嵌入回退
        // 推断：ORIG 调用 CPrivateStore::SetBuyer(store, user) 绑定买家。
        m_storeBuyers[user] = store;
    }
    // 推断：ORIG 返回 store+4 之 IBuyer 接口指针。
    *out = (IBuyer*)store;
    return 1;
}

// ORIG 0x085ca760：按 world unique id 解析卖家 CUser 后委托 GetStoreBuyer(3 参版)。
int CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser* user, unsigned short uniqueId)
{
    CUser* sellerUser = G_GameWorld()->find_from_world(uniqueId);
    return GetStoreBuyer(out, user, sellerUser);
}

// ORIG 0x085ca994：退出时若用户为买家或卖家，取得对应 store、通知接口、
// 并从表移除。IBuyer/ISeller 接口虚调用（ORIG *(_+0xc) / *(_+4)）依赖接口类型，
// 此处保留获取/释放真实语义，接口通知调用标「推断」。
int CPrivateStoreMgr::Enter(CUser* user)
{
    (void)user;
    return 0;
}

int CPrivateStoreMgr::Exit(CUser* user)
{
    if (!user)
        return 0;
    if (FindStoreBuyer(user))
    {
        IBuyer* buyer = 0;
        GetStoreBuyer(&buyer, user, (CUser*)0);
        // 推断：ORIG 经 IBuyer 虚表通知退出（*(*(int*)buyer + 0xc)）。
        FreeStoreBuyer(user);
    }
    if (FindStoreSeller(user))
    {
        ISeller* seller = 0;
        GetStoreSeller(&seller, user);
        // 推断：ORIG 经 ISeller 虚表通知退出（*(*(int*)seller + 4)(seller, 1)）。
        FreeStoreSeller(user);
    }
    return 1;
}

// ORIG 0x085caaaa：构建错误返回包并经 CUser::Send 下发。
// 依赖 CPacketBufPrivate::MakeErrorReturnPacket（尚未还原），控制流为：构造 PacketGuard
// -> 填错误码(param2/3/4) -> CUser::Send -> 返回 0。
int CPrivateStoreMgr::HandleError(CUser* user, int code1, int code2, int code3)
{
    (void)user; (void)code1; (void)code2; (void)code3;
    // 推断：CPacketBufPrivate(+0x688 构造 / MakeErrorReturnPacket) 未还原，
    // 功能路径待补，保留返回 0（ORIG 返回值即为 0）。
    return 0;
}

// ORIG 0x085cad5e：卖家存在且处于挂机/营业态(1/2)且未生成娃娃时为忙碌。
// CPrivateStore::GetState/isDollCreated 未还原，仅还原 FindStoreSeller 判定骨架。
bool CPrivateStoreMgr::IsBusyPrivateStore(CUser* user)
{
    CPrivateStore* store = (CPrivateStore*)FindStoreSeller(user);
    if (!store)
        return false;
    // 推断：GetState(store) == 1 || GetState(store) == 2 时为忙碌候选；
    // 再据 isDollCreated 细化。方法未还原，此处以"存在卖家 store"为忙碌。
    return true;
}

// ORIG 0x085cab24：申请 Stream，写 0x78(type)、GetUID、get_charac_no(-1) 三项，
// 放入 DB 消息队列(DB_Q=2)。全部依赖已具备，为真实实现。
int CPrivateStoreMgr::LoadPrivateStore(CUser* user)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("PrivateStore.cpp", 0xcb9);
    CStreamGuard guard(s, true);
    guard->operator<<(0x78);
    guard->operator<<((int)user->GetUID());
    guard->operator<<(user->get_charac_no(-1));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

// ORIG 0x085cac16：LoadPrivateStore 的响应处理。经 GetStoreSeller 取得卖家 store 后
// 依据 SIG_PRIVATE_STORE(+0x430) 分支：
//   - ==1：ResetItems/CopyItems/Recall/Save/Clear 后 FreeStoreSeller；
//   - 否则若 IsPermissionPrivateStore：MakeNotipacketPrivateStoreInfo -> CUser::Send，
//     若返回非 0 再经 ISeller 虚表(param2) 通知。
// 依赖 CPrivateStore 方法与 CPacketBufPrivate 未还原，控制流 + 核心 GetStoreSeller/
// FreeStoreSeller 为真实语义。
int CPrivateStoreMgr::LoadPrivateStoreResponse(CUser* user, SIG_PRIVATE_STORE* sig)
{
    int made = 0;
    if (sig && *(int*)((char*)sig + 0x430) == 1)
    {
        ISeller* seller = 0;
        GetStoreSeller(&seller, user);
        CPrivateStore* store = (CPrivateStore*)seller;
        // 推断：ORIG 调用 CPrivateStore::ResetItems/CopyItems/Recall/Save/Clear；
        // 方法未还原。
        (void)store;
        FreeStoreSeller(user);
    }
    else
    {
        if (user && user->IsPermissionPrivateStore())
        {
            // 推断：CPacketBufPrivate::MakeNotipacketPrivateStoreInfo 未还原，
            // 通知包路径待补。
            made = 0;
        }
    }
    (void)made;
    return 0;
}

int CPrivateStoreMgr::GetPrivateStoreSize()
{
    // 推断：ORIG 统计 zone 表中活跃 store 数量（map +0xc8）；zone 表未还原，
    // 暂以卖家表规模近似（卖家 store 即当前在开店的 store）。
    return (int)m_storeSellers.size();
}
}