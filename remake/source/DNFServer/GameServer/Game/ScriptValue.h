#ifndef GAME_SCRIPTVALUE_H_
#define GAME_SCRIPTVALUE_H_

#include <vector>

// ScriptValue.h — df_game_r 还原（RDAR 脚本动作参数域，<global> 0x88bd381 区）。
// scanScriptValue / pushScriptValue / getScriptValue 与
// ScriptValue 特殊成员 ORIG 符号/地址：
//   scanScriptValue(ScriptValue&)               _Z15scanScriptValueR11ScriptValue      0x088bd381
//   scanScriptValue(vector<float>&)             _Z15scanScriptValueRSt6vectorIfSaIfEE   0x088bd8e6
//   pushScriptValue(vector<float>&,ScriptValue&) _Z15pushScriptValueRSt6vectorIfSaIfEER11ScriptValue 0x088bd929
//   ScriptValue::ScriptValue()                  _ZN11ScriptValueC1Ev/_C2Ev              0x088c4200
//   ScriptValue::ScriptValue(ScriptValue const&) _ZN11ScriptValueC1ERKS_/_C2ERKS_        0x083661e2
//   ScriptValue::operator=(ScriptValue const&)  _ZN11ScriptValueaSERKS_                 0x08366220
// 三者被 EquipmentParameterInfo::readAction（0x89a13e5）调用以解析动作参数值。
//
// ScriptValue 布局（依据 ORIG 反汇编 + 默认构造 @0x88c4200）：
//   +0x00 int   type     值类型（0=数值, 1=技能等级信息["skill level info"],
//                          2=对象编号["object number"]；默认构造置 3=空）
//   +0x04 float value[4] 动作参数（数值路径仅用 value[0]）
//   总大小 0x14（20 字节）。
struct ScriptValue
{
    int   type;          // +0x00
    float value[4];      // +0x04..+0x10

    ScriptValue();                              // ORIG 0x088c4200
    ScriptValue(const ScriptValue& other);      // ORIG 0x083661e2
    ScriptValue& operator=(const ScriptValue& other);  // ORIG 0x08366220
};

// ChangeStatusData —— ORIG 布局依 scanChangeStatusData（0x088bda1c）反汇编：
//   +0x00 int   type     getChangeStatusType(token)（ctor 默认 0x4d）
//   +0x04 int   percent  token=="%" ? 1 : 0（ctor 置 0）
//   +0x08 float value    ScanInt 转 float（ctor 置 0）
// 总大小 0xc。ChangeStatusData::ChangeStatusData() ORIG 0x089c2dd8。
// 拷贝构造 _ZN16ChangeStatusDataC1ERKS_（0x08366190 W）与 operator=
// （0x083661b8 W）均为编译器隐式生成的逐字段浅拷贝（0x08366190 反汇编
// 依序 mov +0/+4/+8 三个 dword），不含任何用户逻辑；按 P74 不显式声明，
// 保留隐式生成与 ORIG 弱符号同形。gcc4.4.7 在 trivial 拷贝处内联为
// 3×mov（ORIG gcc4.1.2 为独立 call），语义等价，跨工具链结构差异豁免。
struct ChangeStatusData
{
    int   type;      // +0x00
    int   percent;   // +0x04
    float value;     // +0x08

    ChangeStatusData() : type(0x4d), percent(0), value(0.0f) {}
    // ORIG W 0x08366190（C2 同址）：编译器隐式逐字段浅拷贝；为发射 W 符号显式声明
    // （语义与隐式一致，gcc4.4.7 在 force 使用点仍内联 3×mov）。
    ChangeStatusData(const ChangeStatusData& o)
        : type(o.type), percent(o.percent), value(o.value)
    {
    }
    // ORIG W 0x083661b8：隐式逐字段浅拷贝；为发射 W 符号显式声明
    // （语义与隐式一致，gcc4.4.7 在 force 使用点仍内联 3×mov）。
    ChangeStatusData& operator=(const ChangeStatusData& o)
    {
        type = o.type;
        percent = o.percent;
        value = o.value;
        return *this;
    }
};
static_assert(sizeof(ChangeStatusData) == 0xc, "ChangeStatusData size mismatch");

// VariableStat —— ChangeStatusData @+0x00 后接 ScriptValue @+0x0c，
// 总大小 0x20（依 scanVariableStat 0x088bdb27 的 `add $0xc` 与
// VariableStat ctor 0x089c2dfc 逐成员构造）。
struct VariableStat
{
    ChangeStatusData status;   // +0x00..0x0b
    ScriptValue      value;    // +0x0c..0x1f
};
static_assert(sizeof(VariableStat) == 0x20, "VariableStat size mismatch");

// ORIG 0x088bda1c：ScanTypeOrStr → getChangeStatusType → ScanStr("%" 判定) →
// ScanInt 转 float；任一步失败返回 false。
bool scanChangeStatusData(ChangeStatusData& data);

// ORIG 0x088bdb27：先 scanChangeStatusData(&vs)，成功则 scanScriptValue(vs.value)；
// 失败返回 false。
bool scanVariableStat(VariableStat& vs);

// ORIG 0x088bd381：读下一个脚本 token 填充 ScriptValue；成功返回 true。
//   DECIMAL / HEXADECIMAL / FLOAT → type=0, value[0]=数值；
//   STRING "skill level info"（id 0x674）→ type=1 + 3×ScanFloat(value[0..2])；
//   STRING "object number"（id 0x686）→ type=2 + ScanStr→getTargetToEnumValue(value[0])
//                                          / ScanFloat(value[1])
//                                          / ScanStr→getActiveStatusToEnumValue(value[2])
//                                          / ScanFloat(value[3])；
//   其它 token / 未知关键字 → false。
bool scanScriptValue(ScriptValue& v);

// ORIG 0x088bd8e6：调 scanScriptValue(ScriptValue&)，成功则 pushScriptValue 并返回 true。
bool scanScriptValue(std::vector<float>& values);

// ORIG 0x088bd929：把 ScriptValue 的 type（转 float）与 4×value 依次 push 进 values。
void pushScriptValue(std::vector<float>& values, ScriptValue& v);

#endif // GAME_SCRIPTVALUE_H_
