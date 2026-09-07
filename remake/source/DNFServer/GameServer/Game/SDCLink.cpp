// ============================================================================
// SDCLink.cpp —— /tmp/undef_r21.txt SDC/伤害计算族 9 符号核销 TU（LinkHelper.SDCLinkHack）。
// 全部符号 ORIG nm -S 已核实；无独立函数报告，逐条 objdump 反汇编转写。
//
// 证据（ORIG=/home/wangyilei/下载/df_game_r）：
//   getAbilityConvertRateSDC(void*,ENUM_CONVERT_TABLE_TYPE) T 0x080950fb (0x13)
//   getDamageInflict(STComputeDamageVariable*,void*)        T 0x08095193 (0x8d)
//   isValidAttackInfoElement(STComputeDamageVariable*,int)  T 0x0809576e (0x55)
//   getComputeDamageBasisLevelAttacker(STComputeDamageVariable*,bool) T 0x080957c3 (0x4d)
//   getDefenseRateDamager(float&,float&,STComputeDamageVariable*,void*) T 0x08095810 (0x206)
//   checkIsCriticalSDC(STComputeDamageVariable*,float*,unsigned int*)   T 0x08095a16 (0x21c)
//   decryptBVHackPacket(char*,stBVHackInfo_t&)              T 0x088920f1 (0xd1)
//   BLEEDING_DAMAGE_IN_ACTION_RATE  R 0x08af45e4 4B（.rodata 字节 00 00 c0 3f = 0.75f）
//   g_maxTotalDefenseRate           D 0x0936e4a8 4B（.data   字节 0a d7 23 3c = 0.01f）
// ============================================================================

#include "STComputeDamageVariable.h"   // STComputeDamageVariable / ENUM_CONVERT_TABLE_TYPE（权威布局，DspD1 产出）

#include <algorithm>

// ---- 本 TU 外部符号（ORIG 已有/由其它批次提供；不得重复定义） ----
// _SDC::isObjectType T 0x080950e8 —— 定义 W9_SDCCommon.cpp。
namespace _SDC
{
bool isObjectType(int objectType, int code);
}
// ScriptDepsB.cpp 定义（D 0x093f5d18 / 0x093f5d30），声明复用 ScriptDepsB.h；本地直声明避免拖依赖。
extern float g_physicalReduceRateConst_;   // 0x093f5d18
extern float g_magicalReduceRateConst_;    // 0x093f5d30
// SDC_Rand T 0x0808bd3f —— SDC 域独立符号（不在本批清单，链接由其 producer 提供）。
unsigned int SDC_Rand(unsigned int* state);

// LinkFinal2 依赖声明（ORIG nm 已核；定义见 ScriptDepsA.cpp / InitEnvHelpers.cpp）：
//   g_physicalDamageRateConst_ B 0x093f5d10 / g_magicalDamageRateConst_ B 0x093f5d28
//   g_creatureModuleDamageRateMap B 0x094ff12c / g_creatureIndexDamageRateMap B 0x094ff144
//   g_creature_damage_vector_ B 0x093f5d50
#include <map>
#include <vector>
// gcc4.4 不支持无底层类型 enum 前置声明；本地最小定义（与 STStackableScript.h
// 权威定义同值域，mangling/ABI 一致，ScriptDepsA.cpp 的 map 由同 mangled 符号解析）。
enum ENUM_MODULE_TYPE { ENUM_MODULE_TYPE_0 = 0 };
extern float g_physicalDamageRateConst_;
extern float g_magicalDamageRateConst_;
extern std::map<ENUM_MODULE_TYPE, float> g_creatureModuleDamageRateMap;
extern std::map<int, float> g_creatureIndexDamageRateMap;
extern std::vector<int> g_creature_damage_vector_;



// ORIG D 0x0936e4ac（.data 字节 00 00 c0 3f? 实测 3f c0 00 00 = 1.5f）——暴击伤害默认倍率。
// 不在本批清单且全仓库无定义，但 checkIsCriticalSDC 引用之；以 ORIG .data 初值落定义。
extern "C" float g_criticalDamageDefaultRate_ = 1.5f;   // D 0x0936e4ac

// ORIG .rodata/.data 直接读取的常量（getDefenseRateDamager/checkIsCriticalSDC 内联位模式）
static const float kDefRateMul01 = 0.01f;   // 0x08b12f54
static const float kDefRateDrFloorDiv = 400.0f;  // 0x08b12f60
static const float kCritRollScale = 100.0f; // 0x08b12f64

// ============================================================================
// 1) _Z24getAbilityConvertRateSDCPv23ENUM_CONVERT_TABLE_TYPE @0x080950fb (0x13)
//    sub $0x4,%esp; mov $0x3f800000,-0x4; flds -0x4; leave; ret
//    → 两参均未读，恒返 1.0f（ORIG 即纯常量，非桩）。
// ============================================================================
float getAbilityConvertRateSDC(void* /*table*/, ENUM_CONVERT_TABLE_TYPE /*type*/)
{
    return 1.0f;
}
// LinkFinal2：getDamageInflict 的三分派目标定义（ORIG T 0x08095220/0x08095508/0x08095275
// objdump 逐指令转写）及其依赖 getDamageRateCreature T 0x080955e1、
// getCreatureDamageRateFromModule T 0x08095430、getCreatureDamageRateFromIndex T 0x0809549c。
// 常量：0x8b12f54=0.01f（float）、0x8b12f58=100.0（double）。
static const float kDamageRateMul01 = 0.01f;      // 0x08b12f54
static const double kDamageRateDiv100 = 100.0;    // 0x08b12f58

// _Z21getDamageRateCreatureiii @0x080955e1 (0x18d)：a=min(a,50)；sum=a+b；
// g_creature_damage_vector_ size>6 时 result = 100/(d4 + d6/vec[6])，否则 1.0。
//   d1 = vec[1]*sum + vec[0] + c
//   d3 = vec[3]*sum + vec[2]
//   d4 = (vec[4]/d3 + 1) * d1
//   d6 = vec[3]*sum + vec[5]
float getDamageRateCreature(int a, int b, int c)
{
    if (a > 0x32)
        a = 0x32;
    int sum = a + b;
    double result = 1.0;
    if (g_creature_damage_vector_.size() > 6)
    {
        double d1 = static_cast<double>(g_creature_damage_vector_[1] * sum) +
                    static_cast<double>(g_creature_damage_vector_[0]) + static_cast<double>(c);
        double d3 = static_cast<double>(g_creature_damage_vector_[3] * sum) +
                    static_cast<double>(g_creature_damage_vector_[2]);
        double d4 = (static_cast<double>(g_creature_damage_vector_[4]) / d3 + 1.0) * d1;
        double d6 = static_cast<double>(g_creature_damage_vector_[3] * sum) +
                    static_cast<double>(g_creature_damage_vector_[5]);
        result = (kDamageRateDiv100 / (d4 + static_cast<double>(g_creature_damage_vector_[6]) / d6)) * result;
    }
    return static_cast<float>(result);
}

// _Z31getCreatureDamageRateFromModule16ENUM_MODULE_TYPE @0x08095430 (0x6c)：
// map find 命中返 value，未命中 1.0f。
float getCreatureDamageRateFromModule(ENUM_MODULE_TYPE type)
{
    std::map<ENUM_MODULE_TYPE, float>::iterator it =
        g_creatureModuleDamageRateMap.find(type);
    if (it != g_creatureModuleDamageRateMap.end())
        return it->second;
    return 1.0f;
}

// _Z30getCreatureDamageRateFromIndexi @0x0809549c (0x6c)：同型 map<int,float>。
float getCreatureDamageRateFromIndex(int index)
{
    std::map<int, float>::iterator it = g_creatureIndexDamageRateMap.find(index);
    if (it != g_creatureIndexDamageRateMap.end())
        return it->second;
    return 1.0f;
}

// _Z24getDamageInflictCreatureP23STComputeDamageVariable @0x08095508 (0xd9)：
// m_char1B5 门控；dmg=m_field000*(rate(m_char1B7,m_char1B8,(int)m_d1AA))*m_f02B
// *FromModule(m_objTypeByte)；objTypeByte==9 再 *FromIndex(m_char1B9)。
float getDamageInflictCreature(STComputeDamageVariable* v)
{
    if (v->m_char1B5 == 0)
        return 0.0f;
    float dmg = v->m_field000;
    int lv = static_cast<int>(v->m_d1AA);   // fldcw 截断模式
    dmg = dmg * getDamageRateCreature(
                      static_cast<int>(static_cast<signed char>(v->m_char1B7)),
                      static_cast<int>(static_cast<signed char>(v->m_char1B8)), lv);
    dmg = dmg * v->m_f02B;
    dmg = dmg * getCreatureDamageRateFromModule(
                     static_cast<ENUM_MODULE_TYPE>(static_cast<signed char>(v->m_objTypeByte)));
    if (static_cast<signed char>(v->m_objTypeByte) == 9)
        dmg = dmg * getCreatureDamageRateFromIndex(
                         static_cast<int>(static_cast<signed char>(v->m_char1B9)));
    return dmg;
}

// _Z31getDamageInflictCollisionObjectP23STComputeDamageVariablePv @0x08095275 (0x1bb)：
// 逐指令转写。0x8b12f54=0.01f；0x93f5d10/0x93f5d28=Damage 常量；
// getAbilityConvertRateSDC 表类型 3/5/0。
float getDamageInflictCollisionObject(STComputeDamageVariable* v, void* collisionObject)
{
    float dmg = v->m_field000;
    if (v->m_char004 != 0)
    {
        if (v->m_damageType == 0 || v->m_damageType == 2)
            dmg = dmg + v->m_field02F;
        else if (v->m_damageType == 1)
            dmg = dmg + v->m_field033;
    }
    dmg = dmg * (v->m_f006 * kDamageRateMul01 + 1.0f);
    if (v->m_damageType == 0)
        dmg = dmg * (v->m_f037 * kDamageRateMul01 + 1.0f);
    else if (v->m_damageType == 1)
        dmg = dmg * (v->m_f03B * kDamageRateMul01 + 1.0f);
    if (v->m_damageType == 0)
    {
        float s2 = v->m_f03F;
        if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
            s2 = s2 * getAbilityConvertRateSDC(collisionObject, CONVERT_TABLE_3);
        dmg = dmg * (g_physicalDamageRateConst_ / s2 + 1.0f);
    }
    else if (v->m_damageType == 1)
    {
        float s2 = v->m_f043;
        if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
            s2 = s2 * getAbilityConvertRateSDC(collisionObject, CONVERT_TABLE_5);
        dmg = dmg * (g_magicalDamageRateConst_ / s2 + 1.0f);
    }
    dmg = dmg * getAbilityConvertRateSDC(collisionObject, CONVERT_TABLE_0);
    dmg = v->m_f047 * dmg + dmg;
    return dmg;
}

// _Z29getDamageInflictPassiveObjectP23STComputeDamageVariablePv @0x08095220 (0x55)：
// m_char1B5 && m_char1B6 → Creature(v)；否则 CollisionObject(v, passiveObject)。
float getDamageInflictPassiveObject(STComputeDamageVariable* v, void* passiveObject)
{
    float r;
    if (v->m_char1B5 != 0 && v->m_char1B6 != 0)
    {
        r = getDamageInflictCreature(v);
        return r;
    }
    r = getDamageInflictCollisionObject(v, passiveObject);
    return r;
}

// ============================================================================
// 2) _Z24isValidAttackInfoElementP23STComputeDamageVariablei @0x0809576e (0x55)
//    elem==4：依次 test 0xa/0xb/0xc/0xd（m_elemEnable[0..3]），任一非零返 0，否则 1；
//    其它：movzbl 0xa(%edx,%eax) → m_elemEnable[elem] != 0。
// ============================================================================
// 返回 bool：ORIG 尾 setne %al 直返无 movzbl 规范化（P9）
bool isValidAttackInfoElement(STComputeDamageVariable* v, int elem)
{
    if (elem == 4)
    {
        if (!(v->m_elemEnable[0] || v->m_elemEnable[1] ||
              v->m_elemEnable[2] || v->m_elemEnable[3]))
            return true;
        return false;
    }
    return v->m_elemEnable[elem] != 0;
}

// ============================================================================
// 3) _Z34getComputeDamageBasisLevelAttackerP23STComputeDamageVariableb @0x080957c3 (0x4d)
//    bool 为真且 isObjectType(m_attackerObjType,0x111) → (signed char)0x1b4；
//    否则 (signed char)0x1b3。两字节 ctor 初值均为 1。
// ============================================================================
int getComputeDamageBasisLevelAttacker(STComputeDamageVariable* v, bool useAttackerLevel)
{
    if (useAttackerLevel)
    {
        if (_SDC::isObjectType(v->m_attackerObjType, 0x111))
            return (signed char)v->m_char1B4;
    }
    return (signed char)v->m_char1B3;
}

// ============================================================================
// 4) _Z16getDamageInflictP23STComputeDamageVariablePv @0x08095193 (0x8d)
//    按 v+0x1BA（m_field1BA）对象类型三分派：
//      isObjectType(0x21) → PassiveObject(v,obj)；isObjectType(0x81) → Creature(v)；
//      否则 CollisionObject(v,obj)。
// ============================================================================
float getDamageInflict(STComputeDamageVariable* v, void* defender)
{
    if (_SDC::isObjectType(v->m_field1BA, 0x21))
        return getDamageInflictPassiveObject(v, defender);
    if (_SDC::isObjectType(v->m_field1BA, 0x81))
        return getDamageInflictCreature(v);
    return getDamageInflictCollisionObject(v, defender);
}

// ============================================================================
// 5) _Z21getDefenseRateDamagerRfS_P23STComputeDamageVariablePv @0x08095810 (0x206)
//    逐指令转写。out0=防御减伤率、out1=力量换算 s1。
//    常量：0x8b12f54=0.01f、0x8b12f60=400.0f、0x8b12f64=100.0f；
//    全局：0x93f5d18=g_physicalReduceRateConst_、0x93f5d30=g_magicalReduceRateConst_
//          （ScriptDepsB.cpp 定义）。
// ============================================================================
void getDefenseRateDamager(float& outDefenseRate, float& outReduceS1,
                           STComputeDamageVariable* v, void* defender)
{
    // 0x08095816：lvl = getComputeDamageBasisLevelAttacker(v, false)
    int basisLevel = getComputeDamageBasisLevelAttacker(v, false);

    float dr = 0.0f;        // -0x24 防御减伤基数
    float s1 = 0.0f;        // -0x10
    float redConst = 0.0f;  // -0xc

    if (v->m_damageType == 0)            // 0x08095847：test 0x5 → 物理分支
    {
        dr = static_cast<float>(v->m_field093);                       // fildl 0x93
        dr = (static_cast<float>(v->m_field097) * kDefRateMul01 + 1.0f) * dr;  // 0x97
        redConst = g_physicalReduceRateConst_;                        // mov 0x93f5d18
        s1 = v->m_field09B * kDefRateMul01;                           // flds 0x9b
        if (v->m_objTypeByte == 9)                                    // 0x1b2
            s1 *= getAbilityConvertRateSDC(defender, static_cast<ENUM_CONVERT_TABLE_TYPE>(8));
    }
    else if (v->m_damageType == 1)       // 0x080958d6：cmp $1 → 魔法分支
    {
        dr = static_cast<float>(v->m_field09F);                       // fildl 0x9f
        dr = (static_cast<float>(v->m_field0A3) * kDefRateMul01 + 1.0f) * dr;  // 0xa3
        redConst = g_magicalReduceRateConst_;                         // mov 0x93f5d30
        s1 = v->m_field0A7 * kDefRateMul01;                           // flds 0xa7
        if (v->m_objTypeByte == 9)
            s1 *= getAbilityConvertRateSDC(defender, static_cast<ENUM_CONVERT_TABLE_TYPE>(0xA));
    }

    // 0x0809595b：dr *= getAbilityConvertRateSDC(defender, 1)
    dr *= getAbilityConvertRateSDC(defender, static_cast<ENUM_CONVERT_TABLE_TYPE>(1));

    // 0x08095976：if (v->m_char1E5) redConst *= 2（fadd st(0),st(0)）
    if (v->m_char1E5 != 0)
        redConst += redConst;

    // 0x0809598c：dr = std::max(dr, 0.0f)
    dr = std::max(dr, 0.0f);

    // 0x080959ae..0x080959de：out0 = dr / max(1.0f, (float)lvl * redConst + dr)
    const float denom = static_cast<float>(basisLevel) * redConst + dr;
    outDefenseRate = dr / std::max(1.0f, denom);

    // 0x080959e3：if (dr == 0.0f && lvl <= 9) out0 = dr / 400.0f
    //   （fnstsw+test 0x45+sete：非无序且相等，即 C++ dr == 0.0f）
    if (dr == 0.0f && basisLevel <= 9)
        outDefenseRate = dr / kDefRateDrFloorDiv;

    // 0x08095a0c：out1 = s1
    outReduceS1 = s1;
}

// ============================================================================
// 6) _Z18checkIsCriticalSDCP23STComputeDamageVariablePfPj @0x08095a16 (0x21c)
//    暴击判定：累计暴击率 → SDC_Rand()%10000 对比 → 命中写 *dmg 倍率与 m_isCritical。
// ============================================================================
bool checkIsCriticalSDC(STComputeDamageVariable* v, float* dmg, unsigned int* randState)
{
    if (dmg == 0)                                   // 0x08095a1d：cmpl $0,0xc(%ebp)
        return false;

    float crit = 3.0f + v->m_f00F[1];               // 0x40400000 + flds 0x13

    if (v->m_char1B6 != 0)                          // 0x1b6
    {
        if (_SDC::isObjectType(v->m_attackerObjType, 0x81))
            crit += v->m_f05B;                      // flds 0x5b
    }
    else if (_SDC::isObjectType(v->m_attackerObjType, 0x11))
    {
        if (v->m_damageType == 0)
            crit += v->m_f04B;                      // flds 0x4b
        else if (v->m_damageType == 1)
            crit += v->m_f04F;                      // flds 0x4f
    }

    if (v->m_char1DE != 0)                          // 0x1de
    {
        crit += v->m_f00F[2];                       // flds 0x17
        if (_SDC::isObjectType(v->m_attackerObjType, 0x11))
        {
            if (v->m_damageType == 0)
                crit += v->m_f053;                  // flds 0x53
            else if (v->m_damageType == 1)
                crit += v->m_f057;                  // flds 0x57
        }
    }

    if (_SDC::isObjectType(v->m_defenderObjType, 0x11))   // 0x1c2
        crit -= v->m_field0CF;                      // flds 0xcf

    // 0x08095b6a：roll = SDC_Rand(randState) % 10000（有符号 rem，0x68db8bad/0x2710 序列）
    const int roll = static_cast<int>(SDC_Rand(randState) % 10000u);

    // 0x08095b9d..0x08095bb7：if (crit * 100.0f > (float)roll)
    if (crit * kCritRollScale > static_cast<float>(roll))
    {
        float critMul = g_criticalDamageDefaultRate_;     // mov 0x936e4ac（=1.5f 初值）
        if (_SDC::isObjectType(v->m_attackerObjType, 0x11))
            critMul *= v->m_f06F;                   // flds 0x6f
        critMul += v->m_field073;                   // flds 0x73
        *dmg *= critMul;
        v->m_isCritical = 1;                        // movb $1,0x1ce
    }
    else
    {
        v->m_isCritical = 0;                        // movb $0,0x1ce
    }
    return v->m_isCritical != 0;
}

// ============================================================================
// 7) _Z19decryptBVHackPacketPcR14stBVHackInfo_t @0x088920f1 (0xd1)
//    magic 0x6e3f4b9c 校验 → key=payload[0x30]（原 +0x34）→ 12×int XOR 就地解密
//    0x30 字节 → 6×double 依序拷贝到 info。
// ============================================================================
struct stBVHackInfo_t
{
    double m_values[6];   // ORIG 出参偏移 +0x00..+0x28（fldl/fstpl 序列）
};
static_assert(sizeof(stBVHackInfo_t) == 0x30, "stBVHackInfo_t must be 0x30 bytes");

int decryptBVHackPacket(char* payload, stBVHackInfo_t& info)
{
    if (payload == 0)                               // 0x088920f7
        return 0;

    if (*reinterpret_cast<unsigned int*>(payload) != 0x6e3f4b9cu)   // 0x08892113
        return 0;

    payload += 4;                                   // addl $0x4,0x8(%ebp)

    const unsigned int key =
        *reinterpret_cast<unsigned int*>(payload + 0x30);           // mov 0x30(%eax)

    unsigned int* cur = reinterpret_cast<unsigned int*>(payload);   // -0x8 游标
    for (int i = 0; i <= 11; ++i)                   // cmpl $0xb … setbe（i≤11 共 12 次）
    {
        *cur = *cur ^ key;                          // mov (%edx); xor -0xc; mov (%eax)
        ++cur;
    }

    // 6×double：fstpl 到 info+0x00/+0x08/+0x10/+0x18/+0x20/+0x28
    const double* src = reinterpret_cast<const double*>(payload);
    info.m_values[0] = src[0];
    info.m_values[1] = src[1];
    info.m_values[2] = src[2];
    info.m_values[3] = src[3];
    info.m_values[4] = src[4];
    info.m_values[5] = src[5];
    return 1;                                       // mov $0x1,%eax
}

// ============================================================================
// 8) BLEEDING_DAMAGE_IN_ACTION_RATE  R 0x08af45e4（.rodata 00 00 c0 3f = 0.75f）
//    Dispatchers.cpp:233 以 extern "C" float 引用（符号名无 mangle）。
// ============================================================================
extern "C" const float BLEEDING_DAMAGE_IN_ACTION_RATE = 0.75f;

// ============================================================================
// 9) g_maxTotalDefenseRate  D 0x0936e4a8（.data 0a d7 23 3c = 0.01f）
// ============================================================================
float g_maxTotalDefenseRate = 0.01f;
