// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CraneMinigameManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
{
    if (!loadRDARScriptFile("", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                 // ORIG -0xe4
    CraneItemInfo* currentItem = 0;    // ORIG -0xe8（+0x3e movl $0）
    bool ok = false;                   // ORIG -0xe9（+0x48 movb $0；仅
                                       // [need material] 首 ScanInt 取址）

    for (;;)
    {
        if (!ScanType(token, true))    // +0x4f（xor$1 物化 + P164 scan_exit 岛）
        {
            break;                     // +0x70c：mov $1,%ebx → 尾部 dtor → return true
        }

    redo_lookup:                       // +0x70：两条重派发边（+0x250/+0x701）落点
        static LookupMap __lookup;     // guard + __cxa_atexit(map::~map)
        static SpinLock __init_lock;   // guard + __cxa_atexit(SpinLock::~SpinLock)
        static bool __inited = false;  // 零初始化 plain bool（无 guard）

        int tagId;                     // ORIG -0x1c
        if (__inited)
        {
            const char* key = token.c_str();            // +0x173
            LookupMap::const_iterator it = __lookup.find(key);   // +0x17c
            // +0x1d3 end() → +0x206 operator!= → 命中 operator->(+0x218) 取
            // second（+0x220 单点汇聚），未命中 +0x222 mov $-1
            tagId = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                        // +0x22c
            if (__inited)
            {
                __init_lock.leave();                    // +0x24a
                goto redo_lookup;                       // +0x250 → guard 重查
            }
            tagId = 0;                                  // +0x255 注册链入口
        }

        switch (tagId)
        {
        case 0:
        case 0x36:                        // [viewCnt]（+0x274 与 case 0 共 leaf）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[viewCnt]", 0x36));
            }
            else
            {
                // +0x2d1：ScanInt(NULL) → 16 位存 script+0x00
                script->m_viewCnt = static_cast<unsigned short>(ScanInt(static_cast<bool*>(0)));
                break;
            }
            /* fall through（+0x2cf jmp +0x2ea） */
        case 0x3b:                        // [item]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[item]", 0x3b));
            }
            else
            {
                // +0x347：new CraneItemInfo → +0x04 int → +0x00 word = size()
                currentItem = new CraneItemInfo();
                currentItem->m_itemIdx = ScanInt(static_cast<bool*>(0));
                currentItem->m_id =
                    static_cast<unsigned short>(script->m_items.size());
                break;
            }
            /* fall through（+0x345 jmp +0x398） */
        case 0x42:                        // [cnt]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[cnt]", 0x42));
            }
            else
            {
                // +0x3f5：ORIG 无判空，直接 16 位写 currentItem+0x08
                currentItem->m_cnt = static_cast<unsigned short>(ScanInt(static_cast<bool*>(0)));
                break;
            }
            /* fall through（+0x3f3 jmp +0x410） */
        case 0x47:                        // [viewRatio]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[viewRatio]", 0x47));
            }
            else
            {
                currentItem->m_viewRatio = ScanFloat(static_cast<bool*>(0));   // +0x46d fstps +0x0c
                break;
            }
            /* fall through（+0x46b jmp +0x487） */
        case 0x4c:                        // [pickRatio]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[pickRatio]", 0x4c));
            }
            else
            {
                currentItem->m_pickRatio = ScanInt(static_cast<bool*>(0));     // +0x4d3 32 位 +0x10
                break;
            }
            /* fall through（+0x4d3 jmp +0x4ef） */
        case 0x51:                        // [/item]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/item]", 0x51));
            }
            else
            {
                // +0x53d：无条件 push_back(&currentItem)，ORIG 不复位指针
                script->m_items.push_back(currentItem);
                break;
            }
            /* fall through（+0x53b jmp +0x55a） */
        case 0x56:                        // [material]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[material]", 0x56));
            }
            else
            {
                // +0x5a8：script+0x10 int、script+0x14 word
                script->m_material = ScanInt(static_cast<bool*>(0));
                script->m_field14 = static_cast<short>(ScanInt(static_cast<bool*>(0)));
                break;
            }
            /* fall through（+0x5a6 jmp +0x5d4） */
        case 0x5c:                        // [send mail]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[send mail]", 0x5c));
            }
            else
            {
                importActionMailScript(&script->m_sendMail);   // +0x622
                break;
            }
            /* fall through（+0x620 jmp +0x635） */
        case 0x61:                        // [need material]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[need material]", 0x61));
            }
            else
            {
                // +0x683 内层环：pair 每轮默认构造（+0x68c）→ first=ScanInt(&ok)
                // → !ok 跳出（+0x6b1）→ second=ScanInt(NULL) → push_back(引用)
                while (true)
                {
                    std::pair<int, int> material;
                    material.first = ScanInt(&ok);
                    if (ok != true)
                        break;
                    material.second = ScanInt(static_cast<bool*>(0));
                    script->m_needMaterials.push_back(material);
                }
            }
        }

        if (!__inited)                   // 取反短路物化（P32 xor$1 岛）
        {
            __init_lock.leave();         // +0x6f5
            __inited = true;             // +0x6fe
            goto redo_lookup;            // +0x701 → guard 重派发同 token
        }
        else
        {
            goto scan_next;              // +0x706 nop 岛 → +0x4f 回边
        }

    scan_next:;
    }
    return true;                         // +0x70d mov $1,%ebx（dtor 后统一出口）
}

#include "CraneMinigameManager/CraneMinigameScript.inc"


#include "CraneMinigameManager/dtor_CraneMinigameScript.inc"


#include "CraneMinigameManager/CraneMinigameManager.inc"


#include "CraneMinigameManager/dtor_CraneMinigameManager.inc"

#include "CraneMinigameManager/init.inc"


#include "CraneMinigameManager/updateCraneItemNeedMaterial.inc"


#include "CraneMinigameManager/SendCraneItemNeedMaterial.inc"

