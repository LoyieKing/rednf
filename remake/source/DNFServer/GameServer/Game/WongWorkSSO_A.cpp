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
        register char* idStr = NumberToString(mId, 0);
        m_mysql->set_query("upDate %s set expire_time=%d where m_id=%s",
                           tableName, GlobalData::s_systemTime_.getCurSec(),
                           idStr);
        m_mysql->exec(true);
    }
}

// ---- _getGameStartTime(mId, login&, expire&, out&)  ORIG 0x08602da6
bool WongWork::CSimpleSSO::_getGameStartTime(unsigned int mId, long& loginTime,
                                             long& expireTime, stOutputData_t& out)
{
    // [b28d-solo-WongWorkSSO_A] ORIG 0x08602da6 复原：
    // ① idStr 无槽 ⇒ P0 内联实参 NumberToString；② exec/fetch 合并短路
    // （ORIG 汇聚块 mov$1/mov$0+test %al，P133/P214①）；③ loginValue/expireValue
    // 无预清（ORIG 直接 lea 传引用，P198①反例）；④ 原重建多一条 col++ 属真语义
    // bug：跳过 report_cnt 与 last_play_time 之间不存在的列，其后 11 列全部错位。
    char tableName[0x14];
    memset(tableName, 0, 0x14);       // ORIG +0x006..+0x01c call memset
    memcpy(tableName, "member_login", 0xd);  // ORIG +0x021..+0x037 call memcpy
    m_mysql->set_query(
        "seLect login_time,expire_time,report_cnt,last_play_time,account_fail,"
        "total_account_fail,reliable_flag,trade_gold_daily,login_ip,security_flag,"
        "dungeon_gain_gold,school_id,rating,tutorial_skipable,event_charac_flag"
        " from %s where m_id=%s",
        tableName, NumberToString(mId, 0));
    if (!m_mysql->exec(true) || !m_mysql->fetch())
        return false;
    if (m_mysql->get_n_rows() == 0)   // ORIG fetch 后空结果检查
        return false;

    unsigned int loginValue;   // col0 login_time  ORIG -0x24(%ebp)
    unsigned int expireValue;  // col1 expire_time ORIG -0x28(%ebp)

    int col = 0;
    if (!m_mysql->get_uint(col++, loginValue))
        return false;
    if (!m_mysql->get_uint(col++, expireValue))
        return false;
    if (!m_mysql->get_int(col++, out.m_reportCnt))  // report_cnt +0x0c
        return false;
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
    // ORIG 尾部：*param_2=t1; *param_3=t2; +0x40=t1; +0x44=t2
    loginTime = static_cast<long>(loginValue);
    expireTime = static_cast<long>(expireValue);
    out.m_loginTime = loginValue;
    out.m_expireTime = expireValue;
    return true;
}

// ---- getGameStartTime(...)  ORIG 0x086031f6：薄转发 _getGameStartTime
// [b23S3C] bool 透传：ORIG 体末 `call _getGameStartTime; leave; ret`（%eax 直带
// al 结果，无槽），调用点 0x0818f7a8 xor$1/test%al 实证返回承载成败。
bool WongWork::CSimpleSSO::getGameStartTime(unsigned int mId, long& loginTime,
                                            long& expireTime, stOutputData_t& out)
{
    return _getGameStartTime(mId, loginTime, expireTime, out);
}

// ---- checkValidPassword(...)  ORIG 0x086028c6：两相邻 unsigned 局部作 long& 传入；
// ORIG 返回 bool（= _getGameStartTime 结果，GetAccountInfo 失败时记日志返回 false）。
bool WongWork::CSimpleSSO::checkValidPassword(unsigned int mId, char const* a,
                                              char const* b, stOutputData_t& out)
{
    // [b28d-solo-WongWorkSSO_A] ORIG 0x086028c6：帧 0xf8 由 dead char[0xc0] 复现
    // （P31/P232 未读写局部仍占帧）；两个 long 局部落 -0x14/-0x18 需两个未使用
    // int 先占 -0xc/-0x10（P232 死局部占槽）。实参必须为真 long 左值直传——
    // unsigned+reinterpret_cast 引用形触发 4.4 提前 lea(%edx/%eax)+ecx 中转（P0 反例）。
    char framePad[0xc0];
    (void)framePad;
    int slotPad1;
    (void)slotPad1;
    int slotPad2;
    (void)slotPad2;
    long v2 = 0;  // loginTime&  → ORIG -0x14(%ebp)（movl $0 在前）
    long v1 = 0;  // expireTime& → ORIG -0x18(%ebp)
    return _getGameStartTime(mId, v2, v1, out);
}

// ---- checkValidPassword_Jpn(...)  ORIG 0x081b25ec
bool WongWork::CSimpleSSO::checkValidPassword_Jpn(unsigned int mId, char const* a,
                                                  char const* b, stOutputData_t& out)
{
    // [b28d-solo-WongWorkSSO_A] ORIG 0x081b25ec：long 左值直传（非 cast 引用形）。
    long v2 = 0;  // loginTime&  → ORIG -0x1c(%ebp)（movl $0 在前）
    long v1 = 0;  // expireTime& → ORIG -0x20(%ebp)
    getGameStartTime(mId, v2, v1, out);
    // ORIG 判定形：call IsGm 后直接 test %al（bool 返回直测，无 XOR 前缀）
    // + je 跳尾 mov$1 —— if(IsGm){...} 正极性 + 尾部 mov $1;jmp 中继保持
    //（ORIG +0xae..+0xba，中继属在册 N 跨版本 lowering 保留）。
    // [b27 封顶] 残差 = getGameStartTime 实参装载序（ORIG 0x18 槽先 → lea
    // -0x20；4.4 恒 lea 先 → 0x18 后，P75 跨版本实参调度序不可定向）+
    // 尾 mov$1 中继缺失（if 形无中继，early-return 形反生 XOR 前缀——两形
    // 探针均不匹配 ORIG 组合）。语义/调用链/其余指令全等。
    if (GlobalData::s_GM_Manager->IsGm(mId))
    {
        if (strncmp(a, b, 0x84) == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2e, 0)(
                "[!]GM User Check Password Sucess=> m_id(%d)", mId);
        }
        // [b28d-D13] ORIG 0x081b263a mov $1,jmp 中继：if 内独立 return true
        // （ORIG 0x081b263c 尾部再一条 mov $1 共享出口）。
        return true;
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
    // [b28d-D13] ORIG 0x0860326c cmpl+setne %al+movzbl %al,%eax+mov %eax,-0xc
    // ⇒ 源码为 `int x = (accountFail != 0);`（非三目）。ORIG 0x08603290
    // NumberToString 结果经 %eax 直入 0x14(%esp)（无 idStr 槽）⇒ 内联实参（P0）。
    int totalAccountFailInc = (accountFail != 0);
    m_mysql->set_query(
        "upDate %s set last_play_time=unix_timestamp(now()),account_fail=%d,"
        "total_account_fail=total_account_fail+%d where m_id=%s",
        tableName, accountFail, totalAccountFailInc, NumberToString(mId, 0));
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
    // [b28d-solo-WongWorkSSO_A] ORIG 0x08602906：89=89 指令、p167 8/8 CLEAR。
    // 已闭合：reportCnt 无槽（P0 内联实参）；idStr 走裸 register（P231 合规，
    // ORIG %ebx 无槽）；rating 为 double 局部（ORIG flds 0xc+fstpl -0x48）；
    // tutorialSkipable=(unsigned char) 转换（ORIG movzbl 双段）；两次 getCurSec
    // 内联实参（%esi/%ecx）；字段局部「先声明后赋值」两句式（P198①）。
    // N 封顶（NEAR，残差=槽号镜像+callee-saved 角色）：
    // ① 4.4-O0 恒将局部数组置于槽区最深处（探针 mA/mB/mC/mH：tableName 恒 -0x40，
    //    加 0x20 dead int 后 -0x60），4.1 按声明序（ORIG tableName -0x2c 在标量之上、
    //    标量 -0x4c..-0x3c 在数组之下）；标量/数组相对深度互为镜像，合法源形不可翻
    //    （N82/P112）。实测本件 tableName -0x40 vs -0x2c、标量 -0x2c..-0x1c vs
    //    -0x4c..-0x3c，帧 0x6c vs 0x7c（随槽基差）。
    // ② idStr/reportCnt/getCurSec#1 的 callee-saved 角色互换（ORIG ebx/edi/esi vs
    //    我方 edi/esi/ebx；N7/N82附）。语义/调用链/字段列/实参序全等。
    unsigned int tutorialSkipable;
    double rating;
    unsigned int dungeonGainGold;
    unsigned int tradeGoldDaily;
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG call memcpy
    register char* idStr = NumberToString(mId, 0);

    // ORIG movzbl 0x10(%eax),%eax; movzbl %al,%eax; mov %eax,-0x4c(%ebp)
    // ⇒ int 槽 + (unsigned char) 转换（P266/P267 同族）。
    tutorialSkipable = (unsigned char)data.m_tutorialSkipable;  // +0x10
    rating = data.m_rating;                          // +0x0c（double 槽 fstpl）
    dungeonGainGold = data.m_dungeonGainGold;        // +0x08
    tradeGoldDaily = data.m_tradeGoldDaily;          // +0x04

    m_mysql->set_query(
        "upDate %s set expire_time=%d,last_play_time=%d,report_cnt=%d,"
        "trade_gold_daily=%d,dungeon_gain_gold=%d,rating=%f,tutorial_skipable=%d"
        " where m_id=%s",
        tableName, GlobalData::s_systemTime_.getCurSec() + 0xb4,
        GlobalData::s_systemTime_.getCurSec(), data.m_reportCnt, tradeGoldDaily,
        dungeonGainGold, rating, tutorialSkipable, idStr);
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
    // [b28d-D13] ORIG 0x08602a70：idStr→%esi、lastPlayBase→%ebx、expireBase→%eax
    // （lea 0xb4(%eax),%edx）——三者均无栈槽 ⇒ 内联实参（P24/P0），右起求值序
    // NumberToString→getCurSec→getCurSec 与 ORIG 一致。ORIG 0x08602ae8 尾部
    // xor$1/test/je+双 mov ⇒ 源码 if(!exec())return false; return true;（P101）。
    m_mysql->set_query(
        "upDate %s set login_time=%d, expire_time=%d, last_play_time=%d where m_id=%s",
        tableName, loginTime,
        GlobalData::s_systemTime_.getCurSec() + 0xb4,
        GlobalData::s_systemTime_.getCurSec(),
        NumberToString(mId, 0));
    if (!m_mysql->exec(true))
        return false;
    return true;
}
