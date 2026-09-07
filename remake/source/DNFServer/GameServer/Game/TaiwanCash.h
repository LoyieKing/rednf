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
#include <string.h>   // memset（SIPGData 清零构造）

#include "MySQL.h"
#include "GameTypes.h"  // Mutex（+0x42094 成员；processBilling 的 Guard<Mutex>）/ DynamicPool
#include "ThreadInterface.h"  // CIPGWorker 基类（ORIG CIPGWorkerC1 仅调 ThreadInterface 基类构造）


class CUser;  // WongWork::IPG::CIPGHelper 声明所需（完整类型见 CUser.h）
namespace Taiwan
{

// ---- Taiwan::BillTransaction —— 计费事务句柄（0xc 字节 POD）----
struct BillTransaction
{
    // ORIG W 0x08175540：类内 inline（调用方 PurchaseQuery/GiftQuery/setQuery
    // ctor 帧 0x28 依此形；callee 类外定义时 4.4 调用方帧塌缩，NL26 probeH 实证）。
    void init()
    {
        *(char*)this = 0;
        m_tranNo = 0;
    }

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
    bool getResult(MySQL& db);   // ORIG 0x0817460e：select @out_tran,@out_code；失败抛 int 错误码

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
        // 嵌套正向形：4.4 出 test;je 链 + 共享后置失败块（ORIG 0x08173f0c 同形）。
        if (query.setQuery(m_mysql)) {
            if (query.execQuery(m_mysql)) {
                return query.getResult(m_mysql);  // ORIG 透传形（bool 化后 mov %eax,%ebx）
            }
        }
        return false;
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


// ---- WongWork::IPG::IPG_DATA_TYPE —— _SetIPGData / IPGRequest 类型键 ----
// 数值实证：IPGInput→0、IPGInputCancel→1、IPGOutput(Simulation)→2、
// IPGOutputCancel→3、IPGQuery→4、IPGOutputConfirm→5、Input/OutputPoint→6。
// 枚举器名【推断】；数值以各调用点汇编为准（ORIG 宽度=int）。
enum IPG_DATA_TYPE
{
    IPG_TYPE_INPUT          = 0,
    IPG_TYPE_INPUT_CANCEL   = 1,
    IPG_TYPE_OUTPUT         = 2,
    IPG_TYPE_OUTPUT_CANCEL  = 3,
    IPG_TYPE_QUERY          = 4,
    IPG_TYPE_OUTPUT_CONFIRM = 5,
    IPG_TYPE_POINT          = 6
};

// ---- stIPGStatus_t —— getIPGStatus 出参 ----
// ORIG：ctor W 0x080ee3de 清零 +0/+4/+8；CIPGMgr::getIPGStatus W 0x081010ae
// 依次写 pool Remain→+0、pool Size→+4、reqQueue size→+8。
// 字段名【推断】，偏移/写序实证；sizeof 仅建模可证实的 0xc。
struct stIPGStatus_t
{
    unsigned int m_field00;  // +0x00（pool Remain）
    unsigned int m_field04;  // +0x04（pool Size）
    unsigned int m_field08;  // +0x08（reqQueue size）

    stIPGStatus_t();         // ORIG W 0x080ee3de（C1=C2 同址）
};

// ---- WongWork::IPG::IIPGHelperInterface —— IPG 回调纯虚接口 ----
// dispatch 的 +0x34 调用即 vptr+0x34 → slot11 IPGQuery（0x08100790）。
struct SIPGData;   // 前置：IIPGHelperInterface 槽参仅用指针（完整定义见下）
class CIPGMgr;     // 前置：slot15 GetIPGMgr 返回类型（完整定义见下）
class IIPGHelperInterface
{
public:
    virtual void IPGInput(CUser*, const char*, const char*, const char*,          // slot0 T 0x080ffb0a
                          const char*, const char*, void*) = 0;
    virtual void IPGInput(CUser*, unsigned char, unsigned int, const char*,       // slot1 T 0x080ffca4
                          const char*, const char*, const char*, const char*, void*) = 0;
    virtual void IPGInputCancel(CUser*, const char*, const char*, const char*,    // slot2 T 0x080ffea8
                                const char*, const char*, const char*, void*) = 0;
    virtual void IPGInputPoint(CUser*, unsigned int, char, char, void*) = 0;      // slot3 T 0x080fffc0
    virtual void IPGOutputPoint(CUser*, const char*, const char*, const char*,    // slot4 T 0x081000f6
                                const char*, const char*, void*) = 0;
    virtual void IPGOutput(CUser*, const char*, const char*, const char*,         // slot5 T 0x0810022e
                           const char*, const char*, void*) = 0;
    virtual SIPGData* IPGOutputSimulation(CUser*, const char*, const char*,       // slot6 T 0x0810034e
                                          const char*, const char*, const char*, void*) = 0;
    virtual void IPGOutputCancel(CUser*, const char*, const char*, const char*,   // slot7 T 0x081004de
                                 const char*, const char*, const char*, void*,
                                 Taiwan::BillTransaction*) = 0;
    virtual void IPGOutputConfirm(CUser*, const char*, const char*, const char*,  // slot8 T 0x08100cd2
                                  const char*, const char*, const char*, void*,
                                  Taiwan::BillTransaction*) = 0;
    virtual void IPGOutputCancel(SIPGData*, SIPGData*) = 0;                       // slot9  T 0x08100616
    virtual void IPGOutputConfirm(SIPGData*, SIPGData*) = 0;                      // slot10 T 0x08100e0a
    virtual void IPGQuery(CUser*) = 0;                                            // slot11(+0x34) T 0x08100790
    virtual void IPGRequest(SIPGData*) = 0;                                       // slot12(+0x38) T 0x081007e8
    virtual void IPGResult(SIPGData*, SIPGData*) = 0;                             // slot13(+0x3c) T 0x0810085e
    virtual void getIPGStatus(stIPGStatus_t&) = 0;                                // slot14 T 0x080ffae0
    virtual CIPGMgr* GetIPGMgr() = 0;                                             // slot15 W 0x081010a2
};


// ---- WongWork::IPG::SIPGData —— IPG 请求数据（0x2dc）----
// 尺寸实证：DynamicPool<SIPGData>::MoreAlloc 节点步长 imul $0x2e4
// （0x081019c2），CNode = SIPGData + m_type(4) + m_inUse(1) → sizeof=0x2dc。
//
// 字段化（§8.3）：m_pad[0x2dc] 已按 ORIG 汇编逐偏移拆为具名字段；宽度/偏移证据
// 来自 docs/class_func_reports/WongWork__IPG__{CIPGHelper,CIPGMgr,
// CIPGMgr__CIPGWorker}/ 各报告 ## 汇编段（关键指令地址见字段行注）。
// +0x69 之后的载荷区在 ORIG 中按 IPG 命令族复用同一字节区：Input / InputCancel /
// Output(+Simulation) / OutputCancel+Confirm / Point / Query 应答六个视图的字段
// 边界互不兼容（同族内字段首尾严格相接，跨族互相覆盖），故以 union 表达；
// 每个视图 sizeof 均为 0x26f（+0x69..+0x2d7）。视图内 ORIG 拷贝长度常大于字段
// 长度（不安全 strncpy/sprintf，属 ORIG 原样行为），消费侧保持原长度不做“修正”。
#pragma pack(push, 1)

// +0x69..：IPGInput / IPGInput(charge) 族视图
// 证据 IPGInput.md：080ffb38 `lea 0xdd`+0xb / 080ffb58 `lea 0xe8`+0xc /
// 080ffb78 `lea 0xf4`+0x14 / 080ffb9e `lea 0x108`+0x64 / 080ffbc4 `lea 0x16c`+0x64 /
// 080ffbef `add $0x1d0`+0x32 / 080ffc17 `add $0x202`+0xe / 080ffc3f `add $0x210`+0x3c /
// 080ffc5c `add $0x24c` memset 7；IPGInput_080ffca4.md：080ffe76
// `mov %edx,0x98(%eax)`（4B 价格）、080ffe6a `mov %dl,0x26d(%eax)`（1B）。
struct SIPGDataInputView
{
    char m_pad69[0x2f];          // +0x69（本族未访问）
    unsigned int m_price;        // +0x98（IPGInput 9 参 price）
    char m_pad9C[0x41];          // +0x9c
    char m_argDD[0xb];           // +0xdd（strncpy n=0xb）
    char m_argE8[0xc];           // +0xe8（strncpy n=0xc）
    char m_argF4[0x14];          // +0xf4（strncpy n=0x14）
    char m_arg108[0x64];         // +0x108（strncpy n=0x64，实参判空才写）
    char m_arg16C[0x64];         // +0x16c（strncpy n=0x64，实参判空才写）
    char m_userName[0x32];       // +0x1d0（GetUserName，strncpy n=0x32）
    char m_ssn[0xe];             // +0x202（GetSsnString，strncpy n=0xe）
    char m_email[0x3c];          // +0x210（GetUserEMail，strncpy n=0x3c）
    char m_zero24C[7];           // +0x24c（ORIG memset(d+0x24c,0,7)）
    char m_pad253[0x1a];         // +0x253
    unsigned char m_serviceType; // +0x26d（`mov %dl,0x26d` 1B；charge 第 2 参）
    char m_pad26E[0x6a];         // +0x26e
};

// +0x69..：IPGInputCancel 视图
// 证据 IPGInputCancel.md：080ffed6 `add $0x69`+0x15 / 080ffef3 `add $0x7e`+0xb /
// 080fff10 `add $0x89`+0xc / 080fff2f `add $0x95`+0x14 / 080fff54 `add $0xa9`+0x64 /
// 080fff79 `add $0x10d`+0x64。
struct SIPGDataInputCancelView
{
    char m_arg69[0x15];          // +0x69
    char m_arg7E[0xb];           // +0x7e
    char m_arg89[0xc];           // +0x89
    char m_arg95[0x14];          // +0x95
    char m_argA9[0x64];          // +0xa9
    char m_arg10D[0x64];         // +0x10d
    char m_pad171[0x167];        // +0x171
};

// +0x69..：IPGOutput / IPGOutputSimulation / ProcIPGSyncForTaiwan case2 视图
// 证据 IPGOutput.md：0810025c `add $0x69`+8 / 08100279 `add $0x71`+0xc /
// 08100296 `add $0x7d`+0x14 / 081002b9 `add $0x91`+0x64 / 081002de `add $0xf5`+0x64 /
// 08100308 `add $0x1e8`+0x3a；ProcIPGSyncForTaiwan.md case2：08104ae6 atoi(d+0x71)、
// 08104af6 atoi(d+0x69)、08104cf4/08104cfd/08104d06 三条 `mov %edx,0x1dc/0x1e0/0x1e4`、
// 08104d0f `add $0x173` sprintf、08104d2e `add $0x177` memcpy n=1。
struct SIPGDataOutputView
{
    char m_price[8];             // +0x69（IPGOutput p2 n=8；case2 atoi 入参）
    char m_pay[0xc];             // +0x71（p3 n=0xc；case2 atoi 入参）
    char m_arg7D[0x14];          // +0x7d（p4 n=0x14）
    char m_desc91[0x64];         // +0x91（p5 n=0x64，判空才写）
    char m_descF5[0x64];         // +0xf5（p6 n=0x64，判空才写）
    char m_pad159[0x1a];         // +0x159
    char m_resultCode[4];        // +0x173（case2/case6 sprintf "%d"；Sim memcpy "0",2）
    char m_message[0x65];        // +0x177（case2 memcpy n=1；case6 strncpy "cera point",100）
    unsigned int m_tranField0;   // +0x1dc（← 局部 BillTransaction.m_field0）
    unsigned long long m_tranNo; // +0x1e0（← 局部 tran.m_tranNo；0x1e0/0x1e4 两 dword）
    char m_recvCharac[0x3a];     // +0x1e8（getCurCharacName n=0x3a；case2 purchase 末串）
    char m_pad222[0xb6];         // +0x222
};

// +0x69..：IPGOutputCancel / IPGOutputConfirm 族视图（9 参写入 + SIPGData 重载搬运）
// 证据 IPGOutputCancel.md：0810050c `add $0x69`+0x15 / 08100529 `add $0x7e`+8 /
// 08100543 `add $0x86`+0xc / 08100565 `add $0x92`+0x14 / 0810058a `add $0xa6`+0x64 /
// 081005af `add $0x10a`+0x64 / 081005df/081005e8/081005f1（tran → 0x1f0/0x1f4/0x1f8）；
// IPGOutputCancel_08100616.md：08100650 `add $0x16e`（响应块源串 n=0x15）、
// 08100745/08100751/0810075d（响应块 tran 三 dword）。
struct SIPGDataCancelView
{
    char m_arg69[0x15];          // +0x69
    char m_arg7E[8];             // +0x7e
    char m_arg86[0xc];           // +0x86
    char m_arg92[0x14];          // +0x92
    char m_argA6[0x64];          // +0xa6
    char m_arg10A[0x64];         // +0x10a
    char m_srcMsg16E[0x15];      // +0x16e（响应块被读侧：strncpy n=0x15 的源）
    char m_pad183[0x59];         // +0x183
    unsigned int m_resTranField0;   // +0x1dc（响应块 tran.m_field0）
    unsigned long long m_resTranNo; // +0x1e0（响应块 tran.m_tranNo，0x1e0/0x1e4）
    char m_pad1E8[8];            // +0x1e8
    unsigned int m_tranField0;   // +0x1f0（`mov %ecx,0x1f0(%eax)`）
    unsigned long long m_tranNo; // +0x1f4（0x1f4/0x1f8；case3/case5 读回 BillTransaction）
    char m_pad1FC[0xdc];         // +0x1fc
};

// +0x69..：Point 族视图（IPGInputPoint / IPGOutputPoint / case6 ceraPoint）
// 证据 IPGInputPoint.md：08100069 `add $0x8c`+8 / 08100089 `add $0x94`+0x15 /
// 081000ac `mov %edx,0xac(%eax)` 4B / 081000b5 `movb $0x41,0xb0` / 081000c3
// `mov %dl,0xb1` / 081000d0 `mov %dl,0xb2`；ProcIPGSyncForTaiwan.md case6：
// 08104f55 `mov 0xac(%eax)`、08104f60 `movzbl 0xb2`、08104f6d `movzbl 0xb1`、
// 08104f93 `movzbl 0xb0`（08105061 `cmp $0x41,%al`）、08104ff6 `add $0xb3` sprintf。
struct SIPGDataPointView
{
    char m_pad69[0x23];          // +0x69（+0x64 处 n=0x28 的 strncpy 覆盖本区前部【ORIG 越界】）
    char m_itemNo[8];            // +0x8c（strncpy n=8；ceraPoint p4）
    char m_arg94[0x18];          // +0x94（strncpy n=0x15；ceraPoint p5；扩到 0x18 以衔接 +0xac）
    unsigned int m_point;        // +0xac（`mov %edx,0xac(%eax)` 4B）
    unsigned char m_command;     // +0xb0（`movb $0x41`/`$0x55`；case6 `movzbl 0xb0` 比 'A'）
    unsigned char m_cmdA;        // +0xb1（`mov %dl,0xb1`；ceraPoint p6）
    unsigned char m_cmdB;        // +0xb2（`mov %dl,0xb2`；ceraPoint p7）
    char m_reasonB3[4];          // +0xb3（case6 失败 sprintf "%d" rc）
    char m_padB7[0xbc];          // +0xb7
    char m_resultCode[4];        // +0x173（case6 sprintf "%d" rc）
    char m_message[0x65];        // +0x177（case6 strncpy "cera point",100）
    char m_pad1DC[0xfc];         // +0x1dc
};

// +0x69..：Query 应答视图（case4 写侧 / ItemVendingMachine::ProcessIPG_ResultQuery 读侧）
// 证据 ProcIPGSyncForTaiwan.md case4：08104a65 `add $0x69` sprintf point、
// 08104a9e `add $0xb1` sprintf rc、08104a82 `add $0xb5` sprintf bal；
// 消费侧 docs/class_func_reports/ItemVendingMachine/ProcessIPG_ResultQuery.md
// （ORIG 0x081784d4）以 +0x69/0x75/0x81/0x8d/0x99/0xa5 六个 0xc 步进槽 atoi 求和。
struct SIPGDataQueryView
{
    char m_ceraValues[6][0xc];   // +0x69..+0xb4（六项 cera 数值，0xc 步进）
    char m_retCode[4];           // +0xb1（atoi 返回码 / sprintf rc）
    char m_ceraPoint[0xc];       // +0xb5（atoi cera point / sprintf bal）
    char m_padC1[0x217];         // +0xc1
};

// 载荷命令族 union（+0x69 起，0x26f 字节）
union SIPGDataCmdView
{
    SIPGDataInputView m_input;
    SIPGDataInputCancelView m_inputCancel;
    SIPGDataOutputView m_output;
    SIPGDataCancelView m_cancel;
    SIPGDataPointView m_point;
    SIPGDataQueryView m_query;
    char m_raw[0x26f];           // 未建模偏移兜底（整区搬运/清零）
};

struct SIPGData
{
    // ORIG DynamicPool<SIPGData>::CNode::C1（0x081019xx）以 rep stos 清零
    // 整个 0x2dc 节点体；SIPGData 无字段级默认值，由该 memset 统一清零。
    SIPGData() { memset(this, 0, sizeof(*this)); }

    unsigned int m_recordLen;    // +0x00（_SetIPGData 08100ada `mov %edx,(%eax)` ← m_dataTableA[type]；Sim 用作 memcpy 长度）
    char m_envCode[0x14];        // +0x04（strncpy 自 CIPGHelper+0x24，n=0x14；语义=环境/站点码【推断】）
    unsigned short m_productCode; // +0x18（08100b0e `mov %dx,0x18(%eax)` ← m_dataTableB[type]，2B）
    char m_pad1A[2];             // +0x1a（ORIG 无访问；m_type 实证在 +0x1c）
    int m_type;                  // +0x1c（IPG_DATA_TYPE；081048dc `mov 0x1c(%eax),%eax` switch 键）
    unsigned int m_uid;          // +0x20（08100b22 `mov %edx,0x20(%eax)` = CUser::GetUID；Confirm 重载写 0xffffffff）
    unsigned char m_notifyFlag;  // +0x24（`mov %dl,0x24(%eax)` 直存 flag；IPGResult 081009c5 `movzbl 0x24` 决定入队/回收）
    char m_pad25[3];             // +0x25（ORIG 无访问）
    char m_account[0x14];        // +0x28（snprintf/sprintf 账号；case2 strcpy / case6 strncpy ""）
    char m_accName[0x14];        // +0x3c（get_acc_name strncpy n=0x28【ORIG 越界】；getBalance/purchase/ceraPoint 入参）
    char m_characName[0x14];     // +0x50（getCurCharacName strncpy n=0x1e；charge 的 charac 入参）
    char m_gameCode[5];          // +0x64（_SetIPGData memcpy "DUNF",5；case4 strncpy n=5；Point 族以 n=0x28 覆写【ORIG 越界】）
    SIPGDataCmdView m_cmd;       // +0x69（命令族复用区，见上六个视图）
    void* m_callback;            // +0x2d8（`mov %edx,0x2d8(%eax)` 存调用方 callback；IPGQuery 081007be `movl $0x0,0x2d8`）
};

#pragma pack(pop)

static_assert(sizeof(SIPGDataInputView) == 0x26f, "SIPGData input view drift");
static_assert(sizeof(SIPGDataInputCancelView) == 0x26f, "SIPGData input-cancel view drift");
static_assert(sizeof(SIPGDataOutputView) == 0x26f, "SIPGData output view drift");
static_assert(sizeof(SIPGDataCancelView) == 0x26f, "SIPGData cancel view drift");
static_assert(sizeof(SIPGDataPointView) == 0x26f, "SIPGData point view drift");
static_assert(sizeof(SIPGDataQueryView) == 0x26f, "SIPGData query view drift");
static_assert(sizeof(SIPGDataCmdView) == 0x26f, "SIPGData cmd union drift");


class CIPGMgr;   // 前置：CIPGHelper 成员/返回类型引用（完整定义见下）
// ---- WongWork::IPG::CIPGHelper —— IPG 计费门面（ORIG W C1 0x082a7190 / D1 0x082a71b6，
//      定义属其它批次，此处仅声明成员与布局，不给假生命周期）----
class CIPGHelper : public IIPGHelperInterface
{
public:
    // 生命周期声明（未还原定义；勿在本批给空体冒充 ORIG 符号）。
    CIPGHelper();    // ORIG W 0x082a7190
    ~CIPGHelper();   // ORIG W 0x082a71b6（D1=D2 同址）

    int Initialize(unsigned int param1, unsigned int param2);          // ORIG T 0x08100b3e
    void Finalize();                                                   // ORIG T 0x08100c92
    void getIPGStatus(stIPGStatus_t& status);                          // ORIG T 0x080ffae0
    CIPGMgr* GetIPGMgr();                                              // ORIG W 0x081010a2
    SIPGData* _SetIPGData(CUser* user, IPG_DATA_TYPE type, bool flag); // ORIG T 0x08100a2e
    void IPGRequest(SIPGData* data);                                   // ORIG T 0x081007e8
    void IPGQuery(CUser* user);                                        // ORIG T 0x08100790
    void IPGInput(CUser*, const char*, const char*, const char*,       // ORIG T 0x080ffb0a
                  const char*, const char*, void*);
    void IPGInput(CUser*, unsigned char serviceType, unsigned int price,// ORIG T 0x080ffca4
                  const char*, const char*, const char*, const char*, const char*, void*);
    void IPGInputCancel(CUser*, const char*, const char*, const char*, // ORIG T 0x080ffea8
                        const char*, const char*, const char*, void*);
    void IPGInputPoint(CUser*, unsigned int point, char a, char b, void*); // ORIG T 0x080fffc0
    void IPGOutput(CUser*, const char*, const char*, const char*,      // ORIG T 0x0810022e
                   const char*, const char*, void*);
    void IPGOutputPoint(CUser*, const char*, const char*, const char*, // ORIG T 0x081000f6
                        const char*, const char*, void*);
    SIPGData* IPGOutputSimulation(CUser*, const char*, const char*,    // ORIG T 0x0810034e
                                  const char*, const char*, const char*, void*);
    void IPGOutputCancel(CUser*, const char*, const char*, const char*,// ORIG T 0x081004de
                         const char*, const char*, const char*, void*,
                         Taiwan::BillTransaction*);
    void IPGOutputCancel(SIPGData* req, SIPGData* res);                // ORIG T 0x08100616
    void IPGOutputConfirm(CUser*, const char*, const char*, const char*,// ORIG T 0x08100cd2
                          const char*, const char*, const char*, void*,
                          Taiwan::BillTransaction*);
    void IPGOutputConfirm(SIPGData* req, SIPGData* res);               // ORIG T 0x08100e0a
    void IPGResult(SIPGData* req, SIPGData* res);                      // ORIG T 0x0810085e

private:
    // 布局实证（Initialize/IPGRequest/_SetIPGData 访问序）；字段名【推断】。
    CIPGMgr* m_mgr;                // +0x04
    unsigned int m_dataTableA[7];  // +0x08..+0x23（Initialize 写常量表）
    char m_str24[0x14];            // +0x24（Initialize 自 environment+0x2b8 复制）
    unsigned int m_dataTableB[7];  // +0x38..+0x53（Initialize 写常量表，含 +0x50=0）
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
    // SetIPGMgr 0x081053d4 写 *(this+0xc)。
    class CIPGWorker : public ThreadInterface
    {
    public:
        void SetIPGMgr(CIPGMgr* mgr) { m_mgr = mgr; }   // ORIG W 0x081053d4
        virtual void dispatch(void* arg);               // ORIG T 0x08105276（线程体）
        SIPGData* ProcIPGSync(SIPGData* data);          // ORIG T 0x08105258
        SIPGData* ProcIPGSyncForChina(SIPGData* data);  // ORIG T 0x081048b4
        SIPGData* ProcIPGSyncForTaiwan(SIPGData* data); // ORIG T 0x081048c6

        // CIPGNetwork：ORIG 仅见 ConnectTo（T 0x081043e0，socket+connect）。
        class CIPGNetwork
        {
        public:
            int ConnectTo(const char* ip, unsigned short port);
            int m_sock;   // +0x00（ConnectTo 写入 socket fd）
        };

        CIPGMgr* m_mgr;   // +0x0c
    };

    CIPGMgr(IIPGHelperInterface* helper);   // ORIG W 0x08101108
    ~CIPGMgr();                             // ORIG W 0x08101222
    int Initialize(unsigned int workerCount, unsigned int param2);  // ORIG T 0x08104494
    Taiwan::TaiwanCash* getBillingCash();   // ORIG W 0x0817a2ee：return *(this+0xec)
    IIPGHelperInterface* GetIPGHelper() { return m_helper; }        // ORIG W 0x081053c6
    void Finalize();                               // ORIG T 0x08104600
    SIPGData* AllocIPGData();                      // ORIG T 0x0810466c
    void FreeIPGData(SIPGData* data);              // ORIG T 0x081046f0
    SIPGData* PutIPGRequest(SIPGData* data, bool queued);  // ORIG T 0x08104754
    SIPGData* _GetIPGRequest();                    // ORIG T 0x081047ea
    void getIPGStatus(stIPGStatus_t& status);      // ORIG W 0x081010ae

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
