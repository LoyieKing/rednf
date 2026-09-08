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
// b28d-52 判词表（本 TU 剩余 DIFF）：
//   scanChangeStatusData 0x088bda1c —— P264 重定位（std::string C1/ScanTypeOrStr 目标）
//     + 帧/槽差
//   scanScriptValue 0x088bd381 —— P264 重定位（DNFLexWrapper 单例立即数 .o 显示 0）
//     + N82 帧/槽
// ============================================================================
#include "DNFLexWrapper.h"
#include "GameScriptEnumHelpers.h"
#include "SyncScriptDeps.h"
#include "CEnvironment.h"   // less_TCHAR（strcmp 比较器）、SpinLock

#include <map>
#include <string>
#include <utility>

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
    case DNF_TOK_HEXADECIMAL:
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getHexDecimal());
        return true;
    case DNF_TOK_DECIMAL:
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getDecimal());
        return true;
    case DNF_TOK_FLOAT:
        v.type = 0;
        v.value[0] = g_dnfLex.getFloat();
        return true;
    case DNF_TOK_STRING:
        {
        std::string key = toTString(g_dnfLex.getString());
        // ORIG：静态 __lookup（关键字→id，map<const char*,int,less_TCHAR>）+
        // __init_lock（SpinLock）+ __inited 标志。首次进入在锁内填充两张表项
        // 后置 inited；此后按 id 派发。map 插入幂等 + 锁保护，线程安全。
        static std::map<const char*, int, less_TCHAR> lookup;
        static SpinLock lock;
        static bool inited = false;

        if (!inited)
        {
            lock.enter();
            if (!inited)
            {
                lookup.insert(std::pair<const char* const, int>("skill level info", 0x674));
                lookup.insert(std::pair<const char* const, int>("object number", 0x686));
                inited = true;
            }
            lock.leave();
        }

        std::map<const char*, int, less_TCHAR>::const_iterator it = lookup.find(key.c_str());
        int id = (it != lookup.end()) ? it->second : -1;

        if (id == 0x674)          // "skill level info"
        {
            v.type = 1;
            if (ScanFloat(&v.value[0]) && ScanFloat(&v.value[1]) && ScanFloat(&v.value[2]))
                return true;
            return false;
        }
        if (id == 0x686)          // "object number"
        {
            v.type = 2;
            if (ScanStr(&key))
            {
                // ORIG：fildll（8 字节）→ fstps：int 经 long long 中转
                v.value[0] = static_cast<float>(
                    static_cast<long long>(getTargetToEnumValue(key)));
                if (ScanFloat(&v.value[1]))
                {
                    if (ScanStr(&key))
                    {
                        v.value[2] = static_cast<float>(getActiveStatusToEnumValue(key));
                        if (ScanFloat(&v.value[3]))
                            return true;
                    }
                }
            }
            return false;
        }
        return false;
        }
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
// [b28c-L23 N76/P211 三档试尽] 唯一结构残差 = `data.type = getChangeStatusType(s)`
// 的 store 方向：ORIG @0x088bda54 为 RHS 值先 + `mov %eax,%edx` 中转（3 条），4.4 对
// 「int 字段 ← int callee」恒 LHS 地址先（2 条）。档① 改 getChangeStatusType 返回类型
// 会打坏其 8 处跨 TU 消费点（含 5 处 `(float)` 转换）故不采；档② 字段 type
// int→unsigned 会改 ChangeStatusData 全部消费段；档③ 调用点 `(unsigned int)` cast
// 实测被前端折叠（ndiff 12→12 零改善）⇒ 落 N76，其余 9 行是连锁 jmp 偏移。
bool scanChangeStatusData(ChangeStatusData& data)
{
    std::string s;
    bool isType = false;
    if (!ScanTypeOrStr(isType, s, true)) return false;
    if (isType) return false;
    // b28c-l23（P211 双向开关·调用侧异型化）：ORIG @0x088bda54 为
    // `call getChangeStatusType; mov %eax,%edx; mov 0x8(%ebp),%eax; mov %edx,(%eax)`
    // = RHS 值先物化 + 中转 ⇒ 字段(int) 与 RHS 必**异型**；int↔int 同型时 4.4 恒
    // LHS 地址先（`mov 0x8,%edx; mov %eax,(%edx)`）。共享函数返回类型不入 mangled 但
    // 有 8 处跨 TU 消费点（改之风险大），故在**本调用点**做 unsigned 视图转换，
    // 触发 4.4 的转换中转 = ORIG 形；语义（值域 0..0x4d）零变化。
    data.type = (unsigned int)getChangeStatusType(s);
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
