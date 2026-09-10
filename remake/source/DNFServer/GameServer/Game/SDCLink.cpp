// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 SDCLink/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "SDCLink/getAbilityConvertRateSDC.inc"
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
// [b28d-lane15 判词·N（N85 x87 整数↔浮点链）] → [C03CryptoInit68 修·2026-09-10]
// 按 P167 调用点差集修两处真差（本件原 `8×operator[]` vs ORIG `8×at()`，
// 及 int 乘积折 fildl）：
//  ① 全部改 `g_creature_damage_vector_.at(i)`（P175；ORIG 8 处 R_386_PC32 全为
//     `_ZNSt6vectorIiSaIiEE2atEj`）；
//  ② `(double)at(i) * (double)sum`（勿写 `(double)(at(i)*sum)`）⇒ 复现 ORIG
//     `fildl at; fildl sum; fmulp` 双 fildl；
//  ③ 加法的**先求值项**写在左侧（ORIG 恒先算 at(0)+c / at(2) / at(5) 并落槽），
//     即 `(at(0)+c) + at(1)*sum` 形（浮点加法可交换，P243 族）。
// 残余（跨版本族）：4.4 对内存操作数折叠 `faddl/fmull/fdivl mem`（ORIG 分步
// fldl+faddp，N85③）；double 临时槽 4.4 复用而 4.1 每步新槽（帧 0x68 vs 0x78，N82）。
// 语义等价，按 §6 记 N 封顶。
#include "SDCLink/getDamageRateCreature.inc"
}

// _Z31getCreatureDamageRateFromModule16ENUM_MODULE_TYPE @0x08095430 (0x6c)：
// map find 命中返 value，未命中 1.0f。
#include "SDCLink/getCreatureDamageRateFromModule.inc"
}

// _Z30getCreatureDamageRateFromIndexi @0x0809549c (0x6c)：同型 map<int,float>。
#include "SDCLink/getCreatureDamageRateFromIndex.inc"
}

// _Z24getDamageInflictCreatureP23STComputeDamageVariable @0x08095508 (0xd9)：
// m_char1B5 门控；dmg=m_field000*(rate(m_char1B7,m_char1B8,(int)m_d1AA))*m_f02B
// *FromModule(m_objTypeByte)；objTypeByte==9 再 *FromIndex(m_char1B9)。
// [b28d-lane15 判词·N（N85）] → [C03CryptoInit68 修·2026-09-10] 两处已翻正：
//  ① 乘法书写序（P243 变体）：写 `rate(...) * base` ⇒ 4.4 发
//     `call; fmuls -0x14(%ebp); fstps -0xc(%ebp)`，**累加器槽 -0xc 与 ORIG 一致**
//     （旧 `base * rate()` 产 flds+fmulp 三条且累加器落 -0x10）。
//  ② 具名 float 副本（P252/P0）：ORIG `flds (%eax); fstps -0x24` 是具名局部
//     ⇒ 补 `float base = v->m_field000;` 并把 dmg 拆为独立变量后，gap 65→**64=ORIG**。
// 残余（跨版本族，语义等价）：ORIG x87 拷贝 `flds/fstps` vs 4.4 `mov/mov`（N85①）；
// base/lv 槽位比 ORIG 浅 0x10（N126 版本首槽基址常数 + N82 槽复用，实测非声明序可翻）；
// `mov -0x20(%ebp),%ecx` 装载时刻（P75）。按 §6 记 N 封顶。
#include "SDCLink/getDamageInflictCreature.inc"
}

// _Z31getDamageInflictCollisionObjectP23STComputeDamageVariablePv @0x08095275 (0x1bb)：
// 逐指令转写。0x8b12f54=0.01f；0x93f5d10/0x93f5d28=Damage 常量；
// getAbilityConvertRateSDC 表类型 3/5/0。
// [b28d-lane15 判词·N（N85②）] 残差仅 2 处除法：ORIG `flds 全局; flds -0x8;
// fdivp %st,%st(1)` 三步 vs 4.4 `flds 全局; fdivs -0x8` 两步（除操作数在内存时
// 4.4 恒折 fdivs）。其余 diff 行全为重定位地址伪差（0x8b12f54 vs .o 未解析 0x28）。
// 语义等价，封顶。
#include "SDCLink/getDamageInflictCollisionObject.inc"
}

// _Z29getDamageInflictPassiveObjectP23STComputeDamageVariablePv @0x08095220 (0x55)：
// m_char1B5 && m_char1B6 → Creature(v)；否则 CollisionObject(v, passiveObject)。
// [b28c-lane24] 四形（双 return/if-else/if-else+init/P210 前置定义）帧恒 0x38 vs
// ORIG 0x28、尾物化槽 -0x1c vs -0xc ⇒ 曾判 N（跨版本帧策略 + 尾物化槽）。
// [b28d-lane15 翻案] ORIG 实为**三元**：两臂 fstps -0xc → 汇合 +0x4d
// `mov %eax,-0xc; flds -0xc`（float 位模式过 eax），帧 0x28、无具名 float 局部。
// 改三元后 fresh .o 判 identical/ae（SDCLink near 2→1）。P243 同族。
#include "SDCLink/getDamageInflictPassiveObject.inc"
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
#include "SDCLink/isValidAttackInfoElement.inc"
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
#include "SDCLink/getComputeDamageBasisLevelAttacker.inc"
}

// ============================================================================
// 4) _Z16getDamageInflictP23STComputeDamageVariablePv @0x08095193 (0x8d)
//    按 v+0x1BA（m_field1BA）对象类型三分派：
//      isObjectType(0x21) → PassiveObject(v,obj)；isObjectType(0x81) → Creature(v)；
//      否则 CollisionObject(v,obj)。
// ============================================================================
#include "SDCLink/getDamageInflict.inc"
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
// [C03CryptoInit68 复测·2026-09-10] 已试把 ORIG 的两个具名常量局部补回
// （`float zero=0.0f; dr=std::max(zero,dr);` 与 `float one=1.0f;`，含 max 实参序
// `max(denom,one)`）：dr 槽由 -0x20 变 **-0x18（更远）**，整帧上移 4 ⇒ 反劣，已回退。
// ⇒ 本件槽差不可由具名局部翻正，维持 N85①+N82 封顶。（另：`_ZSt3maxIfERKT_S2_S2_`
// 的 ORIG 形 `fucompp/fnstsw/test $0x45/sete` 需 TU 级 `-march=i486 -mtune=generic`，
// 已按五要素上报 Main；实测该旗标下本 TU ae 20→21、diff 7→6。）
// ============================================================================
#include "SDCLink/getDefenseRateDamager.inc"
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
#include "SDCLink/checkIsCriticalSDC.inc"
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
}

// ============================================================================
// 8) BLEEDING_DAMAGE_IN_ACTION_RATE  R 0x08af45e4（.rodata 00 00 c0 3f = 0.75f）
//    Dispatchers.cpp:233 以 extern "C" float 引用（符号名无 mangle）。
// ============================================================================
extern "C" const float BLEEDING_DAMAGE_IN_ACTION_RATE = 0.75f;

// ============================================================================
// 9) g_maxTotalDefenseRate  D 0x0936e4a8（.data 0a d7 23 3c = 0.01f）
//    定义已前移至文件头（见 [T2·P327] 注释），符号与初值不变。
// ============================================================================
