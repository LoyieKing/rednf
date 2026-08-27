// ============================================================================
// W9_SDCCommon.cpp —— SDCActiveStatusSyncCommon 全部 ORIG 方法 +
// _SDC::isObjectType + GetUniformVelocityFloatSDC 真实实现。
// 声明见 DataManagerInitDepends.h；依据 docs/class_func_reports/
// SDCActiveStatusSyncCommon/*.md 与 ORIG objdump 逐函数转写。
//
// 关键常量（ORIG .rodata）：0x08af4c10=1000.0f、0x08af4c14=3.0f、
//   0x08af4c18=1.5f。位掩码（getActiveStatusDurationServer）：
//   0x385aa → ROUND(1000.0f × power1)；0x3a55 → DURATION×1000；0x4000 → 0。
// ============================================================================

#include <algorithm>
#include <vector>
#include "GlobalData.h"           // CDataManager 前置（头要求其后包含）
#include "STStackableScript.h"    // ENUM_MODULE_TYPE
#include "sync_script.h"          // STEquipmentScript 完整类型（头注释要求）
#include "DNFLexWrapper.h"
#include "PvPTypes.h"
#include "DisJoint.h"
#include "StrikerScript.h"
#include "CharacManageScript.h"
#include "CItemUpgrade_Separate.h"
#include "DataManagerInitDepends.h"

extern float ACTIVESTATUS_POWER1[36];       // B 0x093f5900（owner ScriptDepsA.cpp）
extern float ACTIVESTATUS_POWER2[36];       // B 0x093f59a0
extern float ACTIVESTATUS_ETC_PARAMETERS[18][8];  // B 0x093f5a40
extern float ACTIVESTATUS_POWER1[36];                      // B 0x093f5900（owner ScriptDepsA.cpp）
extern float ACTIVESTATUS_POWER2[36];                      // B 0x093f59a0
extern float ACTIVESTATUS_ETC_PARAMETERS[18][8];           // B 0x093f5a40
extern float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE;        // D 0x0936e48c
extern float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER;  // D 0x0936e490
extern int ACTIVESTATUS_MAXLEVEL;                          // D 0x0936e484

static inline int round_f(float v)
{
    return static_cast<int>(v + (v >= 0.0f ? 0.5f : -0.5f));
}

// _Z26GetUniformVelocityFloatSDCffii @0x088924c1
float GetUniformVelocityFloatSDC(float v0, float v1, int cur, int max)
{
    if (cur >= max)
        return v1;
    if (cur <= 0)
        return v0;
    return v0 + (v1 - v0) * (float)cur / (float)max;
}

// _SDC::isObjectType @0x080950e8
namespace _SDC
{
bool isObjectType(int objectType, int code)
{
    unsigned int andv = (unsigned int)(code & objectType);
    return (andv >> 8 << 8) | (andv == (unsigned int)code ? 1u : 0u);
}
}  // namespace _SDC

// ============================ SDCActiveStatusSyncCommon ============================

float SDCActiveStatusSyncCommon::getActiveStatusPower1(int idx, int level)
{
    return GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER1[idx * 2],
                                      ACTIVESTATUS_POWER1[idx * 2 + 1],
                                      level - 1, ACTIVESTATUS_MAXLEVEL - 1);
}

float SDCActiveStatusSyncCommon::getActiveStatusPower2(int idx, int level)
{
    return GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER2[idx * 2],
                                      ACTIVESTATUS_POWER2[idx * 2 + 1],
                                      level - 1, ACTIVESTATUS_MAXLEVEL - 1);
}

float SDCActiveStatusSyncCommon::getActiveStatusEtcParameter(int idx, int level,
                                                             int j)
{
    return GetUniformVelocityFloatSDC(ACTIVESTATUS_ETC_PARAMETERS[idx][j * 2],
                                      ACTIVESTATUS_ETC_PARAMETERS[idx][j * 2 + 1],
                                      level - 1, ACTIVESTATUS_MAXLEVEL - 1);
}

float SDCActiveStatusSyncCommon::getUserToUserDamageRate(bool deathTower)
{
    if (deathTower)
        return ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER;
    return ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE;
}

int SDCActiveStatusSyncCommon::getActiveStatusDurationClient(
    int, int, IRDActiveObject*)
{
    return 0;
}

void SDCActiveStatusSyncCommon::getDuration(ENUM_ACTIVESTATUS status,
                                            int level, void* param)
// 0x0808dc7a：param（DataSend 视图）尾部三 int 作 objectType/a4/hpPenalty。
{
    const unsigned char* p = static_cast<const unsigned char*>(param);
    getActiveStatusDurationServer(
        status, level,
        *reinterpret_cast<const int*>(p + 0x29),
        *reinterpret_cast<const int*>(p + 0x2d),
        *reinterpret_cast<const int*>(p + 0x31));
}

int SDCActiveStatusSyncCommon::getActiveStatusDurationServer(
    int status, int level, int objectType, int a4, int hpPenalty)
{
    int duration = 0;
    if ((unsigned int)status < 0x12)
    {
        unsigned int mask = 1u << status;
        if ((mask & 0x385aa) != 0)
        {
            duration = round_f(1000.0f * getActiveStatusPower1(status, level));
        }
        else if ((mask & 0x3a55) != 0)
        {
            extern int ACTIVESTATUS_DURATION[18];   // B 0x093f58a0
            duration = ACTIVESTATUS_DURATION[status] * 1000;
        }
        else if ((mask & 0x4000) != 0)
        {
            duration = 0;
        }
    }

    if (_SDC::isObjectType(objectType, 0x211))
    {
        float f = static_cast<float>(duration);
        float x = static_cast<float>(hpPenalty);
        duration -= static_cast<int>((f * ((x / 3.0f) / x)) + 0.5f);
    }
    if (duration < 0)
        duration = 0;
    return duration;
}

bool SDCActiveStatusSyncCommon::getBurnDamage(bool& flag,
                                              STSDCActiveStatusSyncDataSend* d,
                                              void*, int t)
{
    float dmg =
        GetUniformVelocityFloatSDC(0.0f, static_cast<float>(d->m_baseDamage),
                                   t, d->m_durationTick);
    if (0 < d->m_baseDamage)
        flag = false;
    if (d->m_isPvp != 0)
        dmg *= getUserToUserDamageRate(d->m_attackerType == 10);
    return round_f(dmg);
}

int SDCActiveStatusSyncCommon::getPoisonDamage(STSDCActiveStatusSyncDataSend* d,
                                               void*, int t)
{
    if (!(d->m_hpPercent < 100))
        return 0;

    float dmg = GetUniformVelocityFloatSDC(0.0f,
                                           static_cast<float>(d->m_baseDamage),
                                           t, d->m_durationTick);
    if (d->m_isPvp != 0)
        dmg *= getUserToUserDamageRate(d->m_attackerType == 10);
    return round_f(dmg);
}

int SDCActiveStatusSyncCommon::getDiseaseDamage(
    STSDCActiveStatusSyncDataSend* d, int t)
{
    if (!(d->m_hpPercent < 100))
        return 0;

    int dmg = round_f(GetUniformVelocityFloatSDC(
        0.0f, static_cast<float>(d->m_baseDamage), t, d->m_durationTick));
    if (d->m_isPvp != 0)
    {
        float rate = getUserToUserDamageRate(d->m_attackerType == 10);
        dmg = round_f(rate * static_cast<float>(dmg));
    }
    return dmg;
}

int SDCActiveStatusSyncCommon::getBleedingDamageServer(
    STSDCActiveStatusSyncDataSend* d)
{
    return round_f(1.5f * static_cast<float>(d->m_baseDamage));
}

int SDCActiveStatusSyncCommon::getBleedingDamageClient(
    STSDCActiveStatusSyncDataSend* d, bool, void*)
{
    int tickA = d->m_durationTick / 500;
    int tickB = 1;
    int tick = std::max(tickB, tickA);          // ORIG std::max(tickB, tickA) 序保持
    return round_f(static_cast<float>(d->m_baseDamage) / static_cast<float>(tick));
}

void SDCActiveStatusSyncCommon::makeEtcParameters(
    ENUM_ACTIVESTATUS status, int level, void* param, std::vector<int>* out,
    int* outPtr)
// 0x0808d4b4：先回填 outPtr，再转发三参版本（param 作 DataSend 视图）。
{
    if (outPtr != 0)
    {
        const unsigned char* p = static_cast<const unsigned char*>(param);
        *outPtr = getActiveStatusDurationServer(
            status, level,
            *reinterpret_cast<const int*>(p + 0x29),
            *reinterpret_cast<const int*>(p + 0x2d),
            *reinterpret_cast<const int*>(p + 0x31));
    }
    if (out != 0)
        makeEtcParameters(status, level, *out);
}

void SDCActiveStatusSyncCommon::makeEtcParameters(ENUM_ACTIVESTATUS status,
                                                  int level,
                                                  std::vector<int>& out)
// 0x0808d50e：switch 表逐 case push_back(ROUND(ETC_PARAMETER))。
{
#define PUSH_ETC(j) \
    out.push_back(round_f(getActiveStatusEtcParameter(status, level, j)))
    switch (status)
    {
    case 0:
        PUSH_ETC(0);
        PUSH_ETC(1);
        break;
    case 2:
        PUSH_ETC(0);
        break;
    case 4:
        PUSH_ETC(0);
        break;
    case 7:
        PUSH_ETC(0);
        break;
    case 9:
        PUSH_ETC(0);
        PUSH_ETC(1);
        out.push_back(1);
        out.push_back(3000);
        break;
    case 10:
        PUSH_ETC(0);
        break;
    case 11:
        PUSH_ETC(0);
        break;
    case 12:
        PUSH_ETC(0);
        PUSH_ETC(1);
        break;
    case 13:
        PUSH_ETC(0);
        PUSH_ETC(1);
        PUSH_ETC(2);
        PUSH_ETC(3);
        break;
    case 15:
        out.push_back(1);
        break;
    case 17:
        PUSH_ETC(0);
        break;
    default:
        break;
    }
#undef PUSH_ETC
}

void SDCActiveStatusSyncCommon::getActiveStatusDamageDurationForSDC(
    char attackType, int statusLevel, int* outPtr, int* outB,
    STSDCActiveStatusSyncDataSend* d, void*)
// 0x0808df82：栈上备份 d、经 makeEtcParameters 取 duration / 参数，
// 按 attackType 分派伤害计算并写入 *outB。
{
    if (outB == 0 || d == 0 || outPtr == 0)
        return;

    STSDCActiveStatusSyncDataSend local = *d;      // ORIG 帧 -0x4a..(-0x16) 全拷
    std::vector<int> params;

    makeEtcParameters(static_cast<ENUM_ACTIVESTATUS>(attackType), statusLevel,
                      &local, &params, outPtr);

    if (!params.empty())
    {
        // ORIG：local.baseDamage=params[0]，local.durationTick=*outB。
        local.m_baseDamage = params[0];
        local.m_durationTick = *outB;
    }

    switch (attackType)
    {
    case 2:                                        // poison
        *outB = getPoisonDamage(&local, 0, local.m_durationTick);
        break;
    case 6:                                        // disease
    case 7:
        *outB = local.m_baseDamage;
        break;
    case 9:                                        // burn
    {
        bool flag = true;
        *outB = getBurnDamage(flag, &local, 0, local.m_durationTick);
        break;
    }
    case 11:                                       // bleeding(server)
        *outB = getBleedingDamageServer(&local);
        break;
    default:
        break;
    }
}
