// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ScriptValue/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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

#include "ScriptValue/ScanFloat.inc"
#include "ScriptValue/ScanInt.inc"
#include "ScriptValue/ScanStr.inc"
#include "ScriptValue/ScanTypeOrStr.inc"
#include "ScriptValue/getActiveStatusToEnumValue.inc"
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
#include "ScriptValue/scanScriptValue.inc"
}

// ORIG 0x088bd8e6：默认构造 ScriptValue → scanScriptValue → 成功则 pushScriptValue。
#include "ScriptValue/scanScriptValue_1.inc"
}

// ORIG 0x088bd929：先 push type（转 float），再 push value[0..3]。
#include "ScriptValue/pushScriptValue.inc"
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
#include "ScriptValue/scanChangeStatusData.inc"
}

// ORIG 0x088bdb27：scanChangeStatusData(vs.status) 成功后
// scanScriptValue(vs.value)（vs +0xc），返回其结果；否则 false。
#include "ScriptValue/scanVariableStat.inc"
}
