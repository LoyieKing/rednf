// df_game_r Game/ AvatarRechargeServer 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 依据 docs/class_func_reports/AvatarRechargeServer/、ARAD__SCRIPT__AvatarRecharge/
// 与 ORIG 反汇编转写。
#include "AvatarRechargeServer.h"

#include <cstdio>
#include <string>
#include <string.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"       // cMyTrace
#include "MoreTypes.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile / unloadRDARScriptFile / Scan*

namespace ARAD { namespace DISPATCHER {
    void make_internal_stream_jpn(CStreamGuard& guard, int type, int param)
    {
        guard << (int)0x2dc;
        guard << param;
        guard << type;
    }
}}

// ============================================================================
// AvatarRechargeServer —— ORIG C1 0x08190200 / D1 0x08190246 /
// Load 0x0819026c / Destroy 0x081902de / Test 0x08190266
// ============================================================================

// ORIG C1：SCRIPT 子对象构造 + 清 +0x54 与字节 +0x58（+0x00 上限字段不初始化，忠实 ORIG）。
AvatarRechargeServer::AvatarRechargeServer()
    : m_enduranceReduceRate(0),
      m_bUseEnduranceReduceRate(false)
{
}

AvatarRechargeServer::~AvatarRechargeServer()
{
}

// ORIG 0x0819026c：cMyTrace(__PRETTY_FUNCTION__,0x22,9,true,false) +
// "\t- Loading Avatar Recharge script - "；loadScript()==0 即成功。
bool AvatarRechargeServer::Load()
{
    cMyTrace trace("virtual bool AvatarRechargeServer::Load()", 0x22, 9, true, false);
    trace("\t- Loading Avatar Recharge script - ");
    return m_script.loadScript() == 0;
}

// ORIG 0x081902de：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
void AvatarRechargeServer::Destroy()
{
}

// ORIG 0x08190266：纯 ret。
void AvatarRechargeServer::Test()
{
}

// ORIG 0x0819114c：非静态成员，find 于 this+0x38（= m_script.m_useEmblemGradeRateMap）。
int AvatarRechargeServer::GetRateFromEmblemGrade(int grade)
{
    std::map<int, int>::const_iterator it = m_script.m_useEmblemGradeRateMap.find(grade);
    if (it == m_script.m_useEmblemGradeRateMap.end())
        return 0;
    return it->second;
}

// ORIG 0x08190dea（静态）：Acquire 流 → make_internal_stream_jpn(0x31, -1) → put_binary 8 字节
// → DB_Q。不触碰 this，与 ORIG 一致保持静态。
void AvatarRechargeServer::SaveAvatarEnduranceUnit(arad::SigAradAvatarEnduranceSave& save)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("localjapan/Arad_EmblemReformServer.cpp", 0x10f);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x31, 0xffffffff);
    guard.put_binary((char*)&save, 8);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x08190eae（静态）：打包 key/value 后走引用重载。
void AvatarRechargeServer::SaveAvatarEnduranceUnit(int key, int value)
{
    arad::SigAradAvatarEnduranceSave save;
    memset(&save, 0, sizeof(save));
    *(int*)&save = key;
    *((int*)&save + 1) = value;
    SaveAvatarEnduranceUnit(save);
}

// ============================================================================
// ARAD::SCRIPT::AvatarRecharge —— ORIG C1 0x088b0dd0 / D1 0x088b0e46 /
// loadScript 0x088b0eba / clearScript 0x088b12d8 / 访问器 0x88b1314..0x88b13a6
// ============================================================================

// ORIG C1：三个 map 默认构造 + 清 +0x4c 已装载标志（+0x00 上限不初始化）。
ARAD::SCRIPT::AvatarRecharge::AvatarRecharge()
    : m_loaded(false)
{
}

ARAD::SCRIPT::AvatarRecharge::~AvatarRecharge()
{
}

// ORIG 0x088b12d8：上限清零 + 三 map clear。
void ARAD::SCRIPT::AvatarRecharge::clearScript()
{
    m_rechargeLimit = 0;
    m_emblemRechargeStoneMap.clear();
    m_emblemRechargeMap.clear();
    m_useEmblemGradeRateMap.clear();
}

int ARAD::SCRIPT::AvatarRecharge::getAvatarRecharageLimit()
{
    return m_rechargeLimit;   // ORIG return *(int*)this
}

bool ARAD::SCRIPT::AvatarRecharge::IsEmblemRechargeStone(int itemIndex)
{
    // ORIG 0x088b13a6：+0x04 map find 命中返回 true。
    return m_emblemRechargeStoneMap.find(itemIndex) != m_emblemRechargeStoneMap.end();
}

std::map<int, int>* ARAD::SCRIPT::AvatarRecharge::getEmblemRechargeStoneInfo()
{
    return &m_emblemRechargeStoneMap;   // ORIG return this+4
}

int ARAD::SCRIPT::AvatarRecharge::getEmblemRecharge(int grade)
{
    // ORIG 0x088b132a：+0x1c map 值或 0。
    std::map<int, int>::const_iterator it = m_emblemRechargeMap.find(grade);
    if (it == m_emblemRechargeMap.end())
        return 0;
    return it->second;
}

// ORIG 0x088b0eba loadScript。忠实转写：
//   已装载→0；打开失败→-1；重复键 printf（ORIG 无换行符）后分别 -2/-3/-4 且不卸载脚本；
//   主循环 ScanType 失败即 unloadRDARScriptFile + 置已装载 + 返回 0。
// [recharge use emblem info] 段 grade>=8 的行不消费数值直接退出内层（ORIG 同）。
int ARAD::SCRIPT::AvatarRecharge::loadScript()
{
    if (m_loaded)
        return 0;

    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_recharge_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        if (token == "[avatar recharge limit]")
        {
            m_rechargeLimit = ScanInt((bool*)NULL);
        }
        else if (token == "[emblem recharge stone info]")
        {
            for (;;)
            {
                int idx = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_emblemRechargeStoneMap.find(idx) != m_emblemRechargeStoneMap.end())
                {
                    printf("[emblem recharge stone info] duplicate item_index:%d", idx);
                    return -2;
                }
                m_emblemRechargeStoneMap.insert(std::make_pair(idx, value));
            }
        }
        else if (token == "[emblem recharge]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_emblemRechargeMap.find(grade) != m_emblemRechargeMap.end())
                {
                    printf("[emblem recharge] duplicate grade:%d", grade);
                    return -3;
                }
                m_emblemRechargeMap.insert(std::make_pair(grade, value));
            }
        }
        else if (token == "[recharge use emblem info]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                if (grade >= 8)
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_useEmblemGradeRateMap.find(grade) != m_useEmblemGradeRateMap.end())
                {
                    printf("[recharge use emblem info] duplicate grade:%d", grade);
                    return -4;
                }
                m_useEmblemGradeRateMap.insert(std::make_pair(grade, value));
            }
        }
    }

    unloadRDARScriptFile();
    m_loaded = true;
    return 0;
}
