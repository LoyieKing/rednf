// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MouseRegisterEJ/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "MySQL.h"
#include "DBMgr.h"
#include "CRijndael.h"
#include "GlobalData.h"
#include "CEnvironment.h"      // IMethod / ENUM_DB_HANDLE_IDX
#include "LogManager.h"
#include "../../ServerCommon/DNFFunctionLib.h"   // DNFFLib::Binary2Hex
#include "CInventory.h"        // NumberToString 声明
#include "CSystemTime.h"
#include "STItemScript.h"      // ENUM_EXPERT_JOB_TYPE
#include "CExpertJob.h"
#include "SigTypes.h"          // SIG_MOUSE_REGISTER / SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD
#include "CStreamGuard.h"      // RAII 消息封装（_sendResult/dispatch/_modifyPassword）
#include "MsgQueueMgr.h"       // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "StreamPool.h"

#include <cstring>
#include <map>
// ---------------------------------------------------------------------------
// DB_CancelRestingUserRestrict / DB_CheckSecuReward —— ORIG T 段 makeRequest
// （_ZN28DB_CancelRestingUserRestrict11makeRequestEji @0x0843741e、
//   _ZN18DB_CheckSecuReward11makeRequestEiji @0x08444408）。完整类与 m_dispatchers
//   登记在 W8LT_DBTask.cpp；本 TU 仅按 ORIG 签名局部声明 static makeRequest 供调用，
//   定义归 W8LT_DBTask.cpp（勿重复实现）。
// ---------------------------------------------------------------------------
class DB_CancelRestingUserRestrict
{
public:
    static void makeRequest(unsigned int uid, int param2);
};

class DB_CheckSecuReward
{
public:
    static void makeRequest(int param1, unsigned int uid, int param3);
};

// 写入视图（P82 族）：+0x00 uid / +0x04 param3，使 4.4 折叠为单条 disp store。
struct SIG_CSR_WRITEVIEW
{
    unsigned int m_uid;
    int m_param3;
};

#include "MouseRegisterEJ/makeRequest.inc"


#include "MouseRegisterEJ/makeRequest_1.inc"


// ---------------------------------------------------------------------------
// SIG_MR_WRITEVIEW —— SIG_MOUSE_REGISTER 的 +0x16..+0x22 写入视图（P1 组件视图，
// 使 movl/movb 折叠为单指令 disp；SigTypes_D.h 为共享头勿动）。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct SIG_MR_WRITEVIEW
{
    unsigned int m_accId;    // +0x00
    unsigned int m_pkgType;  // +0x04
    char m_pad08[0xe];       // +0x08..+0x15
    int m_field16;           // +0x16（movl）
    char m_field1a;          // +0x1a（movb）
    int m_field1b;           // +0x1b（movl）
    int m_field1f;           // +0x1f（movl）
};
#pragma pack(pop)
// ---------------------------------------------------------------------------
// encryptAES_ —— ORIG T 0x0846c398（encryptAES_.md 反编译转写）。
// 结果 0x6fffffff 成功转 hex（长度=out*2+1）；否则 logFormat("Encrypt Error") 且 out=NULL。
// 虚槽直调按 P5（vt+0xc = IMethod::Encrypt），__FUNCTION__ 对齐 _ZZ...E12__FUNCTION__。
// ---------------------------------------------------------------------------
char* encryptAES_(CRijndael* method, char* src, char* dst)
{
    unsigned char encBuf[132];
    size_t len;
    size_t padded;
    int block;
    int ret;

    len = strlen(src);
    padded = 0;
    block = method->GetBlockSize(static_cast<unsigned int*>(0));
    if (block >= 0)
    {
        if (static_cast<int>(len) % block != 0)
        {
            padded = static_cast<size_t>(
                (static_cast<int>(len) / block + 1) * block);
        }
        else
        {
            padded = len;
        }
    }
    else
    {
        padded = len;
    }
    if (block >= 0)
    {
        method->Pad(src, static_cast<int>(len),
                    static_cast<unsigned int*>(0));
    }
    typedef int (*Fn)(const void*, const char*, char*, size_t);
    ret = (*(Fn*)(*(void* const*)method + 0x0c))(method, src, (char*)encBuf, padded);
    if (ret != static_cast<int>(0x6fffffff))
    {
        LogManager::logFormat(1, "DesignPattern.cpp", __FUNCTION__, 0x9e,
                              "[%s][%d][%s]", __FUNCTION__, 0x9e,
                              "Encrypt Error");
        return static_cast<char*>(0);
    }
    len = padded * 2 + 1;
    DNFFLib::Binary2Hex(encBuf, static_cast<int>(padded), dst);
    // [P1T4 #284-303 负结论·残差①] ORIG 0x0846c4d7 `mov -0x18(%ebp),%eax;
    // add 0x10(%ebp),%eax; movb $0x0,(%eax)`（内存操作数直加）；我方 4.4 恒
    // `mov -0x18(%ebp),%eax; mov 0x10(%ebp),%edx; lea (%edx,%eax,1),%eax`（+1 条）。
    // 已试下标形 `dst[len]='\0'` → 与指针加法形逐字节相同，无变化（非源形问题）。
    *(dst + len) = '\0';
    return dst;
}

// ============================================================================
// DB_MouseRegister（台湾鼠标密码注册域；ORIG 完整类带虚表，仅本域方法被
// 全工程引用——无跨 TU 实例构造调用点，故此处不需要产出 vtable/typeinfo；
// 非 static 成员为 i386 cdecl 形态（this 入栈），与 ORIG objdump 一致）
// ============================================================================
class DB_MouseRegister
{
public:
    // ORIG T 0x08422e6a：convertPassPadInfo 主体（见头注释的错误码口径）。
    static int convertPassPadInfo(unsigned int mid, char* mousepass,
                                  int& failCnt, char& cancelCnt,
                                  int& cancelCnt2, bool& validityExpired,
                                  char* ip, bool& flag2, char* buf3,
                                  long& validityTime, bool& flag3);
    // ORIG T 0x08422c8a：查询/校验鼠标密码；错误码 1/0x49/0xa6。
    static int checkPassword(unsigned int mid, const char* mousepass,
                             bool param3, int& failCnt, int& cancelCnt,
                             bool& validityExpired, char* ip, char* buf3);
    // ORIG T 0x08423abe：回写 SIG_MOUSE_REGISTER 并投递 DISPATCH_Q。
    void _sendResult(unsigned int mid, int param2, int param3, int param4,
                     SIG_MOUSE_REGISTER* param5);
    // ORIG T 0x08422964：读流、按 pkgType 分派注册/改密/解绑并回执。
    int dispatch(int param1, int param2, Stream* stream);
    // ORIG T 0x08423528：改密（version_info 保持 '2'，写 security_grade）。
    int _modifyPassword(unsigned int mid, const char* pwd, char* ip,
                        int param4, bool param5);
    // ORIG T 0x084232e0：注册新密码（不存在 insert，存在则 update）。
    int _registNewPassword(unsigned int mid, const char* pwd, char* ip,
                           int param4);
    // ORIG T 0x08423794：解绑（删除记录 + 清 security_grade 时间）。
    int _unregistPassword(unsigned int mid, char* ip);

private:
    // ORIG T 0x08422938：DeviceAccountDB 句柄取回。
    static MySQL* _getDBHandleForDevideAccountDB();
    // ORIG T 0x084231b6：mousepass.fail_cnt 与 security_grade.goblin_fail_cnt 同步。
    // 返回 bool（ORIG 尾 0x084232dd `test %al,%al; leave; ret` 无 movzbl
    // 规范化 ⇒ bool 直返；调用点仅语句式消费，mangled 不含返回类型）。
    static bool _updateFailCnt(unsigned int mid, int failCnt, char* ip);
    // ORIG T 0x08423914：写 member_mousepass_history_%Y%m 月份表。
    static bool _updateHistory(unsigned int mid, int modifyType, char* ip);
};

#include "MouseRegisterEJ/_getDBHandleForDevideAccountDB.inc"


#include "MouseRegisterEJ/_updateFailCnt.inc"


#include "MouseRegisterEJ/_updateHistory.inc"
{
    MySQL* db = _getDBHandleForDevideAccountDB();
    db->set_query("seLect mousepass from member_mousepass where m_id=%s",
                  NumberToString(mid, 0));
    if (!(db->exec(true) && db->get_n_rows() != 0 && db->fetch()))
    {
        return false;
    }

    char prevMousepass[36];
    if (!db->get_str(0, prevMousepass, 0x24))
    {
        return false;
    }

    char ym[10] = { '\0' };
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm lt;
    localtime_r(&now, &lt);
    strftime(ym, 10, "%Y%m", &lt);
    db->set_query(
        "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass,"
        " modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
        ym, NumberToString(mid, 0), prevMousepass, modifyType, ip);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return false;
    }
    return true;
}

#include "MouseRegisterEJ/convertPassPadInfo.inc"


// ============================================================================
// DB_MouseRegister 剩余 6 符号（ORIG T 段全转写）：
//   checkPassword 0x08422c8a（static，this 未入栈）、
//   _sendResult 0x08423abe / dispatch 0x08422964 / _modifyPassword 0x08423528 /
//   _registNewPassword 0x084232e0 / _unregistPassword 0x08423794（非 static，
//   i386 cdecl：this 入栈 0x8，参数自 0xc 起——与 ORIG objdump 逐槽核对）。
// ============================================================================

#include "MouseRegisterEJ/checkPassword.inc"


#include "MouseRegisterEJ/_registNewPassword.inc"


#include "MouseRegisterEJ/_modifyPassword.inc"


#include "MouseRegisterEJ/_unregistPassword.inc"


#include "MouseRegisterEJ/_sendResult.inc"


#include "MouseRegisterEJ/dispatch.inc"


// ============================ expert_job 域 ==================================

namespace expert_job {

#include "MouseRegisterEJ/_TryEnchant.inc"
                            ENUM_TRY_ENCHANT_TYPE type)
{
    int ret = 0;

    typedef void (*CloseFn)(void*, CUser*, int);
    // P251/P286/P324：ORIG 0x0849dcd1 先 `mov 0x14(%ebp),%eax` 再寄存器比较链
    // （cmp $1/je、cmp $2/je、test/jne），且三叶块物理序 = case 书写序 0→1→2
    // （ORIG 0x0849dcea case0 直落、+0x65 case1、+0xa6 case2）⇒ 原码是 switch(type)。
    switch (type)
    {
    case ENUM_TRY_ENCHANT_TYPE_0:
        ret = _TryEnchantMine(user, pos);
        (*(CloseFn*)(*(void* const*)this + 0x04))(this, user, 0);
        break;
    case ENUM_TRY_ENCHANT_TYPE_1:
        ret = _TryEnchantOther(user, pos);
        // P225：ORIG 0x0849dd45 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`
        // 为 4.1 槽比零的 bool 物化；4.4 对 if 恒出 cmp/jne 直跳 ⇒ 用 while+break 复现。
        while (ret != 0)
        {
            m_materials[1].Reset();                       // this+0x18 材料②复位
            m_field40 = 0;                                // 登记配方物品清空
            break;
        }
        break;
    case ENUM_TRY_ENCHANT_TYPE_2:
        (*(CloseFn*)(*(void* const*)this + 0x04))(this, user, 2);
        break;
    }
    return ret;
}

}  // namespace expert_job

// ------------------------------ 统计单例 ------------------------------------

#include "MouseRegisterEJ/CExpertJobStatistic.inc"
{
    m_counts.clear();
}

#include "MouseRegisterEJ/dtor_CExpertJobStatistic.inc"

#include "MouseRegisterEJ/ExpertJobKey.inc"
                                                unsigned long itemIdx)
{
    // P95：ORIG 0x8610720 首句先载 this 后载 type（enum 参数）；4.4 直初始化值先。
    // 具名视图复现（m_field00 = +0x00）。
    struct EJKeyView
    {
        int m_field00;
        unsigned long m_itemIdx04;
    };
    ((EJKeyView*)this)->m_field00 = type;
    ((EJKeyView*)this)->m_itemIdx04 = itemIdx;
}

// ORIG operator< W 0x086106da：int 有符号前置序 + unsigned long 无符号次序。
bool CExpertJobStatistic::ExpertJobKey::operator<(const ExpertJobKey& other) const
{
    if (m_field00 < other.m_field00)
        return true;
    if (m_field00 == other.m_field00 &&
        m_itemIdx04 < other.m_itemIdx04)
    {
        return true;
    }
    return false;
}

#include "MouseRegisterEJ/increaseItemCount.inc"


// ORIG T 0x0860f940：__cxa_guard 单例（instance/guard BSS 0x09473350/0x09473320）。
CExpertJobStatistic* GetInstanceExpertJobStatistic()
{
    static CExpertJobStatistic instance;
    return &instance;
}

#include "MouseRegisterEJ/UpdateExpertJob.inc"

