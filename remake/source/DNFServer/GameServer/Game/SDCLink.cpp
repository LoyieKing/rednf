// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SDCLink/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "STComputeDamageVariable.h"   // STComputeDamageVariable / ENUM_CONVERT_TABLE_TYPE（权威布局，DspD1 产出）

#include <algorithm>

// [T2 · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化（__ioinit）⇒ 本 TU 发射
// `_GLOBAL__I_g_maxTotalDefenseRate`（ORIG 0x08096a10，GI 体 sub $0x18=ioinit 形）。
// zero 语义/布局改动；g_maxTotalDefenseRate 的定义随之前移为本 TU 首个强外部符号。
#include <iostream>

// [T2·P327] 定义前移为本 TU 首个「强符号·外部链接·本 TU 定义」实体 ⇒ GI 键名取裸名
// `g_maxTotalDefenseRate`（ORIG 0x08096a10）。float 常量初始化 = 静态初始化，不参与
// 动态构造序 ⇒ 移动零语义风险（P327 仅禁移「带动态初始化」的对象）。
// ORIG D 0x0936e4a8（.data 0a d7 23 3c = 0.01f）；原定义位于文末，逻辑不变。
float g_maxTotalDefenseRate = 0.01f;

// ---- 本 TU 外部符号（ORIG 已有/由其它批次提供；不得重复定义） ----
// [b28d-lane15 R3 跨 TU 迁定义，Main 批准 2026-09-08] _SDC::isObjectType
// T 0x080950e8 的**定义**自 W9_SDCCommon.cpp 迁入本 TU：ORIG 中它与 SDCLink 族
// （0x08095193..0x080957c3）地址相邻=同 TU；P210② 要求定义先于 caller，4.4 才
// 精确继承 outgoing 帧（sub $0xc，探针 /tmp/b28d_l15_probe4.cpp fF/fG）。
// 迁出侧 W9_SDCCommon.cpp 保留 DataManagerInitDepends.h:447 的声明（U 引用）。
namespace _SDC
{
#include "SDCLink/isObjectType.inc"
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

// [b28d-PoolE 符号轴 · W16 seg34 §4.2 INVENT 清理] ORIG 全 ELF 无 kDefRate* 符号：这三个
// 常量在 ORIG 是 .rodata 匿名项（0x08b12f54=0.01f / 0x08b12f60=400.0f / 0x08b12f64=100.0f，
// `nm -S --defined-only ORIG` 该区无符号）⇒ 使用点内联字面量，删我方发明的 file-scope static
// （内联后 4.4 仍走同一 .rodata 常量装载，表达式形不变）。

#include "SDCLink/getAbilityConvertRateSDC.inc"

// LinkFinal2：getDamageInflict 的三分派目标定义（ORIG T 0x08095220/0x08095508/0x08095275
// objdump 逐指令转写）及其依赖 getDamageRateCreature T 0x080955e1、
// getCreatureDamageRateFromModule T 0x08095430、getCreatureDamageRateFromIndex T 0x0809549c。
// 常量：0x8b12f54=0.01f（float）、0x8b12f58=100.0（double）——ORIG 为 .rodata 匿名项
// （无符号，W16 seg34 §4.2 INVENT）⇒ 使用点内联字面量，删我方发明 static。

#include "SDCLink/getDamageRateCreature.inc"


#include "SDCLink/getCreatureDamageRateFromModule.inc"


#include "SDCLink/getCreatureDamageRateFromIndex.inc"


#include "SDCLink/getDamageInflictCreature.inc"


#include "SDCLink/getDamageInflictCollisionObject.inc"


#include "SDCLink/getDamageInflictPassiveObject.inc"


#include "SDCLink/isValidAttackInfoElement.inc"


#include "SDCLink/getComputeDamageBasisLevelAttacker.inc"


#include "SDCLink/getDamageInflict.inc"


#include "SDCLink/getDefenseRateDamager.inc"


#include "SDCLink/checkIsCriticalSDC.inc"


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
// [C03CryptoInit68 复测·2026-09-10] 追加四形实测（探针 /tmp/rmw/p.cpp fA..fJ，4.4.7）：
//   `*cur=*cur^key`/`cur[0]^=key`/`key^*cur`/`cur[0]` 拆分读 均与 `^=` 同产（6 条）；
//   `register unsigned int* cur` ⇒ cur 免槽进 %ebx + `push %ebx` 破帧（反劣，已回退）；
//   `volatile unsigned int* cur` / `unsigned int v=*cur` 拆分 ⇒ 槽化更差（反劣）；
//   key 改 int 形 ⇒ `mov (%eax),%edx` 值落 edx（接近 ORIG）但多一条 key 装载（6 条）。
//   ⇒ 4.4 恒 CSE 指针地址（ORIG 4.1 双读），源形不可择：N18a/N76 族封顶。
// ============================================================================
struct stBVHackInfo_t
{
    double m_values[6];   // ORIG 出参偏移 +0x00..+0x28（fldl/fstpl 序列）
};
static_assert(sizeof(stBVHackInfo_t) == 0x30, "stBVHackInfo_t must be 0x30 bytes");

#include "SDCLink/decryptBVHackPacket.inc"


// ============================================================================
// 8) BLEEDING_DAMAGE_IN_ACTION_RATE  R 0x08af45e4（.rodata 00 00 c0 3f = 1.5f）
//    Dispatchers.cpp:233 以 extern "C" float 引用（符号名无 mangle）。
//    [Main·2026-09-11] 真数值 bug 修正：0.75f → **1.5f**。ORIG 字节 00 00 c0 3f
//    = IEEE754 1.5f（0.75f 应为 00 00 40 3f）——旧值与其注释均为误读，
//    影响 Dispatchers 侧 `adjDamage = (int)(RATE * clientDamage)` 的流血伤害缩放（差 2 倍）。
//    证据：PoolH 数据符号普查 /tmp/lane_PoolH/datasym_scan.md（ORIG R 4B @0x08af45e4 直读）。
// 8b) BLEEDING_DAMAGE_TERM  R 0x08af45e0（.rodata f4 01 00 00 = 500；ORIG 无引用死数据，
//     符号轴对齐补定义，消 ORIG-only）。
// ============================================================================
extern "C" const float BLEEDING_DAMAGE_IN_ACTION_RATE = 1.5f;
extern "C" const int BLEEDING_DAMAGE_TERM = 500;

// ============================================================================
// 9) g_maxTotalDefenseRate  D 0x0936e4a8（.data 0a d7 23 3c = 0.01f）
//    定义已前移至文件头（见 [T2·P327] 注释），符号与初值不变。
// ============================================================================
