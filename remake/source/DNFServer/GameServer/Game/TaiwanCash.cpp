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
#include "DNFFunctionLib.h"  // NumberToString(ull,int)（billing trace 直调）

// ---- InitBillingCash —— ORIG T 0x0814657c：
//   定义位置：置于 TaiwanCash::TaiwanCash 定义之前（P210 三态③：4.4 只见声明
//   ⇒ new-expression 保守布 EH landing pad；ORIG 0x81465b7..0x81465cc 有
//   `jmp` + operator delete landing 段实证）。
//   type = *(uchar*)(G_CEnvironment()+0x378)（= Server_Envir.m_serverGroup 低字节，
//   CEnvironment.h 布局 +0x198+0x1e0 实证）——ORIG 中保持 %esi 寄存器、无栈槽 ⇒
//   P0 判为右值内联，禁具名局部；G_CEnvironment() 亦临时直用（各次仅寄存器）。
//   new TaiwanCash(type, 1) 存入出参；以 m_dbConnInfo[DB_HANDLE_E] 的连接信息
//   init(host, port, db, user, pass)。
//   ORIG 反汇编尾部：出参判空走 0xd0 → 恒返回 1；init 结果按 0xd5/0xe0/0xe5
//   三态全部走 eax 寄存器（无栈槽 ⇒ P0：ghidra 的 bVar2 非具名局部），
//   即 `!(cash==0 || init==1)` 单表达式返回，勿拆 if 注入 bool 局部。----
bool InitBillingCash(Taiwan::TaiwanCash*& cash)
{
    // ORIG 0x0814657c：G_CEnvironment() 调两次（type 一次、map 访问一次），
    // 两次结果均为临时（寄存器直用，无栈槽）。
    cash = new Taiwan::TaiwanCash(
        static_cast<unsigned char>(
            G_CEnvironment()->m_serverEnvir.m_serverGroup),
        1);

    // ORIG 第二次 G_CEnvironment() 后直访 m_dbConnInfo[DB_HANDLE_E]（+0x68；
    // 禁 get_db_conn_info() accessor——ORIG 无该 call，P167 判据）。
    STDBConnInfo& info =
        G_CEnvironment()->m_dbConnInfo[DB_HANDLE_E];

    // init 返回 bool（0/1），ORIG 汇编 `xor $1,%eax` 即 `!init`；==1 省去。
    return !(cash == 0 ||
             cash->init(info.m_host, static_cast<unsigned int>(info.m_port),
                        info.m_db, info.m_user, info.m_pass));
}

namespace Taiwan
{

// ---- PurchaseQuery::PurchaseQuery —— ORIG 0x0817454c。----
// N76 封顶：uid 写入块 4.4 恒 LHS(this/ecx) 先物化、ORIG 4.1 恒 RHS(槽回读)先，
// 3 行时序差不可翻（exp/probe 见 GiftQuery 同判）。帧 0x28 已由 init 类内 inline 复原。
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
//（0x187=set_query / 0x18a=exec / 0x18d=fetch / 400=get_ulonglong / 0x193=get_int）。
// ORIG 含 catch：LogManager 记录异常后返回 0（行号 0x199）。----
bool PurchaseQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_tran, @out_code"))
            throw static_cast<int>(0x187);
        if (!db.exec(true))
            throw static_cast<int>(0x18a);
        if (!db.fetch())
            throw static_cast<int>(0x18d);
        if (!db.get_ulonglong(0, m_trans.m_tranNo))
            throw static_cast<int>(400);
        if (!db.get_int(1, m_resultCode))
            throw static_cast<int>(0x193);
        return 1;
    }
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x199,
            "[Taiwan, Billing] PurchaseQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
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
    if (code != 0)
        return false;
    return true;
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

    // [b27L8] ORIG 0x08173a00：12B 结构整体赋值 dest 缓存形（mov %edx,(%eax)
    // 三连）+ N2S 返回 %ebx 跨 ctor 保活（mov %eax,%ebx 两步）。
    trans = query.m_trans;

    // ORIG：NumberToString(trans.m_tranNo) + cMyTrace("[Taiwan, Billing] PurchaseQuery
    // Account:%s, Charac:%s, Price:%d, Transaction:%s")。此处保留等价 trace 面，
    // 经由 cMyTrace（GameTypes.h 局部实现）输出。【推断：等价日志】
    // ORIG 0x08173a00 直调 NumberToString(trans.m_tranNo,0)（禁 snprintf 替代）。
    {
        // [b28BM13 N96 清绑] register asm("ebx") 违反 P0；改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0x81, 0)(
            "[Taiwan, Billing] PurchaseQuery Account:%s, Charac:%s, Price:%d, Transaction:%s",
            account, charac, pay, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
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
            __PRETTY_FUNCTION__,
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

// DynamicPool<SIPGData>::CNode 特化 ctor（ORIG W 0x081021d6，声明见 TaiwanCash.h
// 全局作用域）：基类 value-init（POD 聚合清零）→ 4.4 -O0 发射与 ORIG 同形的
// `mov $0x0,%eax; mov $0xb7,%edx; rep stos`（探针 /tmp/bm3/p20 N1）；随后两个
// 成员各带 this 重读的函数体赋值（ORIG `mov 0x8(%ebp),%eax; movl $0x8f21,0x2dc`
// `/ movb $0,0x2e0` 形，P29 函数体序）。
DynamicPool<WongWork::IPG::SIPGData>::CNode::CNode()
    : WongWork::IPG::SIPGData()
{
    m_type = 0x8f21;
    m_inUse = 0;
}

// ---- CheckBillingErrorCode —— ORIG 0x08146669：code==0 → true。----
bool CheckBillingErrorCode(int code)
{
    if (code == 0)
        return true;
    return false;
}

// ============================================================================
// Wave5 仅ORIG批 —— TaiwanCash 新增 9 方法（追加块，不改上方既有实现）。
// 依据 docs/class_func_reports/Taiwan__TaiwanCash/{getBalance,getBalance_0817381a,
// checkEntrance,gift,cancel,charge,confirm,purchase,ceraPoint}.md 汇编逐条转写。
// Query 完整类型来自 TaiwanQueries.h（TwQuery 批次）；各 processBilling<X>
// 显式实例化 → ORIG 独立 W 符号 _ZN6Taiwan10TaiwanCash14processBillingINS_IXXXEEEbRT_。
// NumberToString(y,i) @0x08109098 以 snprintf 等价替代【推断：等价日志面，
// 沿用本 TU purchase 既有模式】。所有 SQL/存储过程文本在各 Query 归属批内。
// ============================================================================

#include "TaiwanQueries.h"

namespace Taiwan
{

// ---- getBalance(const char*, int&) —— ORIG T 0x08173766 ----
int TaiwanCash::getBalance(const char* account, int& balance)
{
    // BalanceQuery: m_balance@+0x04 / m_resultCode@+0x08
    BalanceQuery query(account);
    if (!processBilling<BalanceQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    balance = query.m_balance;
    {
        cMyTrace trace(__FUNCTION__, 0x46, 0);
        trace("[Taiwan, Billing] BalanceQuery Balance:%d, Result:%d",
              balance, query.m_resultCode);
    }
    return 0;
}

// ---- getBalance(const char*, int&, int&) —— ORIG T 0x0817381a ----
int TaiwanCash::getBalance(const char* account, int& balance, int& point)
{
    // BalancePointQuery: m_balance@+0x04 / m_point@+0x08 / m_resultCode@+0x0c
    BalancePointQuery query(account);
    if (!processBilling<BalancePointQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    balance = query.m_balance;
    point = query.m_point;
    {
        cMyTrace trace(__FUNCTION__, 0x5a, 0);
        trace("[Taiwan, Billing] BalancePointQuery Balance:%d, Point:%d, Result:%d",
              balance, point, query.m_resultCode);
    }
    return 0;
}

// ---- checkEntrance(const char*, char&) —— ORIG T 0x081736ce ----
int TaiwanCash::checkEntrance(const char* account, char& entrance)
{
    (void)entrance;  // ORIG 未写回该出参（汇编无 store，仅 trace 结果码）——忠实转写

    EntranceQuery query;
    // EntranceQuery: m_resultCode@+0x04
    if (!processBilling<EntranceQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    {
        cMyTrace trace(__FUNCTION__, 0x34, 0);
        trace("[Taiwan, Billing] EntranceQuery Entrance:%d", query.m_resultCode);
    }
    return 0;
}

// ---- gift —— ORIG T 0x08173b20 ----
int TaiwanCash::gift(unsigned long long uid, int price, int pay,
                     const char* account, const char* sendCharac,
                     const char* recvCharac, const char* giftMsg,
                     BillTransaction& trans)
{
    (void)giftMsg;  // ORIG 未把该参传入 GiftQuery——忠实转写


    // GiftQuery(account, sendCharac, recvCharac, uint(price=p2), uint(pay=p3), uid)；
    // m_resultCode@+0x1c / m_trans@+0x20（asm: esp+0x10=*(ebp+0x14)、esp+0x14=*(ebp+0x18)）。
    GiftQuery query(account, sendCharac, recvCharac,
                    static_cast<unsigned int>(price),
                    static_cast<unsigned int>(pay),
                    static_cast<long long>(uid));
    if (!processBilling<GiftQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b27L8] ORIG 0x08173b20：12B 结构整体赋值 dest 缓存形 + NumberToString
    // 结果跨 ctor %ebx 保活。
    trans = query.m_trans;

    // ORIG +0x0bf 直调 NumberToString(m_tranNo,0)（禁 snprintf 替代，P167 判据）。
    {
        // [b28BM13 N96 清绑] register asm("ebx") 违反 P0；改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0x95, 0)(
            "[Taiwan, Billing] GiftQuery Account:%s, SendCharac:%s, RecvCharac:%s, Price:%d, Transaction:%s",
            account, sendCharac, recvCharac, pay, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
    }
    return 0;
}

// ---- cancel(const BillTransaction&) —— ORIG T 0x08173d1c ----
int TaiwanCash::cancel(const BillTransaction& trans)
{
    // tranNo 高低双字均为 0 → 直接返回 0（ORIG test lo|hi）。
    if ((static_cast<int>(trans.m_tranNo & 0xffffffffULL) |
         static_cast<int>(trans.m_tranNo >> 32)) == 0)
        return 0;

    CancelQuery query(trans);
    // CancelQuery: m_resultCode@+0x00
    if (!processBilling<CancelQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    {
        char* tranStr = NumberToString(
            static_cast<unsigned long long>(trans.m_tranNo), 0);
        cMyTrace trace(__FUNCTION__, 0xb9, 0);
        trace("[Taiwan, Billing] CancelQuery Transaction:%s", tranStr);
    }
    return 0;
}

// ---- charge —— ORIG T 0x08173df0 ----
int TaiwanCash::charge(char* p1, unsigned char serviceType, unsigned char p3,
                       unsigned int price, char* p5, char* p6, char* p7, char* p8,
                       unsigned long long uid, BillTransaction& trans)
{
    (void)p3;   // ORIG 未用于 ChargeQuery 构造——按汇编忠实
    (void)p5; (void)p6; (void)p7;
    (void)uid;  // 同上

    // ChargeQuery(p1, p8, "", price, (char)serviceType)；
    // m_resultCode@+0x14 / m_trans@+0x18。
    ChargeQuery query(p1, p8, "", price, static_cast<char>(serviceType));
    if (!processBilling<ChargeQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b27L8] ORIG 0x08173972 为 12B 结构整体赋值形（dest 缓存 + 3 连 mov，
    // BillTransaction 平凡 POD）；逐字段两行赋值出 dest 重载形（P195/P206 族）。
    trans = query.m_trans;

    {
        // [b28BM13 N96 清绑] ORIG 0x081739a3: N2S 返回经 %ebx 跨 cMyTrace ctor
        // 保活（mov %eax,%ebx → mov %ebx,0x14(%esp) 两步）。register 绑定违反
        // P0，改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0xce, 0)(
            "[Taiwan, Billing] ChargeQuery Account:%s, Price:%d, Transaction:%s",
            p1, price, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
    }
    return 0;
}

// ---- confirm(const BillTransaction&) —— ORIG T 0x08173c48 ----
int TaiwanCash::confirm(const BillTransaction& trans)
{
    // tranNo 高低双字均为 0 → 直接返回 0（ORIG test lo|hi）。
    if ((static_cast<int>(trans.m_tranNo & 0xffffffffULL) |
         static_cast<int>(trans.m_tranNo >> 32)) == 0)
        return 0;

    ConfirmQuery query(trans);
    // ConfirmQuery: m_resultCode@+0x00
    if (!processBilling<ConfirmQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    {
        char* tranStr = NumberToString(
            static_cast<unsigned long long>(trans.m_tranNo), 0);
        cMyTrace trace(__FUNCTION__, 0xa7, 0);
        trace("[Taiwan, Billing] ConfirmQuery Transaction:%s", tranStr);
    }
    return 0;
}

// ---- purchase(6 参重载) —— ORIG T 0x081738e6 ----
int TaiwanCash::purchase(unsigned long long uid, int price, int pay,
                         const char* account, const char* charac,
                         BillTransaction& trans)
{
    PurchaseQuery query(account, charac,
                        static_cast<unsigned int>(price),
                        static_cast<unsigned int>(pay),
                        static_cast<long long>(uid));
    if (!processBilling<PurchaseQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b27L8] ORIG 0x081738e6：12B 结构整体赋值 dest 缓存形 + N2S %ebx 保活。
    trans = query.m_trans;

    {
        // [b28BM13 N96 清绑] register asm("ebx") 违反 P0；改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0x6e, 0)(
            "[Taiwan, Billing] PurchaseQuery Account:%s, Charac:%s, Price:%d, Transaction:%s",
            account, charac, pay, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
    }
    return 0;
}

// ---- ceraPoint —— ORIG T 0x08173f0c ----
int TaiwanCash::ceraPoint(char* p1, char* p2, char command, char* p4, char* p5,
                          char p6, char p7, int price)
{
    // CeraPointQuery(p1, p2, p4, p5, (uint)price, p6, p7, command)；
    // m_resultCode@+0x18。
    CeraPointQuery query(p1, p2, p4, p5,
                         static_cast<unsigned int>(price),
                         p6, p7, command);
    if (!processBilling<CeraPointQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    {
        // [b28BM13] P227 全表达式临时链（原具名 trace 对象与 query 槽向相反，
        // N93/N82附；ORIG query@-0x34 trace@-0x18）。
        cMyTrace(__FUNCTION__, 0xe0, 0)(
            "[Taiwan, Billing] CeraPointQuery Command:%c, Price:%d",
            static_cast<int>(command), static_cast<int>(price));
    }
    return 0;
}

}  // namespace Taiwan

// ---- processBilling<X> 显式实例化 → ORIG W 符号 ----
template bool Taiwan::TaiwanCash::processBilling<Taiwan::BalanceQuery>(
    Taiwan::BalanceQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::BalancePointQuery>(
    Taiwan::BalancePointQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::EntranceQuery>(
    Taiwan::EntranceQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::GiftQuery>(
    Taiwan::GiftQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::CancelQuery>(
    Taiwan::CancelQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::ChargeQuery>(
    Taiwan::ChargeQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::ConfirmQuery>(
    Taiwan::ConfirmQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::CeraPointQuery>(
    Taiwan::CeraPointQuery&);
