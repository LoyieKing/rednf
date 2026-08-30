// df_game_r 还原 —— LinkClosureC 批次散件 producer（杂项组）。
// 全部函数逐报告/逐指令转写；报告缺失处已用 ORIG objdump 逐指令核实并标注。
// 覆盖符号：
//   Global::CBossTowerFunc::DieMob         T 0x081468aa
//   Secu_HackLogCheck::DieMob              T 0x0827a81c
//   PvPDieUserRevive(int,int,uint,int)     T 0x08636555
//   GetItemIdByAddInfo(Inven_Item*,int,int) W 0x084e91a4
//   Sanicova::CPad::{incFailCnt,resetFailCnt,getPassword,checkAnswer}
//                                          W 0x082652d8/0x082652f2/0x08265326 / T 0x08599712
//   StrikerScript::checkSkillIndex         T 0x08a9e868
//   CEnvironment::getProcessSequence       W 0x08234516
#include "BossTowerFunc.h"

 
 #include <cstring>
 #include <cstdio>
 

#include "CEnvironment.h"       // getProcessSequence 声明
 #include "CUser.h"              // CPad/Secu_HackLogCheck 声明（权威）
#include "CGameManager.h"       // G_CGameManager / GetPvp
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整定义
 #include "GlobalData.h"         // s_pcryptRijndael_MousePassword_
#include "Inven_Item.h"
#include "PvP_Room.h"           // CheckTimerKey/HandleTimerUserRevive
#include "RefPvpGrade.h"
#include "StrikerScript.h"
#include "CSystemTime.h"        // getCurTickCount（SetMonsterInfo/init224/225）
#include "WarField.h"           // （CGameManager/TIMER_MESSAGE 依赖链）

// encryptAES_ —— ORIG T 0x0846c398（MouseRegisterEJ.cpp 定义）。
char* encryptAES_(CRijndael* method, char* src, char* dst);

// ---------------------------------------------------------------------------
// ORIG 0x081468aa（报告 Global__CBossTowerFunc/DieMob.md）：
// getBossTower() 非空 → 虚槽 +0x24（handleKillMonster）(user,mobSeq,param,msg,0)；
// 恒返回 1。
int Global::CBossTowerFunc::DieMob(CUser* user, unsigned short mobSeq,
                                   unsigned short param,
                                   const MSG_MONSTER_DIE& msg, int* out)
{
    WongWork::CBossTower* tower = user->getBossTower();
    if (tower != 0)
        tower->handleKillMonster(user, mobSeq, param, msg, out);
    return 1;
}

// ---------------------------------------------------------------------------
// ORIG 0x0827a81c（报告 Secu_HackLogCheck/DieMob.md）：
// msg->m_fieldF(+0x0f short) != 0xffff 时：SetMonsterInfo(msg)；m_flag223/224/225
// 非 0 者分别以 Check223/224/225 结果刷新。
void Secu_HackLogCheck::DieMob(const MSG_MONSTER_DIE* msg)
{
    if (msg->m_fieldF == -1)
        return;

    SetMonsterInfo(msg);

    if (m_flag223 != 0)
        m_flag223 = static_cast<unsigned char>(Check223());
    if (m_flag224 != 0)
        m_flag224 = static_cast<unsigned char>(Check224());
    if (m_flag225 != 0)
        m_flag225 = static_cast<unsigned char>(Check225());
}

// ---------------------------------------------------------------------------
// ORIG 0x08636555（报告缺失；objdump 0x08636555..0x086365d8 逐指令核实）：
//   key = p4 + 0x33
//   room = G_CGameManager()->GetPvp(p1, (CUser*)0, key)
//   room == 0 → return 0
//   if (!room->CheckTimerKey((TIMER_MESSAGE)p4, p2)) return 0
//   room->HandleTimerUserRevive(p4)
//   return 1
// （p3 在 ORIG 中未被读取。）
int PvPDieUserRevive(int p1, int p2, unsigned int p3, int p4)
{
    (void)p3;
    int key = p4 + 0x33;
    PvP_Room* room = G_CGameManager()->GetPvp(p1, static_cast<CUser*>(0), key);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)p4, p2))
        return 0;
    room->HandleTimerUserRevive(p4);
    return 1;
}

// ---------------------------------------------------------------------------
// ORIG W 0x084e91a4（报告缺失；objdump 逐指令核实）：
// for i in 0..count-1: Inven_Item& it = items[i]（stride 0x3d）；
//   it.m_addInfo2(+0x07) == addInfo 且 != 0 → return it.m_addInfo(+0x02)；
// 未命中 return 0。（mangled 参数为非 const 指针 _Z18GetItemIdByAddInfoP10Inven_Itemii。）
int GetItemIdByAddInfo(Inven_Item* items, int count, int addInfo)
{
    for (int i = 0; i < count; ++i)
    {
        Inven_Item& it = items[i];
        if (it.m_addInfo2 == addInfo && it.m_addInfo2 != 0)
            return it.m_addInfo;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG W 0x0844d54c / 0x0844d55a（objdump：mov 0x134/0x138(%eax); ret）。
// ORIG W 0x082652d8（objdump：+0x38 自增返回新值）。
int Sanicova::CPad::incFailCnt()
{
    m_failCnt38 += 1;
    return m_failCnt38;
}

// ORIG W 0x082652f2：写 +0x38 = 0。
void Sanicova::CPad::resetFailCnt()
{
    m_failCnt38 = 0;
}

// ORIG W 0x08265326：返回 m_pad（密文首址，非 const）。
char* Sanicova::CPad::getPassword()
{
    return m_pad;
}

// ORIG T 0x08599712（报告 Sanicova__CPad/checkAnswer.md）：
// 清零 36 字节缓冲 ×2 → strncpy(answer, 8) → encryptAES_ → 与 m_pad 0x24 字节
// strncmp（ORIG call 0x0807e8c0 = strncmp@plt；密文二进制可能含 NUL，
// strncmp 遇 NUL 提前停止——ORIG 语义如此，不得改为 memcmp）。
bool Sanicova::CPad::checkAnswer(char* answer) const
{
    char plain[36];
    char cipher[36];
    memset(plain, 0, sizeof(plain));
    memset(cipher, 0, sizeof(cipher));

    strncpy(plain, answer, 8);
    encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, plain, cipher);
    return strncmp(m_pad, cipher, 0x24) == 0;
}

// encryptAES_ —— ORIG T 0x0846c398（MouseRegisterEJ.cpp 定义）。
char* encryptAES_(CRijndael* method, char* src, char* dst);

// ---------------------------------------------------------------------------
// ORIG 0x08a9e868（报告 StrikerScript/checkSkillIndex.md）：
// pass0 → getStrikerSkillVector(first,second)；pass1 → (first,0)；
// 向量为空指针返回 0；命中 m_field00 == skillIndex 返回 1。
bool StrikerScript::checkSkillIndex(short firstGrowType, short secondGrowType,
                                    int skillIndex)
{
    for (int pass = 0; pass < 2; ++pass)
    {
        std::vector<StrikerSkill>* vec =
            (pass == 0) ? getStrikerSkillVector(firstGrowType, secondGrowType)
                        : getStrikerSkillVector(firstGrowType, 0);
        if (vec == 0)
            return false;

        for (std::vector<StrikerSkill>::iterator it = vec->begin();
             it != vec->end(); ++it)
        {
            if (it->m_field00 == skillIndex)
                return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ORIG W 0x08234516（objdump：mov 0x1ac(%eax),%eax; ret）：
// m_serverEnvir(+0x198) + 0x14 = m_processSequence。
int CEnvironment::getProcessSequence() const
{
    return m_serverEnvir.m_processSequence;
}

// ---------------------------------------------------------------------------
// ORIG T 0x08a9e7e0 sz 0x88（getStrikerSkillVector.md 全文已读）：
// m_skillMap(+0x78).find((int)(unsigned short)first)；命中返回 it->second
// （vector<StrikerSkill>*），未命中返回 0。ORIG 汇编中 second 形参仅被零扩展
// 存放（-0xe 槽），从未参与 find key——忠实保留未用语义。
std::vector<StrikerSkill>* StrikerScript::getStrikerSkillVector(short first,
                                                                short second)
{
    int key = static_cast<int>(static_cast<unsigned short>(first));
    std::map<int, std::vector<StrikerSkill>*>::iterator it =
        m_skillMap.find(key);
    if (it != m_skillMap.end())
        return it->second;
    return 0;
}

// ===========================================================================
// Secu_MonsterCheck 9 函数族剩余 7 个（SetUserPtr/Reset/Secu_HackLogCheck::Init
// 已在 UserCoreDepsB2.cpp；DieMob 在本文件）。报告 Secu_MonsterCheck/
// {SetMonsterInfo,init223,init224,init225,Check223,Check224,Check225}.md 全文已读。
// ===========================================================================

// ORIG T 0x0827abfa sz 0x80（SetMonsterInfo.md）：
void Secu_MonsterCheck::SetMonsterInfo(const MSG_MONSTER_DIE* msg)
{
    m_curKillCount = msg->m_mobSeq;      // +0x28 ← msg+0x0d
    m_curX = msg->m_killPairA24;         // +0x2a ← msg+0xa24
    m_curY = msg->m_killPairA26;         // +0x2c ← msg+0xa26
    m_setTick = GlobalData::s_systemTime_.getCurTickCount();   // +0x30
    init223(msg);
    init224(msg);
    init225(msg);
}

// ORIG T 0x0827b288 sz 0x5c（init223.md）：active 未置 1 时记录首杀快照。
void Secu_MonsterCheck::init223(const MSG_MONSTER_DIE* msg)
{
    if (m_active223 != 1)
    {
        m_field08 = msg->m_mobSeq;       // +0x08 ← msg+0x0d
        m_field0a = msg->m_killPairA24;  // +0x0a ← msg+0xa24
        m_field0c = msg->m_killPairA26;  // +0x0c ← msg+0xa26
        m_field0e = 0;
        m_field10 = 0;
        m_active223 = 1;
    }
}

// ORIG T 0x0827b2e4 sz 0x50（init224.md）
void Secu_MonsterCheck::init224(const MSG_MONSTER_DIE* msg)
{
    if (m_active224 != 1)
    {
        m_field12 = msg->m_mobSeq;       // +0x12 ← msg+0x0d
        m_field14 = GlobalData::s_systemTime_.getCurTickCount();
        m_field18 = 0;
        m_field1a = 0;
        m_active224 = 1;
    }
}

// ORIG T 0x0827b334 sz 0x50（init225.md）
void Secu_MonsterCheck::init225(const MSG_MONSTER_DIE* msg)
{
    if (m_active225 != 1)
    {
        m_field1c = msg->m_mobSeq;       // +0x1c ← msg+0x0d
        m_field20 = GlobalData::s_systemTime_.getCurTickCount();
        m_field24 = 0;
        m_field26 = 0;
        m_active225 = 1;
    }
}

// ORIG T 0x0827ad98 sz 0x1e0（Check223.md）：击杀数快照一致 → 1；
// 坐标/击杀差值阈值判定 → 计数超限上报 0xdf 并清状态返 0，否则重置快照返 1。
char Secu_MonsterCheck::Check223()
{
    if (m_field08 == m_curKillCount)
        return 1;
    char result = 1;
    if (m_active223 != 0)
    {
        int dx = static_cast<int>(static_cast<unsigned short>(m_field0a)) -
                 static_cast<int>(static_cast<unsigned short>(m_curX));
        int dy = static_cast<int>(static_cast<unsigned short>(m_field0c)) -
                 static_cast<int>(static_cast<unsigned short>(m_curY));
        bool moveOk;
        if (dx > 0)
        {
            moveOk = (dx != 0);
        }
        else if (-dx <= 0xc7 && dy > 0)
        {
            moveOk = (dy != 0);
        }
        else
        {
            moveOk = (-dy <= 0x95);
        }
        if (moveOk)
        {
            m_field0e += 1;
            if (m_field0e <= 0xa)
            {
                if (m_field0e > 4)
                    m_field10 = 1;
            }
            else
            {
                m_field10 = 3;
                if (m_user != 0)
                {
                    m_user->getHackAnalyzer()->addServerHackCnt(
                        m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xdf),
                        3, 0, 0);
                }
                result = 0;
            }
        }
        else
        {
            if (m_field10 == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xdf),
                    1, 0, 0);
            }
            m_field08 = m_curKillCount;
            m_field0a = m_curX;
            m_field0c = m_curY;
            m_field0e = 0;
            m_field10 = 0;
        }
    }
    return result;
}

// ORIG T 0x0827af78 sz 0x187（Check224.md）：击杀间隔 1000ms 判定，上报 0xe0。
char Secu_MonsterCheck::Check224()
{
    if (m_field12 == m_curKillCount)
        return 1;
    char result = 1;
    if (m_active224 != 0)
    {
        int dt = m_setTick - m_field14;
        if (dt <= 0x3e8)
        {
            m_field18 += 1;
            if (m_field18 > 4)
            {
                if (m_field18 <= 0xa)
                {
                    m_field1a = 1;
                }
                else
                {
                    m_field1a = 3;
                    if (m_user != 0)
                    {
                        m_user->getHackAnalyzer()->addServerHackCnt(
                            m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe0),
                            3, 0, 0);
                    }
                    result = 0;
                }
            }
        }
        else
        {
            if (m_field1a == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe0),
                    1, 0, 0);
            }
            m_field18 = 0;
            m_field1a = 0;
        }
        m_field12 = m_curKillCount;
        m_field14 = m_setTick;
    }
    return result;
}

// ORIG T 0x0827b100 sz 0x187（Check225.md）：击杀间隔 2000ms 判定，上报 0xe1。
char Secu_MonsterCheck::Check225()
{
    if (m_field1c == m_curKillCount)
        return 1;
    char result = 1;
    if (m_active225 != 0)
    {
        int dt = m_setTick - m_field20;
        if (dt <= 1999)
        {
            m_field24 += 1;
            if (m_field24 > 4)
            {
                if (m_field24 <= 0xa)
                {
                    m_field26 = 1;
                }
                else
                {
                    m_field26 = 3;
                    if (m_user != 0)
                    {
                        m_user->getHackAnalyzer()->addServerHackCnt(
                            m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe1),
                            3, 0, 0);
                    }
                    result = 0;
                }
            }
        }
        else
        {
            if (m_field26 == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe1),
                    1, 0, 0);
            }
            m_field24 = 0;
            m_field26 = 0;
        }
        m_field1c = m_curKillCount;
        m_field20 = m_setTick;
    }
    return result;
}
