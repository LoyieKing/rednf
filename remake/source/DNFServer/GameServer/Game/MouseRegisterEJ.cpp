// ============================================================================
// MouseRegisterEJ.cpp — 台湾 MouseRegister 域 + expert_job 统计（LinkClosureB）
//
// 报告来源（逐份转写，汇编+反编译 C）：
//   docs/class_func_reports/DB_MouseRegister/{convertPassPadInfo,
//       _getDBHandleForDevideAccountDB,_updateFailCnt,_updateHistory}.md
//     DB_MouseRegister::convertPassPadInfo        T 0x08422e6a（static）
//       c++filt 权威形参：(unsigned int, char*, int&, char&, int&, bool&,
//       char*, bool&, char*, long&, bool&)；mangled
//       _ZN16DB_MouseRegister18convertPassPadInfoEjPcRiRcS1_RbS0_S3_S0_RlS3_
//       （返回值以 int 错误码承载：失败计数字路径 eax=0x48/0x49 被上层 dispatch
//       直接消费 ==0x48→errcode5、==0xa6→9；mangled 不含返回类型）
//     DB_MouseRegister::_getDBHandleForDevideAccountDB T 0x08422938
//     DB_MouseRegister::_updateFailCnt            T 0x084231b6
//     DB_MouseRegister::_updateHistory            T 0x08423914
//     encryptAES_(CRijndael*, char*, char*)       T 0x0846c398
//   docs/class_func_reports/expert_job__CEnchanter/_TryEnchant.md
//     expert_job::CEnchanter::_TryEnchant         T 0x0849dcc4
//   docs/class_func_reports/{CExpertJobStatistic,_lt_global_gt_}/
//     CExpertJobStatistic C1 W 0x0861074a / D1 W 0x086108ae
//     ExpertJobKey C1 W 0x08610720 / operator< W 0x086106da
//     increaseItemCount                            T 0x0860f9d4
//     GetInstanceExpertJobStatistic                T 0x0860f940（__cxa_guard 单例，
//                                                  guard@0x09473320 instance@0x09473350）
//
// 说明：IMethod::GetBlockSize/IMethod::Pad 定义已在 DBMW/Method.cpp（mangled 一致）；
// getItemIndex 已在 HeroMissionLink4.cpp；本 TU 均只复用不重复定义。
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

void DB_CancelRestingUserRestrict::makeRequest(unsigned int uid, int param2)
{
    // ORIG T 0x0843741e：Acquire(__PRETTY_FUNCTION__,0x8389) → <<0x195 <<param2
    // → GetInBuffer<SIG_RESTING_USER_RESTRICT> 写 uid@+0 → put DB_Q。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(__PRETTY_FUNCTION__, 0x8389), true);
    **guard << 0x195;
    **guard << param2;
    SIG_RESTING_USER_RESTRICT* sig =
        guard->GetInBuffer<SIG_RESTING_USER_RESTRICT>();
    sig->m_uid = uid;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void DB_CheckSecuReward::makeRequest(int param1, unsigned int uid, int param3)
{
    // ORIG T 0x08444408：Acquire(__PRETTY_FUNCTION__,0xab34) → <<0x2ab <<param1
    // → GetInBuffer 写 uid@+0 param3@+4 → put DB_Q。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(__PRETTY_FUNCTION__, 0xab34), true);
    **guard << 0x2ab;
    **guard << param1;
    SIG_CHECK_USER_SECU_REWARD* sig =
        guard->GetInBuffer<SIG_CHECK_USER_SECU_REWARD>();
    reinterpret_cast<SIG_CSR_WRITEVIEW*>(sig)->m_uid = uid;
    reinterpret_cast<SIG_CSR_WRITEVIEW*>(sig)->m_param3 = param3;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

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

MySQL* DB_MouseRegister::_getDBHandleForDevideAccountDB()
{
    // ORIG：GetDBHandle(s_db_mgr, 6, 0)，结果经 -0xc 局部中转（O0 命名局部形态）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_6, static_cast<ENUM_SERVER_GROUP>(0));
    return db;
}

// [b28d-47] verdict=DIFF(routed) [N93附] ORIG 0x084231b6 orig/ours=74/75
//   残差：尾部双 return 块发射序相反（ORIG Bfalse-ret0 前/Btrue-ret1 后，4.4 恒反）
//   + 1 条 jmp 中继；P222 两形互试已做（反劣 79），N93附 封顶。
bool DB_MouseRegister::_updateFailCnt(unsigned int mid, int failCnt, char* ip)
{
    (void)ip;   // ORIG 形参未被函数体消费（Ghidra 同）。
    MySQL* db = _getDBHandleForDevideAccountDB();
    db->set_query("upDate member_mousepass set fail_cnt=%d where m_id=%s",
                  failCnt, NumberToString(mid, 0));
    db->exec(true);
    MySQL* grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_fail_cnt=%d where m_id=%s",
        failCnt, NumberToString(mid, 0));
    // 实测三形后取双 return（bool 返回）：判定段 xor/test/jne+or/test/jne+
    // mov$1/mov$0 与 ORIG 全等；汇聚 bool 局部形 4.4 必落 -0x9 槽（4 条差）
    // 反劣。残差仅尾部 jmp+mov$0（4.4 双 return 分离块 vs ORIG 汇聚共同
    // leave，跨版本块排布 N 族）。[b28 BM8 复验] P222 两形互试：
    // `if(exec) return false; return true;` 反劣 79 insn（mov$0/mov$1 双岛），
    // 维持 `return !exec()`（75，ndiff 3 封顶，N93附 族）。
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_fail_cnt) values(%s, %d)",
            NumberToString(mid, 0), failCnt);
        return !grade->exec(true);
    }
    return false;
}

bool DB_MouseRegister::_updateHistory(unsigned int mid, int modifyType, char* ip)
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

int DB_MouseRegister::convertPassPadInfo(unsigned int mid, char* mousepass,
                                         int& failCnt, char& cancelCnt,
                                         int& cancelCnt2,
                                         bool& validityExpired, char* ip,
                                         bool& flag2, char* buf3,
                                         long& validityTime, bool& flag3)
{
    validityExpired = false;

    MySQL* db = _getDBHandleForDevideAccountDB();
    db->set_query(
        "seLect mousepass, fail_cnt, cancel_cnt, version_info,"
        " validity_time>unix_timestamp(now()),"
        " adddate(now(),interval -1 month) >= occ_time from member_mousepass"
        " where m_id=%s and enable_flag = '1'",
        NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        cancelCnt = '\0';
        return 0;
    }
    if (!db->fetch())
    {
        return 1;
    }
    if (!db->get_str(0, ip, 0x24))
    {
        return 1;
    }
    if (!db->get_int(1, failCnt))
    {
        return 1;
    }
    if (!db->get_int(2, cancelCnt2))
    {
        return 1;
    }
    char versionInfo;
    if (!db->get_byte(3, versionInfo))
    {
        return 1;
    }
    validityTime = 0;
    if (!db->get_uint(4, reinterpret_cast<unsigned int&>(validityTime)))
    {
        return 1;
    }
    char expiredFlag = '\0';
    if (!db->get_byte(5, expiredFlag))
    {
        return 1;
    }
    flag3 = (expiredFlag != 0);

    flag2 = false;
    if (validityTime != 0)
    {
        flag2 = true;
    }

    if (versionInfo == '\x02')
    {
        validityExpired = true;
        cancelCnt = '\x02';
        return 0;
    }

    cancelCnt = '\x01';
    {
        char plainPadded[36];
        char cipherHex[36];
        strncpy(plainPadded, mousepass, 0x24);
        encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_,
                    plainPadded, cipherHex);
        if (strncmp(ip, cipherHex, 0x24) != 0)
        {
            failCnt = failCnt + 1;
            _updateFailCnt(mid, failCnt, buf3);  // ORIG param_9=buf3（报告 +0x265）
            if (failCnt > 9)
            {
                return 0x49;
            }
            return 0x48;
        }
    }

    failCnt = 0;
    cancelCnt2 = 0;
    if (!_updateHistory(mid, 7, buf3))  // ORIG param_9=buf3（报告 +0x2ab）
    {
        return 1;
    }
    db->set_query(
        "upDate member_mousepass set enable_flag = '0' where m_id=%s",
        NumberToString(mid, 0));
    // ORIG 尾：`exec;xor$1;test;jne(ret1); getAffected;or %edx,%eax;test;
    // mov$1/mov$0; test %al; je` ⇒ 内联判定，无 bool 局部（`register bool`
    // 会强占 ebx 使帧变大 +0x64/push ebx）。
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return 1;
    }
    validityExpired = false;
    return 0;
}

// ============================================================================
// DB_MouseRegister 剩余 6 符号（ORIG T 段全转写）：
//   checkPassword 0x08422c8a（static，this 未入栈）、
//   _sendResult 0x08423abe / dispatch 0x08422964 / _modifyPassword 0x08423528 /
//   _registNewPassword 0x084232e0 / _unregistPassword 0x08423794（非 static，
//   i386 cdecl：this 入栈 0x8，参数自 0xc 起——与 ORIG objdump 逐槽核对）。
// ============================================================================

int DB_MouseRegister::checkPassword(unsigned int mid, const char* mousepass,
                                    bool param3, int& failCnt, int& cancelCnt,
                                    bool& validityExpired, char* ip, char* buf3)
{
    // ORIG 栈槽：0x8=mid 0xc=mousepass 0x10=param3 0x14=failCnt& 0x18=cancelCnt&
    // 0x1c=validityExpired& 0x20=ip 0x24=buf3（static 无 this 槽）。
    char local35[36];       // 明文缓冲（-0x31，strncpy dst / encryptAES src）
    char local59[36];       // encryptAES_ 密文缓冲（-0x55）
    char local11;           // version_info（-0xd）
    MySQL* db;              // -0xc

    db = _getDBHandleForDevideAccountDB();
    validityExpired = false;
    cancelCnt = 0;
    memset(ip, 0, 0x24);
    db->set_query(
        "seLect mousepass,fail_cnt,cancel_cnt, version_info from member_mousepass"
        " where m_id=%s and enable_flag = '1'",
        NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 1;
    }
    if (!db->get_str(0, ip, 0x24))
    {
        return 1;
    }
    if (!db->get_int(1, failCnt))
    {
        return 1;
    }
    if (!db->get_int(2, cancelCnt))
    {
        return 1;
    }
    if (param3 && failCnt > 2)
    {
        return 0x49;
    }
    if (!db->get_byte(3, local11))
    {
        return 1;
    }
    if (local11 == '\x01')
    {
        return 0xa6;
    }
    strncpy(local35, mousepass, 0x24);
    encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, local35, local59);
    validityExpired = true;
    return 0;
}

int DB_MouseRegister::_registNewPassword(unsigned int mid, const char* pwd,
                                         char* ip, int param4)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=pwd 0x14=ip 0x18=param4。
    MySQL* db;      // -0x10
    MySQL* grade;   // -0xc

    db = _getDBHandleForDevideAccountDB();
    db->set_query("seLect enable_flag from member_mousepass where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        db->set_query(
            "inSert into member_mousepass(m_id,mousepass,occ_time,version_info,"
            " validity_time, reward_time,enable_flag) values(%s,'%s',now(),'2',"
            " unix_timestamp(now()), 0,'1')",
            NumberToString(mid, 0), pwd);
    }
    else
    {
        db->set_query(
            "upDate member_mousepass set mousepass = '%s', occ_time = now() ,"
            "version_info = '2', validity_time=unix_timestamp(now()),"
            " enable_flag = '1', reward_time = 0 where m_id = %s",
            pwd, NumberToString(mid, 0));
    }
    if (!db->exec(true))
    {
        if (db->getDBError() == 0x426)
        {
            return 0x12;
        }
        return 1;
    }
    DB_CancelRestingUserRestrict::makeRequest(mid, param4);
    if (!_updateHistory(mid, 8, ip))
    {
        return 1;
    }
    grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_pass_mod=now() where m_id=%s",
        NumberToString(mid, 0));
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_pass_mod)"
            " values(%s, now())",
            NumberToString(mid, 0));
        if (!grade->exec(true))
        {
            return 1;
        }
    }
    DB_CheckSecuReward::makeRequest(param4, mid, 1);
    return 0;
}

int DB_MouseRegister::_modifyPassword(unsigned int mid, const char* pwd,
                                      char* ip, int param4, bool param5)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=pwd 0x14=ip 0x18=param4 0x1c=param5。
    MySQL* db;      // -0x14
    MySQL* grade;   // -0x10
    SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD* p;   // -0xc

    db = _getDBHandleForDevideAccountDB();
    if (!_updateHistory(mid, 9, ip))
    {
        return 0x15;
    }
    db->set_query(
        "upDate member_mousepass set mousepass='%s',occ_time=now() where m_id=%s"
        " and enable_flag = '1'",
        pwd, NumberToString(mid, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return 0x15;
    }
    grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_pass_mod=now() where m_id=%s",
        NumberToString(mid, 0));
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_pass_mod)"
            " values(%s, now())",
            NumberToString(mid, 0));
        if (!grade->exec(true))
        {
            return 0;
        }
    }
    if (param5)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5dcd), true);
        (**guard) << 0x245;
        (**guard) << param4;
        // ORIG 0x0842373b 实有中间槽（`mov %eax,-0xc; movzbl -0x2c,%edx;
        // mov -0xc,%eax; mov %edx,(%eax)`）＝ 具名 p 变量形态。
        p = guard->GetInBuffer<SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD>();
        // [b28d-47] 具名成员访问：4.4 对 `p->m = b` 产 ORIG 的 RHS 值先装载序
        // （探针 f3；reinterpret_cast 视图形恒 LHS 指针先）。
        p->m_adviceChangePwd = param5;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 0;
}

int DB_MouseRegister::_unregistPassword(unsigned int mid, char* ip)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=ip。
    MySQL* db;      // -0x10
    MySQL* grade;   // -0xc

    db = _getDBHandleForDevideAccountDB();
    if (!_updateHistory(mid, 10, ip))
    {
        return 0x15;
    }
    db->set_query("deLete from member_mousepass where m_id=%s",
                  NumberToString(mid, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return 0x15;
    }
    grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_pass_mod='0000-00-00 00:00:00'"
        " where m_id=%s",
        NumberToString(mid, 0));
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_pass_mod)"
            " values(%s, '0000-00-00 00:00:00')",
            NumberToString(mid, 0));
        if (!grade->exec(true))
        {
            return 0;
        }
    }
    return 0;
}

void DB_MouseRegister::_sendResult(unsigned int mid, int param2, int param3,
                                   int param4, SIG_MOUSE_REGISTER* param5)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=param2 0x14=param3 0x18=param4
    // 0x1c=param5（cdecl：this 入栈）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5e4e), true);
    SIG_MOUSE_REGISTER* out;   // GetInBuffer 结果（-0xc）

    (**guard) << 0xa3;
    (**guard) << param4;
    out = guard->GetInBuffer<SIG_MOUSE_REGISTER>();
    memcpy(out, param5, 0x59);
    reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_accId = mid;
    reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_pkgType = param3;
    reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field16 = param2;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

int DB_MouseRegister::dispatch(int param1, int param2, Stream* stream)
{
    // ORIG 栈槽：0x8=this 0xc=param1(未用) 0x10=param2 0x14=stream。
    char buf58[36];        // 明文缓冲（-0x54）
    char buf7c[36];        // encryptAES_ 密文缓冲（-0x78）
    MySQL* db;             // -0x18（仅赋值未消费，ORIG 同）
    SIG_MOUSE_REGISTER* out;   // -0x14
    int ret;               // -0x10
    SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD* p;   // -0xc

    (void)param1;
    db = _getDBHandleForDevideAccountDB();
    out = stream->GetOutBuffer<SIG_MOUSE_REGISTER>();
    ret = 0;
    char buf2c[16] = { '\0' };   // webAddress 快照（-0x28；声明置 GetOutBuffer 后，
                                 // 清零指令序与 ORIG 一致）
    strncpy(buf2c, out->m_webAddress, 0x10);
    strncpy(buf58, out->m_password2, 7);
    encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, buf58, buf7c);
    if (strncmp(out->m_padPassword, buf7c, 0x24) == 0)
    {
        CStreamGuard guard2(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5b59), true);
        (**guard2) << 0x246;
        (**guard2) << param2;
        // ORIG 0x08422a94 实有中间槽（`mov %eax,-0xc; mov -0xc,%eax; movl
        // $0,(%eax)`）＝ 具名 p 变量形态（fn_diff_tu 实证）。
        p = guard2->GetInBuffer<SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD>();
        p->m_adviceChangePwd = 0;   // [b28d-47] 具名字段（同上）
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard2);
        return 0;
    }
    switch (out->m_pkgType)
    {
    case 1:
        ret = _registNewPassword(out->m_accId, buf7c, buf2c, param2);
        if (ret == 0)
        {
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1a = 1;
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1f = 0;
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1b = 0;
            memcpy(out->m_padPassword, buf7c, 0x24);
        }
        break;
    case 2:
        ret = _modifyPassword(out->m_accId, buf7c, buf2c, param2,
                              out->m_advideChangePwd);
        if (ret == 0)
        {
            memcpy(out->m_padPassword, buf7c, 0x24);
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1f = 0;
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1b = 0;
        }
        break;
    case 3:
        ret = _unregistPassword(out->m_accId, buf2c);
        if (ret == 0)
        {
            memset(out->m_padPassword, 0, 0x24);
        }
        break;
    }
    _sendResult(out->m_accId, ret, out->m_pkgType, param2, out);
    return 1;
}

// ============================ expert_job 域 ==================================

namespace expert_job {

// ORIG T 0x0849dcc4（_TryEnchant.md / objdump 全文核对）。分派三种试附魔路径：
//   type==0 自家材料：_TryEnchantMine 后触发基类虚槽1（vptr+0x04）(user,0)；
//   type==1 对家材料：_TryEnchantOther，失败时复位材料②(this+0x18)并清登记配方(+0x40)；
//   type==2 仅虚槽(user,2)；其它类型无副作用。
// 虚槽经 vptr 偏移 +0x04 显式调用：与 ORIG 指令逐条对应；CExpertJob 虚表槽序
// 审计收敛后可替换回语义虚调用。
// [b28d-47] verdict=DIFF(routed) [N7/N82] ORIG 0x0849dcc4 orig/ours=64/59
//   残差：虚槽调用寄存器角色 + 槽/帧分配跨版本差（ORIG 经 vptr+0x04 显式虚调）。
int CEnchanter::_TryEnchant(CUser* user, stItemPos_t* pos,
                            ENUM_TRY_ENCHANT_TYPE type)
{
    int ret = 0;

    typedef void (*CloseFn)(void*, CUser*, int);
    if (type == ENUM_TRY_ENCHANT_TYPE_1)
    {
        ret = _TryEnchantOther(user, pos);
        if (ret != 0)
        {
            m_materials[1].Reset();                       // this+0x18 材料②复位
            m_field40 = 0;                                // 登记配方物品清空
        }
    }
    else if (type == ENUM_TRY_ENCHANT_TYPE_2)
    {
        (*(CloseFn*)(*(void* const*)this + 0x04))(this, user, 2);
    }
    else if (type == ENUM_TRY_ENCHANT_TYPE_0)
    {
        ret = _TryEnchantMine(user, pos);
        (*(CloseFn*)(*(void* const*)this + 0x04))(this, user, 0);
    }
    return ret;
}

}  // namespace expert_job

// ------------------------------ 统计单例 ------------------------------------

// ORIG C1 W 0x0861074a：map 默认构造后紧接 clear()（双调用形态保留）。
CExpertJobStatistic::CExpertJobStatistic()
{
    m_counts.clear();
}

// ORIG D1 W 0x086108ae：map 逆序析构（编译器生成等价）。
CExpertJobStatistic::~CExpertJobStatistic() {}

// ORIG ctor W 0x08610720：直接落两字段。
CExpertJobStatistic::ExpertJobKey::ExpertJobKey(ENUM_EXPERT_JOB_TYPE type,
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

// ORIG T 0x0860f9d4（increaseItemCount.md）：find 未命中则以 count 为初值插入，
// 否则 value += count。
void CExpertJobStatistic::increaseItemCount(ENUM_EXPERT_JOB_TYPE type,
                                            unsigned long itemIdx, int count)
{
    std::map<ExpertJobKey, int>::iterator it =
        m_counts.find(ExpertJobKey(type, itemIdx));
    if (it != m_counts.end())
    {
        it->second = it->second + count;
    }
    else
    {
        m_counts.insert(std::make_pair(ExpertJobKey(type, itemIdx), count));
    }
}

// ORIG T 0x0860f940：__cxa_guard 单例（instance/guard BSS 0x09473350/0x09473320）。
CExpertJobStatistic* GetInstanceExpertJobStatistic()
{
    static CExpertJobStatistic instance;
    return &instance;
}

// ORIG T 0x0860faca（UpdateExpertJob.md）：map 空直接返回；否则 Acquire
// ("Statistics.cpp", 0x65a) 后写头 0x182/-1/map.size()，遍历逐条写
// {key.m_field00, key.m_itemIdx04, value}，put(DB_Q) 后 clear()。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x0860faca orig/ours=137/137
//   残差：槽/寄存器分配跨版本差（指令数全等，语义对齐）。
void CExpertJobStatistic::UpdateExpertJob()
{
    if (m_counts.empty())
    {
        return;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x65a), true);
    guard.operator*()->operator<<(0x182);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(static_cast<int>(m_counts.size()));
    for (std::map<ExpertJobKey, int>::const_iterator it = m_counts.begin();
         it != m_counts.end(); ++it)
    {
        guard.operator*()->operator<<(it->first.m_field00);
        guard.operator*()->operator<<(static_cast<int>(it->first.m_itemIdx04));
        guard.operator*()->operator<<(it->second);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_counts.clear();
}
