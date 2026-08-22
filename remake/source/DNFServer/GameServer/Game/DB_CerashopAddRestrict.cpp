// ============================================================================
// df_game_r 还原 —— DB_CerashopAddRestrict（商城限购 DB 请求，DBThread 派发）。
// 逐函数对照：
//   docs/class_func_reports/DB_LoadCerashopAddRestrictType/makeRequest.md
//   docs/class_func_reports/DB_SaveCerashopAddRestrictType/makeRequest.md
// 语义：向 DB 队列发商城限购请求；包体为 CStreamGuard，经
// GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard) 投递。
//   LOAD（0x341）：GetUID() + get_acc_id() + getCurCharacNo()
//   SAVE（0x340）：-1 + m_pInfo 字段 + head 记录 + acc_id|characNo（按 op 分支）。
// 两类均为全局作用域类（ORIG 符号无命名空间前缀），仅参数类型引入
// CerashopAddRestrict::DBInputData。
// ============================================================================
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "CUser.h"

// ORIG T @ 0x080e1ff6（mangled _ZN30DB_LoadCerashopAddRestrictType11makeRequestEP5CUser）
class DB_LoadCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user);
};

// ORIG T @ 0x080e17ba
//（mangled _ZN30DB_SaveCerashopAddRestrictType11makeRequestEP5CUserRN19CerashopAddRestrict11DBInputDataE）
class DB_SaveCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user, CerashopAddRestrict::DBInputData& data);
};

// ============================================================================
// DB_LoadCerashopAddRestrictType::makeRequest @ 0x080e1ff6
// 包体：0x341 | GetUID() | get_acc_id() | getCurCharacNo()
// ============================================================================
void DB_LoadCerashopAddRestrictType::makeRequest(CUser* user)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("CerashopAddRestrictType.cpp", 500);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x341);                      // SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
    guard.operator*()->operator<<((int)user->GetUID());                     // operator<<(int)
    guard.operator*()->operator<<(user->get_acc_id());                      // operator<<(uint)
    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());    // operator<<(uint)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_SaveCerashopAddRestrictType::makeRequest @ 0x080e17ba
// 按 data.m_pInfo->m_type（op）分派：
//   op==0（登记）：包体 0x340 | -1 | m_type | m_ipgNo | head[+4] | head[+8](long)
//                  | m_pInfo[+0x10](long) | get_acc_id()
//   op==1（删除）：同上，末字段为 getCurCharacNo()
// 注：ORIG DBInputData 的第 0 个指针字段（head 记录，slot +0x00）在源 CUser.h 的
// DBInputData 中未建模（仅 m_pInfo @ +0x04 与 m_pad[0xc]），本 TU 按 ORIG 槽位
// 直接解释，保持 ABI 与 ORIG 字节布局一致。
// ============================================================================
void DB_SaveCerashopAddRestrictType::makeRequest(
    CUser* user, CerashopAddRestrict::DBInputData& data)
{
    const char* head = *(const char**)&data;   // ORIG field_0 槽位（DBInputData +0x00）

    if (data.m_pInfo->m_type == 0) {
        // ---- 登记 ----
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("CerashopAddRestrictType.cpp", 396);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x340);                 // SIG_SAVE_CERASHOP_ADD_RESTRICT_TYPE
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_type);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_ipgNo);
        guard.operator*()->operator<<(*(unsigned int*)(head + 4));            // head[+4] uint
        guard.operator*()->operator<<(*(long*)(head + 8));                    // head[+8] long
        guard.operator*()->operator<<(*(long*)((char*)data.m_pInfo + 0x10));  // m_pInfo[+0x10] long
        guard.operator*()->operator<<(user->get_acc_id());                    // uint
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    } else if (data.m_pInfo->m_type == 1) {
        // ---- 删除 ----
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("CerashopAddRestrictType.cpp", 410);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x340);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_type);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_ipgNo);
        guard.operator*()->operator<<(*(unsigned int*)(head + 4));
        guard.operator*()->operator<<(*(long*)(head + 8));
        guard.operator*()->operator<<(*(long*)((char*)data.m_pInfo + 0x10));
        guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());  // uint
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}