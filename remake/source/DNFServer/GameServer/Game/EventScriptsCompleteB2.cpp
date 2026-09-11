// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventScriptsCompleteB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// EventScriptsCompleteB2.cpp —— 事件脚本/升级支持域独占恢复 TU（EventScriptsCompleteB2 批次）。
//
// 覆盖符号（全部经 ORIG nm 实证存在，地址随各项标注）：
//   A. 10 个事件脚本真实 ctor（W）+ ExchangeRandomItemRewardScript::reset（W）
//   B. LevelupSupportSciprt / LevelupSupport2ndScript 的 C1/D1（T）
//   C. importActionMailScript（T 0x0897f147）
//   D. 10 个 import*EventScript（T）
//
// 证据等级：
//   - LevelupSupportSciprt/LevelupSupport2ndScript C1/D1：docs/class_func_reports/
//     LevelupSupportSciprt/ 与 LevelupSupport2ndScript/ 下对应 .md 报告已完整阅读
//     （## 汇编 + ## 反编译 C），按报告转写。
//   - 其余函数无独立报告 → 按 source/toolchain 规则用 ORIG objdump 反汇编逐指令
//     转写；编辑前已完整阅读同类/调用者报告：
//     InGameEventScriptLoader/importInGameEventScript.md（调用者，全文）、
//     EventClassify__C*/README.md 全部、EventClassify__CObjectBringUp/
//     verifyGiveBringUpReward.md（ObjectBringUpRule 字段语义）、以及同域已交付
//     EventLevelupA2.cpp（基类/邮件动作/装载器定义）。各函数头部均已标记。
//
// 解析器形态说明：ORIG 各 import 函数内的「static map<tag,id> + SpinLock +
// __cxa_guard」是函数级静态惰性建表（map 构造/SpinLock 构造各带 guard +
// __cxa_atexit，__inited 零初始化 bool）；构建完成前首个 token 以 key=0 进入
// case 贯穿注册链，表尾释放锁置位后回 guard 重派发同一 token。batch-9 起，
// importAttendanceScript / importPcBangPlayTimeScript /
// importUsedFatigueGiveItemEventScript / importExchangeRandomItemRewardScript
// 四函数按报告逐指令还原该真实双相形（不再用直接 token 比较简化形）；其余
// 同族函数暂保留简化形（各自报告到位后同法收敛）。
//
// 【2026-09-10 真 bug 修正（jcc 目标下标判据）】原 10 个 import*EventScript
// （Attendance/PcBangPlayTime/FatigueQuantity/AddItemCountDB/AccountFirstLogin/
// Exchange/UsedFatigueGiveItem/AvatarDisjointRandomReward/PurchaseCashItemBonus/
// ObjectBringUp）循环体末尾写 `goto scan_exit;`，而 `scan_exit:` 标签落在
// `if(!ScanType(...)){...}` 体内 ⇒ 编译为 `jmp scan_done`（**退出循环**），
// 脚本每文件只解析首个 token 就 return；ORIG 同处为回边 `jmp <循环头>`
// （Attendance 0x08981dc7 → 0x0898157e，其余 +0x42/+0x45/+0x4f 同族）。
// 旧判 AE/IDENTICAL 系 clean() 把 jcc 目标归一为 `<T>` 造成的**假匹配**
// （本轮新暴露的判据盲区）。已删 10 处 `goto scan_exit;` 改自然回边，
// 语义与 ORIG 一致（jcc 目标 = 循环头）。
//
// 【2026-09-10 收敛：内层 for(;;) 重试循环消 entry jmp】判据① 复验残留的唯一
// 目标差是 10 函数共有的「4.4 对**含内层后跳 goto** 的循环在循环头前插 1 条
// entry jmp（`jmp <L+nop>`）」，ORIG(4.1) 则「入口直落 + 尾部回边岛」。
// 探针族定界（/tmp/l18p/{p,q,r,s}.cpp，routed 4.4 实测）：entry jmp 由
// 「循环体内**后跳** goto 到循环内标签」触发（f1/f2/f4/f5 有、f3/f6 前跳无、
// fa–fd 无 goto 无）；普通循环回边（continue）不触发（g1）。
// ⇒ 修法：把 `if(!ScanType){ scan_exit: goto scan_done; }` + `redo_lookup:`
// 标签 + 两处 `goto redo_lookup;` 改写为 `if(!ScanType) goto scan_done;` +
// **内层 `for(;;)` 重试循环**（guard 竞争路径 `continue;`；重派发块写
// `if(!__inited){leave;__inited=true;} else {break;} continue;` —— continue
// 置于内层体末使 4.4 直出单条 `jmp <guard>`，否则多一条 latch 跳板）。
// 10 函数（FatigueQuantity/AccountFirstLogin/AddItemCountDB/PcBangPlayTime/
// Exchange/AvatarDisjointRandomReward/Attendance/UsedFatigueGiveItem/
// PurchaseCashItemBonus/ObjectBringUp）全部 DIFF→AE，raw 指令数亦与 ORIG 全等。
// ObjectBringUp 另有 [get eventitem fatigue] 的内层后跳 `goto send_sec_head;`
// 两处，改「内层 break + 外层 continue」后同样 AE。
// TU 四数：297/562(859)/1/13 → 297/572(869)/1/3。
// 残 3 = importLevelUpRewardScript（N82/N96：ORIG 的 next 标志在 %ebx 寄存器、
// 4.4 -O0 恒槽化；声明序/帧 0x170↔0x1a0 镜像）+ 本 TU
// `__static_initialization_and_destruction_0ii`（N123：每 TU 同名 local 副本，
// compare_tu 按名索引撞名）+ `std::_Vector_base<…Reward_Item…>::D1`（N89：4.1
// out-of-line / 4.4 内联差）。判据① 复验：全 TU 分支方向不匹配 = 0。

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "DNFLexWrapper.h"             // loadRDARScriptFile/ScanType/ScanInt/ScanStr
#include "CEnvironment.h"              // less_TCHAR / SpinLock（__lookup 双相派发依赖）
#include "InGameEventScripts.h"        // InGameEventScript 域权威声明（本批次补字段）
#include "LevelupSupportEventManger.h" // LevelupSupportSciprt / LevelupSupport2ndScript

#include "EventScriptsCompleteB2/getCharacterJobToEnumValue.inc"

// ============================================================================
// A. 事件脚本 ctor（ctor 内显式语句顺序严格对照各自反汇编）
// ============================================================================

#include "EventScriptsCompleteB2/LevelupSupportSciprt.inc"

#include "EventScriptsCompleteB2/LevelUpRewardScript.inc"

#include "EventScriptsCompleteB2/UsedFatigueGiveItemScript.inc"


#include "EventScriptsCompleteB2/AttendanceScript.inc"


#include "EventScriptsCompleteB2/PcBangPlayTimeScript.inc"


#include "EventScriptsCompleteB2/AccountFirstLoginScript.inc"


#include "EventScriptsCompleteB2/reset.inc"


#include "EventScriptsCompleteB2/ExchangeRandomItemRewardScript.inc"


#include "EventScriptsCompleteB2/AvatarDisjointRandomRewardScript.inc"


#include "EventScriptsCompleteB2/FatigueQuantityScript.inc"


#include "EventScriptsCompleteB2/ObjectBringUpScript.inc"


#include "EventScriptsCompleteB2/AddItemCountDBScript.inc"

#include "EventScriptsCompleteB2/ObjectBringUpRule.inc"
#include "EventScriptsCompleteB2/reset_1.inc"

#include "EventScriptsCompleteB2/PurchaseBonusRule.inc"
#include "EventScriptsCompleteB2/reset_2.inc"


#include "EventScriptsCompleteB2/PurchaseCashItemBonusScript.inc"


// ============================================================================
// B. LevelupSupportSciprt / LevelupSupport2ndScript C1/D1（报告转写）
// ============================================================================

#include "EventScriptsCompleteB2/dtor_LevelupSupportSciprt.inc"

#include "EventScriptsCompleteB2/LevelupSupport2ndScript.inc"

#include "EventScriptsCompleteB2/dtor_LevelupSupport2ndScript.inc"

#include "EventScriptsCompleteB2/importActionMailScript.inc"


// ============================================================================
// D. import*EventScript ×10
// ============================================================================

#include "EventScriptsCompleteB2/importFatigueQuantityEventScript.inc"
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3；ORIG 0x08981539
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x226
                if (__inited)
                {
                    __init_lock.leave();                    // +0x23d
                    continue;   // +0x243 → guard 重查
                }
                key = 0;                                    // +0x248 注册链入口
            }

            switch (key)
            {
            case 0x478:                                     // [/event]
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x478));
                }
                else
                {
                    return true;
                }
            }
            /* fall through */
            case 0x47a:                                     // [title]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x47a));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0x47d:                                     // [explain]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x47d));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0x480:                                     // [db table]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x480));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);
                    break;
                }
            }
            /* fall through */
            case 0x483:                                     // [user type]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[user type]", 0x483));
                }
                else
                {
                    script->m_userType =
                        static_cast<unsigned char>(ScanInt((bool*)0));
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x489
                __inited = true;                            // +0x48e
            }
            else
            {
                break;
            }
                continue;   // +0x495 → guard 重派发
        }
    }
scan_done:
    return result;
}

#include "EventScriptsCompleteB2/importAccountFirstLoginEventScript.inc"
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164/P177 同族）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x226
                if (__inited)
                {
                    __init_lock.leave();                    // +0x23d
                    continue;   // +0x243 → guard 重查
                }
                key = 0;                                    // +0x248 注册链入口
            }

            switch (key)
            {
            case 0xc4:                                      // [/event]
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0xc4));
                }
                else
                {
                    return true;
                }
            }
            /* fall through */
            case 0xc6:                                      // [title]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0xc6));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0xc9:                                      // [explain]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0xc9));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0xcc:                                      // [db table]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0xcc));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);
                    break;
                }
            }
            /* fall through */
            case 0xcf:                                      // [send mail]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send mail]", 0xcf));
                }
                else
                {
                    importActionMailScript(&script->m_mail);   // +0xc
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x483
                __inited = true;                            // +0x488
            }
            else
            {
                break;
            }
                continue;   // +0x48f → guard 重派发
        }
    }
scan_done:
    return result;
}

#include "EventScriptsCompleteB2/importAddItemCountDBEventScript.inc"
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164/P177 同族）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x229
                if (__inited)
    {
                    __init_lock.leave();                    // +0x240
                    continue;   // +0x246 → guard 重查
            }
                key = 0;                                    // +0x24b 注册链入口
            }

            switch (key)
            {
            case 0xe9:                                      // [/event]（+0x2a8）
            case 0:
            {
                if (!__inited)
        {
                    __lookup.insert(LookupMap::value_type("[/event]", 0xe9));
            }
                else
            {
                    return true;                            // +0x305
                }
            }
            /* fall through */
            case 0xeb:                                      // [title]（+0x30f）
            {
                if (!__inited)
            {
                    __lookup.insert(LookupMap::value_type("[title]", 0xeb));
                }
                else
                {
                ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0xee:                                      // [explain]（+0x36d）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0xee));
                }
                else
                {
                ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0xf1:                                      // [db table]（+0x3ce）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0xf1));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);       // +0x20
                    break;
                }
            }
            /* fall through */
            case 0xf4:                                      // [item index]（+0x42f）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[item index]", 0xf4));
                }
                else
                {
                    for (;;)
                    {
                        int v = 0;                          // -0xb4 每轮清 0
                        v = ScanInt(&ok);
                        if (ok)
                            script->m_itemIdxs.push_back(v);   // +0x8
                        else
                            break;                          // +0x4a4 → 公共尾
                    }
                    break;
                }
            }
            /* fall through */
            case 0xff:                                      // [quest index]（+0x4c4）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[quest index]", 0xff));
                }
                else
                {
                    for (;;)
                    {
                        int v = 0;                          // -0xb8 每轮清 0
                        v = ScanInt(&ok);
                        if (ok)
                            script->m_questIds.push_back(v);   // +0x14
                        else
                            break;                          // +0x539 → 公共尾
                    }
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x56e
                __inited = true;                            // +0x573
            }
            else
            {
                break;
            }
                continue;   // +0x57a → guard 重派发
        }
    }
scan_done:
    return ok;
}

#include "EventScriptsCompleteB2/importPcBangPlayTimeScript.inc"
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3；ORIG 0x08985601
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;          // ORIG -0x1d：仅 ScanType 失败路径读取返回

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x226
                if (__inited)
                {
                    __init_lock.leave();                    // +0x23d
                    continue;   // +0x243 → guard 重查
                }
                key = 0;                                    // +0x248 注册链入口
            }

            switch (key)
            {
                case 0x5d2:                                  // [/event]（+0x2d3）
                case 0:
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/event]", 0x5d2));
                    }
                    else
                    {
                        return true;                        // +0x330 mov $1,%ebx
                    }
                }
                /* fall through */
                case 0x5d7:                                  // [title]（+0x33a）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[title]", 0x5d7));
                    }
                    else
                    {
                        ScanStr(&script->m_field00);
                        break;
                    }
                }
                /* fall through */
                case 0x5da:                                  // [explain]（+0x3a7）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[explain]", 0x5da));
                    }
                    else
                    {
                        ScanStr(&script->m_field04);
                        break;
                    }
                }
                /* fall through */
                case 0x5dd:                                  // [db table]（+0x417）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[db table]", 0x5dd));
                    }
                    else
                    {
                        ScanStr(&script->m_dbTableName);
                        break;
                    }
                }
                /* fall through */
                case 0x5e0:                                  // [play time]（+0x47b）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[play time]", 0x5e0));
                    }
                    else
                    {
                        // ORIG +0x4d5 值先物化经 %edx 再重载 this 存 +0xc。
                        // m_playTime int→unsigned 异型化（P211，InGameEventScripts.h
                        // 180 行）触发 4.4 转换中转，store 展开翻为 RHS 先 = ORIG 形。
                        script->m_playTime = ScanInt((bool*)0);
                        break;
                    }
                }
                /* fall through */
                case 0x5e3:                                  // [number of time]（+0x4e2）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[number of time]", 0x5e3));
                    }
                    else
                    {
                        script->m_numberOfTime =
                            static_cast<unsigned char>(ScanInt((bool*)0));
                        break;
                    }
                }
                /* fall through */
                case 0x5e6:                                  // [start tag]（+0x549）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[start tag]", 0x5e6));
                    }
                    else
                    {
                        script->m_startTag =
                            static_cast<unsigned short>(ScanInt((bool*)0));
                        break;
                    }
                }
                /* fall through */
                case 0x5e9:                                  // [end tag]（+0x5ae）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[end tag]", 0x5e9));
                    }
                    else
                    {
                        script->m_endTag =
                            static_cast<unsigned short>(ScanInt((bool*)0));
                        break;
                    }
                }
                }

                if (!__inited)
                {
                    __init_lock.leave();                    // +0x626
                    __inited = true;                        // +0x62b
                }
                else
                {
                    break;
                }
                    continue;   // +0x632 → guard 重派发
        }
    }
scan_done:
    return result;
}

#include "EventScriptsCompleteB2/importExchangeRandomItemRewardScript.inc"
                                          char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164/P177 同族）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x229
                if (__inited)
                {
                    __init_lock.leave();                    // +0x240
                    continue;   // +0x246 → guard 重查
                }
                key = 0;                                    // +0x24b 注册链入口
            }

            switch (key)
            {
            case 0x2e8:                                     // [/event]（+0x2a8）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x2e8));
                }
                else
                {
                    return true;                            // +0x305 mov $1,%ebx
                }
            }
            /* fall through */
            case 0x2ea:                                     // [title]（+0x30f）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x2ea));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0x2ed:                                     // [explain]（+0x376）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x2ed));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0x2f0:                                     // [material]（+0x3d7）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[material]", 0x2f0));
                }
                else
                {
                    if (!ScanType(token, true))
                        return false;                       // +0x445 直接返回假
                    if (token == "[exchange rule]")          // +0x460 串比较（非查表）
                    {
                        script->m_flag08 = static_cast<unsigned char>(
                            ScanInt(&ok));
                        // 规则读失败：跳过整数对循环直回公共尾（+0x48b→+0x636）。
                        if (!ok)
                            break;
                    }
                    // 非规则 token 与规则读取成功两路汇入同一整数对循环
                    // （+0x467 je +0x491 / +0x48b fall-through，ORIG 物理单份）。
                    for (;;)
                    {
                        unsigned int first  = 0;
                        unsigned int second = 0;
                        first = ScanInt(&ok);
                        if (!ok)
                            break;
                        second = ScanInt(&ok);
                        if (!ok)
                            break;
                        // 引用对临时（-0x4c）+ pair&lt;int,int&gt; 转换临时（-0x54）均为
                        // 内联实参 sret 槽（P134），不命名局部。
                        script->m_pairs0c.push_back(
                            std::pair<int, int>(std::make_pair(first, second)));
                    }
                    break;
                }
            }
            /* fall through */
            case 0x308:                                     // [random reward item]（+0x540）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[random reward item]", 0x308));
                }
                else
                {
                    for (;;)
                    {
                        int v = 0;                          // -0xcc 每轮清 0
                        v = ScanInt(&ok);
                        if (!ok)
                            break;
                        script->m_ints18.push_back(v);      // +0x18 vector<int>
                    }
                    break;
                }
            }
            /* fall through */
            case 0x312:                                     // [send mail]（+0x5d7）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send mail]", 0x312));
                }
                else
                {
                    importActionMailScript(&script->m_mail);   // +0x24
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x64b
                __inited = true;                            // +0x650
            }
            else
            {
                break;
            }
                continue;   // +0x657 → guard 重派发
        }
    }
scan_done:
    return ok;
}

#include "EventScriptsCompleteB2/importAvatarDisjointRandomRewardScript.inc"
                                            char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164/P177 同族）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;

            for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x229
                if (__inited)
                {
                    __init_lock.leave();                    // +0x240
                    continue;   // +0x246 → guard 重查
                }
                key = 0;                                    // +0x24b 注册链入口
            }

            switch (key)
            {
            case 0x32e:                                     // [/event]（+0x29d）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x32e));
                }
                else
                {
                    return true;                            // +0x2fa
                }
            }
            /* fall through */
            case 0x330:                                     // [title]（+0x304）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x330));
                }
                else
                {
                ScanStr(&script->m_field00);
                        break;
                }
            }
            /* fall through */
            case 0x333:                                     // [explain]（+0x365）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x333));
                }
                else
                {
                ScanStr(&script->m_field04);
                        break;
                }
            }
            /* fall through */
            case 0x336:                                     // [random reward item]（+0x3c6）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[random reward item]", 0x336));
                }
                else
                {
                for (;;)
    {
                        char         rewardKey = 0;         // -0xb5 每轮清 0
                        unsigned int cnt = 0;               // -0xbc
                        rewardKey = static_cast<char>(ScanInt(&ok));
                    if (!ok)
                            break;                          // +0x44a → 公共尾
                    cnt = static_cast<unsigned int>(ScanInt(&ok));
                    if (!ok)
                            break;                          // +0x46e → 公共尾
                        // 引用对临时(-0x38)+value_type 转换(-0x40)+insert(-0x48)
                        // 三段 sret 内联实参形（P134/P61）。
                    script->m_map08.insert(
                            std::make_pair(rewardKey, cnt));
                }
                    break;
                }
            }
            /* fall through */
            case 0x346:                                     // [send mail]（+0x4ca）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send mail]", 0x346));
                }
                else
                {
                    importActionMailScript(&script->m_mail);   // +0x20
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x53b
                __inited = true;                            // +0x540
            }
            else
            {
                break;
            }
                continue;   // +0x547 → guard 重派发
        }
    }
scan_done:
    return ok;
}

#include "EventScriptsCompleteB2/importAttendanceScript.inc"
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164 scan_exit 岛标签四函数族之一）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            // 三静态位于外层 token 循环体首；ORIG 重派发 jmp +0x66 = guard 检查点，
            // goto 边每轮穿过 guard 测试。__inited 零初始化无 guard。
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x229
                if (__inited)
                {
                    __init_lock.leave();                    // +0x240
                    continue;   // +0x246 → guard 重查
                }
                key = 0;                                    // +0x24b 注册链入口
            }

            switch (key)
            {
                case 0x26f:                                  // [/event]（+0x306）
                case 0:
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/event]", 0x26f));
                    }
                    else
                    {
                        return true;                        // +0x363 mov $1,%ebx
                    }
                }
                /* fall through */
                case 0x271:                                  // [title]（+0x36d）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[title]", 0x271));
                    }
                    else
                    {
                        ScanStr(&script->m_field00);
                        break;
                    }
                }
                /* fall through */
                case 0x274:                                  // [explain]（+0x3da）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[explain]", 0x274));
                    }
                    else
                    {
                        ScanStr(&script->m_field04);
                        break;
                    }
                }
                /* fall through */
                case 0x277:                                  // [db table]（+0x44a）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[db table]", 0x277));
                    }
                    else
                    {
                        ScanStr(&script->m_dbTableName);
                        break;
                    }
                }
                /* fall through */
                case 0x27a:                                  // [fatigue]（+0x4ba）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[fatigue]", 0x27a));
                    }
                    else
                    {
                        script->m_field0c = ScanInt((bool*)0);
                        break;
                    }
                }
                /* fall through */
                case 0x299:                                  // [reward explain]（+0x52e）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[reward explain]", 0x299));
                    }
                    else
                    {
                        ScanStr(&script->m_str70);
                        break;
                    }
                }
                /* fall through */
                case 0x29c:                                  // [final reward explain]（+0x59e）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[final reward explain]", 0x29c));
                    }
                    else
                    {
                        ScanStr(&script->m_str74);
                        break;
                    }
                }
                /* fall through */
                case 0x29f:                                  // [reward]（+0x60e）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[reward]", 0x29f));
                    }
                    else
                    {
                        for (;;)
                        {
                            std::pair<int, int> attend;     // +0x65c 每轮默认构造
                            attend.first = ScanInt(&ok);    // +0x673
                            if (!ok)
                                break;                      // +0x68a → 公共尾
                            attend.second = ScanInt(&ok);   // +0x697
                            if (!ok)
                                break;
                            script->m_attends.push_back(attend);   // +0x6c5
                        }
                        break;
                    }
                }
                /* fall through */
                case 0x2ad:                                  // [final day condition]（+0x6d1）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[final day condition]", 0x2ad));
                    }
                    else
                    {
                        script->m_field24 = ScanInt((bool*)0);
                        break;
                    }
                }
                /* fall through */
                case 0x2b0:                                  // [final reward]（+0x736）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[final reward]", 0x2b0));
                    }
                    else
                    {
                        script->m_pair1c.first  = ScanInt((bool*)0);   // +0x1c
                        script->m_pair1c.second = ScanInt((bool*)0);   // +0x20
                        break;
                    }
                }
                /* fall through */
                case 0x2c7:                                  // [send mail]（+0x7ad）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[send mail]", 0x2c7));
                    }
                    else
                    {
                        importActionMailScript(&script->m_mail);
                        break;
                    }
                }
                /* fall through */
                case 0x2cb:                                  // [msg overbtn]（+0x80b）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[msg overbtn]", 0x2cb));
                    }
                    else
                    {
                        ScanStr(&script->m_str28);
                        break;
                    }
                }
                }

                if (!__inited)
                {
                    __init_lock.leave();                    // +0x87c
                    __inited = true;                        // +0x881
                }
                else
                {
                    break;
                }
                    continue;   // +0x888 → guard 重派发
        }
    }
scan_done:
    return ok;
}

#include "EventScriptsCompleteB2/importUsedFatigueGiveItemEventScript.inc"
                                          char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164/P177 同族）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = true;

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x229
                if (__inited)
                {
                    __init_lock.leave();                    // +0x240
                    continue;   // +0x246 → guard 重查
                }
                key = 0;                                    // +0x24b 注册链入口
            }

            switch (key)
            {
                case 0x58f:                                  // [/event]（+0x2ec）
                case 0:
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/event]", 0x58f));
                    }
                    else
                    {
                        return result;                      // +0x349 movzbl -0xf9
                    }
                }
                /* fall through */
                case 0x592:                                  // [title]（+0x355）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[title]", 0x592));
                    }
                    else
                    {
                        ScanStr(&script->m_field00);
                        break;
                    }
                }
                /* fall through */
                case 0x595:                                  // [explain]（+0x3c2）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[explain]", 0x595));
                    }
                    else
                    {
                        ScanStr(&script->m_field04);
                        break;
                    }
                }
                /* fall through */
                case 0x598:                                  // [db table]（+0x432）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[db table]", 0x598));
                    }
                    else
                    {
                        ScanStr(&script->m_dbTableName);
                        break;
                    }
                }
                /* fall through */
                case 0x59c:                                  // [user type]（+0x4a2）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[user type]", 0x59c));
                    }
                    else
                    {
                        script->m_userType =
                            static_cast<unsigned char>(ScanInt(&result));
                        if (!result)
                            return false;                   // +0x527
                        break;
                    }
                }
                /* fall through */
                case 0x5a1:                                  // [used fatigue]（+0x531）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[used fatigue]", 0x5a1));
                    }
                    else
                    {
                        script->m_usedFatigue = ScanInt(&result);
                        if (!result)
                            return false;                   // +0x5a5
                        break;
                    }
                }
                /* fall through */
                case 0x5a7:                                  // [charc event]（+0x5af）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[charc event]", 0x5a7));
                    }
                    else
                    {
                        script->m_charcEventFlag = 1;       // +0x600 movb $1
                        break;
                    }
                }
                /* fall through */
                case 0x5aa:                                  // [apply job type]（+0x609）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[apply job type]", 0x5aa));
                    }
                    else
                    {
                        std::string jobName;                // +0x660 局部 ctor
                        ScanStr(&jobName);
                        script->m_jobType =
                            getCharacterJobToEnumValue(jobName);
                        break;                              // +0x695 dtor → 公共尾
                    }
                }
                /* fall through */
                case 0x5af:                                  // [used fatigue limit]（+0x6b5）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[used fatigue limit]", 0x5af));
                    }
                    else
                    {
                        script->m_fatigueLimit = ScanInt(&result);
                        if (!result)
                            return false;                   // +0x725
                        break;
                    }
                }
                /* fall through */
                case 0x5b5:                                  // [send mail]（+0x72f）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[send mail]", 0x5b5));
                    }
                    else
                    {
                        // 失败仅置假 result 并继续主循环（+0x792 movb $0,-0xf9）。
                        if (!importActionMailScript(&script->m_mail))
                            result = false;
                        break;
                    }
                }
                }

                if (!__inited)
                {
                    __init_lock.leave();                    // +0x7b7
                    __inited = true;                        // +0x7bc
                }
                else
                {
                    break;
                }
                    continue;   // +0x7c3 → guard 重派发
        }
    }
scan_done:
    return result;
}

#include "EventScriptsCompleteB2/importObjectBringUpEventScript.inc"


#include "EventScriptsCompleteB2/importLevelUpRewardScript.inc"

#include "EventScriptsCompleteB2/importPurchaseCashItemBonusEventScript.inc"
                                            char const* path)
{
    // b28d-solo-EventScriptsCompleteB2 复扫（2026-09-09）：原判 DIFF/N96+N18a。
    // 本轮修复：[send] 节 [purchase start/end count] 与 [purchase mutiple
    // number] 三处（+0x776/+0x7b7/+0x7fc）改显式双 goto（send_loop_head/
    // send_loop_end）复现 ORIG 正极性 `xor;test;je <节内头>; jmp <公共尾>`，
    // ndiff 16→**0**、DIFF→AE（gap 0、p167 CLEAR(69/69) 不变；[item idx]
    // 循环经此布局后亦随块序归位）。语义与 ORIG 一致。
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;
    unsigned int start = 0;          // -0x13c：函数级，[item idx start] 覆写

            for (;;)
    {
                if (!ScanType(token, true))
                    goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x233
                if (__inited)
                {
                    __init_lock.leave();                    // +0x24a
                    continue;   // +0x250 → guard 重查
                }
                key = 0;                                    // +0x255 注册链入口
            }

            switch (key)
            {
            case 0x4a1:                                     // [/event]（+0x2e0）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x4a1));
                }
                else
                {
                    return true;                            // +0x33d
                }
            }
            /* fall through */
            case 0x4a3:                                     // [title]（+0x347）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x4a3));
                }
                else
                {
                ScanStr(&script->m_field00);
                        break;
                }
            }
            /* fall through */
            case 0x4a6:                                     // [explain]（+0x3b4）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x4a6));
                }
                else
                {
                ScanStr(&script->m_field04);
                            break;
                }
            }
            /* fall through */
            case 0x4a9:                                     // [db table]（+0x424）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x4a9));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);        // +0x8
                            break;
                }
            }
            /* fall through */
            case 0x4ac:                                     // [item idx]（+0x494）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[item idx]", 0x4ac));
                }
                else
                {
                for (;;)
                    {
                        unsigned int v = 0;                 // -0x144 每轮清 0
                        v = static_cast<unsigned int>(ScanInt(&ok));
                        // +0x50f `xor;test;je <body>`：4.4 对 `!ok` 恒产 jne
                        // （正极性 je 不可造，N96+N18a 封顶），保留 xor 物化形。
                        if (ok != true)
                            break;                          // +0x51e → 公共尾
                        script->m_set0c.insert(v);          // set<unsigned>+0xc
                    }
                    break;
                }
            }
            /* fall through */
            case 0x4b6:                                     // [item idx start]（+0x54a）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[item idx start]", 0x4b6));
                }
                else
                {
                    start = static_cast<unsigned int>(ScanInt((bool*)0));
                    break;
                }
            }
            /* fall through */
            case 0x4b9:                                     // [item idx end]（+0x5be）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[item idx end]", 0x4b9));
                }
                else
                {
                    unsigned int end = 0;                   // 槽清 0 后 ScanInt
                    end = static_cast<unsigned int>(ScanInt(&ok));
                    if (ok != true)                         // +0x642 xor/jne 直退
                        break;                              // 失败 → 公共尾
                    if (start != 0)                         // +0x650
                    {
                        // +0x656 `cmpl $0,-0x1c; jne <检查块>; jmp <公共尾>`：
                        // ORIG 是否定早返形（end==0 直接退），非正向 if 包裹。
                        if (end == 0)
                            break;
                        for (; start <= end; ++start)
                            script->m_set0c.insert(start);
                    }
                    break;
                }
            }
            /* fall through */
            case 0x4c6:                                     // [send]（+0x6aa）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send]", 0x4c6));
                }
                else
                {
                PurchaseCashItemBonusScript::PurchaseBonusRule rule;
                    for (;;)
                    {
                    send_loop_head:;
                    if (!ScanType(token, true))
                            break;              // 节内 EOF → 公共尾（+0x932）
                    if (token == "[purchase start count]")
                        {
                        rule.m_purchaseStartCount = ScanInt(&ok);
                            if (!ok)
                                goto send_loop_end;         // 失败退出节（+0x778）
                            else
                                goto send_loop_head;        // 成功回节内头
                        }
                    else if (token == "[purchase end count]")
                        {
                        rule.m_purchaseEndCount = ScanInt(&ok);
                            if (!ok)
                                goto send_loop_end;
                            else
                                goto send_loop_head;
                        }
                    else if (token == "[purchase mutiple number]")
                        {
                        rule.m_purchaseMutipleNumber = ScanInt(&ok);
                            if (!ok)
                                goto send_loop_end;
                            else
                                goto send_loop_head;
                        }
                    else if (token == "[send mail]")
                        {
                            Action_SendMail mail;            // +0x82e ctor
                            mail.reset();                    // +0x83c
                            // 返回值先物化回 ok 槽再判（+0x84f mov %al,-0x135 /
                            // +0x855 movzbl 读回形）。
                            ok = importActionMailScript(&mail);
                            if (ok != true)                  // +0x861 xor 物化形
                            {
                                break;          // 失败即退出节，rule 不复位（+0x863）
                            }
                            else
                            {
                                if (rule.m_purchaseStartCount != -1 &&
                                    rule.m_purchaseEndCount != -1 &&
                                    rule.m_purchaseMutipleNumber != -1)
                        {
                                    script->m_ruleMails24.push_back(
                                        std::make_pair(rule, mail));
                        }
                                rule.reset();                // +0x8e7
                            }
                        }
                    }
                    send_loop_end:;
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x948
                __inited = true;                            // +0x94d
            }
            else
            {
                break;
            }
                continue;   // +0x954 → guard 重派发
        }
    }
scan_done:
    return ok;
}
