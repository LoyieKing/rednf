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
#include "STSDCActiveStatusSyncDataCLONG.h"  // CLONG 完整定义（双向 operator=）

extern float ACTIVESTATUS_POWER1[36];       // B 0x093f5900（owner ScriptDepsA.cpp）
extern float ACTIVESTATUS_POWER2[36];       // B 0x093f59a0
extern float ACTIVESTATUS_ETC_PARAMETERS[18][8];  // B 0x093f5a40
extern float ACTIVESTATUS_POWER1[36];                      // B 0x093f5900（owner ScriptDepsA.cpp）
extern float ACTIVESTATUS_POWER2[36];                      // B 0x093f59a0
extern float ACTIVESTATUS_ETC_PARAMETERS[18][8];           // B 0x093f5a40
extern float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE;        // D 0x0936e48c
extern float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER;  // D 0x0936e490
extern int ACTIVESTATUS_MAXLEVEL;                          // D 0x0936e484

// ORIG 各函数实证为 fistpl 截断（fnstcw/fldcw 设 RC=truncate），非四舍五入；
// 故一律用 static_cast<int>（GCC O0 生成相同截断序列）。

// ORIG T 0x0808d0a6：Send（packed 0x35）→ CLONG（0x44 展开）逐字段；
// +0/+4/+0x14/+0x18 符号扩展（movsbl），+0x8 零扩展（movzbl），余 dword。
STSDCActiveStatusSyncDataCLONG& STSDCActiveStatusSyncDataCLONG::operator=(
    const STSDCActiveStatusSyncDataSend& o)
{
    m_field00 = (int)(char)o.m_field00;
    m_field04 = (int)(char)o.m_field01;
    m_field08 = o.m_field02;
    m_field0c = o.m_baseDamage;
    m_field10 = o.m_durationTick;
    m_field14 = (int)(char)o.m_attackerType;
    m_field18 = (int)(char)o.m_isPvp;
    m_field1c = o.m_field0d;
    m_field28 = o.m_field19;    // ORIG 先拷 dest[0x28]（src[0x19]）再 dest[0x20]/0x24
    m_field20 = o.m_field11;
    m_field24 = o.m_field15;
    m_field2c = o.m_hpPercent;
    m_field30 = o.m_field21;
    m_field34 = o.m_field25;
    m_field38 = o.m_field29;
    m_field3c = o.m_field2d;
    m_field40 = o.m_field31;
    return *this;
}

// ORIG T 0x0808d18c：CLONG → Send 截断回填（packed 字节对齐）。
STSDCActiveStatusSyncDataSend& STSDCActiveStatusSyncDataSend::operator=(
    const STSDCActiveStatusSyncDataCLONG& o)
{
    m_field00 = (unsigned char)o.m_field00;
    m_field01 = (unsigned char)o.m_field04;
    m_field02 = (unsigned char)o.m_field08;
    m_baseDamage = o.m_field0c;
    m_durationTick = o.m_field10;
    m_attackerType = (unsigned char)o.m_field14;
    m_isPvp = (unsigned char)o.m_field18;
    m_field0d = o.m_field1c;
    m_field19 = o.m_field28;
    m_field11 = o.m_field20;
    m_field15 = o.m_field24;
    m_hpPercent = o.m_field2c;
    m_field21 = o.m_field30;
    m_field25 = o.m_field34;
    m_field29 = o.m_field38;
    m_field2d = o.m_field3c;
    m_field31 = o.m_field40;
    return *this;
}

// _Z26GetUniformVelocityFloatSDCffii @0x088924c1
float GetUniformVelocityFloatSDC(float v0, float v1, int cur, int max)
{
    if (cur >= max)
        return v1;
    if (cur <= 0)
        return v0;
    // ORIG 0x088924c1 x87 序：fildl cur 先于 flds v1 —— 乘法交换序复刻
    // （IEEE 乘法 bit-exact 同值）。
    return v0 + (float)cur * (v1 - v0) / (float)max;
}

// _SDC::isObjectType @0x080950e8
namespace _SDC
{
bool isObjectType(int objectType, int code)
{
    // ORIG 0x080950e8：无帧无槽，and 后直接 cmp 参数（P0 单表达式）。
    return (objectType & code) == code;
}

// ORIG T 0x0808d074：type<0xc 且 (1<<type & 0xac4)!=0（位 2/6/7/9/11）。
int isSyncActiveStatus(int type)
{
    if ((unsigned int)type < 0xc && ((1u << type) & 0xac4u) != 0)
        return 1;
    return 0;
}
}  // namespace _SDC

// ============================ SDCActiveStatusSyncCommon ============================

float SDCActiveStatusSyncCommon::getActiveStatusPower1(int idx, int level)
{
    // P31/P184：ORIG entry 双死 init 槽 -0x14=0/-0x10=1（照抄）；
    // float 结果具名局部两段物化（fstps → 自拷贝 → flds）。
    int deadA = 0;
    int deadB = 1;
    (void)deadA; (void)deadB;
    float v = GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER1[idx * 2],
                                         ACTIVESTATUS_POWER1[idx * 2 + 1],
                                         level - 1, ACTIVESTATUS_MAXLEVEL - 1);
    return v;
}

float SDCActiveStatusSyncCommon::getActiveStatusPower2(int idx, int level)
{
    int deadA = 0;
    int deadB = 1;
    (void)deadA; (void)deadB;
    float v = GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER2[idx * 2],
                                         ACTIVESTATUS_POWER2[idx * 2 + 1],
                                         level - 1, ACTIVESTATUS_MAXLEVEL - 1);
    return v;
}

float SDCActiveStatusSyncCommon::getActiveStatusEtcParameter(int idx, int level,
                                                             int j)
{
    // ORIG 0x0808d446/0x0808d44d：entry 双 dead init 槽 -0x14=0/-0x10=1（P31）；
    // 返回两段物化（fstps -0xc → mov %eax,-0x1c → flds -0x1c，P0 双具名局部）。
    int lo = 0;
    int hi = 1;
    (void)lo; (void)hi;
    return GetUniformVelocityFloatSDC(
        ACTIVESTATUS_ETC_PARAMETERS[idx][j * 2],
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

int SDCActiveStatusSyncCommon::getDuration(ENUM_ACTIVESTATUS status,
                                           int level, void* param)
// 0x0808dc7a：param（DataSend 视图）尾部三 int 作 objectType/a4/hpPenalty；
// ORIG 透传 getActiveStatusDurationServer 返回值。
{
    const unsigned char* p = static_cast<const unsigned char*>(param);
    return getActiveStatusDurationServer(
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
            duration = static_cast<int>(1000.0f * getActiveStatusPower1(status, level));
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
        duration -= static_cast<int>(f * ((x / 3.0f) / x));
    }
    if (duration < 0)
        duration = 0;
    return duration;
}

int SDCActiveStatusSyncCommon::getBurnDamage(bool& flag,
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
    return static_cast<int>(dmg);
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
    // ORIG 0x0808dcb0：fistpl 截断（fnstcw/fldcw 设 RC=truncate），非四舍五入。
    return static_cast<int>(dmg);
}

int SDCActiveStatusSyncCommon::getDiseaseDamage(
    STSDCActiveStatusSyncDataSend* d, int t)
{
    if (!(d->m_hpPercent < 100))
        return 0;

    int dmg = static_cast<int>(GetUniformVelocityFloatSDC(
        0.0f, static_cast<float>(d->m_baseDamage), t, d->m_durationTick));
    if (d->m_isPvp != 0)
    {
        float rate = getUserToUserDamageRate(d->m_attackerType == 10);
        dmg = static_cast<int>(rate * static_cast<float>(dmg));
    }
    return dmg;
}

int SDCActiveStatusSyncCommon::getBleedingDamageServer(
    STSDCActiveStatusSyncDataSend* d)
{
    // P0：ORIG int 中转槽 -0x14 + float 具名局部 -0x4（fildl→fstps→flds 两段）。
    int rawDamage = d->m_baseDamage;
    float dmg = static_cast<float>(rawDamage);
    dmg = dmg * 1.5f;
    return static_cast<int>(dmg);
}

int SDCActiveStatusSyncCommon::getBleedingDamageClient(
    STSDCActiveStatusSyncDataSend* d, bool, void*)
{
    int tickA = d->m_durationTick / 500;
    int tickB = 1;
    int tick = std::max(tickB, tickA);          // ORIG std::max(tickB, tickA) 序保持
    // P0：ORIG float 具名局部槽 -0xc（fdivrp → fstps → flds 两段物化）。
    float dmg = static_cast<float>(d->m_baseDamage) / static_cast<float>(tick);
    return static_cast<int>(dmg);
}

// ORIG 0x0808d4b4/0x0808d50e 共用 x87 取整宏（每 push_back 前 12 步内联序列，
// 全 case 共享槽 -0x7a/-0x7c/-0x80）：fnstcw / movzwl+mov $0xc,%ah（ah 覆写=
// RC=up(toward +inf)，bit8-9 同步清零）/ mov %ax / fldcw / fistpl(st0=前一
// call 的 float 返回值) / fldcw 还原 / mov 取回。以整段 volatile asm 保序，
// 实现固有 x87 语义（非符号桥；仓库 inline asm 先例见 ActiveStaticInfo.cpp 等）。
// v 以 "t" 约束声明置于 st(0)（call 返回值天然所在，不出 fld/fstp）；
// 整序列单条 volatile asm 阻断 4.4 把 fnstcw 前移。fistpl 目标共享 _ri，
// 再 mov 入 _tv[i]（ORIG -0x80 共享 + 独立槽两步）。
#define SDC_ROUND_UP(v, dst)                                                \
    __extension__({                                                         \
        __asm__ __volatile__(                                               \
            "fnstcw %0\n\t"                                                  \
            "movzwl %0, %%eax\n\t"                                          \
            "movb $12, %%ah\n\t"                                            \
            "movw %%ax, %1\n\t"                                             \
            "fldcw %1\n\t"                                                  \
            "fistpl %2\n\t"                                                 \
            "fldcw %0"                                                      \
            : : "m"(_cw), "m"(_cwUp), "m"(_ri), "t"(v)                      \
            : "eax", "memory");                                             \
        (dst) = _ri;                                                        \
    })

void SDCActiveStatusSyncCommon::makeEtcParameters(
    ENUM_ACTIVESTATUS status, int level, void* param, std::vector<int>* out,
    int* outPtr)
// 0x0808d4b4：先回填 outPtr，再转发三参版本（param 作 DataSend 视图）。
{
    // ORIG 0x0808d4b4 entry：out 形参死 spill 命名槽 -0xc（P0：mov 0x18,%eax;
    // mov %eax,-0xc 先于 outPtr 检查）。
    std::vector<int>* pOut = out;
    if (outPtr != 0)
    {
        *outPtr = getDuration(status, level, param);
    }
    // ORIG 0x0808d4b4 无条件转发三参版本（无 out != 0 检查）。
    makeEtcParameters(status, level, *pOut);
}

void SDCActiveStatusSyncCommon::makeEtcParameters(ENUM_ACTIVESTATUS status,
                                                  int level,
                                                  std::vector<int>& out)
// 0x0808d50e：jump table（cmp $0x11; ja; mov table(,%eax,4); jmp *%eax）。
// 每 push 独立具名结果槽（ORIG -0x6c..-0xc，tv1 最深、tv25 最浅——4.4 先声明
// 更浅 ⇒ 声明序 tv25..tv1 逆序）；fcw 三临时最后声明（最深 -0x7a/-0x7c/-0x80）。
{
    int tv1, tv2, tv3, tv4, tv5, tv6, tv7, tv8, tv9, tv10, tv11, tv12, tv13,
        tv14, tv15, tv16, tv17, tv18, tv19, tv20, tv21, tv22, tv23, tv24, tv25;
    char _gap12[12];  // 范式31：12B dead pad（ORIG fcw 槽 -0x7a/-0x7c/-0x80 精确深度）
    (void)_gap12;
    short _cw, _cwUp;
    int _ri;
#define PUSH_ETC(dst, j)                                                  \
    do {                                                                  \
        SDC_ROUND_UP(getActiveStatusEtcParameter(status, level, j), dst); \
        out.push_back(dst);                                               \
    } while (0)
#define PUSH_CONST(dst, val) ((dst) = (val)); out.push_back(dst)
    switch (status)
    {
    case ACTIVE_STATUS_SLOW:
        PUSH_ETC(tv25, 0);
        PUSH_ETC(tv24, 1);
        break;
    case 2:
        PUSH_ETC(tv23, 0);
        break;
    case 4:
        PUSH_ETC(tv22, 0);
        PUSH_ETC(tv21, 1);
        PUSH_ETC(tv20, 2);
        PUSH_ETC(tv19, 3);
        break;
    case 5:
        PUSH_ETC(tv18, 0);
        PUSH_ETC(tv17, 1);
        break;
    case 6:
        PUSH_ETC(tv16, 0);
        break;
    case 7:
        PUSH_ETC(tv15, 0);
        break;
    case 9:
        PUSH_ETC(tv14, 0);
        PUSH_ETC(tv13, 1);
        PUSH_CONST(tv12, 1);
        PUSH_CONST(tv11, 3000);
        break;
    case 10:
        PUSH_ETC(tv10, 0);
        break;
    case 11:
        PUSH_ETC(tv9, 0);
        break;
    case 12:
        PUSH_ETC(tv8, 0);
        PUSH_ETC(tv7, 1);
        break;
    case 13:
        PUSH_ETC(tv6, 0);
        PUSH_ETC(tv5, 1);
        PUSH_ETC(tv4, 2);
        PUSH_ETC(tv3, 3);
        break;
    case 15:
        PUSH_CONST(tv2, 1);
        break;
    case ACTIVE_STATUS_17:
        PUSH_ETC(tv1, 0);
        break;
    default:
        break;
    }
#undef PUSH_ETC
#undef PUSH_CONST
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
        bool flag = false;
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
