// ============================================================================
// df_game_r 还原 —— ScriptValue 脚本动作参数（<global> 0x88bd381 域）
//   scanScriptValue(ScriptValue&)       0x088bd381
//   scanScriptValue(vector<float>&)     0x088bd8e6
//   pushScriptValue                     0x088bd929
//   ScriptValue::ScriptValue()          0x088c4200（type=3，value[0..3]=0）
//   ScriptValue 拷贝 / 赋值             0x083661e2 / 0x08366220
// 权威头：ScriptValue.h。被 EquipmentParameterInfo::readAction（0x89a13e5）调用。
// ============================================================================

#include "ScriptValue.h"
// ============================================================================
// b28d-ScriptValue 判词表：
//   scanChangeStatusData 0x088bda1c —— ✅ AE（size 0x10b 双侧全等；P211 档① 本 TU 镜像
//     声明 getChangeStatusType→unsigned int，ORIG 0x088bda7b RHS-first+中转逐条同形）。
//   scanScriptValue 0x088bd381 —— gap 49→5（objdump 365/360；尺寸 0x565/0x52b，差 0x3a
//     ≈ ORIG 12 条 `mov $0/1,%esi`）。残余 = GCC 4.1 bool 返回值用 ebx+esi 双寄存器
//     （4.4 单 ebx）+ 2 条对齐 nop（§6.1 豁免）⇒ N7/N18a 封顶；p167 47=47 CLEAR（差集
//     仅 C1/C2、D1/D2 同址别名）；局部静态符号名/尺寸/个数 6/6 全等（P187）。
//   `_Z41__static_initialization_and_destruction_0ii` DIFF = N123 撞名豁免。
// ============================================================================
#include "DNFLexWrapper.h"
#include "SyncScriptDeps.h"
#include "CEnvironment.h"   // less_TCHAR（strcmp 比较器）、SpinLock

#include <map>
#include <string>
#include <utility>

// [b28d-ScriptValue P211 档①] 本 TU 镜像声明（定义侧在 GameScriptEnumHelpers.cpp；
// 返回类型不入 mangled、ABI 同：eax 传值、值域非负）。故本 TU 不 include 该权威头，
// 以免 int/unsigned 冲突声明。ORIG 调用点实证：
//   getChangeStatusType 0x088bda7b `call; mov %eax,%edx; mov 0x8(%ebp),%eax;
//     mov %edx,(%eax)` = RHS 值先 + eax→edx 中转 ⇒ 字段(int) 与 callee 返回异型（P211）。
//   getTargetToEnumValue 0x088bd76b `mov $0x0,%edx; mov %eax,-0x78(%ebp);
//     mov %edx,-0x74(%ebp); fildll` = 零扩展 64 位（int 版被 4.4 前端折叠成 fildl）。
unsigned int getChangeStatusType(std::string& s);      // 0x8892b0d
unsigned int getTargetToEnumValue(std::string& s);     // 0x88c2c34
int getActiveStatusToEnumValue(std::string& s);        // 0x88c29a2

// ORIG 0x088c4200：type=3（空标记），value[0..3]=0（循环 0..3）。
ScriptValue::ScriptValue()
{
    type = 3;
    for (int i = 0; i < 4; ++i)
        value[i] = 0.0f;
}

// ORIG 0x083661e2：逐字段拷贝（type + 4×value）。
ScriptValue::ScriptValue(const ScriptValue& other)
{
    type = other.type;
    for (int i = 0; i < 4; ++i)
        value[i] = other.value[i];
}

// ORIG 0x08366220：同拷贝，返回 *this。
ScriptValue& ScriptValue::operator=(const ScriptValue& other)
{
    type = other.type;
    for (int i = 0; i < 4; ++i)
        value[i] = other.value[i];
    return *this;
}

// ORIG 0x088bd381
// ORIG 0x088bd381：switch(tok) 树（cmp $0x3 为根；DECIMAL=2/FLOAT=4/STRING=7）。
bool scanScriptValue(ScriptValue& v)
{
    DNFLEX_TOKEN tok = static_cast<DNFLEX_TOKEN>(g_dnfLex.getToken());
    switch (tok)
    {
    // [b28d-ScriptValue P324] 叶块物理序 = 书写序：ORIG 0x3ca(STRING)/0x4e1(DECIMAL)/
    // 0x509(HEXADECIMAL)/0x531(FLOAT) ⇒ 书写序照此排列（决策树按值域，与本序无关）。
    case DNF_TOK_STRING:
        {
        std::string key = toTString(g_dnfLex.getString());
        // [b28d-ScriptValue P333/P177] ORIG 0x088bd381 STRING 段 = 「首建表、之后查表」
        // 状态机（ORIG 指纹：0x610/0x6e5/0x808 三处 `movzbl __inited; xor $1; test; je`；
        // insert 尾 `jmp` 下一 case 体头；`movb $0x1,__inited` 后 `jmp 0x427` 回跳重查）。
        // retry 标签置于 static 三件套声明之前 ⇒ 每轮穿 guard（P177）；id==0 为建表哨兵，
        // case 0 与 case 0x674 合并体 + fallthrough 链一次调用插完两键。符号名照 ORIG
        // （_ZZ…E8__lookup / E11__init_lock / E8__inited，P187 双侧核对）。
    retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int id;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(key.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            id = 0;
        }

        switch (id)
        {
        case 0:
        case 0x674:               // "skill level info"
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("skill level info", 0x674));
                // 不 break：首轮建表贯穿到下一 case 继续插键（ORIG fallthrough 链）
            }
            else
            {
                v.type = 1;
                if (!ScanFloat(&v.value[0])) return false;
                if (!ScanFloat(&v.value[1])) return false;
                if (!ScanFloat(&v.value[2])) return false;
                return true;
            }
        case 0x686:               // "object number"
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("object number", 0x686));
                break;
            }
            else
            {
                v.type = 2;
                if (!ScanStr(&key)) return false;
                // ORIG 0x088bd76b：零扩展 64 位（`mov $0,%edx`）→ fildll → fstps
                v.value[0] = static_cast<float>(
                    static_cast<long long>(getTargetToEnumValue(key)));
                if (!ScanFloat(&v.value[1])) return false;
                if (!ScanStr(&key)) return false;
                v.value[2] = static_cast<float>(getActiveStatusToEnumValue(key));
                if (!ScanFloat(&v.value[3])) return false;
                return true;
            }
        default:
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;
        }
        else
        {
            return false;
        }
        }
    case DNF_TOK_DECIMAL:
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getDecimal());
        return true;
    case DNF_TOK_HEXADECIMAL:
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getHexDecimal());
        return true;
    case DNF_TOK_FLOAT:
        v.type = 0;
        v.value[0] = g_dnfLex.getFloat();
        return true;
    default:
        return false;
    }
}

// ORIG 0x088bd8e6：默认构造 ScriptValue → scanScriptValue → 成功则 pushScriptValue。
bool scanScriptValue(std::vector<float>& values)
{
    ScriptValue v;
    // ORIG 0x088bd902 `xor $0x1; test; je +0x2a`：negated 判定走成功路（pushScriptValue;
    // mov $1 直落），失败 `mov $0;jmp` 岛紧贴 test 之后。`if(!f(v))return false;`
    // 负向先返形态复现该块序（P191 早返块落点）。
    if (!scanScriptValue(v))
    {
        return false;
    }
    else
    {
        pushScriptValue(values, v);
    }
    return true;
}

// ORIG 0x088bd929：先 push type（转 float），再 push value[0..3]。
void pushScriptValue(std::vector<float>& values, ScriptValue& v)
{
    values.push_back(static_cast<float>(v.type));
    for (int i = 0; i < 4; ++i)
        values.push_back(v.value[i]);
}

// ORIG 0x088bda1c：
//   ScanTypeOrStr(isType, s, true)；失败或 isType==true → false。
//   data.type = getChangeStatusType(s)；
//   ScanStr(&s)；失败 → false；
//   data.percent = (s == "%") ? 1 : 0；
//   ok=false；data.value = (float)ScanInt(&ok)；ok==false → false；否则 true。
// [b28d-ScriptValue P211 档①] store 方向：ORIG @0x088bda7b 为 RHS 值先 +
// `mov %eax,%edx` 中转（4 条）⇒ 字段(int) 与 callee 返回**异型**。正解 = 本 TU
// 镜像声明 `unsigned int getChangeStatusType(...)`（返回类型不入 mangled、ABI 同；
// 调用点 cast 会被前端折叠，故须改声明）。字段与全部消费段零改动。
bool scanChangeStatusData(ChangeStatusData& data)
{
    std::string s;
    bool isType = false;
    if (!ScanTypeOrStr(isType, s, true)) return false;
    if (isType) return false;
    data.type = getChangeStatusType(s);
    if (!ScanStr(&s)) return false;
    data.percent = (s == "%") ? 1 : 0;
    bool ok = false;
    data.value = (float)ScanInt(&ok);
    if (!ok) return false;
    return true;
}

// ORIG 0x088bdb27：scanChangeStatusData(vs.status) 成功后
// scanScriptValue(vs.value)（vs +0xc），返回其结果；否则 false。
bool scanVariableStat(VariableStat& vs)
{
    if (!scanChangeStatusData(vs.status)) return false;
    return scanScriptValue(vs.value);
}
