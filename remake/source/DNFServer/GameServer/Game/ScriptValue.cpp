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
bool scanScriptValue(ScriptValue& v)
{
    DNFLEX_TOKEN tok = static_cast<DNFLEX_TOKEN>(g_dnfLex.getToken());
    if (tok == DNF_TOK_HEXADECIMAL)
    {
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getHexDecimal());
        return true;
    }
    if (tok == DNF_TOK_DECIMAL)
    {
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getDecimal());
        return true;
    }
    if (tok == DNF_TOK_FLOAT)
    {
        v.type = 0;
        v.value[0] = g_dnfLex.getFloat();
        return true;
    }
    if (tok == DNF_TOK_STRING)
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
                v.value[0] = static_cast<float>(getTargetToEnumValue(key));
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
    return false;
}

// ORIG 0x088bd8e6：默认构造 ScriptValue → scanScriptValue → 成功则 pushScriptValue。
bool scanScriptValue(std::vector<float>& values)
{
    ScriptValue v;
    if (scanScriptValue(v))
    {
        pushScriptValue(values, v);
        return true;
    }
    return false;
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
