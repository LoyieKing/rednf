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