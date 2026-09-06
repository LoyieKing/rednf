// WongWork::CSimpleSSO 方法集（G3 批次 A，ORIG df_game_r）。
// 报告：docs/class_func_reports/WongWork__CSimpleSSO/
//   expireLoginTime 0x08602b24 / reset 0x086028b4 / checkValidPassword 0x086028c6 /
//   updateSSOExpireTime(uint,st) 0x08602906 / updateSSOExpireTime(uint,uint) 0x08602a4a /
//   _checkValiedPassword 0x08602bd0 / _getGameStartTime 0x08602da6 /
//   getGameStartTime 0x086031f6 / updateHumanCertify 0x08603226 /
//   checkValidPassword_Jpn 0x081b25ec / stOutputData_t::ctor(W) 0x0818fb28。
// 类权威声明：CSimpleSSO.h（原 GlobalData.cpp 局部类收敛）；ctor/dtor/init 属 GlobalData.cpp。

#include "CSimpleSSO.h"
#include "MySQL.h"
#include "CSystemTime.h"
#include "CGM_Manager.h"     // CGM_Manager::IsGm
#include "CRijndael.h"
#include "DNFFunctionLib.h"  // DNFFLib::Binary2Hex
#include "LogManager.h"      // LogManager::logFormat
#include "GameTypes.h"       // cMyTrace

// 轻量 extern 声明（避免拉入 CUser.h/GlobalData.h 重型共享头）：
char* NumberToString(unsigned int value, int radix);          // ORIG 0x0810904b
namespace GlobalData
{
extern CSystemTime s_systemTime_;      // 0x0941f714
extern CGM_Manager* s_GM_Manager;      // 0x0941f75c
}


#include <string.h>
#include <stdio.h>

// ---- stOutputData_t::stOutputData_t()  ORIG W 0x0818fb28：逐字段置零，
//      m_loginIp(+0x18)/pad 区仅清首字节，与 ORIG 指令序一致。
WongWork::CSimpleSSO::stOutputData_t::stOutputData_t()
{
    m_lastPlayTime = 0;
    m_accountFail = 0;
    m_totalAccountFail = 0;
    m_reportCnt = 0;
    m_reliableFlag = 0;
    m_tradeGoldDaily = 0;
    m_loginIp[0] = 0;
    m_securityFlag = 0;
    m_dungeonGainGold = 0;
    m_schoolId = 0;
    m_rating = 0.0f;
    m_tutorialSkipable = 0;
    m_eventCharacFlag = 0;
    m_loginTime = 0;
    m_expireTime = 0;
}

// ---- reset()  ORIG 0x086028b4：*(this+0xd0860)=0 → m_mysql=0
void WongWork::CSimpleSSO::reset()
{
    m_mysql = 0;
}

// ---- expireLoginTime(mId)  ORIG 0x08602b24
void WongWork::CSimpleSSO::expireLoginTime(unsigned int mId)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG+0x006..+0x01c call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG+0x021..+0x037 call memcpy
    if (mId != 0)
    {
        char* idStr = NumberToString(mId, 0);
        int curSec = GlobalData::s_systemTime_.getCurSec();
        m_mysql->set_query("upDate %s set expire_time=%d where m_id=%s",
                           tableName, curSec, idStr);
        m_mysql->exec(true);
    }
}

// ---- _getGameStartTime(mId, login&, expire&, out&)  ORIG 0x08602da6
bool WongWork::CSimpleSSO::_getGameStartTime(unsigned int mId, long& loginTime,
                                             long& expireTime, stOutputData_t& out)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);       // ORIG +0x006..+0x01c call memset
    memcpy(tableName, "member_login", 0xd);  // ORIG +0x021..+0x037 call memcpy
    char* idStr = NumberToString(mId, 0);
    m_mysql->set_query(
        "seLect login_time,expire_time,report_cnt,last_play_time,account_fail,"
        "total_account_fail,reliable_flag,trade_gold_daily,login_ip,security_flag,"
        "dungeon_gain_gold,school_id,rating,tutorial_skipable,event_charac_flag"
        " from %s where m_id=%s",
        tableName, idStr);
    if (!m_mysql->exec(true))
        return false;
    if (!m_mysql->fetch())
        return false;
    if (m_mysql->get_n_rows() == 0)   // ORIG fetch 后空结果检查
        return false;

    unsigned int loginValue = 0;   // col0 login_time
    unsigned int expireValue = 0;  // col1 expire_time

    int col = 0;
    if (!m_mysql->get_uint(col++, reinterpret_cast<unsigned int&>(loginValue)))
        return false;
    if (!m_mysql->get_uint(col++, reinterpret_cast<unsigned int&>(expireValue)))
        return false;
    if (!m_mysql->get_int(col++, out.m_reportCnt))  // report_cnt +0x0c
        return false;
    {
        col++;
        if (!m_mysql->get_int(col++, out.m_lastPlayTime))                       // last_play_time +0x00
            return false;
        if (!m_mysql->get_int(col++, out.m_accountFail))  // account_fail +0x04
            return false;
        if (!m_mysql->get_int(col++, out.m_totalAccountFail))  // total_account_fail +0x08
            return false;
        if (!m_mysql->get_byte(col++, out.m_reliableFlag))  // reliable_flag
            return false;
        if (!m_mysql->get_uint(col++, out.m_tradeGoldDaily))  // trade_gold_daily
            return false;
        if (!m_mysql->get_str(col++, out.m_loginIp, 0x14))  // login_ip +0x18
            return false;
        if (!m_mysql->get_byte(col++, out.m_securityFlag))  // security_flag
            return false;
        if (!m_mysql->get_uint(col++, out.m_dungeonGainGold))  // dungeon_gain_gold
            return false;
        if (!m_mysql->get_int(col++, out.m_schoolId))  // school_id
            return false;
        if (!m_mysql->get_float(col++, out.m_rating))  // rating
            return false;
        if (!m_mysql->get_ubyte(col++, out.m_tutorialSkipable))  // tutorial_skipable
            return false;
        if (!m_mysql->get_ubyte(col++, out.m_eventCharacFlag))  // event_charac_flag
            return false;
    }
    // ORIG 尾部：*param_2=t1; *param_3=t2; +0x40=t1; +0x44=t2
    loginTime = static_cast<long>(loginValue);
    expireTime = static_cast<long>(expireValue);
    out.m_loginTime = loginValue;
    out.m_expireTime = expireValue;
    return true;
}

// ---- getGameStartTime(...)  ORIG 0x086031f6：薄转发 _getGameStartTime
void WongWork::CSimpleSSO::getGameStartTime(unsigned int mId, long& loginTime,
                                            long& expireTime, stOutputData_t& out)
{
    _getGameStartTime(mId, loginTime, expireTime, out);
}

// ---- checkValidPassword(...)  ORIG 0x086028c6：两相邻 unsigned 局部作 long& 传入；
// ORIG 返回 bool（= _getGameStartTime 结果，GetAccountInfo 失败时记日志返回 false）。
bool WongWork::CSimpleSSO::checkValidPassword(unsigned int mId, char const* a,
                                              char const* b, stOutputData_t& out)
{
    unsigned int v1 = 0;  // ORIG -0x18(%ebp)
    unsigned int v2 = 0;  // ORIG -0x14(%ebp)
    return _getGameStartTime(mId, *reinterpret_cast<long*>(&v2),
                             *reinterpret_cast<long*>(&v1), out);
}

// ---- checkValidPassword_Jpn(...)  ORIG 0x081b25ec
bool WongWork::CSimpleSSO::checkValidPassword_Jpn(unsigned int mId, char const* a,
                                                  char const* b, stOutputData_t& out)
{
    unsigned int v1 = 0;
    unsigned int v2 = 0;
    getGameStartTime(mId, *reinterpret_cast<long*>(&v2),
                     *reinterpret_cast<long*>(&v1), out);
    if (GlobalData::s_GM_Manager->IsGm(mId))
    {
        if (strncmp(a, b, 0x84) == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x2e, 0);
            trace("[!]GM User Check Password Sucess=> m_id(%d)", mId);
        }
    }
    return true;
}

// ---- _checkValiedPassword(pwd, clientHash, cipher)  ORIG 0x08602bd0
bool WongWork::CSimpleSSO::_checkValiedPassword(char* pwd, char const* clientHash,
                                                CRijndael& cipher)
{
    unsigned int srcLen = strlen(pwd);  // ORIG -0x14(%ebp)
    unsigned int paddedLen = 0;         // ORIG -0x10(%ebp)

    char hexBuf[132];  // ORIG -0x98(%ebp)（catch 分支同样读此缓冲，故放 try 外）
    try
    {
        // IMethod::GetBlockSize(this, 0)（ORIG 0x080b4416）：未初始化返回 0；
        // 初始化时返回 m_blockSize。这里按等价字段访问内联（errOut=NULL 无写出）。
        int blockSize = cipher.m_bInit ? static_cast<int>(cipher.m_blockSize) : 0;

        if (blockSize < 0)
        {
            paddedLen = srcLen;
        }
        else if (static_cast<int>(srcLen) % blockSize == 0)
        {
            paddedLen = srcLen;
        }
        else
        {
            paddedLen = (static_cast<int>(srcLen) / blockSize + 1) * blockSize;
        }

        // IMethod::Pad(pwd, len, NULL)（ORIG 0x080b44a6）：未初始化跳过；
        // rem=len%blockSize 非零时按 m_padding 填 padLen 字节。
        if (blockSize >= 0 && cipher.m_bInit && cipher.m_padding >= 0)
        {
            int rem = static_cast<int>(srcLen) % cipher.m_blockSize;
            if (rem != 0)
            {
                int padLen = cipher.m_blockSize - rem;
                char* p = pwd + static_cast<int>(srcLen);
                for (int i = 0; i < padLen; ++i, ++p)
                {
                    switch (cipher.m_padding)
                    {
                    case 1:
                        *p = ' ';                     // 0x20
                        break;
                    case 2:
                        *p = static_cast<char>(padLen);
                        break;
                    case 0:
                        *p = '\0';
                        break;
                    default:
                        break;                        // 其他 padding 不写
                    }
                }
            }
        }

        unsigned char encBuf[132];  // ORIG -0x120(%ebp) 区（块缓冲 0x84 使用域）
        // ORIG 经 vtable+0xc 虚调用（slot=Encrypt）
        cipher.Encrypt(pwd, reinterpret_cast<char*>(encBuf), paddedLen);

        unsigned int hexLen = paddedLen * 2 + 1;  // ORIG -0x14 更新为奇数 hex 长度
        DNFFLib::Binary2Hex(encBuf, static_cast<int>(paddedLen), hexBuf);
        hexBuf[hexLen] = '\0';

        if (strncmp(hexBuf, clientHash, 0x84) != 0)
        {
            LogManager::logFormat(
                1, "SimpleSSO.cpp",
                "bool WongWork::CSimpleSSO::_checkValiedPassword(char*, const char*, CRijndael&)",
                0xee,
                "_checkValiedPassword() diff password server(%s), client(%s)",
                hexBuf, clientHash);
            return false;
        }
        return true;
    }
    catch (...)
    {
        LogManager::logFormat(
            1, "SimpleSSO.cpp",
            "bool WongWork::CSimpleSSO::_checkValiedPassword(char*, const char*, CRijndael&)",
            0xe7,
            "_checkValiedPassword() exception server(%s), client(%s)", "", clientHash);
        return false;
    }
}

// ---- updateHumanCertify(mId, accountFail)  ORIG 0x08603226
bool WongWork::CSimpleSSO::updateHumanCertify(unsigned int mId, int accountFail)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG call memcpy
    int totalAccountFailInc = (accountFail != 0) ? 1 : 0;  // setne/movzbl
    char* idStr = NumberToString(mId, 0);
    m_mysql->set_query(
        "upDate %s set last_play_time=unix_timestamp(now()),account_fail=%d,"
        "total_account_fail=total_account_fail+%d where m_id=%s",
        tableName, accountFail, totalAccountFailInc, idStr);
    if (!m_mysql->exec(true))
        return false;
    if (m_mysql->getAffectedRowCount() == 0)
        return false;
    return true;
}

// ---- updateSSOExpireTime(mId, data)  ORIG 0x08602906
bool WongWork::CSimpleSSO::updateSSOExpireTime(unsigned int mId,
                                               const stUpdateSSOExpireTime_t& data)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG call memcpy
    char* idStr = NumberToString(mId, 0);

    unsigned int reportCnt = data.m_reportCnt;             // +0x00
    unsigned int tradeGoldDaily = data.m_tradeGoldDaily;   // +0x04
    unsigned int dungeonGainGold = data.m_dungeonGainGold; // +0x08
    float rating = data.m_rating;                          // +0x0c
    unsigned char tutorialSkipable = data.m_tutorialSkipable;  // +0x10

    int lastPlayBase = GlobalData::s_systemTime_.getCurSec();  // 第一次 getCurSec
    int expireBase = GlobalData::s_systemTime_.getCurSec();    // 第二次 getCurSec

    m_mysql->set_query(
        "upDate %s set expire_time=%d,last_play_time=%d,report_cnt=%d,"
        "trade_gold_daily=%d,dungeon_gain_gold=%d,rating=%f,tutorial_skipable=%d"
        " where m_id=%s",
        tableName, expireBase + 0xb4, lastPlayBase, reportCnt, tradeGoldDaily,
        dungeonGainGold, static_cast<double>(rating), tutorialSkipable, idStr);
    if (!m_mysql->exec(true))
        return false;
    if (m_mysql->getAffectedRowCount() == 0)
        return false;
    return true;
}

// ---- updateSSOExpireTime(mId, loginTime)  ORIG 0x08602a4a
bool WongWork::CSimpleSSO::updateSSOExpireTime(unsigned int mId, unsigned int loginTime)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG call memcpy
    char* idStr = NumberToString(mId, 0);
    int lastPlayBase = GlobalData::s_systemTime_.getCurSec();  // 第一次 getCurSec
    int expireBase = GlobalData::s_systemTime_.getCurSec();    // 第二次 getCurSec
    m_mysql->set_query(
        "upDate %s set login_time=%d, expire_time=%d, last_play_time=%d where m_id=%s",
        tableName, loginTime, expireBase + 0xb4, lastPlayBase, idStr);
    return m_mysql->exec(true);
}
