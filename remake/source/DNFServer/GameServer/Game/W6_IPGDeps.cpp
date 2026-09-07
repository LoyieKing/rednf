// ============================================================================
// W6_IPGDeps.cpp —— WongWork::IPG 域 only_orig 清零批次（Wave6_WongWorkAssault）
//
// 目标符号（31）：CIPGHelper 全族 17 + stIPGStatus_t C1/C2 +
//   CIPGMgr 12（CIPGNetwork::ConnectTo / CIPGWorker::{dispatch,ProcIPGSync,
//   ProcIPGSyncForChina,ProcIPGSyncForTaiwan} / AllocIPGData / FreeIPGData /
//   PutIPGRequest / _GetIPGRequest / getIPGStatus / Finalize；
//   GetIPGHelper 由 TaiwanCash.h 类内 inline 定义产生 W 符号）。
//
// 报告：docs/class_func_reports/WongWork__IPG__{CIPGHelper,CIPGMgr,
//   CIPGMgr__CIPGWorker,CIPGMgr__CIPGWorker__CIPGNetwork}/。
// 无独立报告者（GetIPGMgr/CIPGMgr::getIPGStatus/stIPGStatus_t ctor）以
// df_game_r objdump 转写，地址见各函数注释。
// SIPGData 域内字段已在权威头 TaiwanCash.h 具名字段化（m_pad[0x2dc] 拆解 +
//   六个命令族 union 视图，逐字段附 ORIG 汇编宽度/偏移证据）；本 TU 一律
//   成员访问，无裸偏移访存。仅保留两处已注明豁免：put_binary(req/res, 0x2dc)
//   的整块线格式序列化、dispatch 里按 ORIG 原样行为的指针类型强转实参。
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"

#include "GameTypes.h"        // Guard<Mutex> / Mutex / DynamicPool
#include "TaiwanCash.h"       // WongWork::IPG 权威类
#include "LogManager.h"
#include "GlobalData.h"       // s_stream_pool / s_msgq_mgr / s_server_stop
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CEnvironment.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ORIG T 0x0858c4e0（_ZN7OS_API5SleepEi，nm/objdump 实测）：真实定义已由
// LinkFinalDeps.cpp 落地（usleep 包装）；此处声明与 CInventory.h 权威一致。
namespace OS_API
{
void Sleep(int ms);
int snprintf(char* buf, unsigned int n, const char* fmt, ...);  // 同 CInventory.h 权威声明
}

// ORIG T（_Z14NumberToStringyi）：64 位版 NumberToString；32 位版声明在
// CInventory.h。定义属 NumberToString 权威 TU，此处仅声明。
// _SetIPGData 的 get_acc_id()（unsigned int）须选 u32 版（ORIG _Z14NumberToStringji），
// 故两重载都声明；tranNo 为 ull 仍走 64 位版。
char* NumberToString(unsigned long long value, int radix);
char* NumberToString(unsigned int value, int radix);


namespace WongWork { namespace IPG {

// ---------------------------------------------------------------------------
// stIPGStatus_t —— ORIG W 0x080ee3de (0x22)：依次清零 +0/+4/+8。
// ---------------------------------------------------------------------------
stIPGStatus_t::stIPGStatus_t()
{
    m_field00 = 0;
    m_field04 = 0;
    m_field08 = 0;
}

// ---------------------------------------------------------------------------
// CIPGMgr —— 数据池与请求队列
// ---------------------------------------------------------------------------

// ORIG T 0x08104600 (0x6c)：循环对每个 worker 调 vtable 槽0（stop()），随后对
// 数组基址调用槽2 deleting dtor 一次（delete[] 形态），最后清指针。
void CIPGMgr::Finalize()
{
    if (m_workers != 0)
    {
        for (unsigned int i = 0; i < m_workerCount; ++i)
        {
            m_workers[i].stop();
        }
        delete[] m_workers;
        m_workers = 0;
    }
}

// ORIG T 0x0810466c (0x83)：Guard(allocMutex) → pool.Acquire → memset 0x2dc。
SIPGData* CIPGMgr::AllocIPGData()
{
    Guard<Mutex> guard(&m_allocMutex);
    SIPGData* data = m_pool.Acquire();
    if (data != 0)
    {
        memset(data, 0, sizeof(SIPGData));   // ORIG movl $0x2dc（整块清零，非字段访存）
    }
    return data;
}

// ORIG T 0x081046f0 (0x63)：Guard(allocMutex) → pool.Free。
void CIPGMgr::FreeIPGData(SIPGData* data)
{
    Guard<Mutex> guard(&m_allocMutex);
    m_pool.Free(data);
}

// ORIG T 0x08104754 (0x95)：queued 且 workerCount!=0 → Guard(reqMutex) 入队，
// 返回 0；否则同步调 workers[0].ProcIPGSync 并透传其结果。
SIPGData* CIPGMgr::PutIPGRequest(SIPGData* data, bool queued)
{
    if (!queued || m_workerCount == 0)
    {
        return m_workers->ProcIPGSync(data);
    }
    {
        Guard<Mutex> guard(&m_reqMutex);
        m_reqQueue.push(data);
    }
    return 0;
}

// ORIG T 0x081047ea (0x9a)：Guard(reqMutex)；空队→0；front 为 0 → 返回 0 不弹；
// 否则 pop 并返回原 front。
SIPGData* CIPGMgr::_GetIPGRequest()
{
    Guard<Mutex> guard(&m_reqMutex);
    if (m_reqQueue.empty())
    {
        return 0;
    }
    SIPGData* front = m_reqQueue.front();
    if (front == 0)
    {
        return 0;
    }
    m_reqQueue.pop();
    return front;
}

// ORIG W 0x081010ae (0x45)：objdump 转写（无独立报告）——
// status+0=pool.Remain、+4=pool.Size、+8=reqQueue.size。
void CIPGMgr::getIPGStatus(stIPGStatus_t& status)
{
    status.m_field00 = (unsigned int)m_pool.Remain();
    status.m_field04 = (unsigned int)m_pool.Size();
    status.m_field08 = (unsigned int)m_reqQueue.size();
}

// ---------------------------------------------------------------------------
// CIPGMgr::CIPGWorker —— 线程体与同步处理链
// ---------------------------------------------------------------------------

// ORIG T 0x08105258 (0x1d)：tail-call ProcIPGSyncForTaiwan。
SIPGData* CIPGMgr::CIPGWorker::ProcIPGSync(SIPGData* data)
{
    // ORIG 帧 0xc8（仅一次 8 字节出参调用，其余为死局部）：P31 dummy local
    // 撑帧对齐（ORIG 0x08105258，ndiff 残余即 sub $0xc8 vs $0x18）。
    char framePad[0xb0];
    (void)framePad;
    return ProcIPGSyncForTaiwan(data);
}

// ORIG T 0x081048b4 (0x12)：local -0x4 = 0 后返回（恒返空结果）。
SIPGData* CIPGMgr::CIPGWorker::ProcIPGSyncForChina(SIPGData* /*data*/)
{
    SIPGData* result = 0;
    return result;
}

// ORIG T 0x08105276 (0xcb)：主循环 —— 取请求→同步处理→接口结果回调（vptr+0x34 =
// IIPGHelperInterface slot11 IPGResult，CIPGHelper vtable 0x08b420a0 槽序实证）→
// 释放临时结果与请求；s_server_stop 置位退出。
void CIPGMgr::CIPGWorker::dispatch(void* /*arg*/)
{
    SIPGData* req = 0;
    SIPGData* res = 0;
    while (!GlobalData::s_server_stop)
    {
        req = m_mgr->_GetIPGRequest();
        if (req == 0)
        {
            OS_API::Sleep(10);
            continue;
        }
        res = ProcIPGSync(req);

        IIPGHelperInterface* helper = m_mgr->GetIPGHelper();
        // ORIG vtable +0x34 = slot11 IPGQuery（CIPGHelper vtable 0x08b420a0
        // 逐槽实证；res 压栈但 IPGQuery 忽略——ORIG 原样行为）。
        helper->IPGQuery(reinterpret_cast<CUser*>(req));


        if (res != 0)
        {
            m_mgr->FreeIPGData(res);
            res = 0;
        }
        m_mgr->FreeIPGData(req);
    }
}

// ---------------------------------------------------------------------------
// ProcIPGSyncForTaiwan —— ORIG T 0x081048c6 (0x992)。switch(data->m_type)：
// 0=charge、2=purchase、3=cancel、4=getBalance、5=confirm、6=ceraPoint、
// 其余（含 1）记日志。cash 取 m_mgr->m_billingCash（ORIG 6 处均直接解引用
// m_mgr(+0xc)→+0xec，无 getBillingCash call）。载荷按命令族视图访问
// （m_cmd.m_input / m_output / m_cancel / m_point / m_query，见 TaiwanCash.h）。
// §6.2 巨件核销（b25）：P167 修前 50/56 [多6 getBillingCash]→修后 50/50
// [0缺0多]；gap 0→0（orig 601 / our 601 指令数相等）；修法：6 处
// getBillingCash() 调用改 m_mgr->m_billingCash 直接成员访问（ORIG 本函数
// 内联解引用 +0xec，访问器不产生 call；TaiwanCash.cpp 的 out-of-line 定义
// 保留供 0x08175f94 等真实 call 点使用）。残差为 N82 槽序/ndiff 951 字节层
// 排布（详见 docs/asm_translation_patterns.md 负例表）。
// ---------------------------------------------------------------------------
SIPGData* CIPGMgr::CIPGWorker::ProcIPGSyncForTaiwan(SIPGData* data)
{
    SIPGData* result = 0;

    switch (data->m_type)
    {
    case 0:  // IPG_TYPE_INPUT：charge → confirm
    {
        Taiwan::BillTransaction tran;   // -0x68..-0x5c
        SIPGDataInputView* in = &data->m_cmd.m_input;
        Taiwan::TaiwanCash* cash = m_mgr->m_billingCash;   // ORIG 直接解引用 +0xec
        int rc = cash->charge(data->m_account,
                              in->m_serviceType,
                              0,
                              in->m_price,
                              "",
                              "",
                              "",
                              data->m_characName,
                              0ULL,
                              tran);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2a7,
                                  "IPG_INPUT charge user: %s, charac: %s reason: %d",
                                  data->m_account, data->m_characName, rc);
            break;
        }
        rc = cash->confirm(tran);
        if (rc != 0)
        {
            char* tranStr = NumberToString(tran.m_tranNo, 0);   // tran+0x04..0x0b
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2af,
                                  "IPG_INPUT confirm user: %s, charac: %s reason: %d tran :%s",
                                  data->m_account, data->m_characName, rc, tranStr);
        }
        break;
    }

    case 2:  // IPG_TYPE_OUTPUT：purchase（ORIG 实参：account=data->m_accName(+0x3c)、
             // 第二字符串=m_output.m_recvCharac(+0x1e8)，均照 asm）
    {
        Taiwan::BillTransaction tran;          // -0x40..-0x38（本栈帧局部）
        SIPGDataOutputView* out = &data->m_cmd.m_output;
        int pay = atoi(out->m_pay);            // ebx
        int price = atoi(out->m_price);        // eax
        Taiwan::TaiwanCash* cash = m_mgr->m_billingCash;
        int rc = cash->purchase(0ULL, price, pay,
                                data->m_accName,
                                out->m_recvCharac,
                                tran);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x269,
                                  "IPG_OUTPUT user: %s, charac: %s reason: %d",
                                  data->m_account, out->m_recvCharac, rc);
            break;
        }
        result = m_mgr->AllocIPGData();
        SIPGDataOutputView* ro = &result->m_cmd.m_output;
        result->m_recordLen = data->m_recordLen;
        strncpy(result->m_envCode, data->m_envCode, 0x14);
        result->m_productCode = data->m_productCode;
        result->m_type = 2;                                          // 响应类型
        result->m_notifyFlag = data->m_notifyFlag;
        memset(&result->m_account, 0, 0x1fc);                        // 载荷前 0x1fc 字节
        strcpy(result->m_account, data->m_account);
        strcpy(result->m_accName, data->m_accName);
        strcpy(result->m_gameCode, data->m_gameCode);
        strcpy(ro->m_price, out->m_price);
        strcpy(ro->m_pay, out->m_pay);
        strcpy(ro->m_arg7D, out->m_arg7D);
        strcpy(ro->m_desc91, out->m_desc91);
        strcpy(ro->m_descF5, out->m_descF5);
        ro->m_tranField0 = tran.m_field0;                            // 08104cf4 `mov %edx,0x1dc(%eax)`
        ro->m_tranNo = tran.m_tranNo;                                // 08104cfd/08104d06 两 dword
        sprintf(ro->m_resultCode, "%d", rc);
        memcpy(ro->m_message, "", 1);                                // 08104d46 DAT_08b421df = "\0"（n=1）
        result->m_callback = data->m_callback;
        break;
    }

    case 3:  // IPG_TYPE_OUTPUT_CANCEL：BillTransaction@0x1f0 → cancel
    {
        Taiwan::BillTransaction tran;
        const SIPGDataCancelView* cv = &data->m_cmd.m_cancel;
        tran.m_field0 = cv->m_tranField0;
        tran.m_tranNo = cv->m_tranNo;
        m_mgr->m_billingCash->cancel(tran);
        break;
    }

    case 4:  // IPG_TYPE_QUERY：getBalance(account, &bal, &point)
    {
        int bal = 0;    // -0x34
        int point = 0;  // -0x30
        int rc = m_mgr->m_billingCash->getBalance(data->m_accName, bal, point);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x243,
                                  "IPG_QUERY user: %s reason: %d",
                                  data->m_account, rc);
            break;
        }
        result = m_mgr->AllocIPGData();
        SIPGDataQueryView* rq = &result->m_cmd.m_query;
        result->m_recordLen = data->m_recordLen;
        strncpy(result->m_envCode, data->m_envCode, 0x14);
        result->m_productCode = data->m_productCode;
        result->m_type = 4;
        result->m_notifyFlag = data->m_notifyFlag;
        memset(&result->m_account, 0, 0x99);
        strncpy(result->m_account, data->m_account, 0x14);
        strncpy(result->m_accName, data->m_accName, 0x28);
        strncpy(result->m_gameCode, data->m_gameCode, 5);
        sprintf(rq->m_ceraValues[0], "%d", point);  // asm 写序：+0x69←-0x30(point)、+0xb5←-0x34(bal)、+0xb1←rc
        sprintf(rq->m_ceraPoint, "%d", bal);
        sprintf(rq->m_retCode, "%d", rc);
        result->m_callback = data->m_callback;
        break;
    }

    case 5:  // IPG_TYPE_OUTPUT_CONFIRM：BillTransaction@0x1f0 → confirm
    {
        Taiwan::BillTransaction tran;
        const SIPGDataCancelView* cv = &data->m_cmd.m_cancel;
        tran.m_field0 = cv->m_tranField0;
        tran.m_tranNo = cv->m_tranNo;
        m_mgr->m_billingCash->confirm(tran);
        break;
    }

    case 6:  // IPG_TYPE_POINT：ceraPoint；A 型命令位不回包
    {
        Taiwan::TaiwanCash* cash = m_mgr->m_billingCash;
        SIPGDataPointView* pt = &data->m_cmd.m_point;
        int rc = cash->ceraPoint(data->m_accName,
                                 data->m_gameCode,
                                 (char)pt->m_command,
                                 pt->m_itemNo,
                                 pt->m_arg94,
                                 (char)pt->m_cmdA,
                                 (char)pt->m_cmdB,
                                 (int)pt->m_point);
        if (rc != 0)
        {
            sprintf(pt->m_reasonB3, "%d", rc);
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2c4,
                                  "IPG_CERA_POINT user: %s reason: %d",
                                  data->m_accName, rc);
            break;
        }
        if (pt->m_command == 0x41)
        {
            break;   // 'A' 命令位：无响应数据
        }
        result = m_mgr->AllocIPGData();
        SIPGDataOutputView* rv = &result->m_cmd.m_output;
        result->m_recordLen = data->m_recordLen;
        strncpy(result->m_envCode, data->m_envCode, 0x14);
        result->m_productCode = data->m_productCode;
        data->m_type = 2;               // ORIG 081050b9：请求数据 type 同步改 2
        result->m_type = 2;
        result->m_notifyFlag = data->m_notifyFlag;
        memset(&result->m_account, 0, 0x1fc);
        strncpy(result->m_account, "", 0x14);
        strncpy(result->m_gameCode, "", 5);
        strncpy(rv->m_message, "cera point", 100);
        strncpy(result->m_accName, data->m_accName, 0x28);
        strncpy(rv->m_price, pt->m_itemNo, 8);
        strncpy(rv->m_arg7D, pt->m_arg94, 0x14);
        sprintf(rv->m_resultCode, "%d", rc);
        sprintf(rv->m_pay, "%d", pt->m_point);
        result->m_callback = data->m_callback;
        break;
    }

    default:
        LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2e9,
                              "IPGDataType : %d", data->m_type);
        break;
    }

    return result;
}

// ---------------------------------------------------------------------------
// CIPGMgr::CIPGWorker::CIPGNetwork::ConnectTo —— ORIG T 0x081043e0 (0xb4)：
// socket(AF_INET,SOCK_STREAM,0) → connect(inet_addr/htons)，失败码 -1/-2。
// socket/connect/inet_addr/htons 经 PLT（同 ORIG），加 <sys/socket.h 等 include 见尾注。
// ---------------------------------------------------------------------------
} }  // namespace WongWork::IPG

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace WongWork { namespace IPG {

int CIPGMgr::CIPGWorker::CIPGNetwork::ConnectTo(const char* ip, unsigned short port)
{
    sockaddr_in addr;
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock < 0)
    {
        return -1;
    }
    memset(&addr, 0, sizeof(addr));                 // ORIG memset 0x10（sockaddr 前 16B）
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    if (connect(m_sock, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        return -2;
    }
    return 0;
}


// ---------------------------------------------------------------------------
// CIPGHelper —— 报告 docs/class_func_reports/WongWork__IPG__CIPGHelper/
// ---------------------------------------------------------------------------

// ORIG T 0x08100b3e (0x153)。
int CIPGHelper::Initialize(unsigned int param1, unsigned int param2)
{
    if (m_mgr != 0)
    {
        Finalize();
    }
    m_dataTableA[0] = 0x2b0;
    m_dataTableA[1] = 0x1cb;
    m_dataTableA[2] = 0x1fc;
    m_dataTableA[3] = 0x1d4;
    m_dataTableA[4] = 0x99;
    m_dataTableA[5] = 0x1d4;
    m_dataTableA[6] = 0x90;

    // ORIG 未缓存 G_CEnvironment() 返回值，条件与拷贝各调一次（P48）；
    // env+0x2b8 = Server_Envir::m_ipgIP（CEnvironment.h 权威布局 +0x198+0x120）。
    if (G_CEnvironment()->m_serverEnvir.m_ipgIP[0] == '\0')
    {
        return -1;
    }
    strncpy(m_str24, G_CEnvironment()->m_serverEnvir.m_ipgIP, 0x14);   // ORIG n=0x14 > m_ipgIP[0x10]【越界读】
    m_dataTableB[0] = 0x5a3c;
    m_dataTableB[1] = 0x5aa0;
    m_dataTableB[2] = 0x5b04;
    m_dataTableB[3] = 0x5b68;
    m_dataTableB[4] = 0x5bcc;
    m_dataTableB[5] = 0x232e;
    m_dataTableB[6] = 0;
    m_mgr = new CIPGMgr(this);           // ORIG operator new(0xf0)+ctor
    return m_mgr->Initialize(param1, param2);
}

// ORIG T 0x08100c92 (0x3f)：mgr->Finalize → ~CIPGMgr → delete → 清指针。
void CIPGHelper::Finalize()
{
    m_mgr->Finalize();
    // ORIG 0x08100c92：单次 `mov 0x4,%ebx; test %ebx; je` 守卫。外层 if(m_mgr!=0)
    // + delete 造成双重判空（+4 条）。delete 自带空守卫（P141），去外层 if。
    delete m_mgr;
    m_mgr = 0;
}

// ORIG T 0x080ffae0 (0x2a)。
void CIPGHelper::getIPGStatus(stIPGStatus_t& status)
{
    if (m_mgr != 0)
    {
        m_mgr->getIPGStatus(status);
    }
}

// ORIG W 0x081010a2 (0xb)：objdump 转写 —— return *(this+4)。
CIPGMgr* CIPGHelper::GetIPGMgr()
{
    return m_mgr;
}

// ORIG T 0x08100a2e (0x10f)。
SIPGData* CIPGHelper::_SetIPGData(CUser* user, IPG_DATA_TYPE type, bool flag)
{
    SIPGData* data = m_mgr->AllocIPGData();
    char* accStr = NumberToString(user->get_acc_id(), 0);      // _Z14NumberToStringji
    OS_API::snprintf(data->m_account, 0x14, "%s", accStr);
    strncpy(data->m_accName, user->get_acc_name(), 0x28);      // ORIG n=0x28 > m_accName[0x14]【越界写】
    memcpy(data->m_gameCode, "DUNF", 5);                       // DAT_08b41459 = "DUNF\0"
    data->m_recordLen = m_dataTableA[type];
    strncpy(data->m_envCode, m_str24, 0x14);
    data->m_productCode = (unsigned short)m_dataTableB[type];
    data->m_uid = user->GetUID();
    data->m_type = (int)type;
    data->m_notifyFlag = flag;   // ORIG mov %dl,0x24 直存参数字节
    return data;
}

// ORIG T 0x081007e8 (0x76)。
void CIPGHelper::IPGRequest(SIPGData* data)
{
    data->m_recordLen = m_dataTableA[data->m_type];
    strncpy(data->m_envCode, m_str24, 0x14);
    data->m_productCode =
        (unsigned short)m_dataTableB[data->m_type];
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x08100790 (0x57)。
void CIPGHelper::IPGQuery(CUser* user)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_QUERY, true);
    data->m_callback = 0;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080ffb0a (0x19a)。
void CIPGHelper::IPGInput(CUser* user,
                          const char* p2, const char* p3, const char* p4,
                          const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT, true);
    SIPGDataInputView* in = &data->m_cmd.m_input;
    strncpy(in->m_argDD, p2, 0xb);
    strncpy(in->m_argE8, p3, 0xc);
    strncpy(in->m_argF4, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(in->m_arg108, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(in->m_arg16C, p6, 100);
    }
    strncpy(in->m_userName, user->GetUserName(), 0x32);
    strncpy(in->m_ssn, user->GetSsnString(), 0xe);
    strncpy(in->m_email, user->GetUserEMail(), 0x3c);
    memset(in->m_zero24C, 0, 7);
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080ffca4 (0x203)。
void CIPGHelper::IPGInput(CUser* user, unsigned char serviceType, unsigned int price,
                          const char* p4, const char* p5, const char* p6,
                          const char* p7, const char* p8, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT, true);
    SIPGDataInputView* in = &data->m_cmd.m_input;
    strncpy(in->m_argDD, p4, 0xb);
    strncpy(in->m_argE8, p5, 0xc);
    strncpy(in->m_argF4, p6, 0x14);
    if (p7 != 0)
    {
        strncpy(in->m_arg108, p7, 100);
    }
    if (p8 != 0)
    {
        strncpy(in->m_arg16C, p8, 100);
    }
    strncpy(in->m_userName, user->GetUserName(), 0x32);
    strncpy(in->m_ssn, user->GetSsnString(), 0xe);
    strncpy(in->m_email, user->GetUserEMail(), 0x3c);
    memset(in->m_zero24C, 0, 7);
    strncpy(data->m_account, user->get_acc_name(), 0x20);     // ORIG n=0x20 > m_account[0x14]【越界写】
    strncpy(data->m_characName, user->getCurCharacName(), 0x1e); // ORIG n=0x1e > m_characName[0x14]【越界写】
    in->m_serviceType = serviceType;
    in->m_price = price;
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080ffea8 (0x118)。
void CIPGHelper::IPGInputCancel(CUser* user,
                                const char* p2, const char* p3, const char* p4,
                                const char* p5, const char* p6, const char* p7,
                                void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT_CANCEL, false);
    SIPGDataInputCancelView* ic = &data->m_cmd.m_inputCancel;
    strncpy(ic->m_arg69, p2, 0x15);
    strncpy(ic->m_arg7E, p3, 0xb);
    strncpy(ic->m_arg89, p4, 0xc);
    strncpy(ic->m_arg95, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(ic->m_argA9, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(ic->m_arg10D, p7, 100);
    }
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080fffc0 (0x135)。
void CIPGHelper::IPGInputPoint(CUser* user, unsigned int point, char a, char b,
                               void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_POINT, true);
    SIPGDataPointView* pt = &data->m_cmd.m_point;
    sprintf(data->m_account, "%d", user->get_acc_id());
    strncpy(data->m_accName, user->get_acc_name(), 0x28);        // ORIG n=0x28【越界写】
    strncpy(data->m_gameCode, user->getCurCharacName(), 0x28);   // ORIG n=0x28 > m_gameCode[5]【越界写】
    strncpy(pt->m_itemNo, "", 8);
    strncpy(pt->m_arg94, "", 0x15);
    pt->m_point = point;
    pt->m_command = 0x41;              // 'A'：输入点模式
    pt->m_cmdA = (unsigned char)a;
    pt->m_cmdB = (unsigned char)b;
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x0810022e (0x11f)。
void CIPGHelper::IPGOutput(CUser* user,
                           const char* p2, const char* p3, const char* p4,
                           const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_OUTPUT, true);
    SIPGDataOutputView* out = &data->m_cmd.m_output;
    strncpy(out->m_price, p2, 8);
    strncpy(out->m_pay, p3, 0xc);
    strncpy(out->m_arg7D, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(out->m_desc91, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(out->m_descF5, p6, 100);
    }
    strncpy(out->m_recvCharac, user->getCurCharacName(), 0x3a);
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x081000f6 (0x137)。
void CIPGHelper::IPGOutputPoint(CUser* user,
                                const char* p2, const char* p3, const char* p4,
                                const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_POINT, true);
    SIPGDataPointView* pt = &data->m_cmd.m_point;
    sprintf(data->m_account, "%d", user->get_acc_id());
    strncpy(data->m_accName, user->get_acc_name(), 0x28);        // ORIG n=0x28【越界写】
    strncpy(data->m_gameCode, user->getCurCharacName(), 0x28);   // ORIG n=0x28 > m_gameCode[5]【越界写】
    strncpy(pt->m_itemNo, p2, 8);
    strncpy(pt->m_arg94, p4, 0x15);  // asm 只写 p4，p5 不落盘
    pt->m_point = (unsigned int)atoi(p3);
    pt->m_command = 0x55;            // 'U'：输出点模式
    pt->m_cmdA = 0;
    pt->m_cmdB = 0;
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x0810034e (0x18f)。返回值恒为请求临时块地址（调用前已释放）——ORIG 如此。
SIPGData* CIPGHelper::IPGOutputSimulation(CUser* user,
                                          const char* p2, const char* p3,
                                          const char* p4, const char* p5,
                                          const char* p6, void* callback)
{
    SIPGData* req = _SetIPGData(user, IPG_TYPE_OUTPUT, true);
    SIPGDataOutputView* out = &req->m_cmd.m_output;
    strncpy(out->m_price, p2, 8);
    strncpy(out->m_pay, p3, 0xc);
    strncpy(out->m_arg7D, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(out->m_desc91, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(out->m_descF5, p6, 100);
    }
    req->m_callback = callback;

    SIPGData* res = m_mgr->AllocIPGData();
    SIPGDataOutputView* rv = &res->m_cmd.m_output;
    memcpy(&res->m_account, &req->m_account, req->m_recordLen);  // 长度取 req+0 记录长（ORIG mov (%eax)）
    memcpy(rv->m_resultCode, "0", 2);                            // DAT_08b41446
    res->m_recordLen = req->m_recordLen;
    res->m_type = req->m_type;

    // ORIG: vptr+0x34 槽调用（IIPGHelperInterface slot11）= IPGResult(req,res)。
    this->IPGResult(req, res);

    m_mgr->FreeIPGData(req);
    m_mgr->FreeIPGData(res);
    return req;
}

// ORIG T 0x081004de (0x138)。
void CIPGHelper::IPGOutputCancel(CUser* user,
                                 const char* p2, const char* p3, const char* p4,
                                 const char* p5, const char* p6, const char* p7,
                                 void* callback, Taiwan::BillTransaction* tran)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_OUTPUT_CANCEL, false);
    SIPGDataCancelView* cv = &data->m_cmd.m_cancel;
    strncpy(cv->m_arg69, p2, 0x15);
    strncpy(cv->m_arg7E, p3, 8);
    strncpy(cv->m_arg86, p4, 0xc);
    strncpy(cv->m_arg92, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(cv->m_argA6, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(cv->m_arg10A, p7, 100);
    }
    data->m_callback = callback;
    cv->m_tranField0 = tran->m_field0;
    cv->m_tranNo = tran->m_tranNo;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x08100616 (0x179)：Alloc 响应块 → 拷 req/res 字段 → 投递（无前置保留字段）。
void CIPGHelper::IPGOutputCancel(SIPGData* req, SIPGData* res)
{
    SIPGData* d = m_mgr->AllocIPGData();
    SIPGDataCancelView* dv = &d->m_cmd.m_cancel;
    const SIPGDataOutputView* rv = &req->m_cmd.m_output;
    const SIPGDataOutputView* sv = &res->m_cmd.m_output;
    memcpy(&d->m_account, &req->m_account, 0x41);
    // 响应块 +0x16e 文本槽（ORIG 08100650 `add $0x16e` 作 strncpy 源，n=0x15）。
    strncpy(dv->m_arg69, res->m_cmd.m_cancel.m_srcMsg16E, 0x15);
    strncpy(dv->m_arg7E, rv->m_price, 8);
    strncpy(dv->m_arg92, rv->m_arg7D, 0x14);
    strncpy(dv->m_arg86, rv->m_pay, 0xc);
    if (&rv->m_desc91[0] != 0)   // ORIG 081006e3 `add $0x91,%eax; test %eax,%eax`：恒真遗留
    {
        strncpy(dv->m_argA6, rv->m_desc91, 100);
    }
    if (&rv->m_descF5[0] != 0)   // 同上（ORIG 08100715）
    {
        strncpy(dv->m_arg10A, rv->m_descF5, 100);
    }
    dv->m_tranField0 = sv->m_tranField0;
    dv->m_tranNo = sv->m_tranNo;
    d->m_notifyFlag = 0;
    m_mgr->PutIPGRequest(d, true);
}

// ORIG T 0x08100cd2 (0x138)。
void CIPGHelper::IPGOutputConfirm(CUser* user,
                                  const char* p2, const char* p3, const char* p4,
                                  const char* p5, const char* p6, const char* p7,
                                  void* callback, Taiwan::BillTransaction* tran)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_OUTPUT_CONFIRM, false);
    SIPGDataCancelView* cv = &data->m_cmd.m_cancel;
    strncpy(cv->m_arg69, p2, 0x15);
    strncpy(cv->m_arg7E, p3, 8);
    strncpy(cv->m_arg86, p4, 0xc);
    strncpy(cv->m_arg92, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(cv->m_argA6, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(cv->m_arg10A, p7, 100);
    }
    data->m_callback = callback;
    cv->m_tranField0 = tran->m_field0;
    cv->m_tranNo = tran->m_tranNo;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x08100e0a (0x194)：前置写保留字段后按 Cancel 同款拷 req/res → 投递。
void CIPGHelper::IPGOutputConfirm(SIPGData* req, SIPGData* res)
{
    SIPGData* d = m_mgr->AllocIPGData();
    SIPGDataCancelView* dv = &d->m_cmd.m_cancel;
    const SIPGDataOutputView* rv = &req->m_cmd.m_output;
    const SIPGDataOutputView* sv = &res->m_cmd.m_output;
    d->m_uid = 0xffffffff;                       // ORIG 08100e24 `movl $0xffffffff,0x20`
    d->m_type = 5;                               // ORIG 08100e2e `movl $0x5,0x1c`
    d->m_notifyFlag = 0;
    memcpy(&d->m_account, &req->m_account, 0x41);
    // 响应块 +0x16e 文本槽（ORIG 08100e5f `add $0x16e`）。
    strncpy(dv->m_arg69, res->m_cmd.m_cancel.m_srcMsg16E, 0x15);
    strncpy(dv->m_arg7E, rv->m_price, 8);
    strncpy(dv->m_arg92, rv->m_arg7D, 0x14);
    strncpy(dv->m_arg86, rv->m_pay, 0xc);
    if (&rv->m_desc91[0] != 0)   // ORIG 081006e3 `add $0x91,%eax; test %eax,%eax`：恒真遗留
    {
        strncpy(dv->m_argA6, rv->m_desc91, 100);
    }
    if (&rv->m_descF5[0] != 0)   // 同上（ORIG 08100715）
    {
        strncpy(dv->m_arg10A, rv->m_descF5, 100);
    }
    dv->m_tranField0 = sv->m_tranField0;
    dv->m_tranNo = sv->m_tranNo;
    d->m_notifyFlag = 0;
    m_mgr->PutIPGRequest(d, true);
}

// ORIG T 0x0810085e (0x1d0)：结果回传流 —— Acquire→header 0x48/err -1→按有无
// 响应数据组织 0/1 分支 put_binary(req[,res])→回调数据或回收。
void CIPGHelper::IPGResult(SIPGData* req, SIPGData* res)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("IPGHelper.cpp", 0x196);
    CStreamGuard guard(stream, true);

    guard.operator*()->operator<<(0x48);
    guard.operator*()->operator<<(-1);
    if (res == 0)
    {
        guard.operator*()->operator<<('\0');
        guard->put_binary(req, 0x2dc);   // 整块线格式序列化（P156 豁免：协议字节流 n=sizeof(SIPGData)）
    }
    else
    {
        // ORIG 081008f5：载荷前 0x41 字节整段搬运（跨 m_account/m_accName/
        // m_characName/m_gameCode 与 m_cmd 首部），非单字段写。
        memcpy(&res->m_account, &req->m_account, 0x41);
        res->m_uid = req->m_uid;
        res->m_callback = req->m_callback;
        res->m_type = req->m_type;
        guard.operator*()->operator<<('\x01');
        guard->put_binary(res, 0x2dc);   // 同上（线格式整块）
        guard->put_binary(req, 0x2dc);   // 同上
    }

    if (req->m_notifyFlag == 0)
    {
        GlobalData::s_stream_pool->Free(guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }
}

} }  // namespace WongWork::IPG
