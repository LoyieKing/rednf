// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CraneMinigameManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CraneMinigameManager 实现。
//
// 本 TU 非 identical 函数残余归因（batch-10，指南 §6.2/§6.3 + 范式库负例）：
//  · _Z41__static_initialization_and_destruction_0ii —— §6.3 同名多定义假阳性：
//    ORIG `nm -S` 实查该 mangled 共 **493 个 local(t) 定义**（每 TU 一个），其中
//    442 个体尺寸 0x40 = 纯 `__ioinit` 注册（ios_base::Init C1 + __cxa_atexit），
//    与本 TU 我方 16 insns 体逐条同形；报告按名配到的是 0x0807ee76（size 0xba，
//    PacketTypes TU：ios_base::Init + `static std::string base64_chars` 动态构造，
//    AniC9 独立取证一致）⇒ 非语义缺口，勿再按该 oracle 追。
//  · _ZNSt20__uninitialized_copyILb0EE…move_iterator…pairIii… —— 纯
//    libstdc++ 头部实现差（ORIG 体内联 `operator new(8,void*)`+null 检+
//    `pair(pair&&)`，4.4.7 头改为调 `_ZSt10_Construct…`）；源码侧不可控，
//    范式库 N 类「libstdc++ __uninitialized_copy 内部结构差」。
//  · importCraneMinigameScript / updateCraneItemNeedMaterial —— 见各自函数头。
#include "CraneMinigameManager.h"

#include <string>

#include "NeedMaterialDataManager.h"
#include "PacketGuard.h"
#include "CUser.h"             // Send(PacketGuard&)
#include "GameTypes.h"          // cMyTrace
#include "DNFLexWrapper.h"      // loadRDARScriptFile/ScanType/ScanInt/ScanFloat

#include <map>
#include "CEnvironment.h"      // class SpinLock / struct less_TCHAR（本函数惰性注册表）

// ============================================================================
// importCraneMinigameScript —— ORIG T 0x088e3924（报告
// docs/class_func_reports/_lt_global_gt_/importCraneMinigameScript.md，
// 汇编 435 insns + Ghidra C 双段逐块转写）。
// 双相派发族（P163/P164/P165/P177/P178）：函数级静态三件套
//   __lookup = map<char const*,int,less_TCHAR>（guard + __cxa_atexit）
//   __init_lock = SpinLock（guard + __cxa_atexit）  __inited = 零初始化 bool
// 位于 read_token 标签之后 ⇒ guard 每轮穿（+0x70/+0xea）；注册轮持锁
// tagId=0（+0x255）→ case 0 与 case 0x36 共 leaf（+0x274）→ 逐 case
// `if(!__inited){insert}` 贯穿 9 条（+0x2cf/+0x345/+0x3f3/+0x46b/+0x4d3/
// +0x53b/+0x5a6/+0x620）→ 公共尾 +0x6e0 leave+置位+jmp +0x70 重派发同 token。
// 真派发轮（__inited）按 +0x173 find(c_str())→!=end ? it->second : -1（+0x222）
// 取值，分派 `cmpl $0x61; ja +0x6e0` 跳表；未命中/越界静默（ORIG 无 default 体）。
// 字段写入逐条按 ORIG disp 尺寸（+0x00 word / +0x10 dword / +0x14 word /
// item +0x00 word +0x04 dword +0x08 word +0x0c float +0x10 dword）；
// ORIG 除 [need material] 首 ScanInt 外全部传 NULL ok 指针（+0x2d1 等
// `movl $0x0,(%esp)`），不做判空/不复位 currentItem（+0x53d push_back 后无置 0）。
//
// 【batch-10 收敛】213 gap → 1（ORIG 435 / ours 434，P167 调用点差集 0：
// 区间 60 个 call 与 .o R_386_PC32 60 个 relocation 逐符号 Counter 全等）。
// 硬证据（nm -S 双侧实查）：函数体尺寸 _Z25importCraneMinigameScript… = **0x74d
// 与 ORIG 完全一致**；5 个函数级静态对象符号名+尺寸逐条对齐
// （_ZZ…E8__lookup 0x18 / _ZZ…E11__init_lock 0x4 / _ZZ…E8__inited 0x1 /
//  _ZGVZ…E8__lookup 0x8 / _ZGVZ…E11__init_lock 0x8），opcode 多重集差 =
// 仅切片边界 ret 计数口径（按 AE 口径为 0 差）。
// 残余 DIFF 归因（范式库 §8 负例，勿再耗轮次）：
//  ①「空 else nop;jmp 岛 vs jump-threading」（跨版本组）：公共尾 +0x706 的
//    `nop; jmp +0x4f` 两指令岛被 4.4 线化直跳循环头（唯一 1 insns 计数差）；
//    双检 +0x24f leave 后的单个 nop 属 §6.1 对齐填充豁免。
//  ② N82/P112 帧槽号：4.1 给 token/currentItem/ok/key 分配 -0xe4/-0xe8/-0xe9/
//    -0xdc，4.4 为 -0xe0/-0xe4/-0xe5/-0xec（整帧 0xfc 相同、槽位集相同）。
//  ③ .o 未 relink 的 R_386_PC32/R_386_32 显示差（call 目标与 &__lookup/
//    &__init_lock/&"" 数据地址），链接后按 AE 口径核销；本 TU 因此额外带
//    `_GLOBAL__I__Z25importCraneMinigameScript…`（键名归属，N 类）与
//    `_ZSt10_ConstructISt4pairIiiES1_EvPT_OT0_` 两个 ORIG 无同名切片的弱符号
//    （ORIG 只有 C++98 拼写 `…EvPT_RKT0_` @0x085c3936；`…EvPT_OT0_` 是 4.4.7
//    + `-std=gnu++0x` 头文件在 vector<pair> 扩容路径上的内部结构差，N 类）。
// [b28d-lane15 复核] 维持 N：TSV ndiff=394（ORIG 431 / 我方 431，尺寸 0x74d 双侧
// 全等）；残差即上述 ①循环旋转岛 1 insn、②N82/P112 帧槽号、③.o 重定位显示差。
// ============================================================================

// ORIG W 0x088e3708：逐字段拷 m_id/m_itemIdx/m_cnt（0xc 字节），不拷
// m_viewRatio/m_pickRatio（ORIG 反汇编止于 +8）。
CraneItemInfo& CraneItemInfo::operator=(const CraneItemInfo& other)
{
    m_id = other.m_id;
    m_itemIdx = other.m_itemIdx;
    m_cnt = other.m_cnt;
    return *this;
}

#include "CraneMinigameManager/importCraneMinigameScript.inc"
}

// ============================================================================
// CraneMinigameScript C1/D1 —— ORIG 0x088e3736 / 0x088e37a8（外联定义，
// 产独立符号；头内不再内联）。
// C1：空体，仅按声明序默认构造 m_items(+0x04) → m_sendMail(+0x18) →
// m_needMaterials(+0x5c)；m_viewCnt/m_material/m_field14 故意不初始化（ORIG 无 store）。
// D1：begin/end 迭代器（声明序 it 先、end 后 = ORIG 栈槽 -0xc/-0x10），
// 循环内 O0 三次独立 operator*（判空 / delete / 置 0，P48 无 CSE），
// clear 后编译器按声明逆序析构 m_needMaterials → m_sendMail → m_items（ORIG 同序）。
// ============================================================================
#include "CraneMinigameManager/CraneMinigameScript.inc"
}

#include "CraneMinigameManager/dtor_CraneMinigameScript.inc"
}

#include "CraneMinigameManager/CraneMinigameManager.inc"
}

#include "CraneMinigameManager/dtor_CraneMinigameManager.inc"
#include "CraneMinigameManager/init.inc"
}

// ORIG 0x080ebebc：m_selectIdx = count % m_needMaterials.size()；
// 取对应 pair 调 NeedMaterialDataManager::setNeedMaterialInfo(m_material, pair)。
// 逐条对齐：帧 0x20 + `push %esi; push %ebx` 与 ORIG 全等（+0x49 at() 结果留
// %ebx、+0x50 m_material 留 %esi，二者跨 +0x56 getInstance() 保活 ⇒ 用 P179
// register 形复现 4.1 双 callee-saved 形；普通引用形被 4.4 落栈槽，37→40）。
// 残余 DIFF 归因（N 类，40/40 等长，仅 6 行搬移链次序）：4.1 把 count 提前
// 载入 %ebx 再 `mov %ebx,%eax` 喂 divl、余数 `mov %edx,%ecx`；4.4 直取
// 0xc(%ebp) 且 div 结果先归 %eax 再 `mov %eax,%ecx`（N7/N82 RA 族）。
// [b28d-lane15 复核] 维持 N：N96 清绑后残差为 N7/N82 RA 寄存器角色差（ORIG
// +0x26 余数三段搬运 / +0x49 双 callee-saved 保活），语义等价。
#include "CraneMinigameManager/updateCraneItemNeedMaterial.inc"
}

// ORIG 0x080ebf2e（报告 CraneMinigameManager/SendCraneItemNeedMaterial.md）：
// PacketGuard → put_header(0,0x1a7) → put_byte(m_selectIdx +0x68) →
// finalize(true) → CUser::Send。
#include "CraneMinigameManager/SendCraneItemNeedMaterial.inc"
}
