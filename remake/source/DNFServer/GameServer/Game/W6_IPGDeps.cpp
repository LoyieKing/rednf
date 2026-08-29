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
// SIPGData 域内字段在权威头保持 m_pad 视图，实现按汇编偏移直访并注释偏移。
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
char* NumberToString(unsigned long long value, int radix);


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
        memset(data, 0, 0x2dc);
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
        helper->IPGResult(req, res);          // ORIG vtable +0x34 槽调用


        if (res != 0)
        {
            m_mgr->FreeIPGData(res);
            res = 0;
        }
        m_mgr->FreeIPGData(req);
    }
}

// ---------------------------------------------------------------------------
// ProcIPGSyncForTaiwan —— ORIG T 0x081048c6 (0x992)。switch(d->type@0x1c)：
// 0=charge、2=purchase、3=cancel、4=getBalance、5=confirm、6=ceraPoint、
// 其余（含 1）记日志。cash 取 *(m_mgr->getBillingCash())。
// ---------------------------------------------------------------------------
SIPGData* CIPGMgr::CIPGWorker::ProcIPGSyncForTaiwan(SIPGData* data)
{
    char* d = (char*)data;
    SIPGData* result = 0;

    switch (*(int*)(d + 0x1c))
    {
    case 0:  // IPG_TYPE_INPUT：charge → confirm
    {
        Taiwan::BillTransaction tran;   // -0x68..-0x5c
        Taiwan::TaiwanCash* cash = m_mgr->getBillingCash();
        int rc = cash->charge((char*)(d + 0x28),
                              (unsigned char)d[0x26d],
                              0,
                              *(unsigned int*)(d + 0x98),
                              "",
                              "",
                              "",
                              (char*)(d + 0x50),
                              0ULL,
                              tran);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2a7,
                                  "IPG_INPUT charge user: %s, charac: %s reason: %d",
                                  (char*)(d + 0x28), (char*)(d + 0x50), rc);
            break;
        }
        rc = cash->confirm(tran);
        if (rc != 0)
        {
            char* tranStr = NumberToString(tran.m_tranNo, 0);   // tran+0x04..0x0b
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2af,
                                  "IPG_INPUT confirm user: %s, charac: %s reason: %d tran :%s",
                                  (char*)(d + 0x28), (char*)(d + 0x50), rc, tranStr);
        }
        break;
    }

    case 2:  // IPG_TYPE_OUTPUT：purchase（注意：account=d+0x3c、第二字符串=d+0x1e8，均照 asm）
    {
        Taiwan::BillTransaction tran;          // -0x40..-0x38（本栈帧局部）
        int pay = atoi(d + 0x71);              // ebx
        int price = atoi(d + 0x69);            // eax
        Taiwan::TaiwanCash* cash = m_mgr->getBillingCash();
        int rc = cash->purchase(0ULL, price, pay,
                                (const char*)(d + 0x3c),
                                (const char*)(d + 0x1e8),
                                tran);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x269,
                                  "IPG_OUTPUT user: %s, charac: %s reason: %d",
                                  (char*)(d + 0x28), (char*)(d + 0x1e8), rc);
            break;
        }
        result = m_mgr->AllocIPGData();
        char* r = (char*)result;
        *(unsigned int*)r = *(unsigned int*)d;                       // r+0
        strncpy(r + 0x04, d + 0x04, 0x14);                           // r+4
        *(unsigned short*)(r + 0x18) = *(unsigned short*)(d + 0x18); // r+0x18
        *(int*)(r + 0x1c) = 2;                                       // 响应类型
        r[0x24] = d[0x24];
        memset(r + 0x28, 0, 0x1fc);
        strcpy(r + 0x28, d + 0x28);   // user（strncpy 语义按 asm strcpy）
        strcpy(r + 0x3c, d + 0x3c);
        strcpy(r + 0x64, d + 0x64);   // charac
        strcpy(r + 0x69, d + 0x69);
        strcpy(r + 0x71, d + 0x71);
        strcpy(r + 0x7d, d + 0x7d);
        strcpy(r + 0x91, d + 0x91);
        strcpy(r + 0xf5, d + 0xf5);
        memcpy(r + 0x1dc, &tran, 0xc);                               // +0x1dc/+0x1e0/+0x1e4 ← local tran 三 dword
        sprintf((char*)(r + 0x173), "%d", rc);
        r[0x177] = '\0';                                             // DAT_08b421df = "\0"（1 字节拷贝）
        *(unsigned int*)(r + 0x2d8) = *(unsigned int*)(d + 0x2d8);
        break;
    }

    case 3:  // IPG_TYPE_OUTPUT_CANCEL：BillTransaction@0x1f0 → cancel
    {
        Taiwan::BillTransaction tran;
        tran.m_field0 = *(int*)(d + 0x1f0);
        tran.m_tranNo = *(unsigned long long*)(d + 0x1f4);
        m_mgr->getBillingCash()->cancel(tran);
        break;
    }

    case 4:  // IPG_TYPE_QUERY：getBalance(account, &bal, &point)
    {
        int bal = 0;    // -0x34
        int point = 0;  // -0x30
        int rc = m_mgr->getBillingCash()->getBalance((const char*)(d + 0x3c), bal, point);
        if (rc != 0)
        {
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x243,
                                  "IPG_QUERY user: %s reason: %d",
                                  (char*)(d + 0x28), rc);
            break;
        }
        result = m_mgr->AllocIPGData();
        char* r = (char*)result;
        *(unsigned int*)r = *(unsigned int*)d;
        strncpy(r + 0x04, d + 0x04, 0x14);
        *(unsigned short*)(r + 0x18) = *(unsigned short*)(d + 0x18);
        *(int*)(r + 0x1c) = 4;
        r[0x24] = d[0x24];
        memset(r + 0x28, 0, 0x99);
        strncpy(r + 0x28, d + 0x28, 0x14);
        strncpy(r + 0x3c, d + 0x3c, 0x28);
        strncpy(r + 0x64, d + 0x64, 5);
        sprintf(r + 0x69, "%d", point);   // asm 写序：+0x69←-0x30(point)、+0xb5←-0x34(bal)、+0xb1←rc
        sprintf(r + 0xb5, "%d", bal);
        sprintf(r + 0xb1, "%d", rc);
        *(unsigned int*)(r + 0x2d8) = *(unsigned int*)(d + 0x2d8);
        break;
    }

    case 5:  // IPG_TYPE_OUTPUT_CONFIRM：BillTransaction@0x1f0 → confirm
    {
        Taiwan::BillTransaction tran;
        tran.m_field0 = *(int*)(d + 0x1f0);
        tran.m_tranNo = *(unsigned long long*)(d + 0x1f4);
        m_mgr->getBillingCash()->confirm(tran);
        break;
    }

    case 6:  // IPG_TYPE_POINT：ceraPoint；A 型命令位不回包
    {
        Taiwan::TaiwanCash* cash = m_mgr->getBillingCash();
        int rc = cash->ceraPoint((char*)(d + 0x3c),
                                 (char*)(d + 0x64),
                                 (char)d[0xb0],
                                 (char*)(d + 0x8c),
                                 (char*)(d + 0x94),
                                 (char)d[0xb1],
                                 (char)d[0xb2],
                                 *(int*)(d + 0xac));
        if (rc != 0)
        {
            sprintf(d + 0xb3, "%d", rc);
            LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2c4,
                                  "IPG_CERA_POINT user: %s reason: %d",
                                  (char*)(d + 0x3c), rc);
            break;
        }
        if ((unsigned char)d[0xb0] == 0x41)
        {
            break;   // 'A' 命令位：无响应数据
        }
        result = m_mgr->AllocIPGData();
        char* r = (char*)result;
        *(unsigned int*)r = *(unsigned int*)d;
        strncpy(r + 0x04, d + 0x04, 0x14);
        *(unsigned short*)(r + 0x18) = *(unsigned short*)(d + 0x18);
        *(int*)(d + 0x1c) = 2;          // ORIG 081050b9：请求数据 type 同步改 2
        *(int*)(r + 0x1c) = 2;
        r[0x24] = d[0x24];
        memset(r + 0x28, 0, 0x1fc);
        strncpy(r + 0x28, "", 0x14);
        strncpy(r + 0x64, "", 5);
        strncpy(r + 0x177, "cera point", 100);
        strncpy(r + 0x3c, d + 0x3c, 0x28);
        strncpy(r + 0x69, d + 0x8c, 8);
        strncpy(r + 0x7d, d + 0x94, 0x14);
        sprintf(r + 0x173, "%d", rc);
        sprintf(r + 0x71, "%d", *(unsigned int*)(d + 0xac));
        *(unsigned int*)(r + 0x2d8) = *(unsigned int*)(d + 0x2d8);
        break;
    }

    default:
        LogManager::logFormat(1, "IPGMgr.cpp", __PRETTY_FUNCTION__, 0x2e9,
                              "IPGDataType : %d", *(int*)(d + 0x1c));
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

    if (*((char*)G_CEnvironment() + 0x2b8) == '\0')   // env+0x2b8（未建模字段，偏移实证；ORIG 未缓存 env，两次独立调用）
    {
        return -1;
    }
    strncpy(m_str24, (char*)G_CEnvironment() + 0x2b8, 0x14);
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
    if (m_mgr != 0)
    {
        delete m_mgr;
    }
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
    char* d = (char*)data;

    char* accStr = NumberToString(user->get_acc_id(), 0);      // _Z14NumberToStringji
    OS_API::snprintf(d + 0x28, 0x14, "%s", accStr);
    strncpy(d + 0x3c, user->get_acc_name(), 0x28);
    memcpy(d + 0x64, "DUNF", 5);                               // DAT_08b41459 = "DUNF\0"
    *(unsigned int*)d = m_dataTableA[type];
    strncpy(d + 0x04, m_str24, 0x14);
    *(unsigned short*)(d + 0x18) = (unsigned short)m_dataTableB[type];
    *(unsigned int*)(d + 0x20) = user->GetUID();
    *(unsigned int*)(d + 0x1c) = (unsigned int)type;
    d[0x24] = flag;   // ORIG mov %dl,0x24 直存参数字节
    return data;
}

// ORIG T 0x081007e8 (0x76)。
void CIPGHelper::IPGRequest(SIPGData* data)
{
    char* d = (char*)data;
    *(unsigned int*)d = m_dataTableA[*(int*)(d + 0x1c)];
    strncpy(d + 0x04, m_str24, 0x14);
    *(unsigned short*)(d + 0x18) =
        (unsigned short)m_dataTableB[*(int*)(d + 0x1c)];
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x08100790 (0x57)。
void CIPGHelper::IPGQuery(CUser* user)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_QUERY, true);
    *(char**)((char*)data + 0x2d8) = 0;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080ffb0a (0x19a)。
void CIPGHelper::IPGInput(CUser* user,
                          const char* p2, const char* p3, const char* p4,
                          const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT, true);
    char* d = (char*)data;
    strncpy(d + 0xdd, p2, 0xb);
    strncpy(d + 0xe8, p3, 0xc);
    strncpy(d + 0xf4, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(d + 0x108, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(d + 0x16c, p6, 100);
    }
    strncpy(d + 0x1d0, user->GetUserName(), 0x32);
    strncpy(d + 0x202, user->GetSsnString(), 0xe);
    strncpy(d + 0x210, user->GetUserEMail(), 0x3c);
    memset(d + 0x24c, 0, 7);
    *(void**)(d + 0x2d8) = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080ffca4 (0x203)。
void CIPGHelper::IPGInput(CUser* user, unsigned char serviceType, unsigned int price,
                          const char* p4, const char* p5, const char* p6,
                          const char* p7, const char* p8, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT, true);
    char* d = (char*)data;
    strncpy(d + 0xdd, p4, 0xb);
    strncpy(d + 0xe8, p5, 0xc);
    strncpy(d + 0xf4, p6, 0x14);
    if (p7 != 0)
    {
        strncpy(d + 0x108, p7, 100);
    }
    if (p8 != 0)
    {
        strncpy(d + 0x16c, p8, 100);
    }
    strncpy(d + 0x1d0, user->GetUserName(), 0x32);
    strncpy(d + 0x202, user->GetSsnString(), 0xe);
    strncpy(d + 0x210, user->GetUserEMail(), 0x3c);
    memset(d + 0x24c, 0, 7);
    strncpy(d + 0x28, user->get_acc_name(), 0x20);
    strncpy(d + 0x50, user->getCurCharacName(), 0x1e);
    d[0x26d] = serviceType;
    *(unsigned int*)(d + 0x98) = price;
    *(void**)(d + 0x2d8) = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080ffea8 (0x118)。
void CIPGHelper::IPGInputCancel(CUser* user,
                                const char* p2, const char* p3, const char* p4,
                                const char* p5, const char* p6, const char* p7,
                                void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT_CANCEL, false);
    char* d = (char*)data;
    strncpy(d + 0x69, p2, 0x15);
    strncpy(d + 0x7e, p3, 0xb);
    strncpy(d + 0x89, p4, 0xc);
    strncpy(d + 0x95, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(d + 0xa9, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(d + 0x10d, p7, 100);
    }
    *(void**)(d + 0x2d8) = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x080fffc0 (0x135)。
void CIPGHelper::IPGInputPoint(CUser* user, unsigned int point, char a, char b,
                               void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_POINT, true);
    char* d = (char*)data;
    sprintf(d + 0x28, "%d", user->get_acc_id());
    strncpy(d + 0x3c, user->get_acc_name(), 0x28);
    strncpy(d + 0x64, user->getCurCharacName(), 0x28);
    strncpy(d + 0x8c, "", 8);
    strncpy(d + 0x94, "", 0x15);
    *(unsigned int*)(d + 0xac) = point;
    d[0xb0] = 0x41;                  // 'A'：输入点模式
    d[0xb1] = a;
    d[0xb2] = b;
    *(void**)(d + 0x2d8) = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x0810022e (0x11f)。
void CIPGHelper::IPGOutput(CUser* user,
                           const char* p2, const char* p3, const char* p4,
                           const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_OUTPUT, true);
    char* d = (char*)data;
    strncpy(d + 0x69, p2, 8);
    strncpy(d + 0x71, p3, 0xc);
    strncpy(d + 0x7d, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(d + 0x91, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(d + 0xf5, p6, 100);
    }
    strncpy(d + 0x1e8, user->getCurCharacName(), 0x3a);
    *(void**)(d + 0x2d8) = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x081000f6 (0x137)。
void CIPGHelper::IPGOutputPoint(CUser* user,
                                const char* p2, const char* p3, const char* p4,
                                const char* /*p5*/, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_POINT, true);
    char* d = (char*)data;
    sprintf(d + 0x28, "%d", user->get_acc_id());
    strncpy(d + 0x3c, user->get_acc_name(), 0x28);
    strncpy(d + 0x64, user->getCurCharacName(), 0x28);
    strncpy(d + 0x8c, p2, 8);
    strncpy(d + 0x94, p4, 0x15);   // asm 只写 p4，p5 不落盘
    *(unsigned int*)(d + 0xac) = (unsigned int)atoi(p3);
    d[0xb0] = 0x55;                  // 'U'：输出点模式
    d[0xb1] = 0;
    d[0xb2] = 0;
    *(void**)(d + 0x2d8) = callback;
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x0810034e (0x18f)。返回值恒为请求临时块地址（调用前已释放）——ORIG 如此。
SIPGData* CIPGHelper::IPGOutputSimulation(CUser* user,
                                          const char* p2, const char* p3,
                                          const char* p4, const char* p5,
                                          const char* p6, void* callback)
{
    SIPGData* req = _SetIPGData(user, IPG_TYPE_OUTPUT, true);
    char* s = (char*)req;
    strncpy(s + 0x69, p2, 8);
    strncpy(s + 0x71, p3, 0xc);
    strncpy(s + 0x7d, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(s + 0x91, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(s + 0xf5, p6, 100);
    }
    *(void**)(s + 0x2d8) = callback;

    SIPGData* res = m_mgr->AllocIPGData();
    char* r = (char*)res;
    memcpy(r + 0x28, s + 0x28, *(size_t*)s);       // 长度取 req+0 类型键（ORIG mov (%eax)）
    memcpy(r + 0x173, "0", 2);                      // DAT_08b41446
    *(unsigned int*)r = *(unsigned int*)s;
    *(unsigned int*)(r + 0x1c) = *(unsigned int*)(s + 0x1c);

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
    char* d = (char*)data;
    strncpy(d + 0x69, p2, 0x15);
    strncpy(d + 0x7e, p3, 8);
    strncpy(d + 0x86, p4, 0xc);
    strncpy(d + 0x92, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(d + 0xa6, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(d + 0x10a, p7, 100);
    }
    *(void**)(d + 0x2d8) = callback;
    *(unsigned int*)(d + 0x1f0) = tran->m_field0;
    *(unsigned int*)(d + 0x1f4) = (unsigned int)(tran->m_tranNo & 0xffffffffULL);
    *(unsigned int*)(d + 0x1f8) = (unsigned int)((tran->m_tranNo >> 32) & 0xffffffffULL);
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x08100616 (0x179)：Alloc 响应块 → 拷 req/res 字段 → 投递（无前置保留字段）。
void CIPGHelper::IPGOutputCancel(SIPGData* req, SIPGData* res)
{
    SIPGData* d = m_mgr->AllocIPGData();
    memcpy((char*)d + 0x28, (char*)req + 0x28, 0x41);
    strncpy((char*)d + 0x69, (char*)res + 0x16e, 0x15);
    strncpy((char*)d + 0x7e, (char*)req + 0x69, 8);
    strncpy((char*)d + 0x92, (char*)req + 0x7d, 0x14);
    strncpy((char*)d + 0x86, (char*)req + 0x71, 0xc);
    if ((char*)req + 0x91 != 0)   // ORIG add/test 形态照写（指针非空判断）
    {
        strncpy((char*)d + 0xa6, (char*)req + 0x91, 100);
    }
    if ((char*)req + 0xf5 != 0)   // 同上
    {
        strncpy((char*)d + 0x10a, (char*)req + 0xf5, 100);
    }
    *(unsigned int*)((char*)d + 0x1f0) = *(unsigned int*)((char*)res + 0x1dc);
    *(unsigned int*)((char*)d + 0x1f4) = *(unsigned int*)((char*)res + 0x1e0);
    *(unsigned int*)((char*)d + 0x1f8) = *(unsigned int*)((char*)res + 0x1e4);
    ((char*)d)[0x24] = 0;
    m_mgr->PutIPGRequest(d, true);
}

// ORIG T 0x08100cd2 (0x138)。
void CIPGHelper::IPGOutputConfirm(CUser* user,
                                  const char* p2, const char* p3, const char* p4,
                                  const char* p5, const char* p6, const char* p7,
                                  void* callback, Taiwan::BillTransaction* tran)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_OUTPUT_CONFIRM, false);
    char* d = (char*)data;
    strncpy(d + 0x69, p2, 0x15);
    strncpy(d + 0x7e, p3, 8);
    strncpy(d + 0x86, p4, 0xc);
    strncpy(d + 0x92, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(d + 0xa6, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(d + 0x10a, p7, 100);
    }
    *(void**)(d + 0x2d8) = callback;
    *(unsigned int*)(d + 0x1f0) = tran->m_field0;
    *(unsigned int*)(d + 0x1f4) = (unsigned int)(tran->m_tranNo & 0xffffffffULL);
    *(unsigned int*)(d + 0x1f8) = (unsigned int)((tran->m_tranNo >> 32) & 0xffffffffULL);
    m_mgr->PutIPGRequest(data, true);
}

// ORIG T 0x08100e0a (0x194)：前置写保留字段后按 Cancel 同款拷 req/res → 投递。
void CIPGHelper::IPGOutputConfirm(SIPGData* req, SIPGData* res)
{
    SIPGData* d = m_mgr->AllocIPGData();
    ((unsigned int*)d)[0x20 / 4] = 0xffffffff;   // ORIG 08100e24
    ((int*)d)[0x1c / 4] = 5;
    ((char*)d)[0x24] = 0;
    memcpy((char*)d + 0x28, (char*)req + 0x28, 0x41);
    strncpy((char*)d + 0x69, (char*)res + 0x16e, 0x15);
    strncpy((char*)d + 0x7e, (char*)req + 0x69, 8);
    strncpy((char*)d + 0x92, (char*)req + 0x7d, 0x14);
    strncpy((char*)d + 0x86, (char*)req + 0x71, 0xc);
    if ((char*)req + 0x91 != 0)   // ORIG add/test 形态照写（指针非空判断）
    {
        strncpy((char*)d + 0xa6, (char*)req + 0x91, 100);
    }
    if ((char*)req + 0xf5 != 0)   // 同上
    {
        strncpy((char*)d + 0x10a, (char*)req + 0xf5, 100);
    }
    *(unsigned int*)((char*)d + 0x1f0) = *(unsigned int*)((char*)res + 0x1dc);
    *(unsigned int*)((char*)d + 0x1f4) = *(unsigned int*)((char*)res + 0x1e0);
    *(unsigned int*)((char*)d + 0x1f8) = *(unsigned int*)((char*)res + 0x1e4);
    ((char*)d)[0x24] = 0;
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
        guard->put_binary(req, 0x2dc);
    }
    else
    {
        char* r = (char*)res;
        char* q = (char*)req;
        memcpy(r + 0x28, q + 0x28, 0x41);
        *(unsigned int*)(r + 0x20) = *(unsigned int*)(q + 0x20);
        *(unsigned int*)(r + 0x2d8) = *(unsigned int*)(q + 0x2d8);
        *(unsigned int*)(r + 0x1c) = *(unsigned int*)(q + 0x1c);
        guard.operator*()->operator<<('\x01');
        guard->put_binary(res, 0x2dc);
        guard->put_binary(req, 0x2dc);
    }

    if (((char*)req)[0x24] == 0)
    {
        GlobalData::s_stream_pool->Free(guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }
}

} }  // namespace WongWork::IPG
