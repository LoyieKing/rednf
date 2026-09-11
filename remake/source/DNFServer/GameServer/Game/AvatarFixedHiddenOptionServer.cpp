// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AvatarFixedHiddenOptionServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AvatarFixedHiddenOptionServer 实现（独立 TU）。
// 依据 docs/class_func_reports/AvatarFixedHiddenOptionServer/、
// ARAD__SCRIPT__AvatarFixedHiddenOption/（含 parseAbility.md 全量反编译）与 ORIG 反汇编转写。
#include "AvatarFixedHiddenOptionServer.h"

#include <cstdio>
#include <map>

#include "CEnvironment.h"    // less_TCHAR / SpinLock
#include "GameTypes.h"       // cMyTrace
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanFloat

// ---- 布局断言（ORIG elf32-i386 反汇编实证）----
typedef char avatar_hidden_ability_variation_size_assert[
    sizeof(STAvatarHiddenAbilityVariation) == 0x14 ? 1 : -1];
typedef char avatar_fixed_hidden_option_size_assert[
    sizeof(ARAD::SCRIPT::AvatarFixedHiddenOption) == 0x4c ? 1 : -1];
typedef char avatar_fixed_hidden_option_server_size_assert[
    sizeof(AvatarFixedHiddenOptionServer) == 0x98 ? 1 : -1];

#include "AvatarFixedHiddenOptionServer/AvatarFixedHiddenOptionServer.inc"
{
}

// ============================================================================
// STAvatarHiddenAbilityVariation —— ORIG ctor W 0x088b01e8 / 0x088b021a、dtor W 0x088b0aae
// ============================================================================
#include "AvatarFixedHiddenOptionServer/STAvatarHiddenAbilityVariation.inc"
    AVATAR_HIDDEN_ABILITY_TYPE::T type, int value, int rate) throw()
{
    m_type = type;          // +0x00
    m_bFloatValue = false;  // +0x04 = 0
    m_fValue = 0.0f;        // +0x08 清零
    m_nValue = value;       // +0x0c
    m_nRate = rate;         // +0x10
}

#include "AvatarFixedHiddenOptionServer/STAvatarHiddenAbilityVariation_1.inc"
    AVATAR_HIDDEN_ABILITY_TYPE::T type, float value, int rate)
{
    m_type = type;          // +0x00
    m_bFloatValue = true;   // +0x04 = 1
    m_fValue = value;       // +0x08（浮点值）
    m_nValue = 0;           // +0x0c 清零
    m_nRate = rate;         // +0x10
}


// ============================================================================
// ARAD::SCRIPT::AvatarFixedHiddenOption
// ============================================================================

// ORIG C1 0x088aecac：6 个 vector 顺序构造后调 clearScript()。
#include "AvatarFixedHiddenOptionServer/AvatarFixedHiddenOption.inc"
{
    clearScript();
}

// ORIG D1 0x088aed5e：clearScript 后逆序析构 6 个 vector（成员析构自动完成）。
#include "AvatarFixedHiddenOptionServer/dtor_AvatarFixedHiddenOption.inc"
{
    clearScript();
}

// ORIG 0x088b00e0：clear 每桶并 push_back 一个全 0 默认元素（种子保证 Init 的
// reserve(count-1) 非负，与 ORIG 一致）。
#include "AvatarFixedHiddenOptionServer/clearScript.inc"
{
    // [b28d-56 判词·AE] ORIG 0x088b00e6 循环前先清零两具名 int 槽（-0x14/-0x10，
    // P0 槽往返 ⇒ 原码有具名局部），ctor 实参从槽读（0x8(%esp)=value@-0x10、
    // 0xc(%esp)=rate@-0x14）；循环上界为 `i<=5`（ORIG 尾 setle，P2）。
    int rate = 0;
    int value = 0;
    for (int i = 0; i <= 5; ++i)
    {
        m_abilities[i].clear();
        m_abilities[i].push_back(
            STAvatarHiddenAbilityVariation(AVATAR_HIDDEN_ABILITY_TYPE::NONE, value, rate));
    }
}

// ORIG 0x088b0164：return *(int*)(this+0x48)
#include "AvatarFixedHiddenOptionServer/getMaterialItemIndex.inc"
{
    return m_materialItemIndex;
}

// ORIG 0x088b0170：return *(int*)(this+0x48) == itemIndex
#include "AvatarFixedHiddenOptionServer/isMaterialItemIndex.inc"
{
    if (m_materialItemIndex == itemIndex)
        return true;
    return false;
}

// ORIG 0x088aef52 parseAbility：P333 首次建表状态机（21 项静态映射惰性注册）。
// switch(key) 每个 case 体前半是「!inited ⇒ 注册该项、无 break、fallthrough 到下一
// case」，后半是「inited ⇒ 正常动作 break」；未初始化时 key 取哨兵 0（case 0 与
// case 0x46 共享入口），注册链走完落入 default（此时 inited 仍为 false，其
// `if (inited) return false` 不触发），再由公共尾 leave + inited=true + goto 循环头
// 重查，第二次进入 switch 才按 map 查得的 key 执行正常动作；default 同时承担
// find 未命中（key==-1）的 false 返回。
// [DualArith 2026-09-09] gap 172→0（orig 977 / ours 977；基线 ours 805）。
// [L4fEventAvatar 2026-09-11] AE 收口（977/977，clean 逐行相等）：①inited 分支里
//   `find` 命中判定改三目整体赋值 `key = (it != end) ? it->second : -1;`
//   （ORIG 0x088aef7c 真支算完 jmp 汇合 + 假支 mov $imm + 唯一落槽）；②循环尾
//   `if (!inited) { leave; inited = true; } else { return true; }` 且**不写
//   continue**（ORIG 0x088b00c9 的 `jmp 0x088aef74` 即 do-while 尾部回跳；写
//   continue 会被 4.4 把该 jmp 排到 `mov $0x1,%eax` 之后并多 2 条 jmp 中继）。
// p167 修正版（call rel32 + nm 区间 + .plt 反查，唯一认读口径）：MISSING 全为 C2 变体、
// EXTRA 全为 C1 变体（pair/map/SpinLock/const_iterator/STAvatarHiddenAbilityVariation
// ctor），严格 1:1 镜像、call 总数 150/150 ⇒ 按 Main 2026-09-09 裁定② clone 别名对
// 豁免，判 CLEAR、不排期。旧版 p167 / call_multiset_diff.py 读数已弃用（本件无旧版清单）。
#include "AvatarFixedHiddenOptionServer/parseAbility.inc"
    std::vector<STAvatarHiddenAbilityVariation>& variations, std::string& tag)
{
    // ORIG 0x088aef5b 序言后立即清零 -0x28/-0x24/-0x20 三槽：跨 case 复用的具名局部
    // rate/value/fvalue（float 以 `mov $0x0,%eax; mov %eax,-0x20` 位模式清零）。
    // ORIG：__lookup 为 map<const char*,int,less_TCHAR>，__init_lock 为 SpinLock，
    // __inited 在锁保护下置位；使用真实公共类型以保留跨 TU 依赖和初始化语义。
    int rate = 0;
    int value = 0;
    float fvalue = 0.0f;
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    static LookupMap __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;

    for (;;)
    {
        int key;
        if (__inited)
        {
            LookupMap::const_iterator it = __lookup.find(tag.c_str());
            // [L4fEventAvatar 2026-09-11] P47：ORIG 0x088aef7c..0x088aef98 为
            //   `真支算到 %eax → jmp 汇合` + `假支 mov $0xffffffff,%eax` +
            //   唯一 `mov %eax,-0x1c(%ebp)` ⇒ 三目整体赋值；if/else 会各自落槽
            //   （我方旧形多 `mov %eax,-0x1c; jmp` + `movl $imm,-0x1c`）。
            key = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;   // ORIG 0x088af157 jmp 0x088aef74：重查
            }
            key = 0;        // 哨兵：进入注册链（case 0 与 case 0x46 共享入口）
        }

        switch (key)
        {
        // 每个 case 的 !inited 分支就地构造 value_type（pair<const char* const,int>，
        // 模板实参含 char const(&)[N]，故每个字符串长度一个实例）再 map::insert
        // （value_type const&）——与 ORIG 初始化链逐条同形（0x46..0xac）。用
        // make_pair 会多出 pair<const char*,int> → value_type 的转换临时（第三次
        // call），与 ORIG 不符，故直接构造 value_type。
        case 0:
        case 0x46:   // [physical attack] → type 1
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[physical attack]", 0x46));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_ATTACK, value, rate));
                break;
            }
            // fallthrough：ORIG 0x088af1d9 jmp 0x088af237（下一 case 入口）
        case 0x4b:   // [magical attack] → type 2
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[magical attack]", 0x4b));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_ATTACK, value, rate));
                break;
            }
        case 0x50:   // [physical defense] → type 3
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[physical defense]", 0x50));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_DEFENSE, value, rate));
                break;
            }
        case 0x55:   // [magical defense] → type 4
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[magical defense]", 0x55));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_DEFENSE, value, rate));
                break;
            }
        case 0x5a:   // [HP MAX] → type 5
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[HP MAX]", 0x5a));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::HP_MAX, value, rate));
                break;
            }
        case 0x5f:   // [MP MAX] → type 6
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[MP MAX]", 0x5f));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MP_MAX, value, rate));
                break;
            }
        case 100:    // [HP regen speed] → type 7
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[HP regen speed]", 100));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::HP_REGEN_SPEED, value, rate));
                break;
            }
        case 0x69:   // [MP Regen speed] → type 8
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[MP Regen speed]", 0x69));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MP_REGEN_SPEED, value, rate));
                break;
            }
        case 0x6e:   // [attack speed] → type 9
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[attack speed]", 0x6e));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::ATTACK_SPEED, value, rate));
                break;
            }
        case 0x73:   // [move speed] → type 10
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[move speed]", 0x73));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MOVE_SPEED, value, rate));
                break;
            }
        case 0x78:   // [cast speed] → type 11
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[cast speed]", 0x78));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::CAST_SPEED, value, rate));
                break;
            }
        case 0x7d:   // [inventory limit] → type 12
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[inventory limit]", 0x7d));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::INVENTORY_LIMIT, value, rate));
                break;
            }
        case 0x82:   // [stuck] → type 13（第一个数值为浮点）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[stuck]", 0x82));
            }
            else
            {
                fvalue = ScanFloat((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::STUCK, fvalue, rate));
                break;
            }
        case 0x87:   // [stuck resistance] → type 14
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[stuck resistance]", 0x87));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::STUCK_RESISTANCE, value, rate));
                break;
            }
        case 0x8c:   // [all activestatus resistance] → type 15
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[all activestatus resistance]", 0x8c));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::ALL_ACTIVESTATUS_RESISTANCE, value, rate));
                break;
            }
        case 0x91:   // [hit recovery] → type 16
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[hit recovery]", 0x91));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::HIT_RECOVERY, value, rate));
                break;
            }
        case 0x96:   // [equipment magical defence] → type 17（首个 ScanInt 弃读）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[equipment magical defence]", 0x96));
            }
            else
            {
                ScanInt((bool*)NULL);
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_MAGICAL_DEFENCE, value, rate));
                break;
            }
        case 0x9c:   // [equipment physical defence] → type 18（首个 ScanInt 弃读）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[equipment physical defence]", 0x9c));
            }
            else
            {
                ScanInt((bool*)NULL);
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_PHYSICAL_DEFENCE, value, rate));
                break;
            }
        case 0xa2:   // [jump power] → type 19
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[jump power]", 0xa2));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::JUMP_POWER, value, rate));
                break;
            }
        case 0xa7:   // [physical critical hit] → type 20（浮点）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[physical critical hit]", 0xa7));
            }
            else
            {
                fvalue = ScanFloat((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_CRITICAL_HIT, fvalue, rate));
                break;
            }
        case 0xac:   // [magical critical hit] → type 21（浮点）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[magical critical hit]", 0xac));
            }
            else
            {
                fvalue = ScanFloat((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_CRITICAL_HIT, fvalue, rate));
                break;
            }
            // fallthrough：ORIG 0x088b0041 jmp 0x088b0096（default 入口）
        default:
            if (__inited)
                return false;
            break;
        }

        // 公共尾（ORIG 0x088b00a8）：注册链走完（inited 仍 false）时 leave 并置位，
        // 回到循环头重查；正常动作路径（inited true）直接返回 true。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            // ORIG 0x088b00c9 的 `jmp 0x088aef74` 即 do-while(true) 的尾部回跳
            // （原码未写 continue，否则 4.4 会把该 jmp 排到 return true 之后）。
        }
        else
        {
            // [L4fEventAvatar 2026-09-11] ORIG 块序为「continue 的 jmp 在前、
            //   mov $0x1,%eax 直落 epilogue 在后」⇒ 原码 return true 在 else 块
            //   （4.4 对平铺 `...; continue; } return true;` 会把 continue 的 jmp
            //   排到 mov $1 之后并给 return true 另生 `jmp <epilogue>` 中继）。
            return true;
        }
    }
}

// ORIG 0x088aedce loadScript。控制流忠实转写：外层 ScanType 失败即返回 0；
// "[upper]" / "[rare]" 分支内先 ScanType 再 parseAbility，parse 未命中回落外层；
// "[mystic circle]" 读一个整数到材料索引。打开脚本失败返回 -1。
#include "AvatarFixedHiddenOptionServer/loadScript.inc"
{
    if (!loadRDARScriptFile("Etc/Avatar_Roulette/", "AvatarFixedHiddenOptionList.etc"))
        return -1;

    std::string token;
    // [b28d-56 判词·P171/P232] ORIG 0x088aee06 在 token 构造后发一条 dead
    // `movb $0x0,-0x9(%ebp)`（全函数仅此一次读/写该槽）⇒ 原码有该 bool 局部
    // 的预清；补声明使 token 槽回 -0x10、该 bool 落 -0x9，全帧槽号对齐。
    bool flag = false;
    (void)flag;
    for (;;)
    {
        // [b28d-56 判词·P164] ORIG 0x088aee25 `jne <尾部 cleanup>`：ScanType 失败
        // 跳函数尾的 token 析构块（`mov $0x0,%ebx; lea token; call D1`）再统一
        // 返回；写 `return 0` 会被 4.4 就地内联到检查点后 ⇒ 改 break + 循环外
        // 统一 return 0 复现尾部共享块。
        if (!ScanType(token, true))
            break;
        if (token == "[upper]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[2], token))   // ORIG this+0x18
                    break;
            }
        }
        else if (token == "[rare]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[3], token))   // ORIG this+0x24
                    break;
            }
        }
        else if (token == "[mystic circle]")
        {
            m_materialItemIndex = ScanInt((bool*)NULL);           // ORIG *(int*)(this+0x48)
        }
    }
    return 0;
}

// ============================================================================
// AvatarFixedHiddenOptionServer —— ORIG C1 0x0817e02c / D1 0x0817e0e6 /
// Load 0x0817e154 / Destroy 0x0817e1e2 / Init 0x0817e3e8
// ============================================================================

#include "AvatarFixedHiddenOptionServer/dtor_AvatarFixedHiddenOptionServer.inc"
{
}

// ORIG 0x0817e154：cMyTrace(__PRETTY_FUNCTION__,0x1a,9,true,false) +
// "\t- Loading Avatar Hidden Option - "；loadScript==0 且 Init()!=0 才算成功。
#include "AvatarFixedHiddenOptionServer/Load.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1a, 9, true, false);
    trace("\t- Loading Avatar Hidden Option - ");
    // [b28c-16] P84/P102：ORIG 短路结果先物化 bool 局部（loadScript!=0 与
    // Init()==0 各置失败标志 1，再统一 test/je 二次物化返回），4.4 直返
    // `a==0 && b!=0` 折叠丢二次 bool 物化与 xor 取反。
    // [b28c-16 修正] 具名 bool 局部反多 -0x9 槽 + setne 物化（帧 0x38→0x48）。
    // ORIG 实为「失败标志汇聚 eax → 尾部 !r 归一」：`jA L1(mov$1) / Init→xor$1
    // →je L0(mov$0) / L2: test;je L3; mov$0 / L3: mov$1` ⇒ 源形 = `!(A || B)`。
    // [b28d-56 判词·AE] 旧判 N 作废：真因是 Init 返回类型 int（非 bool）致调用点
    // 无 xor 段。ORIG 0x0817e1ae 段为 `call loadScript; test %eax,%eax; jne <fail>；
    // call Init; xor $1,%eax; test %al,%al; je <ok>；fail: mov $1; jmp；ok: mov $0;
    // test %al,%al; je <ret1>; mov $0; ret1: mov $1`（P214① 正条件短路物化）。
    // 探针 /tmp/lane56/load.cpp L1/L4 逐条复现；依赖 Init 返回类型 bool（已改）。
    if (loadScript() != 0 || !Init())
        return false;
    return true;
}

// ORIG 0x0817e1e2：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
#include "AvatarFixedHiddenOptionServer/Destroy.inc"
{
    return 1;
}

// ORIG 0x0817e3e8：6 个桶各自把下标 1..count-1 的 m_nRate（元素 +0x10）累加成前缀和
// push 入 m_cumulativeRate[i]，之前 reserve(count-1)。恒返回 1。
// [b28d-56] 返回类型 int→bool：ORIG Load 0x0817e1bd 对 Init 的调用点形为
// `call; xor $1,%eax; test %al,%al; je`（bool callee 特征，P292/P248 档①）；
// int callee 恒 `test %eax; sete; test`。mangled 不含返回类型，符号/ABI 不变。
#include "AvatarFixedHiddenOptionServer/Init.inc"
{
    for (int i = 0; i < 6; ++i)
    {
        int cumulative = 0;
        int count = (int)m_abilities[i].size();
        m_cumulativeRate[i].reserve((std::vector<int>::size_type)(count - 1));
        for (int k = 1; k < count; ++k)
        {
            cumulative += m_abilities[i][k].m_nRate;
            m_cumulativeRate[i].push_back(cumulative);
        }
    }
    return true;
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   GetSkillIndex(rarity)       ORIG T 0x0817e1ec（GetSkillIndex.md 转写）
//   GetSkillIndex(rarity, idx)  ORIG T 0x0817e23e（GetSkillIndex_0817e23e.md）
//   GetRandomInput              ORIG T 0x0817e334（GetRandomInput.md）
//   IsVaildSkillIndex           ORIG T 0x0817e392（IsVaildSkillIndex.md）
//   CheckCondition              ORIG T 0x0817e4c6（CheckCondition.md 全文转写）
//   ChangeHiddenOption          ORIG T 0x0817e8d6（ChangeHiddenOption.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item
#include "LogManager.h"
#include "GlobalData.h"
#include "MoreTypes.h"          // arad::SigAvatarHiddenOptionChange / SigAradAvatarHiddenOptionSave
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "Arad_InternalDispatcher.h"   // DISPATCHER::make_internal_stream_jpn

#include "AvatarFixedHiddenOptionServer/GetRandomInput.inc"
{
    if (rarity >= 6)
        return -1;
    if (m_cumulativeRate[rarity].size() == 0)
        return -1;
    return m_cumulativeRate[rarity].back();
}

#include "AvatarFixedHiddenOptionServer/IsVaildSkillIndex.inc"
                                                      int idx)
{
    if (rarity >= 6)
        return false;
    int size = (int)m_abilities[rarity].size();
    // [b28c-16] P133：ORIG 两独立条件（cmpl $0;jne 续 / cmp;jge 落零），
    // 合并 `||` 4.4 产 je 单出口反形。
    if (size == 0)
        return false;
    if (idx < size)
        return true;
    return false;
}

#include "AvatarFixedHiddenOptionServer/GetSkillIndex.inc"
{
    // ORIG 0x0817e23e：无 empty() 前置检查，直接 back()（GetRandomInput 保证非空进入）。
    // [b28d-56 判词·AE] P183/P157 去引用缓存：ORIG 每处访问都重发完整
    // `mov rarity,%edx; mov %edx,%eax; add %eax,%eax; add %edx,%eax; shl $2;
    // add $0x50; add this` 链（back/end/begin/begin 共 4 处），无 rates 引用槽。
    if (!(randIdx < m_cumulativeRate[rarity].back() && 0 <= randIdx))
        return -1;
    std::vector<int>::iterator up =
        std::upper_bound(m_cumulativeRate[rarity].begin(),
                         m_cumulativeRate[rarity].end(), randIdx);
    return (int)(up - m_cumulativeRate[rarity].begin()) + 1;
}

#include "AvatarFixedHiddenOptionServer/GetSkillIndex_1.inc"
{
    int total = GetRandomInput(rarity);
    if (total == -1)
        return -1;
    int randIdx = get_rand_int(total);
    return GetSkillIndex(rarity, randIdx);
}

#include "AvatarFixedHiddenOptionServer/CheckCondition.inc"
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    // [b28d-56 判词·N → L4fEventAvatar 2026-09-11 清零] ORIG 0x0817e4c6。
    // 旧「残余寄存器角色差」经 P59 guard 形复原后消失：原 isMaterialItemIndex
    // 判断写的是 if(真){正文} 后置失败块，4.4 把失败块排到函数尾（我方 239 条、
    // ORIG 置前 236 条）；改为 `if (!isMaterialItemIndex(...)) { 失败块; return
    // false; }` 后两侧逐行一致（AE，236/236）。
    CInventory* invW = user->getCurCharacInvenW();
    // 材料槽（INVEN 1）—— ORIG 以 sret 形式调用 GetInvenSlot（按值返回）
    Inven_Item material;
    material = invW->GetInvenSlot(1, (int)sig->m_materialSlot);
    if (material.isEmpty())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0x71,
            "AVATAR HIDDEN OPTION CHANGE : Material GetInvenSlot() failed. [slot:%d] [charac_no:%d]",
            // [b28d-56] ORIG 0x0817e5?? 发两段 `movzwl (%edx),%edx; movswl %dx,%edx`
            // （SIGFIX 双段判据 ⇒ 源码 `(int)(short)字段`，字段本身 ushort）。
            (int)(short)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x16);
        return false;
    }
    // [b23S3C] ORIG +0x162 call Singleton::Get(); add $4 → m_script 再调成员
    // （P48 使用点重取单例），m_script 直成员调会少一发 call（p167 实证）。
    // [L4fEventAvatar 2026-09-11] P59 guard 形复原：ORIG 0x0817e63c `xor $0x1,%eax;
    //   test %al,%al; je 0x0817e6aa <+0x1e4>`（je 直达 avatar 正文），失败块
    //   （0x0817e643 logFormat「isMaterial() failed」+ SendCmdError + mov $0 + jmp）
    //   紧随判断之后 ⇒ 原码 `if (!isMaterialItemIndex(...)) { …失败…; return false; }`；
    //   写 if(真){正文} 后置失败块会被 4.4 排到函数尾（实测与我方旧形同）。
    if (!ARAD::Singleton<AvatarFixedHiddenOptionServer>::Get()
             ->isMaterialItemIndex((int)material.m_addInfo))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0x78,
            "AVATAR HIDDEN OPTION CHANGE : isMaterial() failed [index:%d] [charac_no:%d]",
            (int)material.m_addInfo, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x11);
        return false;
    }
    Inven_Item avatar;
    avatar = invW->GetInvenSlot(2, (int)sig->m_avatarSlot);
    if (avatar.isEmpty())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0x83,
            "AVATAR HIDDEN OPTION CHANGE : Avatar GetInvenSlot() failed. [slot:%d] [charac_no:%d]",
            (int)sig->m_avatarSlot, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x16);
        return false;
    }
    CItem* item = G_CDataManager()->find_item((int)avatar.m_addInfo);
    if (item == 0 || !item->IsAvatarItem())
        return false;
    if (!item->IsHiddenOption())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0x94,
            "AVATAR HIDDEN OPTION CHANGE : IsHiddenOption() failed. [index:%d] [charac_no:%d]",
            (int)avatar.m_addInfo, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x11);
        return false;
    }
    return true;
}

#include "AvatarFixedHiddenOptionServer/ChangeHiddenOption.inc"
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    // [b28d-56 判词·N → L4fEventAvatar 2026-09-11 收敛] ORIG 0x0817e8d6 230 条；
    // 本轮按 ORIG 直证复原 4 处后：prologue 已对齐（push edi/esi/ebx + sub $0x7c）、
    // traceDel/traceOk 已用链式临时形使实参先于 ctor 求值（4.1 调度）、
    // GetSkillIndex 与 Acquire 的结果改寄存器直传（无具名槽）、save.m_slot 的
    // `sig->m_avatarSlot+10` 改 unsigned 语境（ORIG 零扩展、无 movswl）。
    // 残余 N：我方 231 条 / 帧同 0x7c，仅剩**全函数槽位整体偏移 4**（ORIG ref
    // -0x20/item -0x1c/traceDel -0x40… 对 我方 -0x24/-0x20/-0x44…）——
    // ORIG 把 traceOk 第三实参的 int 临时放**对象区最深槽 -0x5c**，4.4 则把
    // 具名 int 放标量区最浅槽 -0x1c（删具名改用表达式临时实测劣化：帧 0x7c→0x8c、
    // 231→237 条）⇒ 属 4.1/4.4 栈槽分配器分区差异，无源级旋钮。语义完成。
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(1), (int)sig->m_materialSlot, 1,
                          eItemDelReason(3), true) != 1)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0xa1,
            "AVATAR HIDDEN OPTION CHANGE : delete_item() failed. [slot:%d] [charac_no:%d]",
            (int)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x13);
        return false;
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)sig->m_materialSlot);
    // [L4fEventAvatar 2026-09-11] P37：ORIG 0x0817e9b9..0x0817e9ed 在 cMyTrace ctor
    //   之前先求值 operator() 的两个实参（getCurCharacNo→%ebx、m_materialSlot→%esi）
    //   —— 这是**链式临时**形（`cMyTrace(...)(...)` 的实参先于对象构造求值），
    //   命名对象形（ctor 语句在前）在 4.4 下必然先 ctor 后求参。
    cMyTrace(__PRETTY_FUNCTION__, 0xa6, 0)(
        "AVATAR HIDDEN OPTION CHANGE : delete_item() success. [slot:%d] "
        "[charac_no:%d]",
        (int)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());

    Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(2, (int)sig->m_avatarSlot);
    if (ref == 0)
        return false;
    CItem* item = G_CDataManager()->find_item((int)ref->m_addInfo);
    if (item == 0)
        return false;
    if (!item->IsAvatarItem())
        return false;

    // [L4fEventAvatar 2026-09-11] P0/P183：ORIG 0x0817eaa3 `call GetSkillIndex; mov %eax,%edx;
    //   mov -0x20,%eax; mov %dx,0xd(%eax)` 全程寄存器直传、无槽往返 ⇒ 原码无具名
    //   skillIdx（我方具名形多 `mov %eax,-0x10; mov -0x10,%eax` 两次往返）。
    ref->m_fieldSplit.m_fielddLo = (short)GetSkillIndex((ENUM_RARITY)item->get_grade());
    sig->m_hiddenOption = (unsigned short)(short)ref->m_fieldSplit.m_fielddLo;

    // [L4fEventAvatar 2026-09-11] P0：ORIG 0x0817eb21 `movzwl -0x42(%ebp),%eax; cwtl;
    //   mov %eax,-0x5c(%ebp)` 后由 0x0817eb5a `mov -0x5c(%ebp),%eax` 读回 ⇒
    //   该 int 是具名局部（store+load 往返；且 -0x5c 是全函数最深槽 = 最后声明）。
    int hiddenOption;
    arad::SigAradAvatarHiddenOptionSave save;             // +0/…清零语义同 ORIG W ctor
    save.m_field0 = user->get_acc_id();
    save.m_characNo = (unsigned int)user->getCurCharacR()->m_characNo;
    save.m_itId = (unsigned int)ref->m_addInfo;
    // [L4fEventAvatar 2026-09-11] 类型轴：ORIG 0x0817eafb 为 `movzwl 0x2(%eax),%eax;
    //   add $0xa,%eax; mov %ax,-0x44`（**零扩展**、无 `movswl`）⇒ 原码该表达式在
    //   unsigned 语境（m_avatarSlot 声明为 signed short，`(int)` 转换会生 movswl）。
    save.m_slot = (short)((unsigned short)sig->m_avatarSlot + 10);
    save.m_hiddenOption = (short)ref->m_fieldSplit.m_fielddLo;

    hiddenOption = (int)(short)save.m_hiddenOption;
    // [L4fEventAvatar 2026-09-11] ORIG 0x0817eb14..0x0817eb51 在 ctor 前求值全部实参
    //   （getCurCharacNo→%ebx、itId→%edi、slot→%esi、hiddenOption→-0x5c）⇒ 链式临时形；
    //   ORIG 0x0817eb2c `movzwl -0x44(%ebp),%eax; movswl %ax,%esi` 是**从 save.m_slot
    //   读回**再符号扩展（不是重算 sig->m_avatarSlot+10）。
    cMyTrace(__PRETTY_FUNCTION__, 0xc2, 0)(
        "AVATAR HIDDEN OPTION CHANGE : success [slot:%d] [item_id:%d] "
        "[hidden_option:%d] [charac_no:%d]",
        (int)save.m_slot, save.m_itId,
        hiddenOption,
        (unsigned int)user->getCurCharacNo());

    // [L4fEventAvatar 2026-09-11] P0/P183：ORIG 0x0817eb94 `call Acquire` 后结果
    //   直接进 CStreamGuard ctor 的 0x4(%esp)、无 -0xc 具名槽（我方具名 stream 多
    //   `mov %eax,-0xc; mov -0xc,%eax` 往返）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp", 0xc6),
                       true);
    ARAD::DISPATCHER::make_internal_stream_jpn(
        guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2f, -1);
    guard.operator->()->put_binary(&save, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
                             (int)sig->m_avatarSlot);
    return true;
}
// ============================================================================
// UnitTest / Test1 —— ORIG UnitTest T 0x0817ecd8、Test1 t 0x0817ec67
// （docs/class_func_reports/AvatarFixedHiddenOptionServer/UnitTest.md、
//  docs/class_func_reports/_lt_global_gt_/Test1.md 全文转写）
// ============================================================================

// ORIG 0x0817ec67（文件内 static，_ZL 局部符号）：调 GetSkillIndex(rarity=3, input)
// 并把结果与期望值 printf 比对。
static void Test1(AvatarFixedHiddenOptionServer* server, int input, int expect)
{
    int output = server->GetSkillIndex((ENUM_RARITY)3, input);
    if (expect != output) {
        printf("input==%d, output!=%d/%d\n", input, output, expect);
    } else {
        printf("input==%d, output==%d/%d\n", input, output, expect);
    }
}

// ORIG 0x0817ecd8：非空 server 时依次跑 6 组 (input, expect) 用例。
#include "AvatarFixedHiddenOptionServer/UnitTest.inc"
{
    if (server == 0)
        return;
    Test1(server, 0, 1);
    Test1(server, 0x95, 1);
    Test1(server, 0x96, 2);
    Test1(server, 0x26dd, 0x3e);
    Test1(server, 0x26de, 0x3f);
    Test1(server, 9999, 0x3f);
}
