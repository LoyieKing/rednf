// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Sanicova/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "Sanicova/init.inc"
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
#include "Sanicova/reset.inc"
{
    // b28d-37 [N78/N82 封顶，ORIG 0x8?]：ORIG 内联 rep stos 清零 vs 4.4 循环/调用形（P116/N78 族）。
    if (isCertified())
    {
        int checkTime = (int)(m_checkTime28 + 0xb4);
        int curTime = (int)time(0);
        // [b28f-SweepGroupR3 P227] ORIG 0x08599616 在 C1 call **之前**右起求 varargs
        //   并把 `m_certified25` 读入 **callee-saved %ebx** 跨 ctor 保活
        //   （`movzbl 0x25(%eax),%eax; movzbl %al,%ebx` → `mov %ebx,0x10(%esp)`）；
        //   两语句形（具名 trace1/trace2）4.4 只在 ctor 后才求实参 ⇒ 每次重读内存
        //   + 少 push %ebx。改全表达式临时链即复现。
        cMyTrace(__PRETTY_FUNCTION__, 65, 0)(
            "[Before]Goblin Certified check_time(%d), cur_time(%d), Certified(%d)",
            checkTime, curTime, m_certified25);
        if (checkTime < curTime)
            setCertified(false);
        cMyTrace(__PRETTY_FUNCTION__, 74, 0)(
            "[After]Goblin Certified check_time(%d), cur_time(%d), Certified(%d)",
            checkTime, curTime, m_certified25);
    }
}

// ORIG T 0x0859979c _ZN8Sanicova4CPad19isTimeAttackSeccessEv：取全局当前秒
// （结果存局部未参与判定），恒返回 true。
#include "Sanicova/isTimeAttackSeccess.inc"
{
    int curTime = (int)GlobalData::s_systemTime_.getCurSec();
    return true;
}

#include "Sanicova/getCancelCnt.inc"

#include "Sanicova/incCancelCnt.inc"

#include "Sanicova/setCancelCnt.inc"

#include "Sanicova/getRequestState.inc"

#include "Sanicova/setRequestState.inc"
{
    // ORIG 0822efcc: this-先 store（mov 0x8,%eax; mov 0xc,%edx; 写回）。
    // N76 同型化翻案（NL7 定律）：m_requestState40 int→ENUM_REQUEST_STATE
    // （CUser.h Main 代落；st10 探针实证同型 store 出 this-先形），落 identical。
    m_requestState40 = state;
}

#include "Sanicova/isActivate.inc"

#include "Sanicova/setActivate.inc"

#include "Sanicova/isCertified.inc"

#include "Sanicova/setCertified.inc"

#include "Sanicova/getFailCnt.inc"

#include "Sanicova/setFailCnt.inc"

#include "Sanicova/getVersionInfo.inc"

#include "Sanicova/setVersionInfo.inc"

// ORIG W 0x08265302：返回 +0x3c 字节（是否建议改密）。
#include "Sanicova/getAdvideChangePwd.inc"
{
    return m_advidePwd;
}

// ORIG W 0x084ebdb2：写 +0x3c 字节。
// P210：本 setter 与 setChangedPwd 定义位于 SendChagePwdRewardMail 之后，
// 使 4.4 对该 callee 采用保守帧（ORIG sub $0x18）。
// ORIG T 0x0859981e：置密改与建议标记为 0（发奖邮件已由上层处理）。
// 定义前置到 setter 之前（P210 callee 定义顺序定律）。
#include "Sanicova/SendChagePwdRewardMail.inc"
{
    setChangedPwd(false);
    setAdvideChangePwd(false);
}

#include "Sanicova/setAdvideChangePwd.inc"
{
    m_advidePwd = flag ;
}

// ORIG W 0x084ebdca：movzbl 0x3d(%eax) 直返 +0x3d 字节。
#include "Sanicova/getChangedPwd.inc"
{
    return m_changedPwd;
}

// ORIG T 0x085998a6：当前秒距上次生成 >20s 判有效并刷新时间戳。
#include "Sanicova/isValidKeyGenTime.inc"
{
    if ((GlobalData::s_systemTime_.getCurSec() - m_lastGenTime30) > 0x14)
    {
        m_lastGenTime30 = GlobalData::s_systemTime_.getCurSec();
        return true;
    }
    return false;
}

// ORIG T 0x08599918：+0x2e = get_rand_int(5000)。
#include "Sanicova/GenerateRandomKeyIdx.inc"
{
    m_keyIdx2e = (unsigned short)get_rand_int(5000);
}

// ORIG T 0x08599946：+0x34[i] = get_rand_int(200)+20，i∈[0,4)。
#include "Sanicova/GenerateRandomShakeData.inc"
{
    int i = 0;
    for (i = 0; i < 4; ++i)
        m_shake34[i] = (char)(get_rand_int(200) + 0x14);
}

// ORIG T 0x085998ea：key 生成时间有效时刷新 keyIdx 与扰动数据。
#include "Sanicova/GenerateRandomData.inc"
{
    if (isValidKeyGenTime())
    {
        GenerateRandomKeyIdx();
        GenerateRandomShakeData();
    }
}

// ORIG T 0x0859984c：min(+0x2e, 0x1387)。
#include "Sanicova/GetSelectedCipherIdx.inc"
{
    unsigned short limit = 0x1387;
    return std::min(m_keyIdx2e, limit);
}

// ORIG T 0x08599872：按 +0x2e 索引读全局密钥管理器表（0x940be80 基址，
// 步长 16 字节；idx==0 取 +4 槽、否则取 +8 槽 → manager[key*4+1/2]）。
#include "Sanicova/GetKeyIdx.inc"
{
    if (idx == 0)
        return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].keyId;
    return GlobalData::s_secu_cipher_keystring_manager_[m_keyIdx2e].tblId;
}

// ORIG T 0x08599936：读 +0x34[idx] 字节。
#include "Sanicova/GetShakeData.inc"
{
    return m_shake34[idx];
}

#include "Sanicova/setChangedPwd.inc"
{
    m_changedPwd = flag ;
}


// ORIG T 0x085997b8：建议改密时复位建议并触发发奖（无角色则仅置密改标记）。
#include "Sanicova/SendChagePwdReward.inc"
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
#include "Sanicova/getRequestState_1.inc"
{
    return m_requestState44;
}

// ORIG T 0x085fddb2：问题四元组合法性检查后导出（越界/全零则重置为 0,0,1,1）。
#include "Sanicova/generateQuestion.inc"
                                               short& d)
{
    // [b28f-SweepGroupR3 判据 P0/P183] ORIG 0x085fddb2 全程**无具名局部槽**：4 个字段
    //   各自 `movzbl 0x3c+i(%eax),%eax; test %al,%al; jne <T>` 直测（widthscan：
    //   我方 clr_b our=2/cmp_b our=14 vs ORIG movzbl 16 处）⇒ 原码把 4 个 char
    //   与两个 bool **全部内联**到表达式/条件；原写的 q0..q3 + allZero/outOfRange
    //   具名局部各占槽并令条件退化为 `cmpb $0x0,-0xN(%ebp)`。
    if ((m_question3c[0] == 0 && m_question3c[1] == 0 &&
         m_question3c[2] == 0 && m_question3c[3] == 0) ||
        ((m_question3c[0] < 0 || m_question3c[0] > 9) ||
         (m_question3c[1] < 0 || m_question3c[1] > 3) ||
         (m_question3c[2] < 0 || m_question3c[2] > 9) ||
         (m_question3c[3] < 0 || m_question3c[3] > 3)))
    {
        m_question3c[0] = 0;
        m_question3c[1] = 0;
        m_question3c[2] = 1;
        m_question3c[3] = 1;
    }
    a = (short)m_question3c[0];
    b = (short)m_question3c[1];
    c = (short)m_question3c[2];
    d = (short)m_question3c[3];
}

// ===========================================================================
// CUser::sendBackSecurityAuthRequest（报告 CUser/sendBackSecurityAuthRequest.md，
// ORIG T 0x08680706 sz 0x38f）：生成安全卡问题 → 随机加密算法号 → Pad 随机
// 数据 → Cipher 加密 → 0x98 包下发 → 置请求状态。
// ===========================================================================
#include "Sanicova/sendBackSecurityAuthRequest.inc"
{
    // b28d-37 [N78/P60 封顶，ORIG 0x86?]：ORIG 内联 rep stos 清零 vs 4.4 循环/调用形（P116/N78 族）；236/230。
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
