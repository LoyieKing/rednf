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
// [b28d-lane15 R3 跨 TU 迁定义，Main 批准 2026-09-08] _SDC::isObjectType
// T 0x080950e8 的**定义**自 W9_SDCCommon.cpp 迁入本 TU：ORIG 中它与 SDCLink 族
// （0x08095193..0x080957c3）地址相邻=同 TU；P210② 要求定义先于 caller，4.4 才
// 精确继承 outgoing 帧（sub $0xc，探针 /tmp/b28d_l15_probe4.cpp fF/fG）。
// 迁出侧 W9_SDCCommon.cpp 保留 DataManagerInitDepends.h:447 的声明（U 引用）。
namespace _SDC
{
bool isObjectType(int objectType, int code)
{
    // ORIG 0x080950e8：无帧无槽，and 后直接 cmp 参数（P0 单表达式）。
    return (objectType & code) == code;
}
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
// [b28d-lane15 判词·N（N85 x87 整数↔浮点链）] ORIG 0x080955e1 对每个 int 乘积走
// `imul → mov 槽 → fildl → fldl → faddp` 的 x87 长链（帧 0x78）；4.4 折叠为
// imul+内存直算（帧 0x68、98 vs 108）。N85/P252 探针族已证源形不可择（具名 double
// 局部会改槽布局）。fildl 截断语义一致 ⇒ 封顶。
float getDamageRateCreature(int a, int b, int c)
{
    if (a > 0x32)
        a = 0x32;
    int sum = a + b;
    double result = 1.0;
    if (g_creature_damage_vector_.size() > 6)
    {
        // P175/P167：ORIG 8 处调用点全是 `vector::at(unsigned)`（非 operator[]）；
        // 逐点写 at()，勿缓存引用。
        // ORIG `fildl at(1); fildl sum; fmulp` ⇒ 两操作数各自 fildl（double 乘法），
        // 不是先做 int 乘法再 fildl（后者产 imul+mov+fildl）。
        // 且 ORIG 恒先求值「加法的右项」（at(0)+c / at(2) / at(5) 先落槽）⇒ 把该项
        // 写在加法左侧（浮点加法可交换，值不变）。
        double d1 = (static_cast<double>(g_creature_damage_vector_.at(0)) + static_cast<double>(c)) +
                    static_cast<double>(g_creature_damage_vector_.at(1)) * static_cast<double>(sum);
        double d3 = static_cast<double>(g_creature_damage_vector_.at(2)) +
                    static_cast<double>(g_creature_damage_vector_.at(3)) * static_cast<double>(sum);
        double d4 = (static_cast<double>(g_creature_damage_vector_.at(4)) / d3 + 1.0) * d1;
        double d6 = static_cast<double>(g_creature_damage_vector_.at(5)) +
                    static_cast<double>(g_creature_damage_vector_.at(3)) * static_cast<double>(sum);
        result = (kDamageRateDiv100 / (d4 + static_cast<double>(g_creature_damage_vector_.at(6)) / d6)) * result;
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
// [b28d-lane15 判词·N（N85）] ORIG 对 float 纯拷贝发 `flds (%eax); fstps -0x24`
// 往返（4.4 恒 `mov (%eax),%eax; mov %eax,槽` 直搬，N85①），乘法为
// `flds; fmuls mem; fstps` 三步 vs 4.4 `flds; fmulp; fstps` 折叠（N85③）；
// 由此槽组 -0x24/-0x20/-0xc vs -0x10/-0xc 整体平移。语义等价，封顶。
float getDamageInflictCreature(STComputeDamageVariable* v)
{
    if (v->m_char1B5 == 0)
        return 0.0f;
    float base = v->m_field000;
    int lv = static_cast<int>(v->m_d1AA);   // fldcw 截断模式
    float dmg = getDamageRateCreature(
                    static_cast<int>(static_cast<signed char>(v->m_char1B7)),
                    static_cast<int>(static_cast<signed char>(v->m_char1B8)), lv) * base;
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
// [b28d-lane15 判词·N（N85②）] 残差仅 2 处除法：ORIG `flds 全局; flds -0x8;
// fdivp %st,%st(1)` 三步 vs 4.4 `flds 全局; fdivs -0x8` 两步（除操作数在内存时
// 4.4 恒折 fdivs）。其余 diff 行全为重定位地址伪差（0x8b12f54 vs .o 未解析 0x28）。
// 语义等价，封顶。
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
// [b28c-lane24] 四形（双 return/if-else/if-else+init/P210 前置定义）帧恒 0x38 vs
// ORIG 0x28、尾物化槽 -0x1c vs -0xc ⇒ 曾判 N（跨版本帧策略 + 尾物化槽）。
// [b28d-lane15 翻案] ORIG 实为**三元**：两臂 fstps -0xc → 汇合 +0x4d
// `mov %eax,-0xc; flds -0xc`（float 位模式过 eax），帧 0x28、无具名 float 局部。
// 改三元后 fresh .o 判 identical/ae（SDCLink near 2→1）。P243 同族。
float getDamageInflictPassiveObject(STComputeDamageVariable* v, void* passiveObject)
{
    return (v->m_char1B5 != 0 && v->m_char1B6 != 0)
               ? getDamageInflictCreature(v)
               : getDamageInflictCollisionObject(v, passiveObject);
}

// ============================================================================
// 2) _Z24isValidAttackInfoElementP23STComputeDamageVariablei @0x0809576e (0x55)
//    elem==4：依次 test 0xa/0xb/0xc/0xd（m_elemEnable[0..3]），任一非零返 0，否则 1；
//    其它：movzbl 0xa(%edx,%eax) → m_elemEnable[elem] != 0。
// ============================================================================
// 返回 bool：ORIG 尾 setne %al 直返无 movzbl 规范化（P9）
// [b28 BM2 判词·AE] 清单 NEAR(2) 为 jmp 目标偏移标签伪差（ORIG 双 epilogue 岛
// +0x41/+0x53 vs 4.4 共享尾，跨版本 epilogue merge 族）；fresh /tmp/tu_SDCLink.o
// fn_diff_tu 归一 0 差（31=31 逐条）。
// [b28d-lane15] check_tu_routed 复核：本件不在 near/diff 列表（identical/ae）；
// game_func_report 的 DIFF 系 jmp 目标标签归一伪差，维持 AE。
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
// [b28d-lane15 判词·N（跨 TU 帧继承，P210②）] ORIG 帧 sub $0xc / bool spill -0x4；
// 4.4 对**仅声明**的 _SDC::isObjectType 恒保守 sub $0x28 / spill -0xc（探针
// /tmp/b28d_l15_probe3.cpp fA..fE 五形全 0x28）。把 isObjectType **定义**置于
// caller 之前（探针 /tmp/b28d_l15_probe4.cpp fF/fG）即复现 0xc。ORIG 中
// isObjectType @0x080950e8 与本族地址相邻（同 TU）；我方定义在 W9_SDCCommon.cpp
// （非本泳道 TU，R1 禁编辑）⇒ 已上报 Main 定夺 TU 归属迁移，本件暂 N 封顶。
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
// [b28d-lane15 判词·N（N85①/N82 槽组平移）] 残差为 float 中转槽 -0x24 vs -0x20 的
// 整体浅移 4（ORIG flds/fstps 往返占槽，4.4 mov 直搬省槽），指令序列/长度逐条一致
// （152 vs 151）；常量/全局引用行为 .o 重定位伪差。封顶。
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
    // [C03 实测] ORIG 该 max 的第一实参是具名 0.0f 局部（-0x20）、第二为 dr（-0x24），
    //   但把 0.0f/1.0f 具名化后 4.4 全帧槽位反而上升 4（dr -0x20→-0x18，本件变差），
    //   已回退；残差属 N85①+槽组族（见下）。
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
// [b28d-lane15 判词·N（N85① + 常量装载形）] ORIG `push %ebx; sub $0x34`，kCritRollScale
// 常量地址先 `mov $HEX,%eax; mov %eax,-0x10` 入槽再 `flds -0x10`；4.4 直接
// `flds 0x30` 立即数寻址、帧 0x28、无 push ebx ⇒ float 常量/中转槽整体平移。
// 语义等价，封顶。
// [b28d-L28 重核·修 10 条，gap 9→-1] 判据①(jcc 目标下标) + p167 复核后按 P244/P243/
// P232 翻正两处**真缺口**（非 N85）：
//  ① ORIG 0x08095a2d `mov $0x40400000,%eax; mov %eax,-0x10(%ebp)`（3.0f 常量→槽二跳，
//     P244）⇒ 原码为具名 float 常量局部 `baseCrit`（P0：有槽往返 ⇒ 有具名局部），
//     加法按 ORIG `flds 成员; flds 槽; faddp` 序写 `v->m_f00F[1] + baseCrit`（P243）。
//  ② ORIG 0x08095b6a 的 rem 为**有符号**序列（`imul %edx`(0x68db8bad) + `sar $0xc` +
//     `sar $0x1f` 修正 + `imul $0x2710`）⇒ 源码须先转 int 再对 int 常量取余；
//     旧 `SDC_Rand(...) % 10000u` 产无符号 `mul/shr` 序列 —— 除形态差外，负值语义
//     亦不同（真 bug 修复）。
// 残余（不可翻）：③ `flds 槽 + faddp` 被 4.4 折 `fadds 槽`（N85③ 内存操作数折叠）；
// ④ 我方 rem 展开多 1 对 store/load（N87 固有）；⑤ ORIG 同槽 `fildl`+`flds` 双视图
// （局部 union 视角，P260 不适用于自动局部）。判 N（N85③+N87）。
// ============================================================================
bool checkIsCriticalSDC(STComputeDamageVariable* v, float* dmg, unsigned int* randState)
{
    if (dmg == 0)                                   // 0x08095a1d：cmpl $0,0xc(%ebp)
        return false;

    // [b28d-L28 重核] ORIG 0x08095a2d `mov $0x40400000,%eax; mov %eax,-0x10(%ebp)`（常量
    // →槽二跳，P244/P232）后 `flds 0x13(%ebx); flds -0x10(%ebp); faddp`（成员先 fld，
    // P243）⇒ 原码是具名 float 常量局部 + 成员在左的加法。
    float baseCrit = 3.0f;
    float crit = v->m_f00F[1] + baseCrit;

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
    // [b28d-L28 重核] ORIG 0x08095b6a 出**有符号** rem 序列（`imul %edx`(0x68db8bad)
    // + `sar $0xc` + `sar $0x1f` 修正 + `imul $0x2710`），我方旧 `% 10000u` 走无符号
    // `mul/shr` 序列 ⇒ 原码是先转 int 再对 int 常量取余。
    const int roll = static_cast<int>(SDC_Rand(randState)) % 10000;

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
// [b28d-lane15 判词·N（P211/N76 族，残 1 条）] 本波已复现：magic 死写入槽 -0x10
// （P171）、unsigned 循环变量出 `setbe`（P2）、payload 形参自增 6×fldl/fstpl。
// 唯一残差 = `*cur ^= key` 的 LOD 地址寄存器：ORIG 0x08892142 双读 cur
// （`mov -0x8,%eax; mov -0x8,%edx; mov (%edx),%edx`），4.4 复用 eax 作值寄存器
// 再重载 cur（多 1 条 mov，68 vs 67）。`*cur=*cur^key`/`*cur^=key`/下标形三形同产，
// 具名中间量会引入 ORIG 不存在的槽 ⇒ 封顶。
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

    // [b28d-lane15] ORIG 0x08892107 `movl $0x6e3f4b9c,-0x10(%ebp)` 是**死写入**局部
    // （比较仍用立即数 `cmp $0x6e3f4b9c`）——P171 照抄，勿删。
    unsigned int magic = 0x6e3f4b9cu;

    if (*reinterpret_cast<unsigned int*>(payload) != 0x6e3f4b9cu)   // 0x08892113
        return 0;

    payload += 4;                                   // addl $0x4,0x8(%ebp)

    const unsigned int key =
        *reinterpret_cast<unsigned int*>(payload + 0x30);           // mov 0x30(%eax)

    unsigned int* cur = reinterpret_cast<unsigned int*>(payload);   // -0x8 游标
    // ORIG 0x0889215b `setbe`（无符号 ≤）⇒ 循环变量必须 unsigned（P2）。
    for (unsigned int i = 0; i <= 11; ++i)          // cmpl $0xb … setbe（i≤11 共 12 次）
    {
        *cur = *cur ^ key;                          // mov (%edx); xor -0xc; mov (%eax)
        ++cur;
    }

    // 6×double：ORIG 逐次 `fldl (payload); fstpl off(info); addl $0x8,payload`
    // （形参自增，非数组下标 src[i]）。
    info.m_values[0] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[1] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[2] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[3] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[4] = *reinterpret_cast<double*>(payload); payload += 8;
    info.m_values[5] = *reinterpret_cast<double*>(payload); payload += 8;
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
