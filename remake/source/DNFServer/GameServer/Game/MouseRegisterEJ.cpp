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

#include <cstring>
#include <map>

namespace {

// 一处文件内小工具：把 CRijndael* 当 IMethod 槽直调（与 ORIG vt+0xc Encrypt 形态一致）。
inline int ciEncrypt(IMethod* method, const char* in, char* out, size_t len)
{
    typedef int (*Fn)(const void*, const char*, char*, size_t);
    return (*reinterpret_cast<Fn*>(*reinterpret_cast<void* const*>(method) +
                                   0x0c))(method, in, out, len);
}

}  // namespace

// ---------------------------------------------------------------------------
// encryptAES_ —— ORIG T 0x0846c398（encryptAES_.md 反编译转写）。
// 结果 0x6fffffff 成功转 hex（长度=out*2+1）；否则 logFormat("Encrypt Error") 且 out=NULL。
// ---------------------------------------------------------------------------
char* encryptAES_(CRijndael* method, char* src, char* dst)
{
    unsigned char encBuf[132];
    size_t len = strlen(src);
    int block =
        method->GetBlockSize(static_cast<unsigned int*>(0));
    size_t padded = len;
    if (block < 0)
    {
        padded = len;
    }
    else if (static_cast<int>(len) % block == 0)
    {
        padded = len;
    }
    else
    {
        padded = static_cast<size_t>(
            (static_cast<int>(len) / block + 1) * block);
    }
    if (block >= 0)
    {
        method->Pad(src, static_cast<int>(len),
                    static_cast<unsigned int*>(0));
    }
    const int ret = ciEncrypt(method, src, reinterpret_cast<char*>(encBuf), padded);
    if (ret == static_cast<int>(0x6fffffff))
    {
        const size_t hexLen = padded * 2 + 1;
        DNFFLib::Binary2Hex(encBuf, static_cast<int>(padded), dst);
        dst[hexLen] = '\0';
        return dst;
    }
    LogManager::logFormat(1, "DesignPattern.cpp", "encryptAES_", 0x9e,
                          "[%s][%d][%s]", "encryptAES_", 0x9e,
                          "Encrypt Error");
    return static_cast<char*>(0);
}

// ============================================================================
// DB_MouseRegister（台湾鼠标密码注册域；ORIG 完整类带虚表，仅本域 static 方法被
// 全工程引用——无跨 TU 实例构造调用点，故此处不需要产出 vtable/typeinfo）
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

private:
    // ORIG T 0x08422938：DeviceAccountDB 句柄取回。
    static MySQL* _getDBHandleForDevideAccountDB();
    // ORIG T 0x084231b6：mousepass.fail_cnt 与 security_grade.goblin_fail_cnt 同步。
    static int _updateFailCnt(unsigned int mid, int failCnt, char* ip);
    // ORIG T 0x08423914：写 member_mousepass_history_%Y%m 月份表。
    static bool _updateHistory(unsigned int mid, int modifyType, char* ip);
};

MySQL* DB_MouseRegister::_getDBHandleForDevideAccountDB()
{
    // ORIG：GetDBHandle(s_db_mgr, 6, 0)。
    return GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_6, static_cast<ENUM_SERVER_GROUP>(0));
}

int DB_MouseRegister::_updateFailCnt(unsigned int mid, int failCnt, char* ip)
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
    int result;
    if (grade->exec(true) && grade->getAffectedRowCount() != 0)
    {
        result = 0;
    }
    else
    {
        result = 1;
    }
    if (result != 0)
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_fail_cnt) values(%s, %d)",
            NumberToString(mid, 0), failCnt);
        result = grade->exec(true) ? 0 : 1;
    }
    return result;
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

    char ym[10];
    ym[0] = ym[1] = ym[2] = ym[3] = ym[4] = ym[5] = ym[6] = ym[7] = ym[8] =
        ym[9] = '\0';
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm lt;
    localtime_r(&now, &lt);
    strftime(ym, 10, "%Y%m", &lt);
    db->set_query(
        "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass,"
        " modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
        ym, NumberToString(mid, 0), prevMousepass, modifyType, ip);
    return db->exec(true) && db->getAffectedRowCount() != 0;
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
    if (!db->get_str(0, buf3, 0x24))
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
    char versionInfo = '\0';
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
        if (strncmp(buf3, cipherHex, 0x24) != 0)
        {
            failCnt = failCnt + 1;
            _updateFailCnt(mid, failCnt, ip);
            return failCnt < 10 ? 0x48 : 0x49;
        }
    }

    failCnt = 0;
    cancelCnt2 = 0;
    if (!_updateHistory(mid, 7, ip))
    {
        return 1;
    }
}

// ============================ expert_job 域 ==================================

namespace expert_job {

// ORIG T 0x0849dcc4（_TryEnchant.md / objdump 全文核对）。分派三种试附魔路径：
//   type==0 自家材料：_TryEnchantMine 后触发基类虚槽1（vptr+0x04）(user,0)；
//   type==1 对家材料：_TryEnchantOther，失败时复位材料②(this+0x18)并清登记配方(+0x40)；
//   type==2 仅虚槽(user,2)；其它类型无副作用。
// 虚槽经 vptr 偏移 +0x04 显式调用：与 ORIG 指令逐条对应；CExpertJob 虚表槽序
// 审计收敛后可替换回语义虚调用。
int CEnchanter::_TryEnchant(CUser* user, stItemPos_t* pos,
                            ENUM_TRY_ENCHANT_TYPE type)
{
    int ret = 0;

    typedef void (*CloseFn)(void*, CUser*, int);
    CloseFn closeFn = *reinterpret_cast<CloseFn*>(
        *reinterpret_cast<void* const*>(this) + 0x04);

    if ((int)type == 1)
    {
        ret = _TryEnchantOther(user, pos);
        if (ret != 0)
        {
            m_materials[1].Reset();                       // this+0x18 材料②复位
            m_field40 = 0;                                // 登记配方物品清空
        }
    }
    else if ((int)type == 2)
    {
        closeFn(this, user, 2);
    }
    else if ((int)type == 0)
    {
        ret = _TryEnchantMine(user, pos);
        closeFn(this, user, 0);
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
    m_field00 = static_cast<int>(type);
    m_itemIdx04 = itemIdx;
}

// ORIG operator< W 0x086106da：int 有符号前置序 + unsigned long 无符号次序。
bool CExpertJobStatistic::ExpertJobKey::operator<(const ExpertJobKey& other) const
{
    if (m_field00 < other.m_field00)
        return true;
    if (m_field00 != other.m_field00)
        return false;
    return m_itemIdx04 < other.m_itemIdx04;
}

// ORIG T 0x0860f9d4（increaseItemCount.md）：find 未命中则以 count 为初值插入，
// 否则 value += count。
void CExpertJobStatistic::increaseItemCount(ENUM_EXPERT_JOB_TYPE type,
                                            unsigned long itemIdx, int count)
{
    std::map<ExpertJobKey, int>::iterator it =
        m_counts.find(ExpertJobKey(type, itemIdx));
    if (it == m_counts.end())
    {
        m_counts.insert(std::make_pair(ExpertJobKey(type, itemIdx), count));
    }
    else
    {
        it->second += count;
    }
}

// ORIG T 0x0860f940：__cxa_guard 单例（instance/guard BSS 0x09473350/0x09473320）。
CExpertJobStatistic* GetInstanceExpertJobStatistic()
{
    static CExpertJobStatistic instance;
    return &instance;
}
