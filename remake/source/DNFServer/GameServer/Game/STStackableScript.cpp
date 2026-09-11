// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STStackableScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// b28d-45 + sweep 判词汇总：
//   STStackableScript::Clear 0x0899087a AE（322=322）P238 float 字段形。
//   STStackableScript::operator= 0x089c418e N-ptrfold：621:627（p167 43=43
//     CLEAR；4.4 折 `&a->f != &b->f` 为 `a != b` + memcpy 实参重算）。
//   STStackableScript::C1ERKS_ 0x0898f218 NEAR：643=643（p167 仅 C2/D2 同址
//     别名伪差 P93；残差=块内迭代器槽序 N93/P212）。
//   ImportStackableScript 0x089aeaf4 N89/N82：10096:10155，§6.2 三件套判语义
//     完成（p167 1174=1174 CLEAR；丢弃写入 26/26 一一对应，槽整体平移 0x34；
//     jcc 差异全为块序镜像/折痕，无符号性反转 P196）。
//   [sweep] GetExpertJobTypeList 0x089bf6ca：原直线形有真语义 bug（首轮建表后
//     未命中应 return false，旧形却 typeSel=0 继续循环）+ 多 1 次 find/end/c_str
//     ⇒ 改写为 P333 状态机形（P177 三件套入循环体），327=327 归零。
//   [sweep] GetItemCategory 0x089bfdc5：改写为 P333 状态机形（gap +33 → −1）；
//     残差 = 2×4.1 nop 岛（P223/N112）+ state 槽比零折痕（P197④/N）+ 槽分区
//     （token/迭代器整体深 8、state 浅 0x58，N82附）⇒ N 封顶。
//   [sweep] GetExpertJobValueMaterialVariation 0x089bfc2f：27:26 N85（x87
//     fucompp/fnstsw/sete vs fucomip/jp 代际）。
//   [sweep] 虚调用核对：ORIG/我方各 2 处 `call *%edx`（new_allocator<CRabbitBox>
//     ::destroy / _Destroy<CRabbitBox>）位置与槽一致。
//   [sweep] TU 级符号：去掉 `#include "DNFLexWrapper.h"`（→ DNFLex.h →
//     <iostream>）后本 TU 不再发射 `_Z41__static_initialization_and_destruction_0ii`
//     与 `_GLOBAL__I_*`（P338/P327：ORIG 本 TU 无任何 GI —— nm 全表无
//     stStackable/STStatChange/STRecovery/CountinuousItemOption/
//     AvatarOptionChangeType/RecipeInfo/ImportStackable 键名，且
//     0x0898xxxx..0x089cxxxx 区间 GI 计数 = 0）。扫描器接口改局部声明
//      （签名与 DNFLexWrapper.h 逐字一致；nm -u 7 个符号与 ORIG T 全等）。
//     四数：diff 11→9、missing 470→468。
//   [sweep] 判据㉓（符号绑定 W vs T）：本 TU 原有 34 个符号我方 T / ORIG W
//     （RecipeInfo::additionalInfo、STStatChange/CVector、STStatusRecovery、
//     STBuff、STSummonMonster、STSummonApc、AvatarOptionChangeType、
//     Arad_STStackableScript、stStackableBooster_t、STStackableScript::operator=
//     的 C1/C2/D1/D2/clear/operator=）⇒ 全部在**定义处**加
//     `__attribute__((weak))`（.cpp 内单点，零头风险；全仓 grep 确认这些定义
//     仅本 TU 存在）。改后绑定比对 T→W 34→0，四数不变。
//     余 9 个 W→t（stStackableBoosterElement_t 的 C1/C2/C1ERKS_/C2ERKS_/D1/D2/
//     aSERKS_、stStackableBoosterItemInfo_t 的 D1/D2）：ORIG 为**每 TU 各一份
//     局部副本**（0x085138ee 与 0x089aee9c 两处同名 t），即原码头内/ .cpp 内部
//     定义；本重建因 `stStackableBooster_t m_booster280` 是共享头成员、类型必须
//     可见 ⇒ 定义位置不可迁移 ⇒ N（符号可见性差异，P154 结构性噪音）。
//   [Lane35 判据① 2026-09-10] jcc/jmp 目标下标逐位回扫（jcc_map 2492 函数）：
//     修复 GetItemCategory 0x089bfdc5 / GetExpertJobTypeList 0x089bf6ca 的
//     `retry:` 标签位置——ORIG 回跳目标 0x89bfde8 / 0x89bf72d 是三件套 static
//     guard 检查段首条，故标签须在 static 声明**之前**；置于声明之后时 4.4 的
//     回跳落到 __inited 检查处（我方 0x2e35），jcc 目标逐位偏离（判据①命中）。
//     修后全 TU 仅 ImportStackableScript 仍报 10 处，逐条核对为「4.1 头测
//     cond@+0x80 vs 4.4 尾测 cond@0xf055」的**块序映射伪差**（我方目标=我方
//     cond、ORIG 目标=ORIG cond，语义一致；第 7 条 je↔jne 为同块镜像极性）。
//     P164 `for(;;)+break` 头测改写实测**反劣**（insn 10239→10255、mismatch
//     6→10）⇒ 已回滚，记 N（4.4 循环旋转不可翻）。
//   [Lane35 视图 cast] DyeInfoView ×2 删除（DyeInfo 成员 private→public 由
//     Lane41 落于 STItemScript.h，零布局）：ORIG 0x089b9601
//     `mov %edx,0x164(%eax)` / 0x089b9629 `lea 0x168(%eax),%ecx` 为单条直访；
//     我方原为 `add $0x164,%eax` + 视图指针槽往返（每处 3 条）⇒ 改为
//     `script->m_dyeInfo.m_field0/.m_vec4` 直访后 ImportStackableScript
//     10239→10233（gap −42→−36），四数不回退。
//   [Lane35 判 N 清单] ImportStackableScript（N89 libstdc++ 全内联 + N82 槽复用；
//     p167 1174=1174 CLEAR）；STStackableScript::operator=（N-ptrfold：6 处
//     self 守卫被 4.4 折成 `this != &other` + memcpy 实参重算地址，621 vs 627）；
//     _Vector_base<stStackableBoosterElement_t>::~D1（N89，Lane41 根因 + 本 lane
//     复核 nm -S：ORIG 0x6d / 本 TU .o 0x49 / **链接产物 df_game_r 0x6d**
//     （由 CStackableItem.cpp.o 提供，同为 0x6d）⇒ 本 TU 版本非链接保留者，
//     比对噪声，无运行时影响）；__uninitialized_copy<false> ×4 =
//     **N89 跨版本 libstdc++ 模板实例（ORIG 0x50 无 EH landing vs 4.4.7 0xa9
//     含 landing）；探针实证 -fno-exceptions 可复现但整 TU 崩盘（784/1518/2/164）
//     ⇒ 判 N，三方域豁免**（Main 2026-09-10 裁决；附本 lane nm -S 复核：ORIG
//     0x50 / 全部 7 个发射 TU 与链接产物均 0xa9）；同族 _ZSt13__copy_move_a ×2
//     （NEAR）一并按此口径；GetExpertJobValueMaterialVariation
//     = **路由缺项（已补）**：Main 检查点 302 已把本 TU 从 446 共享分支拆出，
//     独走 `-march=i486 -mtune=generic`（dnf_game_cxx.sh:507）⇒ 该件 DIFF→AE、
//     TU 四数 **797/1683/4/8 → 797/1684/4/7**（orig 0x089bfc69 逐条命中
//     `fucompp;fnstsw %ax;test $0x5,%ah;sete;test;je`），其余件读数逐项不变；
//     `nm --defined-only` 名目 3475=3475、双向差集空（铁律 1）；风险面 = 仅 x87
//     比较 lowering（4.4 默认 `fucomip/fstp` → ORIG `fucompp/fnstsw/sahf`）；
//     仅带 `-march=i486`（缺 mtune）反崩 771/11/0/1710 ⇒ 两旗标必须同带。

#include "STStackableScript.h"
#include "CStackableItem.h"  // RecipeInfo 权威布局（0x4c，ORIG 0x8513c6a 族）
#include "GameScriptEnumHelpers.h"   // getActiveStatusToEnumValue（my status）
#include "EquipmentParameterInfo.h"  // importEquipmentParameterInfo / m_414（enchant）
#include "CCubeStatistic.h"            // getItemGroupNameTable（item group name）
#include "LogManager.h"                // ImportStackableScript 错误日志
#include "CEnvironment.h"              // SpinLock / less_TCHAR（ImportStackableScript 六张静态标签表）

#include <string.h>

#include "NpcScript.h"   // STScriptFileList（g_stStackableScriptFileList 双视图）
                         // + loadRDARScriptFile / getCharacterJob（权威声明）

#include "STStackableScript/ScanType.inc"
#include "STStackableScript/ScanInt.inc"
#include "STStackableScript/ScanInt_1.inc"
#include "STStackableScript/ScanFloat.inc"
#include "STStackableScript/ScanStr.inc"
#include "STStackableScript/ScanTypeOrInt.inc"


__attribute__((weak)) stStackableBooster_t& stStackableBooster_t::operator=(
    const stStackableBooster_t& other)
{
    m_elements = other.m_elements;
    m_map = other.m_map;
    return *this;
}

// ---- STStatChange（ORIG clear 0x089c329e：+0x0c→+0x00→+0x04→+0x08 字节清零）----
__attribute__((weak)) STStatChange::STStatChange()
{
    clear();
}
__attribute__((weak)) void STStatChange::clear()
{
    m_fieldC = 0;
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// ---- STStatChangeVector（ORIG C1 0x089c32c8 / clear 0x089c330a /
//      D1 0x089c3ace / operator= 0x089c40fe）----
__attribute__((weak)) STStatChangeVector::STStatChangeVector() : m_vec0()
{
    clear();
}
__attribute__((weak)) STStatChangeVector::~STStatChangeVector() {}
__attribute__((weak)) STStatChangeVector& STStatChangeVector::operator=(const STStatChangeVector& other)
{
    // ORIG：vector operator= 后逐个拷贝 +0x0c/+0x10 int 与 +0x14 字节。
    m_vec0 = other.m_vec0;
    m_fieldC = other.m_fieldC;
    m_field10 = other.m_field10;
    m_field14 = other.m_field14;
    return *this;
}

// ---- ORIG clear 0x089c330a：容器清空 + 三标量清零。保持显式定义供引用。----
__attribute__((weak)) void STStatChangeVector::clear()
{
    m_vec0.clear();
    m_fieldC = 0;
    m_field10 = 0;
    m_field14 = 0;
}

// ---- STStatusRecovery（ORIG C1 0x089c30dc 调 clear；clear 0x089c30f0）----
__attribute__((weak)) STStatusRecovery::STStatusRecovery()
{
    clear();
}
__attribute__((weak)) void STStatusRecovery::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
}

// ---- STBuff（ORIG C1 0x089c3194 调 clear；clear 0x089c31a8）----
__attribute__((weak)) STBuff::STBuff()
{
    clear();
}
__attribute__((weak)) void STBuff::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
}

// ---- STSummonMonster（ORIG C1 0x089c31d2 调 clear；clear 0x089c31e6）----
__attribute__((weak)) STSummonMonster::STSummonMonster()
{
    clear();
}
__attribute__((weak)) void STSummonMonster::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field14 = 0;
    m_field18 = 0;
}

// ---- STSummonApc（ORIG C1 0x089c3238 调 clear；clear 0x089c324c）----
__attribute__((weak)) STSummonApc::STSummonApc()
{
    clear();
}
__attribute__((weak)) void STSummonApc::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
    m_field14 = 0;
}

#include "STStackableScript/CountinuousItemOption.inc"

#include "STStackableScript/clear.inc"


// ---- AvatarOptionChangeType（ORIG C1 0x08513b6e 调 clear；clear 0x08513b48）----
// clear 链：m_b2 = 0xff → m_b1 = m_b2 → m_b0 = m_b1。
__attribute__((weak)) AvatarOptionChangeType::AvatarOptionChangeType()
{
    clear();
}
__attribute__((weak)) void AvatarOptionChangeType::clear()
{
    m_field2 = 0xff;
    m_field1 = m_field2;
    m_field0 = m_field1;
}

__attribute__((weak)) Arad_STStackableScript::Arad_STStackableScript()
{
    AradStClear();
}

__attribute__((weak)) Arad_STStackableScript::~Arad_STStackableScript()
{
}

__attribute__((weak)) Arad_STStackableScript& Arad_STStackableScript::operator=(
    const Arad_STStackableScript& other)
{
    m_0 = other.m_0;
    m_4 = other.m_4;
    m_str8 = other.m_str8;
    m_strc = other.m_strc;
    m_str10 = other.m_str10;
    return *this;
}

// ORIG 0x089c348e：清 +0x00/+0x04/-0x08..+0x10 三个 string；+0x14 由
// STStackableScript::Clear 单独清（见 Clear 尾部 movl $0,0x18c）。
__attribute__((weak)) void Arad_STStackableScript::AradStClear()
{
    m_0 = 0;
    m_4 = -1;
    m_str8 = "";
    m_strc = "";
    m_str10 = "";
}

#include "STStackableScript/STStackableScript.inc"


#include "STStackableScript/STStackableScript_1.inc"


#include "STStackableScript/dtor_STStackableScript.inc"


// ORIG 0x089c418e：无 self 检查；基类 op= → arad op= → +0x18c →
// 依序拷贝各容器/string/标量；STRecovery×2/STStatusRecovery 与
// STBuff/STSummonMonster/STSummonApc 三个平凡区用 memcpy（ORIG call memcpy@plt）。
// [b28d-solo-STStackableScript] DIFF→N-ptrfold 封顶（621 vs 627：6 处
// self 守卫 4.4 折成 `this != &other` 且 memcpy 实参重算地址；p167 CLEAR 43=43）。
// [P2S3STSkillStack 2026-09-11] 复核「ORIG 是否真无 self 守卫」：**ORIG 确有守卫**，
// 形式为成员级地址比较 `mov 0x8(%ebp),%eax; add $0x2b0,%eax; mov 0xc(%ebp),%edx;
// add $0x2b0,%edx; cmp %edx,%eax; je <T>`（即源码 `if (&m_x != &other.m_x)`），
// 不是整对象 `this != &other` 检查。⇒ 守卫必须保留：删掉源码 if 会连 6 处 `je`
// 一起丢（变成我方缺 6 条 + ORIG 有），只会更劣；我方现有 `cmp 0xc(%ebp),%eax; je`
// 系 4.4 把「同偏移取址比较」折成整对象比较（每处 +1 条 = gap −6），属 N-ptrfold。
__attribute__((weak)) STStackableScript& STStackableScript::operator=(const STStackableScript& other)
{
    STItemScript::operator=(other);
    m_arad178 = other.m_arad178;
    m_arad178.m_field14 = other.m_arad178.m_field14;
    m_vec190 = other.m_vec190;
    m_rabbit19c = other.m_rabbit19c;
    m_1a8 = other.m_1a8;
    m_1ac = other.m_1ac;
    m_1b0 = other.m_1b0;
    m_1b4 = other.m_1b4;
    m_1b8 = other.m_1b8;
    m_option1b9 = other.m_option1b9;
    m_option1bc = other.m_option1bc;
    m_1bf = other.m_1bf;
    m_1c0 = other.m_1c0;
    m_1c4 = other.m_1c4;
    m_1c8 = other.m_1c8;
    m_1cc = other.m_1cc;
    m_1d0 = other.m_1d0;
    m_1d4 = other.m_1d4;
    m_str1d8 = other.m_str1d8;
    m_1dc = other.m_1dc;
    m_1e0 = other.m_1e0;
    m_1e4 = other.m_1e4;
    m_1e8 = other.m_1e8;
    m_vec1ec = other.m_vec1ec;
    m_place1f8 = other.m_place1f8;
    m_204 = other.m_204;
    m_205 = other.m_205;
    m_passive208 = other.m_passive208;
    m_server214 = other.m_server214;
    m_220 = other.m_220;
    m_vec224 = other.m_vec224;
    m_str230 = other.m_str230;
    m_str234 = other.m_str234;
    m_str238 = other.m_str238;
    m_23c = other.m_23c;
    m_240 = other.m_240;
    m_vec244 = other.m_vec244;
    m_vec250 = other.m_vec250;
    m_vec25c = other.m_vec25c;
    m_package268 = other.m_package268;
    m_package274 = other.m_package274;
    m_booster280 = other.m_booster280;
    m_package2a4 = other.m_package2a4;
    // ORIG 对 6 个平凡区 memcpy 均带 self 守卫（cmp this+off vs other+off; je 跳过）。
    // [b28d-solo-STStackableScript] N-ptrfold：4.4 把 `&a->f != &b->f`（同偏移）
    // 折成 `a != b`，且 memcpy 实参重算地址；ORIG 4.1 保留两次 add+cmp 并复用
    // 地址寄存器跨 call。整数化守卫（(int)&f）可阻 fold 但 memcpy 段仍重算
    // （探针 /tmp/probe_guard.cpp 四形实证：原形 +6、整数化 +30）⇒ 封顶。
    if (&m_recovery2b0 != &other.m_recovery2b0)
        memcpy(&m_recovery2b0, &other.m_recovery2b0, 0x11);
    if (&m_recovery2c4 != &other.m_recovery2c4)
        memcpy(&m_recovery2c4, &other.m_recovery2c4, 0x11);
    if (&m_statusRecovery2d8 != &other.m_statusRecovery2d8)
        memcpy(&m_statusRecovery2d8, &other.m_statusRecovery2d8, 0x11);
    m_status2ec = other.m_status2ec;
    if (&m_buff304 != &other.m_buff304)
        memcpy(&m_buff304, &other.m_buff304, 0xd);
    if (&m_summonMonster314 != &other.m_summonMonster314)
        memcpy(&m_summonMonster314, &other.m_summonMonster314, 0x19);
    if (&m_summonApc330 != &other.m_summonApc330)
        memcpy(&m_summonApc330, &other.m_summonApc330, 0x15);
    m_348 = other.m_348;
    m_statChange34c = other.m_statChange34c;
    m_364 = other.m_364;
    m_36c = other.m_36c;
    m_370 = other.m_370;
    m_str368 = other.m_str368;
    m_380 = other.m_380;
    m_384 = other.m_384;
    m_388 = other.m_388;
    m_38c = other.m_38c;
    m_390 = other.m_390;
    m_394 = other.m_394;
    m_396 = other.m_396;
    m_398 = other.m_398;
    m_39a = other.m_39a;
    m_39b = other.m_39b;
    m_39c = other.m_39c;
    m_3a0 = other.m_3a0;
    m_3a1 = other.m_3a1;
    m_vec374 = other.m_vec374;
    m_map3a4 = other.m_map3a4;
    m_vec3c0 = other.m_vec3c0;
    m_3cc = other.m_3cc;
    m_3d0 = other.m_3d0;
    m_3d1 = other.m_3d1;
    m_cube3d4 = other.m_cube3d4;
    m_enchant3f4 = other.m_enchant3f4;
    m_40c = other.m_40c;
    *(unsigned short*)&m_40d[0] = *(const unsigned short*)&other.m_40d[0];
    m_str410 = other.m_str410;
    m_414 = other.m_414;
    m_418 = other.m_418;
    m_boosterSelect41c = other.m_boosterSelect41c;
    m_438 = other.m_438;
    m_pair43c = other.m_pair43c;
    m_444 = other.m_444;
    m_448 = other.m_448;
    m_44c = other.m_44c;
    m_450 = other.m_450;
    m_454 = other.m_454;
    m_458 = other.m_458;
    m_45c = other.m_45c;
    m_460 = other.m_460;
    m_464 = other.m_464;
    m_468 = other.m_468;
    m_46c = other.m_46c;
    m_470 = other.m_470;
    m_474 = other.m_474;
    m_478 = other.m_478;
    m_avatar47c = other.m_avatar47c;
    m_set480 = other.m_set480;
    m_set498 = other.m_set498;
    m_4b0 = other.m_4b0;
    m_str4b4 = other.m_str4b4;
    m_4b8 = other.m_4b8;
    m_module4bc = other.m_module4bc;
    m_set4d4 = other.m_set4d4;
    m_4ec = other.m_4ec;
    m_4f0 = other.m_4f0;
    m_4f4 = other.m_4f4;
    m_4f8 = other.m_4f8;
    m_vec4fc = other.m_vec4fc;
    m_508 = other.m_508;
    m_set50c = other.m_set50c;
    return *this;
}

#include "STStackableScript/Clear.inc"


__attribute__((weak)) void RecipeInfo::additionalInfo(STStackableScript& script)
{
    // ORIG 0x8513d44（_ZN10RecipeInfo14additionalInfoER17STStackableScript）：
    // m_field34 = script.m_4f0; m_field38 = script.m_4f4; m_field3c = script.m_4f8;
    // 随后 while (size() > (unsigned)i) 循环：每轮读 v[i] 与 v[i+1] 组成 pair
    // push_back 到 +0x40 的 vector<pair<int,int>>，i 步进 2（ORIG 循环体两次
    // `addl $1,-0xc`，即相邻不重叠配对）；越界保护 size()<=i 时退出；
    // 比较为无符号（setbe/seta）。
    m_field34 = script.m_4f0;
    m_field38 = script.m_4f4;
    m_field3c = script.m_4f8;
    // i 声明 unsigned（P209：4.4 对 unsigned 局部比较恒 mem 直比 `cmp -0xc,%eax`；
    // int+cast 恒先载 edx，+2 条）。
    unsigned int i = 0;
    // ORIG 槽序：i(-0xc) 最浅、pair 临时(-0x14)、first(-0x18)、second(-0x1c)
    // ⇒ first/second 提到函数头声明（P212 全头部声明；块内仅赋值）。
    int first;
    int second;
    while (script.m_vec4fc.size() > i)
    {
        first = script.m_vec4fc[i++];
        if (script.m_vec4fc.size() <= i)
        {
            break;
        }
        second = script.m_vec4fc[i++];
        // ORIG 调 push_back(OS1_)（右值重载）⇒ 源形为临时 pair 直传。
        m_vec40.push_back(std::pair<int, int>(first, second));
    }
}

// ---- ORIG 全局（定义 InitScriptProducers_4.cpp）：脚本目录与文件列表 ----
extern char g_szStackableScriptBaseDirectory[0x100];
// [sweep] P237：改为权威型（定义见 InitScriptProducers_4.cpp:274
// `STScriptFileList g_stStackableScriptFileList;`），消除「局部 extern 声明为
// std::map 视图 + reinterpret_cast」的 ODR 违规与多余 cast。
extern STScriptFileList g_stStackableScriptFileList;

// ---- ORIG TU-local 静态 helper（ORIG 与 ImportStackableScript 同 TU：
//      RDARScriptItem.cpp，ORIG t 0x089bf6ca/0x089bfc2f/0x089bfc84/0x089bfdc5）。
//      重建按 TU 拆分，本 TU 需自有副本（mangled _ZL… 内部链接）。----
static bool GetItemCategory(STItemScript* item);
static bool GetExpertJobTypeList(STItemScript* item);
static bool GetExpertJobValueMaterialVariation(STItemScript* item);
static bool GetExpertJobValueCompoundRate(STItemScript* item);

static bool GetItemCategory(STItemScript* item)
{
    // [Lane41] P212：块内局部提至函数头、块内仅赋值。state 用枚举型
    // （4.4 对 int 标量分浅区、枚举走对象区）⇒ 槽与 ORIG 同区；声明序按
    // ORIG 槽深序（token -0x74 浅于 state -0x78）⇒ token 先声明。
    std::string token;
    STItemScript::ENUM_ITEM_CATEGORY state;
    while (ScanStr(&token))
    {
        state = STItemScript::ITEM_CATEGORY_0;
        // [sweep-STStackableScript] P333/P177（ORIG 0x089bfdc5 逐条实证）：
        // 三件套 + retry 标签同落循环体：ORIG retry 回跳目标 0x89bfde8 = 三件套
        // guard 检查段首条（`mov $&__lookup,%eax`）⇒ 标签必须在 static 声明
        // **之前**（guard 每轮执行）；放在声明之后会让 4.4 的回跳落到 __inited
        // 检查处（0x2e35），jcc 目标偏离 ORIG（判据①）。
        // 单次 find（仅 __inited 分支）+ case 0/0x30ae 共享块 + case 尾 fallthrough
        // 插入 + default `if(__inited) return false` + 建表后 `goto retry` 重查。
    retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            v = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            v = 0;
        }
        switch (v)
        {
        case 0:
        case 0x30ae:                                   // "monster card"
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("monster card", 0x30ae)); }
            else { state = STItemScript::ITEM_CATEGORY_1; break; }
        case 0x30b1:                                   // "cannot bind"
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("cannot bind", 0x30b1)); }
            else { state = STItemScript::ITEM_CATEGORY_2; break; }
        case 0x30b4:                                   // "boss drop"
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("boss drop", 0x30b4)); }
            else { state = STItemScript::ITEM_CATEGORY_3; break; }
        default:
            if (__inited)
                return false;                          // 未命中 → 导入失败
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;
        }
        if (state != STItemScript::ITEM_CATEGORY_0)
        {
            item->m_set11c.insert(state);
        }
    }
    return true;
}

static bool GetExpertJobTypeList(STItemScript* item)
{
    if (item == 0)
        return false;
    item->m_expertJob.m_usable.clear();

    // [sweep-STStackableScript] P333「首建表、之后查表」状态机形（ORIG
    // 0x089bf6ca 逐条实证）：单次 find（仅 __inited 分支发射）+ case 0/0x303a
    // 共享块 + 各 case 尾 fallthrough 到下一 case + default `if(__inited)
    // return false` + 建表后 `goto retry` 重查。旧直线形（if/else 各一次 find）
    // 有真语义 bug：首轮建表后未命中的 token 应 **return false**（ORIG
    // 0x89bfb23 `movzbl __inited; test; je` → `mov $0,%ebx; jmp 0x89bfc14`），
    // 旧形却 typeSel=0 继续循环 ⇒ 既少指令（gap +9）又多 1 次 find/end/c_str。
    std::string token;
    bool scanning = true;
    while (scanning)
    {
        if (!ScanStr(&token))
            break;

        int typeSel = 0;
        // P177：三件套 + retry 标签同落循环体——ORIG retry 回跳目标 0x89bf72d
        // 就是 guard 检查段首条，故标签须置于 static 声明**之前**（guard 每轮
        // 执行）；放声明之后会让 4.4 回跳落到 __inited 检查处（jcc 目标偏离，
        // 判据①）。函数级声明则把 guard 提到函数入口 = 我方更旧形。
    retry:
        static std::map<const char*, int, less_TCHAR> __lookup;   // 0x94fde48
        static SpinLock __init_lock;                              // 0x94fde60
        static bool __inited = false;                             // 0x94fde64
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            v = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            v = 0;
        }
        switch (v)
        {
        // 极性照 ORIG：`movzbl __inited; xor $1; test; je <typeSel 段>` ⇒ 源写
        // `if (!__inited) { insert } else { typeSel = N; break; }`（P59/P248 档②）。
        case 0:
        case 0x303a:                                   // enchanter
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("enchanter", 0x303a)); }
            else { typeSel = 1; break; }
        case 0x303d:                                   // alchemist
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("alchemist", 0x303d)); }
            else { typeSel = 2; break; }
        case 0x3040:                                   // disjointer
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("disjointer", 0x3040)); }
            else { typeSel = 3; break; }
        case 0x3043:                                   // doll_controller
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("doll_controller", 0x3043)); }
            else { typeSel = 4; break; }
        case 0x3046:                                   // all
            if (!__inited) { __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("all", 0x3046)); }
            else { typeSel = 5; break; }
        default:
            if (__inited)
                return false;
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;
        }

        int level = 0;
        if (!ScanInt(&level))
            return false;
        if (typeSel != 0)
        {
            // ORIG 0x89bfb8e/0x89bfb98 两条 `movl $0x0,-0xac/-0xa8` = entry 预清零。
            STItemScript::SEXPERTJOB::SUSABLE entry;
            entry.m_field0 = static_cast<ENUM_EXPERT_JOB_TYPE>(0);
            entry.m_field4 = 0;
            entry.m_field0 = static_cast<ENUM_EXPERT_JOB_TYPE>(typeSel);
            entry.m_field4 = level;
            item->m_expertJob.m_usable.push_back(entry);
        }
    }
    return !item->m_expertJob.m_usable.empty();
}

// ORIG t 0x089bfc2f：-100.0 哨兵（0x8e1a574）视为未设置。
// [Lane41] 判据⑥：ORIG 0x89bfc69 `fucompp; fnstsw %ax; test $0x5,%ah; sete %al;
// test %al,%al; je` = 「有序且 ST0(-100.0) >= ST1(v)」⇒ 源码为 `v <= -100.0f`，
// 不是 `==`（v < -100.0 时 ORIG 也 return false，真语义差异）。
// 该 lowering 需本 TU 路由补 `-march=i486 -mtune=generic`（探针 11 形态实证：
// 仅 i486 组产生 ORIG 逐条形；默认旗标产 fucomip/setae）——需求已报 Main。
static bool GetExpertJobValueMaterialVariation(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field18 = ScanFloat(&ok);
    if (item->m_expertJob.m_field18 <= -100.0f)
        return false;
    return ok;
}

// ORIG t 0x089bfc84：仅透传 ScanFloat 成败。
static bool GetExpertJobValueCompoundRate(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field1c = ScanFloat(&ok);
    return ok;
}


#include "STStackableScript/ImportStackableScript.inc"



#include "STStackableScript/StackableScriptSpecialMonsterDrop.inc"

