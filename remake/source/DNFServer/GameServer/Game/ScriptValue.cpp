// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScriptValue/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

// [b28d-ScriptValue P211 档①] 本 TU 镜像声明（定义侧在 GameScriptEnumHelpers.cpp；
// 返回类型不入 mangled、ABI 同：eax 传值、值域非负）。故本 TU 不 include 该权威头，
// 以免 int/unsigned 冲突声明。ORIG 调用点实证：
//   getChangeStatusType 0x088bda7b `call; mov %eax,%edx; mov 0x8(%ebp),%eax;
//     mov %edx,(%eax)` = RHS 值先 + eax→edx 中转 ⇒ 字段(int) 与 callee 返回异型（P211）。
//   getTargetToEnumValue 0x088bd76b `mov $0x0,%edx; mov %eax,-0x78(%ebp);
//     mov %edx,-0x74(%ebp); fildll` = 零扩展 64 位（int 版被 4.4 前端折叠成 fildl）。
unsigned int getChangeStatusType(std::string& s);      // 0x8892b0d
unsigned int getTargetToEnumValue(std::string& s);     // 0x88c2c34
#include "ScriptValue/getActiveStatusToEnumValue.inc"


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

#include "ScriptValue/scanScriptValue.inc"


#include "ScriptValue/scanScriptValue_1.inc"


#include "ScriptValue/pushScriptValue.inc"


#include "ScriptValue/scanChangeStatusData.inc"


#include "ScriptValue/scanVariableStat.inc"

