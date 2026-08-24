// ============================================================================
// df_game_r 还原 —— Taiwan 计费子系统实现（声明见 TaiwanCash.h）。
// 逐函数依据 docs/class_func_reports/Taiwan__TaiwanCash/{TaiwanCash,purchase_08173a00,
// 及 df_game_r objdump 实测（BillTransaction::init 0x08175540、increaseRequest
// 0x0817365a、CIPGMgr::getBillingCash 0x0817a2ee、CheckBillingErrorCode 0x08146669）。
// C++98 / -m32 / O0 基线。
// ============================================================================

#include "TaiwanCash.h"
#include <cstdio>
#include <cstring>

#include "CEnvironment.h"  // G_CEnvironment / STDBConnInfo / DB_HANDLE_E（InitBillingCash）
#include "LogManager.h"    // Initialize 失败路径 logFormat（ORIG IPGMgr.cpp 0x98 行文案）

namespace Taiwan
{

// ---- BillTransaction::init —— ORIG 0x08175540：movb $0,(%eax); movl $0,4/8(%eax)。----
void BillTransaction::init()
{
    m_field0 = 0;
    m_tranNo = 0;
}

// ---- PurchaseQuery::PurchaseQuery —— ORIG 0x0817454c。----
PurchaseQuery::PurchaseQuery(const char* account, const char* charac,
                             unsigned int price, unsigned int price2, long long uid)
{
    m_account = account;
    m_charac = charac;
    m_price = price;
    m_uid = uid;
    m_price2 = price2;
    m_resultCode = 0;
    m_trans.init();
}

// ---- setQuery —— ORIG 0x081745aa。----
bool PurchaseQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_purchase('%s', '%s', %u, %u, @out_tran, @out_code)",
                        m_account, m_charac, m_price, m_price2);
}
// ---- execQuery —— ORIG 0x081745f2。----
bool PurchaseQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ---- getResult —— ORIG 0x0817460e：任一步失败抛 int 错误码
//（0x187=set_query / 0x18a=exec / 0x18d=fetch / 400=get_ulonglong / 0x193=get_int）。----
int PurchaseQuery::getResult(MySQL& db)
{
    if (!db.set_query("select @out_tran, @out_code"))
        throw 0x187;
    if (!db.exec(true))
        throw 0x18a;
    if (!db.fetch())
        throw 0x18d;
    if (!db.get_ulonglong(0, m_trans.m_tranNo))
        throw 400;
    if (!db.get_int(1, m_resultCode))
        throw 0x193;
    return 1;
}

// ---- TaiwanCash::TaiwanCash —— ORIG 0x08173608：MySQL@+8 自动构造、Mutex@+0x42094
// 自动构造，随后 +4=0、+0=type、+2=field2。----
TaiwanCash::TaiwanCash(unsigned char type, short field2)
    : m_mysql(), m_mutex()
{
    m_requestCount = 0;
    m_type = type;
    m_field2 = field2;
}

// ---- init —— ORIG T 0x08173686：m_mysql.init()（返回值忽略）后
// m_mysql.open(host, port, db, user, pass)。----
bool TaiwanCash::init(char* host, unsigned int port,
                      char* db, char* user, char* pass)
{
    m_mysql.init();
    return m_mysql.open(host, port, db, user, pass);
}

// ---- increaseRequest —— ORIG 0x0817365a：++*(int*)(this+4)。----
void TaiwanCash::increaseRequest()
{
    m_requestCount += 1;
}

// ---- checkResultCode —— ORIG 0x0817366e。----
bool TaiwanCash::checkResultCode(int code)
{
    return code == 0;
}

// ---- purchase —— ORIG 0x08173a00：
//   PurchaseQuery(account, charac, price, pay, uid)
//   → processBilling 失败返回 4；
//   → checkResultCode(m_resultCode) 失败返回 m_resultCode；
//   → 成功把 query 内部事务（+0x18..0x23）拷给出参 trans 并 trace 后返回 0。----
int TaiwanCash::purchase(unsigned long long uid, int price, int pay,
                         const char* account, const char* charac,
                         BillTransaction& trans, short kind)
{
    (void)kind;  // ORIG 该重载未使用 kind 参数

    PurchaseQuery query(account, charac,
                        static_cast<unsigned int>(price),
                        static_cast<unsigned int>(pay),
                        static_cast<long long>(uid));
    if (!processBilling<PurchaseQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    trans.m_field0 = query.m_trans.m_field0;
    trans.m_tranNo = query.m_trans.m_tranNo;

    // ORIG：NumberToString(trans.m_tranNo) + cMyTrace("[Taiwan, Billing] PurchaseQuery
    // Account:%s, Charac:%s, Price:%d, Transaction:%s")。此处保留等价 trace 面，
    // 经由 cMyTrace（GameTypes.h 局部实现）输出。【推断：等价日志】
    {
        char tranBuf[32];
        std::snprintf(tranBuf, sizeof(tranBuf), "%llu",
                      static_cast<unsigned long long>(trans.m_tranNo));
        cMyTrace trace("purchase", 0x81, 0);
        trace("[Taiwan, Billing] PurchaseQuery Account:%s, Charac:%s, Price:%d, Transaction:%s",
              account, charac, pay, tranBuf);
    }
    return 0;
}

}  // namespace Taiwan

// explicit instantiation → ORIG W 符号 _ZN6Taiwan10TaiwanCash14processBillingINS_13PurchaseQueryEEEbRT_
template bool Taiwan::TaiwanCash::processBilling<Taiwan::PurchaseQuery>(
    Taiwan::PurchaseQuery&);

// ============================================================================
// WongWork::IPG::CIPGMgr —— 权威布局见 TaiwanCash.h；逐函数 ORIG 转写。
// 依据：docs/class_func_reports/WongWork__IPG__CIPGMgr/{Initialize,Finalize}.md
// 及 objdump 实测（ctor 0x08101108、dtor 0x08101222、GetIPGHelper 0x081053c6）。
// ============================================================================

// ---- CIPGMgr::CIPGMgr —— ORIG W 0x08101108：
//   *(this+0)=0；两个 queue<SIPGData*>（临时 deque move 构造，等价默认构造）
//   落 +0xc/+0x34；DynamicPool<SIPGData>(1000)@+0x5c；helper@+0xb8；
//   Mutex@+0xbc/+0xd4 自动构造。billingCash(+0xec) 不在此初始化（ORIG 同）。----
namespace WongWork { namespace IPG {

CIPGMgr::CIPGMgr(IIPGHelperInterface* helper)
    : m_workers(0),
      m_workerCount(0),
      m_param2(0),
      m_reqQueue(),
      m_resQueue(),
      m_pool(1000),
      m_helper(helper),
      m_allocMutex(),
      m_reqMutex()
{
}

// ---- CIPGMgr::~CIPGMgr —— ORIG W 0x08101222：逆序析构 mutex/pool/queue（成员自动）。----
CIPGMgr::~CIPGMgr()
{
}

// ---- CIPGMgr::Initialize —— ORIG T 0x08104494：
//   workers 已存在 → -1；count!=0 时 new CIPGWorker[count]，逐个
//   SetIPGMgr(this)+ThreadInterface::begin()；写 param2@+8、count@+4；
//   InitBillingCash(billingCash) 失败 → logFormat("IPGMgr.cpp",0x98) 后 -1。----
int CIPGMgr::Initialize(unsigned int workerCount, unsigned int param2)
{
    if (m_workers != 0)
    {
        return -1;
    }
    if (workerCount != 0)
    {
        m_workers = new CIPGWorker[workerCount];
        for (unsigned int i = 0; i < workerCount; ++i)
        {
            m_workers[i].SetIPGMgr(this);
            m_workers[i].begin();
        }
    }
    m_param2 = param2;
    m_workerCount = workerCount;

    if (!InitBillingCash(m_billingCash))
    {
        LogManager::logFormat(
            1, "IPGMgr.cpp",
            "int WongWork::IPG::CIPGMgr::Initialize(unsigned int, unsigned int)",
            0x98, "Global billing cash initialize failed.");
        return -1;
    }
    return 0;
}

Taiwan::TaiwanCash* CIPGMgr::getBillingCash()   // ORIG W 0x0817a2ee
{
    return m_billingCash;
}
 }}

// ---- InitBillingCash —— ORIG T 0x0814657c：
//   type = *(uchar*)(G_CEnvironment()+0x378)（= Server_Envir.m_serverGroup 低字节，
//   CEnvironment.h 布局 +0x198+0x1e0 实证）；new TaiwanCash(type, 1) 存入出参；
//   以 m_dbConnInfo[DB_HANDLE_E] 的连接信息 init(host, port, db, user, pass)；
//   出参为空或 init 失败 → false。----
bool InitBillingCash(Taiwan::TaiwanCash*& cash)
{
    CEnvironment* env = G_CEnvironment();
    unsigned char type = static_cast<unsigned char>(env->m_serverEnvir.m_serverGroup);

    // CEnvironment::m_dbConnInfo 为 private（权威布局 +0x68，ORIG lea 0x68(%eax)
    // @0x081465e4）；本 TU 非 CEnvironment 域，按工程既有裸偏移先例访问。
    typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DbConnMap;
    DbConnMap& connMap =
        *reinterpret_cast<DbConnMap*>(reinterpret_cast<char*>(env) + 0x68);
    STDBConnInfo& info = connMap[DB_HANDLE_E];

    if (cash == 0)
    {
        return false;
    }
    return cash->init(info.m_host, static_cast<unsigned int>(info.m_port),
                      info.m_db, info.m_user, info.m_pass);
}

// ---- CheckBillingErrorCode —— ORIG 0x08146669：code==0 → true。----
bool CheckBillingErrorCode(int code)
{
    return code == 0;
}
