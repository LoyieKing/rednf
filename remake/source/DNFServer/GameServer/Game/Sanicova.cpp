// df_game_r Game/ Sanicova 命名空间类实现（独立 TU，从 GameStubs.cpp 拆出）。
// LinkMisc5 收口：CPad 全字段访问器真实化（ORIG 反汇编逐条核对），并承载
// WongWork::CSecurityCard 补充方法与 CUser::sendBackSecurityAuthRequest。
#include "CUser.h"

#include "Cipher.h"
#include "GameRand.h"    // get_rand_int
#include "GlobalData.h"  // s_systemTime_ / s_secu_cipher_keystring_manager_
#include "PacketGuard.h"

namespace Sanicova
{

// ORIG T 0x0859957c _ZN8Sanicova4CPad4initEv：
// activate/certified 清零 → memset(this,0,0x24) → failCnt/requestState/cancelCnt
// 清零 → +0x2c/+0x26/+0x2d 字节清零 → lastGenTime=getCurSec() → advide/changedPwd
// 清零 → GenerateRandomKeyIdx → GenerateRandomShakeData。
void CPad::init()
{
    m_activate24 = 0;
    m_certified25 = 0;
    memset(m_pad, 0, sizeof(m_pad));           // ORIG memset(this,0,0x24)
    m_failCnt38 = 0;                           // +0x38（dword）
    m_requestState40 = Sanicova::CPad::eRequestStateNone;  // +0x40（ORIG setRequestState(0)）
    m_cancelCnt44 = 0;                         // +0x44
    m_pad2c[0] = 0;                            // +0x2c（字节）
    m_version26 = 0;                           // +0x26
    m_pad2c[1] = 0;                            // +0x2d（字节）
    m_lastGenTime30 = (int)GlobalData::s_systemTime_.getCurSec();
    m_advidePwd = 0;                           // +0x3c
    m_changedPwd = 0;                          // +0x3d
    GenerateRandomKeyIdx();
    GenerateRandomShakeData();
}

// ORIG T 0x08599616 _ZN8Sanicova4CPad5resetEv：已认证时读取 +0x28 认证时间戳，
// 有效期 0xb4（180s）：checkTime+0xb4 < 当前秒则 setCertified(false)；前后各打
// cMyTrace 日志（[Before]/[After]Goblin Certified，line 65/74）。
void CPad::reset()
{
    if (isCertified())
    {
        int checkTime = (int)(m_checkTime28 + 0xb4);
        int curTime = (int)time(0);
        cMyTrace trace1(__PRETTY_FUNCTION__, 65, 0);
        trace1("[Before]Goblin Certified check_time(%d), cur_time(%d), Certified(%d)",
               checkTime, curTime, m_certified25);
        if (checkTime < curTime)
            setCertified(false);
        cMyTrace trace2(__PRETTY_FUNCTION__, 74, 0);
        trace2("[After]Goblin Certified check_time(%d), cur_time(%d), Certified(%d)",
               checkTime, curTime, m_certified25);
    }
}

// ORIG T 0x0859979c _ZN8Sanicova4CPad19isTimeAttackSeccessEv：取全局当前秒
// （结果存局部未参与判定），恒返回 true。
bool CPad::isTimeAttackSeccess()
{
    int curTime = (int)GlobalData::s_systemTime_.getCurSec();
    return true;
}

int CPad::getCancelCnt() { return m_cancelCnt44; }        // ORIG W 0x0822efb4

int CPad::incCancelCnt() { m_cancelCnt44 += 1; return m_cancelCnt44; }   // ORIG W 0x0822ef9a

void CPad::setCancelCnt(int count) { m_cancelCnt44 = count; }  // ORIG W 0x084ebdfa

int CPad::getRequestState() const { return m_requestState40; }  // ORIG W 0x0822efc0（const）

void CPad::setRequestState(ENUM_REQUEST_STATE state)      // ORIG W 0x0822efcc
{
    // ORIG 0822efcc: this-先 store（mov 0x8,%eax; mov 0xc,%edx; 写回）。
    // N76 同型化翻案（NL7 定律）：m_requestState40 int→ENUM_REQUEST_STATE
    // （CUser.h Main 代落；st10 探针实证同型 store 出 this-先形），落 identical。
    m_requestState40 = state;
}

bool CPad::isActivate() const { return m_activate24; }   // ORIG W 0x0822efda

void CPad::setActivate(bool flag) { m_activate24 = flag ; }  // ORIG W 0x084ebe08

bool CPad::isCertified() const { return m_certified25; } // ORIG W 0x0822efe6

void CPad::setCertified(bool flag) { m_certified25 = flag ; }  // ORIG W 0x0826530e

int CPad::getFailCnt() { return m_failCnt38; }            // ORIG W 0x0822ef8e

void CPad::setFailCnt(int cnt) { m_failCnt38 = cnt; }     // ORIG W 0x0828712c

char CPad::getVersionInfo() { return m_version26; }       // ORIG W 0x084ebdee

void CPad::setVersionInfo(char v) { m_version26 = v; }    // ORIG W 0x084ebdd6

// ORIG W 0x08265302：返回 +0x3c 字节（是否建议改密）。
bool CPad::getAdvideChangePwd()
{
    return m_advidePwd;
}

// ORIG W 0x084ebdb2：写 +0x3c 字节。
// P210：本 setter 与 setChangedPwd 定义位于 SendChagePwdRewardMail 之后，
// 使 4.4 对该 callee 采用保守帧（ORIG sub $0x18）。
// ORIG T 0x0859981e：置密改与建议标记为 0（发奖邮件已由上层处理）。
// 定义前置到 setter 之前（P210 callee 定义顺序定律）。
void CPad::SendChagePwdRewardMail(CUser* user)
{
    setChangedPwd(false);
    setAdvideChangePwd(false);
}

void CPad::setAdvideChangePwd(bool flag)
{
    m_advidePwd = flag ;
}

// ORIG W 0x084ebdca：movzbl 0x3d(%eax) 直返 +0x3d 字节。
bool CPad::getChangedPwd()
{
    return m_changedPwd;
}

// ORIG T 0x085998a6：当前秒距上次生成 >20s 判有效并刷新时间戳。
bool CPad::isValidKeyGenTime()
{
    if ((GlobalData::s_systemTime_.getCurSec() - m_lastGenTime30) > 0x14)
    {
        m_lastGenTime30 = GlobalData::s_systemTime_.getCurSec();
        return true;
    }
    return false;
}

// ORIG T 0x08599918：+0x2e = get_rand_int(5000)。
void CPad::GenerateRandomKeyIdx()
{
    m_keyIdx2e = (unsigned short)get_rand_int(5000);
}

// ORIG T 0x08599946：+0x34[i] = get_rand_int(200)+20，i∈[0,4)。
void CPad::GenerateRandomShakeData()
{
    int i = 0;
    for (i = 0; i < 4; ++i)
        m_shake34[i] = (char)(get_rand_int(200) + 0x14);
}

// ORIG T 0x085998ea：key 生成时间有效时刷新 keyIdx 与扰动数据。
void CPad::GenerateRandomData()
{
    if (isValidKeyGenTime())
    {
        GenerateRandomKeyIdx();
        GenerateRandomShakeData();
    }
}

// ORIG T 0x0859984c：min(+0x2e, 0x1387)。
unsigned short CPad::GetSelectedCipherIdx()
{
    unsigned short limit = 0x1387;
    return std::min(m_keyIdx2e, limit);
}

// ORIG T 0x08599872：按 +0x2e 索引读全局密钥管理器表（0x940be80 基址，
// 步长 16 字节；idx==0 取 +4 槽、否则取 +8 槽 → manager[key*4+1/2]）。
int CPad::GetKeyIdx(int idx)
{
    if (idx == 0)
        return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].keyId;
    return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].tblId;
}

// ORIG T 0x08599936：读 +0x34[idx] 字节。
char CPad::GetShakeData(int idx)
{
    return m_shake34[idx];
}

void CPad::setChangedPwd(bool flag)  // ORIG W 0x085999e8：写 +0x3d
{
    m_changedPwd = flag ;
}


// ORIG T 0x085997b8：建议改密时复位建议并触发发奖（无角色则仅置密改标记）。
void CPad::SendChagePwdReward(CUser* user)
{
    if (getAdvideChangePwd())
    {
        setAdvideChangePwd(false);
        if (user->getCurCharacNo() == 0)
            setChangedPwd(true);
        else
            SendChagePwdRewardMail(user);
    }
}

}  // namespace Sanicova

// ===========================================================================
// WongWork::CSecurityCard 补充方法（LinkMisc5）
// ===========================================================================

// ORIG W 0x0822ef58：读 +0x44。
int WongWork::CSecurityCard::getRequestState()
{
    return m_requestState44;
}

// ORIG T 0x085fddb2：问题四元组合法性检查后导出（越界/全零则重置为 0,0,1,1）。
void WongWork::CSecurityCard::generateQuestion(short& a, short& b, short& c,
                                               short& d)
{
    char q0 = m_question3c[0];
    char q1 = m_question3c[1];
    char q2 = m_question3c[2];
    char q3 = m_question3c[3];

    bool allZero = (q0 == 0 && q1 == 0 && q2 == 0 && q3 == 0);
    bool outOfRange =
        ((q0 < 0 || q0 > 9) || (q1 < 0 || q1 > 3) ||
         (q2 < 0 || q2 > 9) || (q3 < 0 || q3 > 3));
    if (allZero || outOfRange)
    {
        m_question3c[0] = 0;
        m_question3c[1] = 0;
        m_question3c[2] = 1;
        m_question3c[3] = 1;
        q0 = 0;
        q1 = 0;
        q2 = 1;
        q3 = 1;
    }
    a = (short)q0;
    b = (short)q1;
    c = (short)q2;
    d = (short)q3;
}

// ===========================================================================
// CUser::sendBackSecurityAuthRequest（报告 CUser/sendBackSecurityAuthRequest.md，
// ORIG T 0x08680706 sz 0x38f）：生成安全卡问题 → 随机加密算法号 → Pad 随机
// 数据 → Cipher 加密 → 0x98 包下发 → 置请求状态。
// ===========================================================================
void CUser::sendBackSecurityAuthRequest()
{
    short question[3];      // -0x94/-0x92/-0x90（问题四元组前三个）
    short answers[29];      // -0x8e..（第四元组起，29×short 缓冲）
    for (int i = 0; i < 3; ++i)
        question[i] = 0;
    for (int i = 0; i < 29; ++i)
        answers[i] = 0;
    question[0] = -1;

    int encryptedLen = 0x800;                       // -0x24
    unsigned char encrypted[2048];                  // -0x894
    for (int i = 0; i < 0x800; ++i)
        encrypted[i] = 0;

    WongWork::CSecurityCard* card = getSecurityCard();
    card->generateQuestion(question[0], question[1], question[2],
                           *(short*)&answers[0]);

    char algo = 1;                                  // -0x1e
    do
    {
        algo = (char)get_rand_int(0xe);
    } while (algo == 0 || algo == 5);

    Sanicova::CPad* pad = getPad();
    pad->GenerateRandomData();
    unsigned int cipherIdx = pad->GetSelectedCipherIdx() & 0xffff;

    int cipherKey = (int)algo;
    Cipher* cipher =
        GlobalData::s_secu_cipher_keystring_manager_[cipherIdx].cipher;
    cipher->Encrypt(cipherKey, (unsigned char*)question, 8, encrypted,
                    &encryptedLen);
    char certFlag = card->getCertFlag();            // -0x1d

    PacketGuard packet;
    packet.put_header(0, 0x98);
    packet.put_byte((int)algo);
    pad = getPad();
    packet.put_short((int)(pad->GetKeyIdx(0) & 0xffff));
    pad = getPad();
    packet.put_short((int)(pad->GetKeyIdx(1) & 0xffff));
    for (int i = 0; i < 4; ++i)
    {
        pad = getPad();
        packet.put_byte((int)(pad->GetShakeData(i) & 0xff));
    }
    packet.put_int(encryptedLen);
    packet.put_binary((char*)encrypted, encryptedLen);
    packet.put_byte((int)certFlag);
    packet.finalize(true);
    Send(packet);

    card = getSecurityCard();
    if (card->isActivate())
    {
        card->setRequestState(WongWork::CSecurityCard::eRequestStateAuth);
    }
    else
    {
        pad = getPad();
        if (pad->isActivate())
            pad->setRequestState(Sanicova::CPad::eRequestStateAuth);
    }
}
