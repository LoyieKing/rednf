// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EquipmentParameterInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "EquipmentParameterInfo.h"

#include <stdio.h>
#include <string.h>
#include <math.h>   // [A18 本波] fabsf（ORIG export separate-rate 段 x87 fabs 内联形）
#include <limits>   // ORIG export @0x89adeab 尾区：numeric_limits<float>::epsilon() 调用两处
#include <algorithm>  // ORIG export 0x089ae30a：std::min<int>（_ZSt3minIiERKT_S2_S2_）

#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "ScriptValue.h"
#include "SyncScriptDeps.h"   // toTString / getCharacterJob
#include "CreatureScript.h"  // ENUM_ELEMENT（getElementName 形参）
#include "Quest.h"           // ENUM_CHARACTERJOB 的权威声明
#include "CEnvironment.h"    // less_TCHAR / SpinLock（GetSkillPhase static map）
#include "EffectAnimationInfo.h"  // readCondtion [attack success effect animation] 局部对象

// ORIG 全局记账（0x949b28c/0x949b294，C1/C2 递增、D1 递减）：
// 实例计数与按 sizeof 累积的分配字节数。
int g_EquipmentParameterInfoCount = 0;
int g_EquipmentParameterInfoSize = 0;

// [OurOnly33 符号名轴 2026-09-11] ORIG 权威：`093f4840 000000bc D AVTVARIATION_TAG_NAME`
// （全局 .data，0xbc=188B=47 指针）——import（读 [avatar select ability]）与 export
// 两处共用同一 ORIG 全局表。原两处函数内 static
//（`_ZZ28importEquipmentParameterInfo…E19kAvatarAbilityNames` /
//  `_ZZ30exportEquipmentParameterScript…E10kAvatarTag`）系我方发明名 ⇒ 改用该权威全局表
//（定义见 OnlyOrigBatch12_server_utility.cpp:154，本 TU 仅 extern 引用，勿重复定义）。
extern const char* const AVTVARIATION_TAG_NAME[47];

#include "EquipmentParameterInfo/EquipmentParameterInfo.inc"


#include "EquipmentParameterInfo/EquipmentParameterInfo_1.inc"


#include "EquipmentParameterInfo/dtor_EquipmentParameterInfo.inc"


#include "EquipmentParameterInfo/t_119.inc"


#include "EquipmentParameterInfo/dtor_t_119.inc"


EquipmentParameterInfo::t_119&
EquipmentParameterInfo::t_119::operator=(const t_119& other)
{
    // ORIG 0x088fd910：vector<SASPROPERTY> op= → +0xc char → +0xd char →
    // vector<vector<float>> op=；无 self-check，返回 *this。
    m_418 = other.m_418;
    m_424 = other.m_424;
    m_425 = other.m_425;
    m_428 = other.m_428;
    return *this;
}

// ORIG 另一 TU 的 `._159aSERKS0_`（0x08348024/0x0861c9b8）与本 TU `._119aSERKS0_`
// 函数体逐条相同（norm 实测相等）；独立类使函数体独立地址，objdump 方能同时
// 标注 `5._119aSERKS0_` 与 `5._159aSERKS0_`（P203 附注：同址只标一个）。
EquipmentParameterInfo::tt_159&
EquipmentParameterInfo::tt_159::operator=(const tt_159& other)
{
    m_418 = other.m_418;
    m_424 = other.m_424;
    m_425 = other.m_425;
    m_428 = other.m_428;
    return *this;
}


#include "EquipmentParameterInfo/clear.inc"


#include "EquipmentParameterInfo/addString.inc"


#include "EquipmentParameterInfo/getEquipmentPhysicalAttack.inc"


#include "EquipmentParameterInfo/getEquipmentPhysicalDefense.inc"


#include "EquipmentParameterInfo/getEquipmentMagicalAttack.inc"


#include "EquipmentParameterInfo/getEquipmentMagicalDefense.inc"


#include "EquipmentParameterInfo/RebirthAppendageinfo.inc"


#include "EquipmentParameterInfo/clear_1.inc"






#include "EquipmentParameterInfo/getCharacterJobToEnumValue.inc"
#include "EquipmentParameterInfo/getActiveStatusToEnumValue.inc"
#include "EquipmentParameterInfo/getStateToEnumValue.inc"
#include "EquipmentParameterInfo/getEquipmentEventTiming.inc"
// ORIG 0x088be60：职业枚举 → 作业标签名（STMapScript.cpp 全局 T）。
const char* getCharacterJobName(ENUM_CHARACTERJOB job);
#include "EquipmentParameterInfo/getElementToEnumValue.inc"
#include "EquipmentParameterInfo/getAttackConditionToEnumValue.inc"
#include "EquipmentParameterInfo/getCategoryToEnumValue.inc"
#include "EquipmentParameterInfo/getGradeToEnumValue.inc"
// [b28d-solo-EquipmentParameterInfo] P211 档① 局部镜像声明旋钮：ORIG 调用点
// （getStatusType 0x089a74dc、ScriptValue scanChangeStatusData 0x088bda54）均为
// `mov %eax,%edx; mov this,%eax; mov %edx,(mem)` = RHS 值先 + eax→edx 中转；
// 4.4 对「int 目标 ← int callee」恒折 LHS 地址先（探针 /tmp/b28d_epi/p1.cpp g1 vs
// g4，p5.cpp gu1：仅当 callee 返回类型与 int 异型才产 RHS-first）。返回类型不入
// mangled、ABI 同（eax 传值、值域非负），故本 TU 镜像声明改 unsigned int 复现
// ORIG 形；本文件 `(float)` 消费点同步加 `(int)` 内层 cast 保持 fildl（p5.cpp fu1）。
unsigned int getChangeStatusType(std::string& s);      // 0x8892b0d（定义侧返回 int，ABI 同）
#include "EquipmentParameterInfo/getObjectTypeToEnumValue.inc"
#include "EquipmentParameterInfo/getTargetToEnumValue.inc"
#include "EquipmentParameterInfo/scanModuleType.inc"
#include "EquipmentParameterInfo/ScanTypeOrStr.inc"
#include "EquipmentParameterInfo/getElement.inc"
#include "EquipmentParameterInfo/getActiveStatusType.inc"
const char* getElementName(ENUM_ELEMENT element);      // 0x88bfee0（CreatureScript.cpp）

#include "EquipmentParameterInfo/GetEquipmentTypeStringToEnum.inc"


// [b28d-solo-EquipmentParameterInfo] DIFF N76：store RHS 值先不可翻（P211 三档旋钮试尽）。
static bool getStatusType(int* out)   // [b28d-Main·P292] int→bool
{
    // [b28c-lane24] ORIG 0x089a74a2：`if (*out != 0x4d) return 1;` ⇒ je 失败块 +
    // 成功块 fall-through（与 getTargetTag 同族）；*out 由调用表达式直存（无 v 槽，P0）。
    if (out == 0) return false;
    std::string s;
    if (ScanStr(&s))
    {
        // [b28d-solo-EquipmentParameterInfo] N76 封顶：ORIG 为 RHS 值先 +
        // `mov %eax,%edx` 中转，4.4 恒 LHS 地址先。已试尽 P211 三档旋钮：
        // ①调用侧 (unsigned int)/(ENUM) /双层 cast/`out[0]`/unsigned 左值视图
        //   （末者反多 `mov 0x8(%ebp),%ebx` 一条）；②callee 跨 TU 定义于
        //   GameScriptEnumHelpers.cpp（返回类型异型化需改他人 .cpp，超泳道）；
        //   ③目标为 int* 形参解引用，字段侧不可动 ⇒ 无源形可翻。
        *out = getChangeStatusType(s);
        if (*out != 0x4d)
            return true;
    }
    return false;
}

static bool getSign(int* out)   // [b28d-Main·P292] int→bool
{
    if (out == 0) return false;
    std::string s;
    if (!ScanStr(&s)) return false;
    if (s == "+") { *out = 0; return true; }
    if (s == "%") { *out = 1; return true; }
    return false;
}

#include "EquipmentParameterInfo/GetStatValue.inc"


#include "EquipmentParameterInfo/GetStat.inc"


#include "EquipmentParameterInfo/GetStatByCondition.inc"


#include "EquipmentParameterInfo/GetApplyStatusInRange.inc"


#include "EquipmentParameterInfo/GetSkillLevelUp.inc"


#include "EquipmentParameterInfo/getBuffToEnumValue.inc"


#include "EquipmentParameterInfo/getSkillDataExpansionToEnumValue.inc"


#include "EquipmentParameterInfo/getEquipmentTypeToEnumValue.inc"


// ---- importSkillDataUp（ORIG 0x88c049c，readAction [skill data up] 依赖） ----
// scanScriptValue/pushScriptValue/ScriptValue 由 ScriptValue.h/ScriptValue.cpp
// （FixScriptValuePipeline）提供；importSkillDataUp 在本文件末尾定义。

#include "EquipmentParameterInfo/readAction.inc"


#include "EquipmentParameterInfo/readCondtion.inc"


#include "EquipmentParameterInfo/scanRandomOptionValue.inc"


#include "EquipmentParameterInfo/scanIf.inc"



#include "EquipmentParameterInfo/scanThen.inc"


// ---- SASPROPERTY 子解析 helper（ORIG static，本 TU） ----
static bool GetItemGrowtype(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (sp == 0) return false;
    std::string s;
    if (!ScanStr(&s)) return false;
    sp->m_head0 = getCharacterJob(s.c_str());
    if (!ScanInt(&sp->m_head1)) return false;
    return true;
}

// [skill apply condition] 块直至 "[/skill apply condition]"。ORIG 用三张
// 函数局部静态 map<char const*,int,less_TCHAR>（主分派 __lookup、
// [skill group] 子表 __lookup_0、[extra condition] 子表 __lookup_1）以表查找
// 分派（报告 0x089bd8ea 实证：三组 __cxa_guard/SpinLock/__inited 双检 +
// 13 处 pair<char const(&)[N],int>::insert）。语义：
//   0x2f31 [/skill apply condition]→结束并返回 1；
//   0x2f35 [lower bound level]→ScanInt→m_head3（+0xc）；
//   0x2f39 [upper bound level]→ScanInt→m_head4（+0x10）；
//   0x2f3d [value]→ScanInt→m_head5（+0x14）；
//   0x2f41 [skill group]→ScanStr 查子表：0x2f49 passive→1 /
//     0x2f4c active→0 / 0x2f4f all→-1，写 m_head2（+0x8）；
//   0x2f59 [extra condition]→ScanStr 循环查子表：0x2f60 [normal skill]|1 /
//     0x2f63 [consume item skill]|2 / 0x2f66 [ex skill]|4 /
//     0x2f69 [except for ex skill]|3 位或入 flags，==0 返回 0，否则写
//     m_head6（+0x18）。
// 任一 ScanType/ScanInt/ScanStr 失败、或子表/主表未命中 → 返回 0。
// [b28d-solo-EquipmentParameterInfo] P167 CLEAR（本波补 2 个 SpinLock::leave 点）⇒ 超大件 N82 封顶。
static bool GetSkillApplyCondition(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (sp == 0) return 0;
    std::string token;
    // ORIG 0x89bd914/0x89bd918：先 `movb $0,-0x2e`（result）再 `movb $1,-0x2d`
    // （terminated）⇒ 声明序 result 在前、terminated 初值 1；命中 0x2f31 时
    // 0x89bdbe5/0x89bdbe9 写 result=1 / terminated=0。
    bool result = false;
    bool terminated = true;
    while (terminated && ScanType(token, true))
    {
        int id;
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                }
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[/skill apply condition]", 0x2f31));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[lower bound level]", 0x2f35));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[upper bound level]", 0x2f39));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[value]", 0x2f3d));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[skill group]", 0x2f41));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[extra condition]", 0x2f59));
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                }
            }
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        if (id == 0x2f39)
        {
            if (!ScanInt(&sp->m_head4)) return 0;
        }
        else if (id < 0x2f3a)
        {
            if (id == 0x2f31)
            {
                result = true;
                terminated = false;
            }
            else if (id == 0x2f35)
            {
                if (!ScanInt(&sp->m_head3)) return 0;
            }
            else
            {
                return 0;   // 主表未命中
            }
        }
        else
        {
            if (id == 0x2f3d)
            {
                if (!ScanInt(&sp->m_head5)) return 0;
            }
            else if (id == 0x2f41)
            {
                std::string str;
                if (!ScanStr(&str)) return 0;
                int gid;
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    if (!__inited)
                    {
                        // [b28d-solo-EquipmentParameterInfo] P167：ORIG 0x089bdfcb enter
                        // → 0x089bdfe2 `if (__inited) leave`（点1）→ inserts →
                        // 0x089be186 `if (!__inited) { leave; __inited = true; }`（点2）
                        // 与主表块同形（两个 leave 调用点）。
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                        }
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[passive]", 0x2f49));
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[active]", 0x2f4c));
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[all]", 0x2f4f));
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                        }
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(str.c_str());
                    gid = (it != __lookup.end()) ? it->second : -1;
                }
                int group;
                if (gid == 0x2f49)      group = 1;
                else if (gid == 0x2f4c) group = 0;
                else if (gid == 0x2f4f) group = -1;
                else                    return 0;   // 子表未命中
                sp->m_head2 = group;
            }
            else if (id == 0x2f59)
            {
                unsigned int flags = 0;
                std::string str;
                while (ScanStr(&str))
                {
                    int eid;
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (!__inited)
                        {
                            // 同块2：ORIG 0x089be41f enter → 0x089be436 leave（点1）→
                            // inserts → 0x089be60a `leave; __inited = true`（点2）。
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                            }
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[normal skill]", 0x2f60));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[consume item skill]", 0x2f63));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[ex skill]", 0x2f66));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[except for ex skill]", 0x2f69));
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                            }
                        }
                        std::map<const char*, int, less_TCHAR>::const_iterator it =
                            __lookup.find(str.c_str());
                        eid = (it != __lookup.end()) ? it->second : -1;
                    }
                    if (eid == 0x2f60)      flags |= 1;
                    else if (eid == 0x2f63) flags |= 2;
                    else if (eid == 0x2f66) flags |= 4;
                    else if (eid == 0x2f69) flags |= 3;
                    else                    return 0;   // 子表未命中
                }
                if (flags == 0) return 0;
                sp->m_head6 = flags;
            }
            else
            {
                return 0;   // 主表未命中
            }
        }
    }
    return result ? 1 : 0;
}

// GetSkillPhase —— ORIG 0x89be70c（static local）。消费 [skill phase] 块：
// 惰性初始化 static map（"[all]"→0x2f8c … "[fourth]"→0x2f9b），ScanStr 读
// token 查表，命中写 sp->m_head7 = phase（0..5）并返回 1，未命中返回 0。
// [b28d-solo-EquipmentParameterInfo] GetSkillPhase：✅ **IDENTICAL**（0x089be70c，
// 317=317）。P165/P177 结构还原（`if(!ScanStr) return false;` 负极性 + goto guard_retry
// 双检 + `int phase` 值槽 + 六 case 链式共享 insert 调用点，case 0 与 0x2f8c 共享）；
// 关键旋钮 = `int phase; int id;` 声明序（4.4 先声明占深槽 -0x20/-0x1c 与 ORIG 反，
// 交换后逐槽全等）。旧判词「删 continue 真 bug + 残差 N104」已由本波结构还原消解。
static bool GetSkillPhase(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (!sp) return false;
    std::string token;
    // [b28d-solo-EquipmentParameterInfo] P165/P177 结构还原（照 ORIG 0x089be70c 逐条
    // 转写）：`if (ScanStr(&token))` 正向包裹 + `for(;;)` 双检（find 在 `if(__inited)`
    // 分支内、双检命中 `continue` 回 guard 重查）+ `int phase` 值槽 + 六 case 链式
    // 共享 insert 调用点（case 0 与 0x2f8c 共享）+ switch 后统一
    // `if(!__inited){leave; __inited=true; goto guard_retry;}` 再 `sp->m_head7 = phase`。
    if (!ScanStr(&token)) return false;
    guard_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int phase;
        int id;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto guard_retry;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x2f8c:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[all]", 0x2f8c));
                goto L_2f8f;
            }
            phase = 5;
            goto after_switch;
    L_2f8f:
        case 0x2f8f:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[common]", 0x2f8f));
                goto L_2f92;
            }
            phase = 4;
            goto after_switch;
    L_2f92:
        case 0x2f92:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[first]", 0x2f92));
                goto L_2f95;
            }
            phase = 0;
            goto after_switch;
    L_2f95:
        case 0x2f95:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[second]", 0x2f95));
                goto L_2f98;
            }
            phase = 1;
            goto after_switch;
    L_2f98:
        case 0x2f98:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[third]", 0x2f98));
                goto L_2f9b;
            }
            phase = 2;
            goto after_switch;
    L_2f9b:
        case 0x2f9b:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[fourth]", 0x2f9b));
                goto L_default;
            }
            phase = 3;
            goto after_switch;
    L_default:
        default:
            if (__inited)
                return false;
            break;
        }
    after_switch:
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto guard_retry;
        }
        sp->m_head7 = phase;
        return true;
}

// ORIG：static map<const char*,int> 懒初始化——首次调用丢弃第一个 token（仅
// 触发初始化），其后未知 token 一律返回 0（default 分支）；[skill levelup] 的
// GetSkillLevelUp 直接填充 sp.m_vec20（+0x20），非局部 vector。
// [b28d-solo-EquipmentParameterInfo] importAllSkillItem：P165/P177 结构还原（贯穿建表
// 链 + 双检 goto 重试 + 五 case 链式共享 insert 调用点，P167 35/35 CLEAR）+ 四个
// callee bool 化（P248 档①：GetItemGrowtype/GetSkillApplyCondition/GetSkillPhase/
// GetSkillLevelUp 返回 int→bool，调用点 `call; xor $1; test; je` 逐条同形）⇒
// ndiff 318→2。残差 = 4.4 把外层 `return 0` 中继块落在成功块之后（ORIG 落函数末尾
// 与 && 短路失败共享）——块物理位置差（N121/P63 族），语义等价。
// 调用点 ORIG 0x89ac54e `call; xor $1,%eax; test %al,%al; je` ⇒ 返回 bool（P168a）。
static bool importAllSkillItem(EquipmentParameterInfo* info)
{
    EquipmentParameterInfo::SASPROPERTY sp;
    sp.m_head0 = 0xb; sp.m_head1 = 0; sp.m_head2 = 0xffffffff;
    sp.m_head3 = 0;   sp.m_head4 = 0; sp.m_head5 = 0;
    sp.m_head6 = 7;   sp.m_head7 = 5;
    std::string token;
    bool terminated = false;
    // [b28d-solo-EquipmentParameterInfo] 真语义修正：ORIG 0x089becbd
    // `movb $0x1,-0x1d(%ebp)` 是**局部**标志（每次调用都丢弃首 token），
    // 原 static 只在首次调用丢弃 ⇒ 语义偏差。
    bool s_first = true;
    int id;
    // [b28d-solo-EquipmentParameterInfo] P165/P177 结构还原（照 ORIG 0x089bec41 逐条
    // 转写）：while(s_first) 底部条件 + 循环体内 static 三件套（每轮穿 __cxa_guard）
    // + 双检 `if(__inited){leave; goto guard_retry}` + id=0 贯穿建表：case 0/0x2fc1
    // 共享块插入首项后 goto 到 case 0x2fc5 的 `if(!__inited)` 检查，各 case 的 insert
    // 块再链式 goto 到下一 case（P167 实证每项 insert 仅一个调用点）。
    while (s_first)
    {
        if (!ScanType(token, true))
            break;
    guard_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto guard_retry;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x2fc1:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[/all skill item]", 0x2fc1));
                goto L_2fc5;
            }
            terminated = true;
            s_first = false;
            goto after_switch;
    L_2fc5:
        case 0x2fc5:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[item growtype]", 0x2fc5));
                goto L_2fc9;
            }
            if (!GetItemGrowtype(&sp)) return 0;
            goto after_switch;
    L_2fc9:
        case 0x2fc9:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[skill apply condition]", 0x2fc9));
                goto L_2fcd;
            }
            if (!GetSkillApplyCondition(&sp)) return 0;
            goto after_switch;
    L_2fcd:
        case 0x2fcd:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[skill phase]", 0x2fcd));
                goto L_2fd1;
            }
            if (!GetSkillPhase(&sp)) return 0;
            goto after_switch;
    L_2fd1:
        case 0x2fd1:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[skill levelup]", 0x2fd1));
                goto L_default;
            }
            if (!GetSkillLevelUp(&sp.m_vec20)) return 0;
            goto after_switch;
    L_default:
        default:
            if (__inited)
                return 0;             // ORIG 0x89bf13f：未知 token（已初始化）→ 失败
            break;
        }
    after_switch:
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto guard_retry;
        }
    }
    // ORIG 0x089bf192：`if (!terminated) return 0;`（xor 物化 + 共享 return 0）。
    if (!terminated)
        return 0;
    if ((sp.m_head0 == 0xb && sp.m_head7 != 5) && sp.m_head7 != 4) return 0;
    // [b28d-solo-EquipmentParameterInfo] P222 两形互试定案：ORIG 0x89bf1f5 是三条失败
    // 路径（head3>0x46 / head4>0x46 / head3>head4）共享的 `mov $0,%ebx; jmp`，与函数末
    // return 0 同出口。正向包裹形（`if(h3<=h4){…}` 嵌套）4.4 反产 `jg`+独立 mov 块
    // （389 vs 391，更差）；早返形 391=391 仅余该共享块落点差 ⇒ N 封顶（N121/P63 族）。
    // [A18 本波 三形实测定稿] ①包裹形（本形）ndiff=**2**；②提前 return 形
    // （`if(h3>=0x47||h4>=0x47) return 0;`）劣化 **4**（ORIG 0x089bf1f5 两条 `ja`
    // 共享同一失败块，4.4 恒拆 `jbe`+独立块）；③C05① 多层嵌套形
    // （`if(a){ if(b){ … return 1; } } return 0;`）与包裹形**逐条等价、ndiff 仍 2**。
    // 残差 = 成功块后多发 1 个 `mov $0,%ebx; jmp` 外块（ORIG 该 return 0 落点与他块
    // 共享/后置，4.4 无法逐点控制）⇒ N121/P63 族封顶，gap 恒 +2。
    if ((unsigned)sp.m_head3 < 0x47 && (unsigned)sp.m_head4 < 0x47)
    {
        if (sp.m_head3 > sp.m_head4) return 0;
        if (sp.m_head5 < 1) return 0;
        info->m_119.m_418.push_back(sp);
        return 1;
    }
    return 0;
}

// ---- importItemOverpowerDesc（ORIG 0x089bf290，static local） ----
// 循环 ScanType 读 token；"[stat]"→局部 vector+GetStat（失败不 push）后 push
// 到 info->m_428；"[/item overpower desc]"→结束。ORIG 结尾语义：token 耗尽
// （未 terminated）返回 0；terminated 且 m_428 **非空**时置 m_424=m_425=1 返回 1；
// terminated 且 m_428 为空返回 0（0x089bf618 `je` 落 mov $0 块实证）。未知 token
// 返回 0（default）；首次调用以 id=0 贯穿建表后重查（P177）。
// [b28d-solo-EquipmentParameterInfo] importItemOverpowerDesc：✅ **IDENTICAL**
// （0x089bf290，266=266，P167 CLEAR 35/35）。本波 P165/P177 结构还原（贯穿建表链
// + 双检 goto 重试 + case 0/0x3001 共享 [stat] insert 调用点）+ 真语义修正（见尾部）。
// 调用点 ORIG 0x89ac5cb 同形 `xor $1` ⇒ 返回 bool（P168a）。
static bool importItemOverpowerDesc(EquipmentParameterInfo* info)
{
    std::string token;
    bool terminated = false;
    // ORIG 0x089bf2a8 `movb $0x1,-0x1d(%ebp)`：局部标志（同 importAllSkillItem）。
    bool s_first = true;
    int id;
    // [b28d-solo-EquipmentParameterInfo] P165/P177 结构还原（照 ORIG 0x089bf290
    // 逐条转写）：while(s_first) 底部条件 + 循环体内 static 三件套（每轮穿
    // __cxa_guard 检查）+ 双检 `if(__inited){leave; goto guard_retry}` +
    // `id=0` 贯穿建表（case 0 与 0x3001 共享块，首次插入两项后 goto 重试）+
    // switch 后 `if(!__inited){leave; __inited=true; goto guard_retry}`。
    while (s_first)
    {
        if (!ScanType(token, true))
            break;
    guard_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto guard_retry;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x3001:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[/item overpower desc]", 0x3001));
                // ORIG 0x089bf4fc `jmp 089bf50b`：第一项插入后**跳到 case 0x3005
                // 的 `if (!__inited)` 检查块**（共享 [stat] insert 调用点，
                // P167 实证 ORIG 该 insert 仅一个调用点）。
                goto L_insert_stat;
            }
            else
            {
                terminated = true;
                s_first = false;
            }
            break;
    L_insert_stat:
        case 0x3005:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[stat]", 0x3005));
            }
            else
            {
                // ORIG 0x89bf559：局部 vector C1 + GetStat（内部 push 12.0f +
                // GetStatValue，返回值取 %eax 残留）成功才 push 到 m_428；D1。
                std::vector<float> v;
                if (GetStat(&v))
                    info->m_119.m_428.push_back(v);
            }
            break;
        default:
            if (__inited)
                return 0;         // ORIG 0x89bf5b2：未知 token（已初始化）→ 失败
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto guard_retry;
        }
    }
    // ORIG 0x089bf5fb 段实证：`movzbl terminated; xor $1; test; jne <mov $1>` +
    // `call empty; test %al; je <mov $0>` ⇒ cond = `!terminated || empty()`；
    // cond==0（terminated 且 m_428 **非空**）才置 m_424/m_425=1 返回 1。
    // [真语义修正] 旧实现（及旧判词「空时置 1」）把条件判反：ORIG 是**非空**置 1。
    // 德摩根取反包裹形（P258）复现 cond 物化 + 共享 return 0 块，逐条全等。
    if (!(terminated && !info->m_119.m_428.empty()))
        return 0;
    info->m_119.m_424 = 1;
    info->m_119.m_425 = 1;
    return 1;
}

#include "EquipmentParameterInfo/importEquipmentParameterInfo.inc"


// exportEquipmentParameterScript —— ORIG 0x089acf78（对照报告 1527-2007 行逐段转写）。
// 各段条件与标签文本照 ORIG：无名称的状态类型（如 0x12 默认）不输出任何内容。
// 注：ORIG 的 %.2f/%.1f 传参经 fistpl 取整后按 int 传入（Ghidra ROUND 伪影，原
// 生即为未定义 vararg 用法）；此处以可读 double 直出（数值舍入差异记录于报告）。
// avatar 能力标签表与 importEquipmentParameterInfo 内一致（AVTVARIATION_TAG_NAME）。
// [b28d-solo-EquipmentParameterInfo] 修 it++（P28）/std::min<int>；残差 avatar 段 string 形 + empty 结构 ⇒ N。
// [A18 本波] ndiff **500→360**（ORIG 1506 / OURS 1468）。四修 + 真语义修正 ×1：
//  ①**删多余判空**（P171 照抄）：ORIG 头部无 `if(!file||!info)`，直接
//     `mov 0xc(%ebp),%eax; mov 0x4(%eax),%eax; test; je` 判 info->m_field4；
//  ②**真语义修正**（ORIG 0x089add6d..0x089ade75 常量序实证）[active status] 名称段：
//     ORIG 链序 = 0..0xa, **0x11**, 0xb, 0xc, 0xd, 0xe（armor break@0x11、
//     bleeding@0xb、haste@0xc、bless@0xd、element@0xe；无 0xf/0x10）——
//     旧实现 armor break@0xb、bleeding..element 顺移一格（11..15）整体错位；
//     并改 **else-if 链**（ORIG 每条 fwrite 后 `jmp 0x89ade9d` 共享尾）；
//  ③**删具名 t 缓存**（P157/P183）：ORIG 每链 `mov 0x144(%eax),%eax; test/cmp $K`
//     字段直读（我方旧形 `mov %eax,-0x24; cmpl $K,-0x24`）；
//  ④separate-rate 段 x87 形（P243/P252/N85）：`fabs((double))` + 比较写 **`>`**
//     （`fabsf(sa-1.0f) > eps*fabsf(sa)`）⇒ 复现 ORIG 先算左值存 double 槽
//     `fstpl` + 后算 `epsilon()*fabs(sa)` 的求值序与 `fsubrp` 装载序；
//     残余 = **N85** x87 比较 lowering（ORIG `fucompp/fnstsw/test $5/sete/xor`
//     vs 4.4 `fucomip/fstp/seta`）+ 帧 0x180/0x170、槽平移 8（N95/N115 族）
//     + skill-levelup 迭代器三槽（ORIG 单槽 -0x3c，N86/P50 族）。
// [OurOnly33 符号名轴 2026-09-11] ORIG 权威（export 0x089ae692 / 0x89ae6f3 两处
// `mov 0x93f5540(,%eax,4),%eax`）：状态名表 = 全局 `ACTIVE_STATUS_NO_BOX_BRACKET_TAG`
// （D 0x093f5540, 0x4c=19 指针=18 项+NULL；定义见 CreatureScript.cpp，与该 TU 的
// getActiveStatusType 0x088944fb 同表）⇒ 删本函数内 static kActStatNoBox
//（我方发明名 `_ZZ30exportEquipmentParameterScript…E13kActStatNoBox`，ORIG 全 ELF 无）。
extern "C" const char* ACTIVE_STATUS_NO_BOX_BRACKET_TAG[19];
#include "EquipmentParameterInfo/exportEquipmentParameterScript.inc"


#include "EquipmentParameterInfo/importSkillDataUp.inc"


// ============================ EquipmentParameterInfo::._119/._159 别名 ============================
// GCC 对 ctor/dtor 的 asm 别名（asm("...")）无效（硬限制）；匿名嵌套 struct 又按本 TU
// 行号计数命名（如 4._85），与 ORIG `5._119`/`5._159` 不一致。故在类内命名 `_119`，
// 用 `.set` 把自然 mangled `5t_119*` 强制改名为 ORIG 带点号 `5._119*`（ctor/dtor 四件）。
// **operator= 例外（b28d-solo-EquipmentParameterInfo 实证）**：asm 标签对 operator=
// **有效**（探针 /tmp/b28d_epi_probe/asmlabel.cpp：声明带标签 ⇒ 定义与调用点重定位
// 均用标签名），故两个 op= 的符号名由头文件 asm 标签直接指定（t_119 → `._159aSERKS0_`
// 匹配 ORIG 0x08348024 的调用点；tt_159 → `._119aSERKS0_` 对应 ORIG 0x088fd910），
// 不再走 `.set`（P262：两别名须各有独立函数体，此式满足）。
// cf. docs/class_func_reports/EquipmentParameterInfo__._119
// （0x088fd884/0x0898c680）与 ._159（0x08348024/0x0861c9b8）。
asm(".set _ZN22EquipmentParameterInfo5._119C1Ev, _ZN22EquipmentParameterInfo5t_119C1Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119C1Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119C1Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119C1Ev,0x45");
asm(".set _ZN22EquipmentParameterInfo5._119C2Ev, _ZN22EquipmentParameterInfo5t_119C2Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119C2Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119C2Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119C2Ev,0x45");
asm(".set _ZN22EquipmentParameterInfo5._119D1Ev, _ZN22EquipmentParameterInfo5t_119D1Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119D1Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119D1Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119D1Ev,0x45");
asm(".set _ZN22EquipmentParameterInfo5._119D2Ev, _ZN22EquipmentParameterInfo5t_119D2Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119D2Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119D2Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119D2Ev,0x45");

// [P203-K 实测 2026-09-11 GiantB] 本 TU 匿名类型计数器 K=**149**（顶部无哑类型时
// 目标位置编号 149 > ORIG `._119`）⇒ 计数器只增不减，物理不可能对齐到 119
// ⇒ 按 P262「同址别名豁免」落判词：`._119C1/C2/D1/D2` 已由 `.set` 别名存在（T 同址），
// p167 缺的是**重定位目标名**（我方 `5t_119C1Ev` vs ORIG `5._119C1Ev`），源级无解。
