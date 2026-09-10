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
//   判据㉓ 符号绑定：ScriptValue 5 符号 ORIG 全 W / 我方原 T ⇒ 三处定义加
//     __attribute__((weak))（尺寸 0x38/0x3e/0x41 不变，全仓唯一 .cpp 定义无覆盖风险）。
//   判据⑫/P338：本 TU 原 include DNFLexWrapper.h → DNFLex.h → FlexLexer.h → <iostream>
//     带出 __ioinit，多发 `__static_initialization_and_destruction_0ii` +
//     `_GLOBAL__I_…`（ORIG 本 TU 无 GI，nm 实证）⇒ 改 TU 局部权威签名声明
//     （P144 镜像 + DNF_TOK_* 值照抄）后 GI/__static_init/ioinit 三者全消。
//   TU 四数（check_tu_routed fresh）：43/74/0/3/23 → 43/75/0/1/22，零回退。
// ============================================================================
#include "SyncScriptDeps.h"
#include "CEnvironment.h"   // less_TCHAR（strcmp 比较器）、SpinLock

#include <map>
#include <string>
#include <utility>

// [b28d-ScriptValue P338/判据⑫] 本 TU **不** include DNFLexWrapper.h：其
// DNFLex.h → FlexLexer.h → <iostream> 会带出 std::ios_base::Init __ioinit，
// 进而多发 `_Z41__static_initialization_and_destruction_0ii` +
// `_GLOBAL__I__Z15scanScriptValueR11ScriptValue`（仅 NEW 符号）。ORIG 本 TU 无 GI
// （nm 全二进制 509 个 `_GLOBAL__I_` 无任何 ScriptValue/scanScriptValue 键名实证）
// ⇒ 原码不经重头取得这些声明。按 P144 改用 TU 局部权威签名声明（非虚成员调用
// 仅依赖 mangled 名，布局无关）；DNF_TOK_* 值逐条照抄 DNFLex.h:32-46。
enum
{
    DNF_TOK_DECIMAL     = 2,
    DNF_TOK_HEXADECIMAL = 3,
    DNF_TOK_FLOAT       = 4,
    DNF_TOK_STRING      = 7
};

class DNFLexWrapper
{
public:
    int   getToken();       // 0x8ace7e0
    char* getString();      // 0x8ace6f0
    int   getHexDecimal();  // 0x8ace780
    int   getDecimal();     // 0x8ace2f0
    float getFloat();       // 0x8ace730
};
extern DNFLexWrapper g_dnfLex;

bool ScanFloat(float* value);                                  // 0x88bc58e 族
int  ScanInt(bool* ok);                                        // 0x88bc37b
bool ScanStr(std::string* value);                              // 0x88bc7a2
bool ScanTypeOrStr(bool& isType, std::string& s, bool consume); // 0x88bc9bf

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

// [b28d-ScriptValue 判据㉓ 符号绑定] ORIG 五个符号全为 **W**（弱绑定）：
//   C1/C2 0x088c4200 W 0x38、C1ERKS_/C2ERKS_ 0x083661e2 W 0x3e、aSERKS_ 0x08366220 W 0x41；
// 我方原为 T（强绑定，唯一 .cpp 外联定义）。ORIG 的两处地址分布说明其定义在头内、
// 由各使用点发射；按 Main 批准的「属主对自己 TU 内定义加 __attribute__((weak))」
// 单点修法（SweepCipher 广播 判据㉓ 清单口径），此处三处定义加 weak ⇒ 绑定对齐 W，
// 尺寸/函数体/mangled 零变化（本类全仓仅本 TU 定义，无覆盖风险）。
// ORIG 0x088c4200：type=3（空标记），value[0..3]=0（循环 0..3）。
__attribute__((weak)) ScriptValue::ScriptValue()
{
    type = 3;
    for (int i = 0; i < 4; ++i)
        value[i] = 0.0f;
}

// ORIG 0x083661e2：逐字段拷贝（type + 4×value）。
__attribute__((weak)) ScriptValue::ScriptValue(const ScriptValue& other)
{
    type = other.type;
    for (int i = 0; i < 4; ++i)
        value[i] = other.value[i];
}

// ORIG 0x08366220：同拷贝，返回 *this。
__attribute__((weak)) ScriptValue& ScriptValue::operator=(const ScriptValue& other)
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
    // getToken() 返回 DNFLEX_TOKEN（ORIG 底层 int）；本 TU 不引入该枚举类型
    // （见顶部 P338 说明），用 int 承载——二进制与枚举形相同。
    int tok = g_dnfLex.getToken();
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
