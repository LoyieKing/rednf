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


#include "AvatarFixedHiddenOptionServer/STAvatarHiddenAbilityVariation.inc"


#include "AvatarFixedHiddenOptionServer/STAvatarHiddenAbilityVariation_1.inc"



// ============================================================================
// ARAD::SCRIPT::AvatarFixedHiddenOption
// ============================================================================

#include "AvatarFixedHiddenOptionServer/AvatarFixedHiddenOption.inc"


#include "AvatarFixedHiddenOptionServer/dtor_AvatarFixedHiddenOption.inc"


#include "AvatarFixedHiddenOptionServer/clearScript.inc"


#include "AvatarFixedHiddenOptionServer/getMaterialItemIndex.inc"


#include "AvatarFixedHiddenOptionServer/isMaterialItemIndex.inc"


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

#include "AvatarFixedHiddenOptionServer/loadScript.inc"


// ============================================================================
// AvatarFixedHiddenOptionServer —— ORIG C1 0x0817e02c / D1 0x0817e0e6 /
// Load 0x0817e154 / Destroy 0x0817e1e2 / Init 0x0817e3e8
// ============================================================================

#include "AvatarFixedHiddenOptionServer/dtor_AvatarFixedHiddenOptionServer.inc"


#include "AvatarFixedHiddenOptionServer/Load.inc"


#include "AvatarFixedHiddenOptionServer/Destroy.inc"


#include "AvatarFixedHiddenOptionServer/Init.inc"


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


#include "AvatarFixedHiddenOptionServer/IsVaildSkillIndex.inc"


#include "AvatarFixedHiddenOptionServer/GetSkillIndex.inc"


#include "AvatarFixedHiddenOptionServer/GetSkillIndex_1.inc"


#include "AvatarFixedHiddenOptionServer/CheckCondition.inc"


#include "AvatarFixedHiddenOptionServer/ChangeHiddenOption.inc"

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

#include "AvatarFixedHiddenOptionServer/UnitTest.inc"

