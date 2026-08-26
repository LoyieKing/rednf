// WongWorkSecurityCard_A.cpp — WongWork::CSecurityCard ORIG-only 成员（worklist A 段）
// 逐函数自 ORIG df_game_r 反汇编忠实转写（地址见各处注释）。
// 布局依赖：CUser.h CSecurityCard（+0x40 字节与 +0x44 requestState 由
// OnlyOrig_CUser 批次按 ORIG 补正后，本 TU 与真机偏移一致）。
#include "CUser.h"
#include <string.h>

namespace WongWork
{

// W 0x0826528a：mov 0x38(%eax),%eax —— 读 cert 计数（+0x38）。
int CSecurityCard::getCertCnt()
{
    return m_certCnt38;
}

// W 0x082652ae：++(+0x34) 后返回新值。
int CSecurityCard::incFailCnt()
{
    m_failCnt34 = m_failCnt34 + 1;
    return m_failCnt34;
}

// W 0x082652c8：movl $0x0,0x34(%eax)。
void CSecurityCard::resetFailCnt()
{
    m_failCnt34 = 0;
}

// W 0x084ebd82：byte 参数经 movzbl 写 +0x2f。
void CSecurityCard::setActivate(bool flag)
{
    m_activate2f = static_cast<unsigned char>(flag);
}

// W 0x08265296：byte 参数经 movzbl 写 +0x30。
void CSecurityCard::setCertified(bool flag)
{
    m_certified30 = static_cast<unsigned char>(flag);
}

// W 0x084ebd9a：char 参数经 movzbl 写字节 +0x31。
void CSecurityCard::setCertFlag(char flag)
{
    m_certFlag31 = static_cast<unsigned char>(flag);
}
// W 0x0822ef64：int 参数写入 +0x44。
void CSecurityCard::setRequestState(ENUM_REQUEST_STATE state)
{
    m_requestState44 = static_cast<int>(state);
}

// W 0x0822ef72：movzbl 0x31; cmp $0x1 → (0x31 == 1)。
bool CSecurityCard::isCertFlagOn()
{
    return m_certFlag31 == 1;
}

// W 0x0822ef32：+0x48 自增并返回新值。
int CSecurityCard::incCancelCnt()
{
    m_cancelCnt48 = m_cancelCnt48 + 1;
    return m_cancelCnt48;
}

// T 0x085fdd68 init：清 +0x2f/0x30/0x34/0x44/0x48/0x38/0x31/0x40。
// 注意字段顺序与 ORIG 清零序列一致；+0x40 为单字节占位区（init 只清该字节），
// 当前头以 m_pad40 区表达（不做匿名读写的其它语义）。
void CSecurityCard::init()
{
    m_activate2f    = 0;                                          // movb $0x0,0x2f
    m_certified30   = 0;                                          // movb $0x0,0x30
    m_failCnt34     = 0;                                          // movl $0x0,0x34
    m_requestState44= 0;                                          // movl $0x0,0x44
    m_cancelCnt48   = 0;                                          // movl $0x0,0x48
    m_certCnt38     = 0;                                          // movl $0x0,0x38
    m_certFlag31    = 0;                                          // movb $0x0,0x31
    *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(this) + 0x40) = 0; // movb $0x0,0x40
}

// T 0x085fde9c setSecurityCardInfo(char const*, bool, bool, int, int, int, char*)：
//   strncpy(this+8, serial, 0x27);
//   this[0x2f]=activate; this[0x30]=certified; *(int*)(this+0x34)=certCnt;
//   this[0x31]=(char)certFlag; *(int*)(this+0x38)=cancelCnt;
//   if (answers != NULL) { strncpy(this+0x3c, answers, 5); 每字节 -'0'; }
void CSecurityCard::setSecurityCardInfo(char const* serial, bool activate,
                                        bool certified, int certCnt,
                                        int certFlag, int cancelCnt,
                                        char* answers)
{
    char* base = reinterpret_cast<char*>(this);
    strncpy(base + 8, serial, 0x27);
    m_activate2f  = activate ? 1 : 0;
    m_certified30 = certified ? 1 : 0;
    m_failCnt34   = certCnt;
    m_certFlag31  = static_cast<unsigned char>(certFlag);
    m_certCnt38   = cancelCnt;
    if (answers != reinterpret_cast<char*>(0))
    {
        char* q = reinterpret_cast<char*>(m_question3c);
        strncpy(q, answers, 5);
        for (int i = 0; i < 4; ++i)
            q[i] = static_cast<char>(q[i] - '0');
    }
}

} // namespace WongWork
