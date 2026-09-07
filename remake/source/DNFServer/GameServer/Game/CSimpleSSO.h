#ifndef GAME_CSIMPLESSO_H_
#define GAME_CSIMPLESSO_H_

// WongWork::CSimpleSSO —— SSO 会话密码/过期时间数据库侧（ORIG 0x86027fc 起）。
// 权威声明点（原 GlobalData.cpp 局部类定义收敛至此；布局不变）。
// 逐符号证据见 docs/class_func_reports/WongWork__CSimpleSSO/。

class MySQL;
class CRijndael;
#include "CRijndael.h"   // m_keys[31][24] 数组成员需完整类型（CRijndael sizeof=0x47c，与 ORIG 一致）

namespace WongWork
{

class CSimpleSSO
{
public:
    // ORIG stOutputData_t 布局（_getGameStartTime 0x8602da6 读出 + ctor 0x818fb28 置零）
    struct stOutputData_t
    {
        int m_lastPlayTime;               // +0x00
        int m_accountFail;                // +0x04
        int m_totalAccountFail;           // +0x08
        int m_reportCnt;                  // +0x0c
        char m_reliableFlag;              // +0x10（+0x11..+0x13 pad，ctor 不清零区仅清本字节）
        unsigned int m_tradeGoldDaily;    // +0x14
        char m_loginIp[0x14];             // +0x18..+0x2b
        char m_securityFlag;              // +0x2c（ctor 仅清首字节）
        unsigned int m_dungeonGainGold;   // +0x30
        int m_schoolId;                   // +0x34
        float m_rating;                   // +0x38
        unsigned char m_tutorialSkipable; // +0x3c
        unsigned char m_eventCharacFlag;  // +0x3d（+0x3e..+0x3f pad）
        unsigned int m_loginTime;         // +0x40
        unsigned int m_expireTime;        // +0x44

        stOutputData_t();  // ORIG W 0x0818fb28（逐字段置零，sizeof 0x48）
    };

    // ORIG updateSSOExpireTime(uint, const&) 0x8602906 读出：无 ORIG ctor 符号。
    struct stUpdateSSOExpireTime_t
    {
        unsigned int m_reportCnt;          // +0x00
        unsigned int m_tradeGoldDaily;     // +0x04
        unsigned int m_dungeonGainGold;    // +0x08
        float m_rating;                    // +0x0c
        unsigned char m_tutorialSkipable;  // +0x10（推断 sizeof 0x14，尾部 pad 无访问）
    };

    CSimpleSSO();      // ORIG W 0x082a7260
    ~CSimpleSSO();     // ORIG W 0x082a7352
    void expireLoginTime(unsigned int mId);  // ORIG T 0x08602b24
    void init(MySQL* mysql);  // ORIG T 0x086027fc

    void reset();  // ORIG T 0x086028b4：m_mysql = 0

    bool checkValidPassword(unsigned int mId, char const* serverPwd,
                            char const* clientPwd, stOutputData_t& out);  // ORIG T 0x086028c6（返回 bool）
    bool checkValidPassword_Jpn(unsigned int mId, char const* a, char const* b,
                                stOutputData_t& out);  // ORIG T 0x081b25ec
    // [b23S3C] bool 返回：ORIG 调用点 0x0818f7a3 call 后 `xor $0x1,%eax; test %al`
    // 直测转发结果（薄转发体 0x086031f6 末 call _getGameStartTime; leave; ret 透传
    // %eax），void 声明无法表达该测试。
    bool getGameStartTime(unsigned int mId, long& loginTime, long& expireTime,
                          stOutputData_t& out);  // ORIG T 0x086031f6（薄转发 _getGameStartTime）
    bool _getGameStartTime(unsigned int mId, long& loginTime, long& expireTime,
                           stOutputData_t& out);  // ORIG T 0x08602da6
    bool _checkValiedPassword(char* pwd, char const* clientHash,
                              CRijndael& cipher);  // ORIG T 0x08602bd0
    bool updateHumanCertify(unsigned int mId, int accountFail);  // ORIG T 0x08603226
    bool updateSSOExpireTime(unsigned int mId,
                             const stUpdateSSOExpireTime_t& data);  // ORIG T 0x08602906
    bool updateSSOExpireTime(unsigned int mId, unsigned int loginTime);  // ORIG T 0x08602a4a

    // ORIG 0x093ac220（.data）：31*24 个 16 字节 SSO 会话密钥串（定义在
    // GlobalData.cpp，include GlobalData_SSO_keys.inc）。
    static const char* const strKeyTable_[31 * 24];

private:
    // ORIG 布局：CRijndael m_keyTable[31][24]（每块 0x47c，共 0xd0860 字节）+
    // MySQL* m_mysql（+0xd0860）。2026-08-28 随 CRijndael sizeof 收至 0x47c，
    // 由字节区还原为真实数组成员；ctor/dtor 由编译器按 ORIG 同形生成。
    CRijndael m_keys[31][24];
    MySQL* m_mysql;
};

}  // namespace WongWork

#endif  // GAME_CSIMPLESSO_H_
