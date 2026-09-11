#ifndef DNF_GAME_ARAD_DISPATCHER_H_
#define DNF_GAME_ARAD_DISPATCHER_H_

// ============================================================================
// df_game_r 还原 —— ARAD 内部/DB 派发器链（localjapan/Arad_Dispatch_Manager.cpp、
// localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp）。
//
// ORIG 证据（nm -S --defined-only df_game_r / objdump）：
//   ARAD::Arad_Dispatcher
//     C1/C2(int) 0x0818edd0 W：vptr=0x8b96750；m_code(=参数) 写 +0x04。
//     D1/D2      0x0818e900 W：写基类 vptr；D0 0x0818e926 条件 operator delete。
//     vtable @0x8b96750：[D1, D0, db_dispatch 0x0818e84c, inter_dispatch 0x0818e856]
//     dispathc_code()        0x0818e898 T：return *(int*)(this+4)（非虚）。
//     register_dispatcher    0x0818e860 T：GlobalInstance<Manager>::inst()->register_dispatcher。
//     unregister_dispatcher  0x0818e87c T：同上转发 unregister_dispatcher(code)。
//   ARAD::Arad_Dispatcher_Manager（无虚函数/无 vtable，非多态）
//     C1/C2 0x0818fdd4 T：两个循环各清零 0x354 个 dword → 两组 852 项指针数组，
//         合计 size 0x1AA0。+0x0000 = inter 组（inter_dispatch/register 第一写），
//         +0x0D50 = db 组（db_dispatch/(code+0x354) 第二写）。
//     D1/D2 0x0818fe32 T：空。
//     open()               0x0818fe38 T：new Arad_Dispatcher_BuyItemCheck_Impl → register。
//     register_dispatcher  0x0818fe98 T：dispathc_code() 命中两组同槽位写入，返回 0。
//     unregister_dispatcher0x0818fed6 T：inter/db 两槽逐个判空置空并调其析构（Ghidra
//         标 unreachable 的虚调用块），全空返回 1，否则返回 0。
//     inter_dispatch       0x0818ff6c T：查 inter 组 → 虚槽3（vtable+0xc）；失败记日志、
//         user 非空时 DisConnSig(2,1,err+1000000)，返回 0/1。
//     db_dispatch          0x08190080 T：查 db 组 → 虚槽2（vtable+0x8）；ErrLine!=0 记日志；
//         找到即返回 1，未找到返回 0。
//   ARAD::Arad_Dispatcher_BuyItemCheck_Impl
//     C1/C2 0x0818e9cc T：基类(0x1fa) + 自身 vptr。派发码 0x1fa=506。
//     inter_dispatch 0x0818ea3e T / db_dispatch 0x0818eb0c T（覆盖基类两虚槽）。
//
// 本头为唯一声明点；实现在 Arad_Dispatcher.cpp。
// ============================================================================

#include "CEnvironment.h"   // GlobalInstance<T>（ORIG register/unregister 转发所用单例模板）

class CUser;
class Stream;

namespace ARAD
{

class Arad_Dispatcher
{
public:
    Arad_Dispatcher(int code);                    // 0x0818edd0
    virtual ~Arad_Dispatcher();                   // D1 0x0818e900 / D0 0x0818e926
    // vtable 槽2（ORIG 基类实现直接返回 0）
    virtual int db_dispatch(int code, int uid, Stream* stream);
    // vtable 槽3（ORIG 基类实现直接返回 0）
    virtual int inter_dispatch(CUser* user, char* body, int len);

    int dispathc_code();                          // 0x0818e898 T（ORIG 非 const 强符号）

    void register_dispatcher(Arad_Dispatcher* d); // 0x0818e860
    void unregister_dispatcher(int code);         // 0x0818e87c

protected:
    int m_code;                                   // +0x04
};

class Arad_Dispatcher_Manager
{
public:
    // ORIG prett 串形参名（`ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE`）：
    // 纯增量类内 typedef（与 int 同类型 ⇒ 不入 mangled、零 ABI/布局影响）。
    // 定义侧按此拼写即可让 `__PRETTY_FUNCTION__` 复现 ORIG 串（见 Arad_Dispatcher.cpp）。
    typedef int INTERNAL_PACKET_CODE;
    Arad_Dispatcher_Manager();                    // 0x0818fdd4
    ~Arad_Dispatcher_Manager();                   // D1/D2 0x0818fe32（非虚析构）
    bool open();                                  // 0x0818fe38
    bool register_dispatcher(Arad_Dispatcher* d); // 0x0818fe98
    bool unregister_dispatcher(int code);         // 0x0818fed6
    bool inter_dispatch(int code, CUser* user, char* body, int len);  // 0x0818ff6c
    bool db_dispatch(int code, int uid, Stream* stream);              // 0x08190080

private:
    Arad_Dispatcher* m_interDispatchers[0x354];   // +0x0000（852 项）
    Arad_Dispatcher* m_dbDispatchers[0x354];      // +0x0D50（852 项）
};

class Arad_Dispatcher_BuyItemCheck_Impl : public Arad_Dispatcher
{
public:
    Arad_Dispatcher_BuyItemCheck_Impl();          // 0x0818e9cc（基类码 0x1fa）
    ~Arad_Dispatcher_BuyItemCheck_Impl();         // D1 0x0818e9f0 / D0 0x0818ea20
    virtual int db_dispatch(int code, int uid, Stream* stream);    // 0x0818eb0c
    virtual int inter_dispatch(CUser* user, char* body, int len);  // 0x0818ea3e
};

}  // namespace ARAD

// ============================================================================
// ARAD_DATAS::BuyItemInfo —— arad_sp_buy_item_check / BuyItemCheck_Impl::db_dispatch
// 共用的查询入参（ORIG 栈布局 -0x28(%ebp)，按 Stream 读序与 SQL 参数序还原）：
//   +0x00 char   buyable    （查询结果回写 1/0）
//   +0x02 short  checkType  （==1 时追加 charac_no 过滤）
//   +0x04 short  limitCount （count>=limit → buyable=0）
//   +0x08 uint   characNo   （SQL charac_no）
//   +0x0c uint   mid        （SQL m_id）
//   +0x10 uint   ipgNo      （SQL ipg_no）
// ============================================================================
namespace ARAD_DATAS
{

struct BuyItemInfo
{
    char buyable;            // +0x00
    char m_pad01;
    short checkType;         // +0x02
    short limitCount;        // +0x04
    unsigned int characNo;   // +0x08
    unsigned int mid;        // +0x0c
    unsigned int ipgNo;      // +0x10
};

}  // namespace ARAD_DATAS

namespace ARAD
{

// ORIG T 0x0819e8d0（localjapan/Arad_DB_Query.cpp）：buf ← sprintf("%02d%d", 年, 季度)。
void GetQuarterOfYear(char* buf);

namespace DATABASE { namespace ITEM_QUERY {

// ORIG T 0x0818cfab：prod_sale_entry_<quarter> count(*) 限购校验。0=成功（info.buyable 已回写），-1=错误。
int arad_sp_buy_item_check(ARAD_DATAS::BuyItemInfo& info);

}}  // namespace DATABASE::ITEM_QUERY
}  // namespace ARAD

#include <string>

namespace ARAD
{

// ---------------------------------------------------------------------------
// ARAD 邮件数据族（localjapan/Arad_Function_Set.cpp 域）。
// ORIG 证据（objdump ctor 0x081a3fc2 / 0x081a3ff2、GetItemAttr 0x08197e12）：
//   AradMailItemInfo（sizeof 0x18【推断：0x15 字段区对齐到 4】）
//     +0x00 uchar            （send_mail → sig+0x2a）
//     +0x04 uint             （→ sig+0x2f）
//     +0x0a ushort           （→ sig+0x34）
//     +0x0c uint             （→ sig+0x2b）
//     +0x10 uint             （→ sig+0x36）
//     +0x14 uchar attr       （GetItemAttr 返回值，→ sig+0x33）
//   AradMailInfo（ctor 清零 +0x00..+0x1d 后于 +0x20 构造 ItemInfo；send_mail 参数）
struct AradMailItemInfo
{
    unsigned char flag;          // +0x00
    char m_pad01_03[3];
    unsigned int field04;        // +0x04
    char m_pad08_09[2];
    unsigned short field0a;      // +0x0a
    unsigned int field0c;        // +0x0c
    unsigned int field10;        // +0x10
    unsigned char itemAttr;      // +0x14
    char m_pad15_17[3];

    unsigned char GetItemAttr() const { return itemAttr; }  // 0x08197e12 W const
};

struct AradMailInfo
{
    unsigned char mailType;      // +0x00（send_mail → sig+0x15f 低字节）
    unsigned int field04;        // +0x04 （→ sig+0x26）
    unsigned int field08;        // +0x08 （→ sig+0x15b）
    unsigned int field0c;        // +0x0c （→ sig+0x22）
    unsigned int field10;        // +0x10 （→ sig+0x1e）
    short field14;               // +0x14 （→ sig+0x59）
    char m_pad16_17[2];
    char* titleText;             // +0x18（非空时 strncpy 到 sig+0x5b）
    char* senderName;            // +0x1c（strncpy(sig,..,0x1d)）
    AradMailItemInfo item;       // +0x20

    AradMailInfo();
private:
    AradMailInfo(const AradMailInfo&);            // -wcopy（ORIG 未用拷贝语义建模）
    AradMailInfo& operator=(const AradMailInfo&);
};

// ORIG T 0x08196b03：Acquire stream → CStreamGuard → 写 SIG_MAILBOX_SEND_NEW_MAIL
// （msgcode 0x80/uid -1），MsgQueueMgr::put(QUEUE_MAIL=2) 投递邮件队列。
int send_mail(AradMailInfo& info);

}  // namespace ARAD


#endif  // DNF_GAME_ARAD_DISPATCHER_H_
