#ifndef DNF_GAME_TAIWAN_CASH_H_
#define DNF_GAME_TAIWAN_CASH_H_

// ============================================================================
// df_game_r 还原 —— Taiwan 计费子系统（Taiwan::BillTransaction / PurchaseQuery /
// TaiwanCash / WongWork::IPG::CIPGMgr::getBillingCash / CheckBillingErrorCode）。
//
// ORIG 依据（docs/class_func_reports/Taiwan__TaiwanCash/、WongWork__IPG__CIPGMgr/、
// _lt_global_gt_/CheckBillingErrorCode.md + df_game_r objdump 实测）：
//   - BillTransaction::init                W 0x08175540
//   - PurchaseQuery ctor                   T 0x0817454c
//   - PurchaseQuery::setQuery              T 0x081745aa
//   - PurchaseQuery::execQuery             T 0x081745f2
//   - PurchaseQuery::getResult             T 0x0817460e
//   - TaiwanCash::TaiwanCash(uchar,short)  T 0x08173608
//   - TaiwanCash::increaseRequest          T 0x0817365a
//   - TaiwanCash::checkResultCode          T 0x0817366e
//   - TaiwanCash::purchase(7 参)           T 0x08173a00
//   - processBilling<PurchaseQuery>        W 0x08175770
//   - CIPGMgr::getBillingCash              W 0x0817a2ee（return *(this+0xec)）
//   - CheckBillingErrorCode                T 0x08146669（code==0 → true）
//
// 布局要点（ORIG）：
//   TaiwanCash = 0x420ac 字节：+0 uchar | +2 short | +4 int 请求计数 |
//               +8 MySQL(0x4208c) | +0x42094 Mutex(0x18)。
//   BillTransaction = 0xc 字节：+0 int | +4 longlong。
// ============================================================================

#include <queue>

#include "MySQL.h"
#include "GameTypes.h"  // Mutex（+0x42094 成员；processBilling 的 Guard<Mutex>）/ DynamicPool
#include "ThreadInterface.h"  // CIPGWorker 基类（ORIG CIPGWorkerC1 仅调 ThreadInterface 基类构造）

namespace Taiwan
{

// ---- Taiwan::BillTransaction —— 计费事务句柄（0xc 字节 POD）----
struct BillTransaction
{
    void init();            // ORIG 0x08175540（W）：清零 12 字节

    int m_field0;           // +0x00（init 清零；getResult 不写此字段）
    unsigned long long m_tranNo;  // +0x04（usp_purchase @out_tran 回传的事务号）
};

// ---- Taiwan::PurchaseQuery —— usp_purchase 存储过程查询 ----
class PurchaseQuery
{
public:
    PurchaseQuery(const char* account, const char* charac,
                  unsigned int price, unsigned int price2, long long uid);

    friend class TaiwanCash;  // purchase 直读 m_trans/m_resultCode（ORIG 同 TU 内联访问）


    bool setQuery(MySQL& db);    // ORIG 0x081745aa：call usp_purchase('%s','%s',%u,%u,@out_tran,@out_code)
    bool execQuery(MySQL& db);   // ORIG 0x081745f2：return db.exec(true)（processBilling 检查返回值）
    int getResult(MySQL& db);    // ORIG 0x0817460e：select @out_tran,@out_code；失败抛 int 错误码

private:
    const char* m_account;       // +0x00
    const char* m_charac;        // +0x04
    unsigned int m_price;        // +0x08（purchase 第 2 参，调用方恒传 99999）
    long long m_uid;             // +0x0c（拍卖物品唯一号）
    unsigned int m_price2;       // +0x14（实付金额）
    BillTransaction m_trans;     // +0x18（0xc）
    int m_resultCode;            // +0x24（@out_code）
};

// ---- Taiwan::TaiwanCash —— 台湾计费入口（含 MySQL 连接 + 串行锁）----
    // ---------------------------------------------------------------------------
    // 以下 8 个方法使用的存储过程 Query 类（ORIG Taiwan::xxxQuery）：权威定义由
    // TaiwanQueries.h 提供（TwQuery 批次），此处仅前置声明；本 TU 在拿到完整类型后
    // 显式实例化 processBilling<X>（ORIG 各实例为独立 W 符号）。各 Query 统一约定
    // 暴露：int m_resultCode（@out_code）；BillTransaction m_trans（@out_tran，
    // Gift/Charge/Purchase 族）；附加输出字段见 docs/class_func_reports/Taiwan__*Query/。
    // ---------------------------------------------------------------------------
    class BalanceQuery;
    class BalancePointQuery;
    class EntranceQuery;
    class GiftQuery;
    class CancelQuery;
    class ChargeQuery;
    class ConfirmQuery;
    class CeraPointQuery;

class TaiwanCash
{
public:
    TaiwanCash(unsigned char type, short field2);   // ORIG 0x08173608

    // ORIG T 0x08173686：m_mysql.init() 后 m_mysql.open(host, port, db, user, pass)。
    bool init(char* host, unsigned int port, char* db, char* user, char* pass);

    // ORIG 0x08173a00：purchase(uid, price, pay, account, charac, trans&, kind)。
    // 返回 0=成功；4=processBilling 失败；其余为存储过程 @out_code。
    int purchase(unsigned long long uid, int price, int pay,
                 const char* account, const char* charac,
                 BillTransaction& trans, short kind);

    bool checkResultCode(int code);                 // ORIG 0x0817366e：code==0
    void increaseRequest();                         // ORIG 0x0817365a：++m_requestCount

    // ---- 以下 9 个方法：ORIG Taiwan__TaiwanCash 报告逐函数转写（Wave5 仅ORIG批）----
    // ORIG T 0x08173766：BalanceQuery(account)；0=成功、4=processBilling 失败、
    // 其余为 @out_code；成功时 balance = m_balance。
    int getBalance(const char* account, int& balance);

    // ORIG T 0x0817381a：BalancePointQuery(account)；balance=m_balance、point=m_point。
    int getBalance(const char* account, int& balance, int& point);

    // ORIG T 0x081736ce：EntranceQuery()；ORIG 未写回 entrance 出参（汇编无 store，
    // 仅 trace 结果码）——忠实转写。参数名【推断】。
    int checkEntrance(const char* account, char& entrance);

    // ORIG T 0x08173b20：GiftQuery(account, sendCharac, recvCharac, price, pay, uid)；
    // giftMsg 参数未被 ORIG 使用；成功拷贝 query.m_trans → trans。
    int gift(unsigned long long uid, int price, int pay,
             const char* account, const char* sendCharac,
             const char* recvCharac, const char* giftMsg,
             BillTransaction& trans);

    // ORIG T 0x08173d1c：tranNo==0 直接返回 0；否则 CancelQuery(trans)。
    int cancel(const BillTransaction& trans);

    // ORIG T 0x08173df0：ChargeQuery(account=p1, p8, "", price=p4, type=(char)p2)；
    // p3/p5/p6/p7/uid 未被 ORIG 用于查询构造（按汇编忠实）。
    int charge(char* p1, unsigned char serviceType, unsigned char p3,
               unsigned int price, char* p5, char* p6, char* p7, char* p8,
               unsigned long long uid, BillTransaction& trans);

    // ORIG T 0x08173c48：tranNo==0 直接返回 0；否则 ConfirmQuery(trans)。
    int confirm(const BillTransaction& trans);

    // ORIG T 0x081738e6：PurchaseQuery(account, charac, price, pay, uid)；
    // 与上方 7 参版本构成重载；成功拷贝 query.m_trans → trans。
    int purchase(unsigned long long uid, int price, int pay,
                 const char* account, const char* charac,
                 BillTransaction& trans);

    // ORIG T 0x08173f0c：
    // CeraPointQuery(p1, p2, p4, p5, (uint)p8, p6, p7, p3)。
    int ceraPoint(char* p1, char* p2, char command, char* p4, char* p5,
                  char p6, char p7, int price);

    // ORIG 模板成员（实例 _ZN6Taiwan10TaiwanCash14processBillingINS_13PurchaseQueryEEEbRT_
    // W 0x08175770）：Guard<Mutex> → increaseRequest → setQuery/execQuery/getResult。
    template <class T> bool processBilling(T& query)
    {
        Guard<Mutex> guard(&m_mutex);
        increaseRequest();
        if (!query.setQuery(m_mysql))
            return false;
        if (!query.execQuery(m_mysql))
            return false;
        return query.getResult(m_mysql) != 0;  // getResult 失败路径以 int 异常上抛
    }

private:
    unsigned char m_type;         // +0x00
    short m_field2;               // +0x02
    int m_requestCount;           // +0x04
    MySQL m_mysql;                // +0x08（sizeof(MySQL)=0x4208c）
    Mutex m_mutex;                // +0x42094（sizeof=0x18）
};                                // sizeof = 0x420ac

}  // namespace Taiwan

namespace WongWork { namespace IPG {

// ---- WongWork::IPG::IIPGHelperInterface —— 接口基类（ORIG W ctor 0x082a7182；
//      vtable _ZTV 0x48 共 12 槽，其余虚函数属 CIPGHelper 批次，此处最小视图）。----
class IIPGHelperInterface
{
public:
    virtual ~IIPGHelperInterface() {}
};

// ---- WongWork::IPG::SIPGData —— IPG 请求数据（0x2dc）----
// 尺寸实证：DynamicPool<SIPGData>::MoreAlloc 节点步长 imul $0x2e4
// （0x081019c2），CNode = SIPGData + m_type(4) + m_inUse(1) → sizeof=0x2dc。
struct SIPGData
{
    char m_pad[0x2dc];
};

// ---- WongWork::IPG::CIPGMgr —— ORIG 权威布局（0xf0 字节）----
// 字段偏移实证：ctor 0x08101108（workers@0 清零、queue@0xc/0x34、
// DynamicPool<SIPGData>(1000)@0x5c、helper@0xb8、Mutex@0xbc/0xd4）、
// Initialize 0x08104494（count@4、param2@8、InitBillingCash 写 @0xec）、
// getBillingCash 0x0817a2ee（return *(this+0xec)）、AllocIPGData/PutIPGRequest
// 的 Guard<Mutex>（+0xbc/+0xd4）。
class CIPGMgr
{
public:
    // ---- CIPGWorker（0x10；Initialize 数组步长实证）----
    // ORIG CIPGWorkerC1 0x081053e2 仅调 ThreadInterface 基类构造；
    // SetIPGMgr 0x081053d4 写 *(this+0xc)。线程体 dispatch（ORIG T 0x08105276）
    // 属 WongWork__IPG__CIPGMgr__CIPGWorker 后续批次，当前回落 ThreadInterface
    // 空默认体（ThreadInterface.h 既定模式，非本批次范围）。
    class CIPGWorker : public ThreadInterface
    {
    public:
        void SetIPGMgr(CIPGMgr* mgr) { m_mgr = mgr; }
        CIPGMgr* m_mgr;   // +0x0c
    };

    CIPGMgr(IIPGHelperInterface* helper);   // ORIG W 0x08101108
    ~CIPGMgr();                             // ORIG W 0x08101222
    int Initialize(unsigned int workerCount, unsigned int param2);  // ORIG T 0x08104494
    Taiwan::TaiwanCash* getBillingCash();   // ORIG W 0x0817a2ee：return *(this+0xec)
    IIPGHelperInterface* GetIPGHelper() { return m_helper; }        // ORIG W 0x081053c6

private:
    CIPGWorker* m_workers;              // +0x000
    unsigned int m_workerCount;         // +0x004（Initialize 参数 1）
    unsigned int m_param2;              // +0x008（Initialize 参数 2）
    std::queue<SIPGData*> m_reqQueue;   // +0x00c（0x28；PutIPGRequest push /_GetIPGRequest pop）
    std::queue<SIPGData*> m_resQueue;   // +0x034（0x28）
    DynamicPool<SIPGData> m_pool;       // +0x05c（ctor 初始池 1000）
    IIPGHelperInterface* m_helper;      // +0x0b8
    Mutex m_allocMutex;                 // +0x0bc（0x18；AllocIPGData Guard 实证）
    Mutex m_reqMutex;                   // +0x0d4（0x18；PutIPGRequest Guard 实证）
    Taiwan::TaiwanCash* m_billingCash;  // +0x0ec（ORIG ctor 不初始化，仅 InitBillingCash 写入）
};                                      // sizeof = 0xf0

static_assert(sizeof(CIPGMgr) == 0xf0, "CIPGMgr ABI drift");
static_assert(sizeof(CIPGMgr::CIPGWorker) == 0x10, "CIPGWorker ABI drift");
static_assert(sizeof(SIPGData) == 0x2dc, "SIPGData ABI drift");

}}  // namespace WongWork::IPG

// ORIG 0x08146669：计费返回码检查（0 = 成功）。
bool CheckBillingErrorCode(int code);

// ORIG T 0x0814657c：创建/初始化全局计费句柄（new TaiwanCash(env 类型字节,1) 后
// 以 CEnvironment::m_dbConnInfo[DB_HANDLE_E] 连接信息 open；失败返回 false）。
bool InitBillingCash(Taiwan::TaiwanCash*& cash);

#endif  // DNF_GAME_TAIWAN_CASH_H_
