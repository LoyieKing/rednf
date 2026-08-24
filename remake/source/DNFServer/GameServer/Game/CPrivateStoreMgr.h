#ifndef DNF_GAME_PRIVATE_STORE_MGR_H_
#define DNF_GAME_PRIVATE_STORE_MGR_H_

#include <list>
#include <map>
#include <string>
#include <vector>

#include "CExpertJob.h"        // collision_detection::Rectangle
#include "CVillageObjectMgr.h" // village_object::IObject
#include "Inven_Item.h"
#include "CInventory.h"   // INVEN_TYPE / CInventory 完整类型（CheckValidItem 族）
#include "PacketGuard.h"

class CUser;

class CItem;
namespace private_store
{
struct SIG_PRIVATE_STORE;

class CPrivateStore;      // CPrivateStorePos 带参构造参数（ORIG W 0x85cb7fc）
class CPrivateStoreMgr;   // CPrivateStore::m_pParent / SetParent

// ---- 接口基类（ORIG ISeller@+0x00 / IBuyer@+0x04 各占一个 vptr）----
// 【虚表核对（FixPrivateStoreLoadDeps/PrivateStorePos_full.md §7，nm+vtable 实证）】
//   vtable for ISeller @0x8cbebe0（10 槽纯虚）、vtable for IBuyer @0x8cbebb8
//   （4 槽纯虚）；ISellerC1 0x85ccac0 写 0x8cbebe8、IBuyerC1 0x85ccace 写 0x8cbebc0。
//   CPrivateStore 主基类(ISeller) 10 槽（vtable @0x8cbeb60，entries 0x8cbeb68..）：
//     +0x00 CreatePrivateStore(uchar,uchar,short,short,ushort) 0x85c53ea
//     +0x04 RemovePrivateStore(bool)                            0x85c5fd0
//     +0x08 CompleteDisplay(char const*,ushort)                 0x85c7800
//     +0x0c MoveToGate()                                        0x85c751a
//     +0x10 AddItem(PrivateStoreItem&,int)                      0x85c6d48
//     +0x14 RecreatePrivateStore(SIG_PRIVATE_STORE*)            0x85c5b60
//     +0x18 BuyPrivateStoreItem(int,int)                        0x85c7e74
//     +0x1c VerifyPrivateStoreItem(int,int)                     0x85c874c
//     +0x20 EnterPrivateStore()                                 0x85c8dd6
//     +0x24 ExitPrivateStore()                                  0x85c8fd4
//   次级基类(IBuyer,+4) 4 槽为 non-virtual thunk（vtable 0x8cbeb98..0x8ebeba4）：
//     BuyPrivateStoreItem/VerifyPrivateStoreItem/EnterPrivateStore/ExitPrivateStore。
struct PrivateStoreItem;
class CCharacterControllerPrivate;

class ISeller
{
protected:
    ISeller() {}
    ~ISeller() {}
public:
    virtual bool CreatePrivateStore(unsigned char zoneA, unsigned char zoneB,
                                    short posX, short posY,
                                    unsigned short dollSlot) = 0;          // ORIG T 0x85c53ea（槽+0x00）
    virtual bool RemovePrivateStore(bool sendFail) = 0;                  // ORIG T 0x85c5fd0（槽+0x04）
    virtual bool CompleteDisplay(char const* subject, unsigned short dollSlot) = 0;  // ORIG T 0x85c7800（槽+0x08）
    virtual bool MoveToGate() = 0;                                       // ORIG T 0x85c751a（槽+0x0c）
    virtual bool AddItem(PrivateStoreItem& item, int prevCountSlot) = 0; // ORIG T 0x85c6d48（槽+0x10）
    virtual int RecreatePrivateStore(SIG_PRIVATE_STORE* sig) = 0;        // ORIG T 0x85c5b60（槽+0x14）
    virtual bool BuyPrivateStoreItem(int slot, int count) = 0;           // ORIG T 0x85c7e74（槽+0x18）
    virtual bool VerifyPrivateStoreItem(int slot, int count) = 0;        // ORIG T 0x85c874c（槽+0x1c）
    virtual bool EnterPrivateStore() = 0;                                // ORIG T 0x85c8dd6（槽+0x20）
    virtual bool ExitPrivateStore() = 0;                                 // ORIG T 0x85c8fd4（槽+0x24）
};

class IBuyer
{
protected:
    IBuyer() {}
    ~IBuyer() {}
public:
    virtual bool BuyPrivateStoreItem(int slot, int count) = 0;           // ORIG thunk -7（槽+0x00）
    virtual bool VerifyPrivateStoreItem(int slot, int count) = 0;        // （槽+0x04）
    virtual bool EnterPrivateStore() = 0;                                // （槽+0x08）
    virtual bool ExitPrivateStore() = 0;                                 // （槽+0x0c）
};

// ---- CErrorHandler（私商错误处理器；嵌入 CPrivateStore+0x14，12 字节。
//      ORIG Set W 0x85cb7a8 / Do T 0x85c4c7c；nm 实证本类无 C1/D1 符号，
//      纯 POD——CPrivateStore ctor 不初始化 +0x14..+0x1f）----
class CErrorHandler
{
public:
    void Set(CUser* user, int sendError, int code);   // ORIG W 0x85cb7a8
    bool Do(int errorCode);                            // ORIG T 0x85c4c7c（返回 m_user!=0）

private:
    CUser* m_user;        // +0x00（Set 写入；Do 的发送目标与空判定）
    int m_sendError;      // +0x04（Set 第 2 参 → MakeErrorReturnPacket 第 1 参）
    int m_code;           // +0x08（Set 第 3 参 → MakeErrorReturnPacket 第 2 参）
};                        // sizeof 0x0c

// ---- CBroadcastPrivate（无状态广播助手；ORIG 仅一个符号 W 0x85cca73）----
class CBroadcastPrivate
{
public:
    static void Send(int vill, int area, PacketGuard& packet);  // ORIG W 0x85cca73
};

// ORIG B _ZN13private_store21g_CharacterControllerE @0x09472de0（BSS，size 6）
extern CCharacterControllerPrivate g_CharacterController;
// ---- COperator<T>（溢出检测运算助手；ORIG 仅存在 int 特化 W _ZN13private_store9COperatorIiE7ProductEiiRi @0x85cd22a）----
template <typename T>
class COperator
{
public:
    static char Product(T a, T b, T& out);   // 返回 1=成功（%al）
};


// ---- PrivateStorePos（6 字节：zone a/b + short/short；ORIG C1 0x85cb6f6）----
struct PrivateStorePos
{
    PrivateStorePos();
    void clear();

    unsigned char m_a;
    unsigned char m_b;
    unsigned short m_field2;
    unsigned short m_field4;
};

// ---- CCharacterControllerPrivate（私商开店起点控制器；ORIG C1 W 0x85d0f90 /
//      GetStartPoint W 0x85cc894）----
class CCharacterControllerPrivate
{
public:
    CCharacterControllerPrivate();
    void GetStartPoint(CUser* user, PrivateStorePos& pos);
    void MoveCharacterStartPoint(CUser* user);            // ORIG W 0x85cc9d2：把用户移回开店起点

private:
    PrivateStorePos m_startPos;  // +0x00（ctor 初始化；GetStartPoint 不读 this 状态）
};

// ---- PrivateStoreItem（Inven_Item(0x3d)+byte/int/short/byte/short = 0x4c；
//      ORIG C1 0x823457c / Reset 0x823459a）----
struct PrivateStoreItem
{
    PrivateStoreItem();

    unsigned char GetState() { return m_flag3d; }  // ORIG W 0x85cb662：movzbl +0x3d 直返
    void Reset();

    Inven_Item m_item;          // +0x00（0x3d）
    char m_pad3d[3];
    unsigned char m_flag3d;     // +0x3d
    int m_field40;              // +0x40
    unsigned short m_field44;   // +0x44
    unsigned char m_field46;    // +0x46
    unsigned short m_field48;   // +0x48
};                              // sizeof 0x4c

// ---- CPrivateStorePos（村庄对象；IObject vptr + Rectangle + 宿主 store 指针，
//      sizeof 0x18）----
class CPrivateStorePos : public village_object::IObject
{
public:
    CPrivateStorePos();                                               // ORIG W 0x85cb7c8
    CPrivateStorePos(CPrivateStore* store, int centerX, int centerY); // ORIG W 0x85cb7fc

    void reset();  // IObject vtable+0（ORIG 内联空体，无独立符号）
    virtual bool put_object_info(PacketGuard& p);  // vtable+4（ORIG T 0x85cadf6）
    bool include(village_object::IObject const& obj) const;       // ORIG W 0x85cb85c
    CPrivateStorePos& operator=(CPrivateStorePos const& other);   // ORIG W 0x85ccae4

private:
    collision_detection::Rectangle m_rect;  // +0x04
    CPrivateStore* m_pStore;                // +0x14（宿主 store；put_object_info 判空对象）
};


// ---- CPrivateStore（ORIG C1 0x85c4d10 / D1 0x85c4ecc）----
// 布局：+0x00 ISeller、+0x04 IBuyer、+0x08 byte、+0x0c int、+0x10 byte、
// +0x20 int、+0x24 PrivateStorePos、+0x34 list<CUser*>、+0x3c
// vector<PrivateStoreItem>（reserve/resize 0xe）、+0x48 string(reserve 0x28)、
// +0x4c CPrivateStorePos。
class CPrivateStore : public ISeller, public IBuyer
{
public:
    CPrivateStore();
    ~CPrivateStore();
    // ---- ISeller 十槽 / IBuyer 四槽实现（唯一权威定义于 CPrivateStoreMgr.cpp）----
    bool CreatePrivateStore(unsigned char zoneA, unsigned char zoneB,
                            short posX, short posY, unsigned short dollSlot);   // ORIG T 0x85c53ea
    bool RemovePrivateStore(bool sendFail);                                     // ORIG T 0x85c5fd0
    bool CompleteDisplay(char const* subject, unsigned short dollSlot);         // ORIG T 0x85c7800
    bool MoveToGate();                                                          // ORIG T 0x85c751a
    bool AddItem(PrivateStoreItem& item, int prevCountSlot);                    // ORIG T 0x85c6d48
    bool BuyPrivateStoreItem(int slot, int count);                              // ORIG T 0x85c7e74
    bool VerifyPrivateStoreItem(int slot, int count);                           // ORIG T 0x85c874c
    bool EnterPrivateStore();                                                   // ORIG T 0x85c8dd6
    bool ExitPrivateStore();                                                    // ORIG T 0x85c8fd4

    unsigned char GetState() { return m_state; }          // ORIG W 0x82345d8：movzbl +0x10（调用方仅读 %al，故按字节返回）
    unsigned char isDollCreated();                        // ORIG W 0x82345e4：movzbl +0x08 直返（定义于 .cpp）
    CUser* GetSeller() { return m_pSeller; }              // ORIG W 0x85cb87c：返回 +0x2c
    std::string* GetSubject() { return &m_name; }         // ORIG W 0x85cb888：返回 &+0x48
    int GetSellerIndex();                                 // ORIG W 0x85cb894：GetSeller()->get_unique_id()
    void SetSeller(CUser* user) { m_pSeller = user; }     // ORIG W 0x85cb8aa：写 +0x2c
    void SetBuyer(CUser* user) { m_pBuyer = user; }       // ORIG W 0x85cb8b8：写 +0x30
    void SetParent(CPrivateStoreMgr* parent) { m_pParent = parent; }  // ORIG W 0x85cb8c6：写 +0x20
    PrivateStorePos* GetPos() { return &m_pos; }          // ORIG W 0x85cb8d4：返回 &+0x24
    void SetDollCreated(bool created) { m_dollCreated = created ? 1 : 0; }  // ORIG W 0x85cb8e0：写 +0x08
    unsigned long GetDollItemID()                         // ORIG W 0x85cb8f8：doll 已生成才返回 +0x0c
    {
        return isDollCreated() ? m_dollItemID : 0;
    }
    void SetDollItemID(unsigned long itemId)              // ORIG W 0x85cb91c：doll 已生成才写 +0x0c
    {
        if (isDollCreated())
            m_dollItemID = itemId;
    }

    void ResetItems();                                   // ORIG T 0x85c4f52：逐项 PrivateStoreItem::Reset
    void CopyItems(PrivateStoreItem* items, int count);  // ORIG T 0x85c4f9c：ResetItems 后 std::copy 至 m_items 头部
    void Clear();                                        // ORIG T 0x85c4e70：清卖家/买家/状态/位置/列表并 ResetItems
    bool RecallPrivateStoreItemToMail();                 // ORIG T 0x85c5cec：state==3 条目打包系统邮件退还，恒 true（ORIG 源码返回类型 bool，trace 原串实证）
    int RecreatePrivateStore(SIG_PRIVATE_STORE* sig);    // ORIG T 0x85c5b60：重算开店起点并从 SIG 恢复 14 条目，置状态 2（ISeller 虚表槽 +0x14）
    // ---- 成员辅助函数（9 槽方法全链依赖，全部 ORIG 真实符号）----
    bool MoveItems();                                   // ORIG T 0x85c7246：把过期/下架条目退回背包
    bool RecallPrivateStoreItem();                      // ORIG T 0x85c63ca：非 doll 路径条目退还
    bool IsExistBuyerList(CUser* user);                 // ORIG T 0x85c91b6：m_users 是否含 user
    int GetCommissionPercent();                         // ORIG T 0x85c9b8c：手续费百分比
    int CheckTotalItemPrice();                          // ORIG T 0x85c76f0：在售条目总价校验
    int CheckPrivateStoreDoll(unsigned short dollSlot); // ORIG T 0x85c532c：返回错误码（0=可用）
    bool IsAreaNearEntranceDungeon(CUser* user);        // ORIG T 0x85c5082
    bool DeleteItem(PrivateStoreItem& item);            // ORIG T 0x85c68b4（PRETTY 串 bool 实证）
    bool GetItemInfo(CUser* user, PrivateStoreItem& item);              // ORIG T 0x85c9f22（返回装载 0/1）
    int SendUsers(PacketGuard* packet, CUser* excludeA, CUser* excludeB);   // ORIG T 0x85c7adc：返回发送计数
    bool GetItemPrice(CItem const* item, PrivateStoreItem& storeItem, int count,
                      int& outPrice, unsigned int& outBuyGold, unsigned int& outSellGold);  // ORIG T 0x85c7d04
    bool IsPossibleDeal(int gold, unsigned int a3, unsigned int a4);            // ORIG T 0x85c7c0a
    void SubtractItemCount(CItem const* item, PrivateStoreItem& storeItem, int count);  // ORIG T 0x85c7bbc
    int BuyItem(int slot, Inven_Item& item, int gold, int commissionBase, int flag,
                int& outSlot);                          // ORIG T 0x85c924c
    int SellItem(int slot, Inven_Item& item, int gold, int commission, int count,
                 int field7);                           // ORIG T 0x85c97e2
    bool CheckValidItem(PrivateStoreItem& item, int prevAddInfo2);      // ORIG T 0x85c9d4e
    void SavePrivateStore();                             // ORIG T 0x85ca116：SIG_PRIVATE_STORE(0x77) 入 DB 队列

    bool CheckValidItem(INVEN_TYPE type, int slot, int count);   // ORIG T 0x85c9bb8
    bool CheckValidItem(int cargoSlot, int count);               // ORIG T 0x85c9cc2

    unsigned char m_dollCreated;        // +0x08（ctor 置 0）
    char m_pad09[3];
    unsigned long m_dollItemID;         // +0x0c（ctor 置 0）
    unsigned char m_state;              // +0x10（ctor 置 0）
    char m_pad11[3];                    // +0x11 对齐填充
    CErrorHandler m_errorHandler;       // +0x14（ORIG 嵌入错误处理器，12 字节）
    CPrivateStoreMgr* m_pParent;        // +0x20（ctor 置 0）
    PrivateStorePos m_pos;              // +0x24（6 字节 + 对齐填充）
    CUser* m_pSeller;                   // +0x2c（ORIG ctor 不初始化，开店流程经 SetSeller 写入）
    CUser* m_pBuyer;                    // +0x30（同上，SetBuyer 写入）
    std::list<CUser*> m_users;          // +0x34
    std::vector<PrivateStoreItem> m_items;  // +0x3c
    std::string m_name;                 // +0x48（GetSubject 即此店名/告示）
    CPrivateStorePos m_storePos;        // +0x4c
};

// ---- CPacketBufPrivate（PacketGuard 子类；MakeErrorReturnPacket ORIG W 0x85cc830）----
class CPacketBufPrivate : public PacketGuard
{
public:
    void MakeErrorReturnPacket(int code1, int code2, int code3);              // ORIG W 0x85cc830
    // ---- 9 槽全链包构造（ORIG W 内联符号，定义于 CPrivateStoreMgr.cpp）----
    void MakeCmdpacketCreatePrivateStore();                                       // W 0x85cb986
    void MakeNotipacketCreatePrivateStore(unsigned short sellerIndex,
                                          PrivateStorePos& pos, bool dollCreated);  // W 0x85cb9da
    void MakeCmdpacketMoveToGate();                                               // W 0x85cc7dc
    void MakeNotipacketRemovePrivateStore(unsigned short sellerIndex);            // W 0x85cbb2c
    void MakeCmdpacketRemovePrivateStore();                                       // W 0x85cbad8
    void MakeNotipacketForceRemovePrivateStore();                                 // W 0x85cbb88
    void MakeCmdpacketExitPrivateStore();                                         // W 0x85cc568
    void MakeCmdpacketEnterPrivateStore(CUser* sellerUser, unsigned short sellerIndex,
                                        std::vector<PrivateStoreItem>& items);    // W 0x85cc160
    void MakeNotipacketCompleteDisplay(unsigned short sellerIndex,
                                       std::string& subject, bool dollCreated);   // W 0x85cbc52
    void MakeCmdpacketCompleteDisplay(bool dollCreated);                          // W 0x85cbbca
    void MakeNotipacketPrivateStoreItem(int slot, int count);                     // W 0x85cc0fa
    void MakeNotipacketBuyPrivateStoreItem(CUser* buyerUser, int itemId, int slot,
                                           int count, char const* name,
                                           int gold, int commission);             // W 0x85cbfea
    void MakeCmdpacketBuyPrivateStoreItem(CUser* buyerUser, Inven_Item& item,
                                          int a3, int a4, int a5, int a6);        // W 0x85cbd38
    int MakeNotipacketPrivateStoreInfo(SIG_PRIVATE_STORE* sig);               // ORIG W 0x85cc5bc
};

// 私有商店管理器（ORIG 域 0x085ca3d6）。
// 布局依据 docs/class_func_reports/private_store__CPrivateStoreMgr/ 报告：
//   +0x04  CPrivateStore（管理器自带/回退 store）
//   +0x68  StaticPool<CPrivateStore,300>
//   +0x98  map<CUser*,CPrivateStore*>   —— 店主(卖家)表
//   +0xb0  map<CUser*,CPrivateStore*>   —— 买家表
//   +0xc8  map<PrivateStoreZone,list<CPrivateStore*>*>
// 当前头文件为渐进重建的最小可编译声明：stores 表已用真实 std::map 表达，
// StaticPool/zone 表等深层依赖尚未还原（见各函数报告标注）。
class CPrivateStoreMgr
{
public:
    CPrivateStoreMgr();
    ~CPrivateStoreMgr();
    int Enter(CUser*);
    int Exit(CUser*);
    void* FindStoreBuyer(CUser*);
    void* FindStoreSeller(CUser*);

    int FreeStoreBuyer(CUser*);
    int FreeStoreSeller(CUser*);
    int GetStoreBuyer(IBuyer**, CUser*, unsigned short);
    int GetStoreBuyer(IBuyer**, CUser*, CUser*);
    int GetStoreSeller(ISeller**, CUser*);
    int HandleError(CUser*, int, int, int);
    bool IsBusyPrivateStore(CUser*);
    int LoadPrivateStore(CUser*);
    void LoadPrivateStoreResponse(CUser*, SIG_PRIVATE_STORE*);   // ORIG T 0x85cac16（返回 void，ORIG 尾部无 eax 装载）
    int GetPrivateStoreSize();

private:
    std::map<CUser*, CPrivateStore*> m_storeSellers;  // ORIG +0x98
    std::map<CUser*, CPrivateStore*> m_storeBuyers;   // ORIG +0xb0
};

CPrivateStoreMgr* GetInstancePrivateStoreMgr();
}

#endif