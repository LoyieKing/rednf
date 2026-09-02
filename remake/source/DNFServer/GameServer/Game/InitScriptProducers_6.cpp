#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_6/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include "CEnvironment.h"   // SpinLock（ORIG 全局类）
#include <map>

#include "CDataManager.h"             // 脚本类型权威布局（stDeathTower_t 等）
#include "sync_script.h"             // STEquipmentPartSetPiece/STEquipmentScript
                                     // （DataManagerInitDepends.h 前置要求）
#include "DataManagerInitDepends.h"   // 本批 import* 声明 + AvatarRoulette 完整视图
#include "DNFLexWrapper.h"            // loadRDARScriptFile/unload/Scan*/ScanTypeOrInt
#include "CEnvironment.h"             // less_TCHAR / SpinLock（getGrowTypeIndex __lookup）

#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

// ============================================================================
// 共享叶子 helper
// ============================================================================

#include "InitScriptProducers_6/ConvertStringToTime.inc"
{
    if (s.size() != 0x13)
    {
        return false;
    }
    struct tm t;
    t.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;  // -0x38（ORIG 首段 sub $0x76c）
    t.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;   // -0x3c
    t.tm_mday = atoi(s.substr(8, 2).c_str());   // -0x40
    t.tm_hour = atoi(s.substr(11, 2).c_str());  // -0x44
    t.tm_min = atoi(s.substr(14, 2).c_str());   // -0x48
    t.tm_sec = atoi(s.substr(17, 2).c_str());   // -0x4c
    // tm_wday/tm_yday/tm_isdst 保持未初始化（与 ORIG 栈槽一致，mktime 归一化）
    out = mktime(&t);
    return true;
}

#include "InitScriptProducers_6/ConvertHHMMSSStringToInt.inc"


#include "InitScriptProducers_6/ConvertWeekdayStringToInt.inc"


#include "InitScriptProducers_6/tokenizeStr.inc"


#include "InitScriptProducers_6/getCharacterJob.inc"

#include "InitScriptProducers_6/makeFairPVPEquipmentPackageKey.inc"

#include "InitScriptProducers_6/getCharacterJobToEnumValue.inc"

// ----------------------------------------------------------------------------
// getGrowTypeIndex —— ORIG T 0x088be63d（0xc7b 字节，定义归本 TU）。
// ORIG 结构（/tmp/getGrowTypeIndex.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x8b5 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 25 条，表尾释放锁、置 __inited 并跳回 guard(088be650) 重新 find 派发；
// 建表后 find 命中按码组 return："none"→0；weaponmaster/nenmaster/ranger/
// elementalmaster/crusader/rogue/elementalbomber(0x8ba..0x8bf,0x8c1)→1；
// soulbringer/striker/launcher/summoner/infighter/necromancer/glacialmaster
// (0x8c7..0x8cc,0x8ce)→2；berserker/streetfighter/mechanic/battlemage/
// exorcist(0x8d4..0x8d8)→3；asura/grappler/spitfire/witch/avenger
// (0x8dd..0x8e1)→4；未命中/未知码 return 0xffff（local_24 入口初值）。
// 键比较为 less_TCHAR（大小写无关），与旧 strcmp 线性链语义不同。
// ----------------------------------------------------------------------------
typedef std::map<const char*, int, less_TCHAR> GrowTypeMap;

#include "InitScriptProducers_6/getGrowTypeIndex.inc"


// ============================================================================
// AvatarRoulette 生命周期（ORIG C1 0x08911b98 / D1 0x0837e484 / clear 0x8911cf6）
// ============================================================================

#include "InitScriptProducers_6/AvatarRoulette.inc"


#include "InitScriptProducers_6/dtor_AvatarRoulette.inc"


#include "InitScriptProducers_6/clear.inc"


#include "InitScriptProducers_6/importAvatarRouletteListDetail.inc"


#include "InitScriptProducers_6/importAvatarRouletteList.inc"


#include "InitScriptProducers_6/importDeathTowerScript.inc"


#include "InitScriptProducers_6/importBloodDungeonScript.inc"


#include "InitScriptProducers_6/importCharacLinkSystemScript.inc"


#include "InitScriptProducers_6/importCharacterCreateGiftScript.inc"


#include "InitScriptProducers_6/importCharacterCreateCreatureGiftScript.inc"
                                             const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == 0)
    {
        return false;
    }

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;                       // ORIG -0x10d
    int curJob = -1;                       // ORIG -0x114
    CharacterCreateCreatureGiftScript::eventJob eventJobItem;   // ORIG -0x11c

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        for (;;)
        {
            // ORIG 重派发落点 = +0x8e（__lookup 的 __cxa_guard 检查起点）：
            // [SemB03 2026-09-12] static 三件套移入内层循环体开头 ⇒ 循环头即
            // guard 检查，`continue`（重派发）落回此处重做 guard 测试（ORIG 形态）。
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            int id;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                id = 0;
            }

            switch (id)
            {
            case 0x277:                      // [event job]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[event job]", 0x277));
                }
                else
                {
                    std::string jobName;    // ORIG -0x124 每轮构造/析构
                    ScanStr(&jobName);
                    curJob = getCharacterJobToEnumValue(jobName);
                    break;
                }
                /* fall through（ORIG +0x342 jmp +0x3a2） */
            case 0x27d:                      // [start]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[start]", 0x27d));
                }
                else
                {
                    ScanInt(&eventJobItem.m_minLevel);   // ORIG -0x11c
                    break;
                }
                /* fall through（+0x3fd jmp +0x412） */
            case 0x281:                      // [end]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[end]", 0x281));
                }
                else
                {
                    ScanInt(&eventJobItem.m_maxLevel);   // ORIG -0x11c+4
                    break;
                }
                /* fall through（+0x46d jmp +0x485） */
            case 0x285:                      // [/event job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event job]", 0x285));
                }
                else
                {
                    s->m_eventJobMap.insert(std::make_pair(curJob, eventJobItem));
                    eventJobItem.m_minLevel = -1;        // ORIG 复位三连
                    eventJobItem.m_maxLevel = -1;
                    curJob = -1;
                    break;
                }
                /* fall through（+0x4e0 jmp +0x567） */
            case 0x28c:                      // [change able creature lev]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[change able creature lev]", 0x28c));
                }
                else
                {
                    ScanInt(&s->m_changeAbleCreatureLev);  // ORIG +0x18
                    break;
                }
                /* fall through（+0x5b6 jmp +0x5cb） */
            case 0x290:                      // [change infinity creature]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[change infinity creature]", 0x290));
                }
                else
                {
                    while (true)
                    {
                        int first = 0;                   // ORIG -0x128
                        int second = 0;                  // ORIG -0x12c
                        first = ScanInt(&ok);
                        if (!ok) break;
                        second = ScanInt(&ok);
                        if (!ok) break;
                        s->m_clientChangeMap.insert(std::make_pair(first, second));
                    }
                    break;
                }
                /* fall through（+0x617 jmp +0x6d5） */
            case 0x2a2:                      // [client infinity creature]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[client infinity creature]", 0x2a2));
                }
                else
                {
                    while (true)
                    {
                        int second = 0;                  // ORIG -0x130（先声明）
                        int first = 0;                   // ORIG -0x134（后声明）
                        first = ScanInt(&ok);
                        if (!ok) break;
                        second = ScanInt(&ok);
                        if (!ok) break;
                        s->m_infinityChangeMap.insert(std::make_pair(first, second));
                    }
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_089607f4 公共尾：注册链走完释放锁并置位，重派发当前
            // 令牌；正常解析态直接 ScanType 下一轮。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return true;                           // ORIG EOF → ebx=1
}

#include "InitScriptProducers_6/importConditionEventChkDungeonScript.inc"


#include "InitScriptProducers_6/importConditionEventScript.inc"


#include "InitScriptProducers_6/importDBLogItem.inc"



#include "InitScriptProducers_6/importDailyScheduleEtcScript.inc"


#include "InitScriptProducers_6/importDimensionActivationData.inc"
                                   DimensionActivationData* out)
{
    // ORIG 实证两张函数级静态表同为 map<char const*,int,less_TCHAR>。
    typedef std::map<const char*, int, less_TCHAR> DimSectionMap;
    typedef std::map<const char*, int, less_TCHAR> DimCoinDiffMap;

    if (out == 0)
    {
        return false;                   // ORIG 0x89160fc 先于 load 判空
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    // [L16 本轮] 槽基对齐试验（P232 两个 char[4] 死局部）：前 99 条槽逐条对齐、
    // ndiff 1181→623，但第 99 条起仍差 8 ⇒ 未闭合且台账四数不变，已回滚（判 N126/N95）。
    // b28d-21: ORIG 08916130/0891613e 两串 ctor 在 load 检查之后（声明位置即
    // ctor 发射点）——旧版声明在判空之前，序言 8 条整体前移。
    std::string token;                  // ORIG -0x19c（节名，ctor 首位）
    std::string sub;                    // ORIG -0x1a0（子串）
    bool ok = false;                    // ORIG -0x1a1（ScanInt 出错 flag，两串后写）
    // ORIG -0x1a8：Ghidra local_1ac 在 prologue 清零，并被 [coin info] used、
    // [section] count、[give group] value 三处**复用**（源码级同一具名变量，
    // 非编译器槽复用）——拆成多个局部会令全帧槽号漂移。
    int v = 0;

    for (;;)                            // ORIG LAB_0891615d 主循环
    {
        if (!ScanType(token, true))
        {
            // P164：`return true` 会被 4.4 inline 到检查点后（多 je+mov+jmp）；
            // 改 break + 循环外 return true 复现 ORIG 尾部共享块（08917387 jne 直达）。
            break;                      // ORIG 0x8917388 EOF → 1
        }
        for (;;)                        // ORIG LAB_0891617e guard 重试
        {
            static DimSectionMap __lookup;      // ORIG ...E8__lookup
            static SpinLock __init_lock;        // ORIG ...E11__init_lock
            static bool __inited = false;       // ORIG ...E8__inited
            int idx;
            // ORIG 08916272 `movzbl __inited; test; je 锁路径`（无 xor）⇒ P168a②
            // 正极性 + 交换分支（`!__inited`/`==0` 均多一条 xor 物化）。
            if (__inited)
            {
                DimSectionMap::const_iterator it = __lookup.find(token.c_str());
                idx = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;           // ORIG 0x891635e 回 guard 重检
                }
                idx = 0;
            }
            switch (idx)
            {
            case 0:
            case 0x3488:                // [play day count]
                // b28d-21: ORIG 089163b5 `movzbl __inited; xor $1; test; je 派发块`
                // ⇒ 原码负极性 `if (!__inited) { insert } else { 派发 }`（P168a②）。
                if (!__inited)
                {
                    __lookup.insert(
                        DimSectionMap::value_type("[play day count]", 0x3488));
                }
                else
                {
                    out->m_field0 = ScanInt(&ok);  // ORIG 不查 flag
                    break;
                }
                /* fall through */
            case 0x348d:                // [coin info]
                // b28d-21: ORIG 0891642a 同形（`movzbl __inited; xor $1; test; je 派发`）。
                if (!__inited)
                {
                    __lookup.insert(DimSectionMap::value_type("[coin info]", 0x348d));
                }
                else
                {
                    for (;;)
                    {
                        if (!ScanStr(&sub))
                        {
                            break;      // ORIG 0x89164a0 回主循环
                        }
                        int coinType = -1;           // ORIG local_1b4 初值
                        DimensionCoinInfo coinInfo;  // ctor 0x891bcb6（查表前）
                        for (;;)                     // ORIG LAB_089164bd
                        {
                            static DimCoinDiffMap __lookup_0;    // ...__lookup_0
                            static SpinLock __init_lock_0;       // ...__init_lock_0
                            static bool __inited_0 = false;      // ...__inited_0
                            int diff;
                            // ORIG 089165b1 同形（movzbl; test; je，无 xor）。
                            if (__inited_0)
                            {
                                DimCoinDiffMap::const_iterator it2 =
                                    __lookup_0.find(sub.c_str());
                                diff = (it2 != __lookup_0.end())
                                           ? it2->second : -1;
                            }
                            else
                            {
                                __init_lock_0.enter();
                                if (__inited_0)
                                {
                                    __init_lock_0.leave();
                                    continue;
                                }
                                diff = 0;
                            }
                            switch (diff)
                            {
                            case 0:
                            case 0x3499:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[easy]", 0x3499));
                                }
                                else
                                {
                                    coinType = 0;
                                    break;
                                }
                                /* fall through */
                            case 0x349e:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[medium]", 0x349e));
                                }
                                else
                                {
                                    coinType = 1;
                                    break;
                                }
                                /* fall through */
                            case 0x34a3:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[hard]", 0x34a3));
                                }
                                else
                                {
                                    coinType = 2;
                                    break;
                                }
                                /* fall through */
                            case 0x34a8:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[ultimate]", 0x34a8));
                                }
                                else
                                {
                                    coinType = 3;
                                    break;
                                }
                            }
                            // b28d-21: ORIG 08916891 `movzbl __inited_0; xor $1;
                            // test; je 读字段` ⇒ 原码负极性（未建表则 leave+置位+回查）。
                            if (!__inited_0)
                            {
                                __init_lock_0.leave();
                                __inited_0 = true;
                                continue;
                            }
                            break;  // 未命中保持 -1，仍走公共尾 insert
                        }
                        coinInfo.m_field0 = ScanInt(&ok);        // -0x1b8
                        v = ScanInt(&ok);                        // ORIG -0x1a8
                        coinInfo.m_field4 = (v != 0) ? 1 : 0;    // setne 字节
                        // ORIG 0x8921dcd/0x8921e14：make_pair<int,DimensionCoinInfo>
                        // （按值实例化）+ value_type 转换 ctor 两级临时。
                        out->m_coinMap.insert(
                            std::map<int, DimensionCoinInfo>::value_type(
                                std::make_pair<int, DimensionCoinInfo>(
                                    coinType, coinInfo)));
                    }
                    break;
                }
                /* fall through */
            case 0x34b7:                // [section]
                if (!__inited)
                {
                    __lookup.insert(DimSectionMap::value_type("[section]", 0x34b7));
                }
                else
                {
                    for (;;)
                    {
                        EventRewardSection section;   // ctor 0x89154ee
                        section.m_field0 =
                            static_cast<unsigned char>(ScanInt(&ok));
                        // b28d-21: ORIG 0x89169e0 `movzbl ok; xor $1; test; je 记录块`
                        // ⇒ 源码为负极性 `if (!ok) break;`。旧写 `if (ok) break;`
                        // 方向反（成功读到记录时反而不读后续字段）——真语义 bug。
                        if (!ok)
                        {
                            break;      // ORIG 0x89169e8 ok 假 → 结束节块
                        }
                        section.m_field1 = ScanInt(static_cast<bool*>(0));
                        section.m_field2 = ScanInt(static_cast<bool*>(0));
                        v = ScanInt(static_cast<bool*>(0));      // ORIG -0x1a8（复用）
                        for (int i = 0; i < v; ++i)
                        {
                            section.m_items.push_back(
                                static_cast<unsigned char>(
                                    ScanInt(static_cast<bool*>(0))));
                        }
                        out->m_rewardData.m_vec0.push_back(section);
                    }
                    break;
                }
                /* fall through */
            case 0x34ca:                // [select group]
                if (!__inited)
                {
                    __lookup.insert(
                        DimSectionMap::value_type("[select group]", 0x34ca));
                }
                else
                {
                    if (!ScanStr(&sub))
                    {
                        return false;   // ORIG 0x8916b3c ebx=0 → 返回
                    }
                    int job = getCharacterJob(sub.c_str());
                    int grade = ScanInt(static_cast<bool*>(0));
                    int packageKey = makeFairPVPEquipmentPackageKey(job, grade);

                    std::vector<std::vector<EventRewardItemInfo> > groups;
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;      // ORIG 0x8916bb2 EOF → 主循环
                        }
                        if (token == "[package selection]")
                        {
                            std::vector<EventRewardItemInfo> items;  // 0x8921bee
                            int first;      // ORIG -0x1e0 无预清（ScanInt 直写）
                            // 注：ORIG 08916bed 条件在头（`call; xor $1; test; jne 退出`）；
                            // for(;;)+break 两形实测 +2 反劣（4.4 仍插前置 jmp）⇒ 维持 while。
                            while (ScanInt(&first))
                            {
                                EventRewardItemInfo info;   // ctor 0x891548a
                                info.m_field0 = first;
                                info.m_short8 =
                                    static_cast<short>(ScanInt(&ok));   // %ax
                                info.m_field4 = ScanInt(&ok);
                                ScanStr(&sub);   // 返回值被忽略（0x8916c45）
                                {
                                    std::string tmp(sub.c_str());
                                    std::vector<std::string> tokens;
                                    std::string delim(",");
                                    tokenizeStr(tmp, tokens, delim);
                                    for (int k = 0;
                                         k < (int)tokens.size(); ++k)
                                    {
                                        int growIdx = getGrowTypeIndex(
                                            tokens.at(k).c_str());
                                        info.m_growTypes.push_back(growIdx);
                                    }
                                }
                                items.push_back(info);
                            }
                            groups.push_back(items);
                        }
                        else if (token == "[/select group]")
                        {
                            // ORIG 0x8921ce9/0x8921d2e：make_pair(int&,vector&)
                            // + value_type 转换 ctor（两级临时析构链）。
                            out->m_rewardData.m_mapC.insert(
                                std::map<int,
                                    std::vector<std::vector<EventRewardItemInfo> >
                                >::value_type(
                                    std::make_pair(packageKey, groups)));
                            break;      // 回主循环
                        }
                        // 其余 token：ORIG 回内层头继续扫描
                    }
                    break;
                }
                /* fall through */
            case 0x34ff:                // [give group]
                if (!__inited)
                {
                    __lookup.insert(
                        DimSectionMap::value_type("[give group]", 0x34ff));
                }
                else
                {
                    if (!ScanStr(&sub))
                    {
                        return false;   // ORIG 0x891700c ebx=0 → 返回
                    }
                    int job = getCharacterJob(sub.c_str());
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;      // ORIG 0x891704d → 主循环
                        }
                        // ORIG 比较顺序：先 [grow type]（0x8917074），未中再查
                        // [/give group]（0x8917345）——operator== 多重集保持一致。
                        if (token == "[grow type]")
                        {
                            if (!ScanStr(&sub))
                            {
                                break;  // ORIG 0x891735a → 主循环
                            }
                            int growIdx = getGrowTypeIndex(sub.c_str());

                            std::vector<std::vector<EventRewardItemInfo> > groups;
                            for (;;)
                            {
                                if (!ScanType(token, true))
                                {
                                    break;  // ORIG 0x89170cf → ~groups 回 give 层
                                }
                                std::vector<EventRewardItemInfo> items;  // 0x8921bee
                                if (token == "[package selection]")
                                {
                                    // ORIG 0x891710f `mov %eax,-0x1c`：返回值落具名
                                    // 死局部（P0 槽往返 ⇒ 原码有该局部）。
                                    int dead20 = ScanInt(&ok);
                                    (void)dead20;
                                    EventRewardItemInfo info;  // ctor 0x891548a 一次
                                    while (ScanInt(&v))     // ORIG -0x1a8（复用）
                                    {
                                        info.m_field0 = v;
                                        info.m_short8 = static_cast<short>(
                                            ScanInt(&ok));               // %ax
                                        info.m_field4 = ScanInt(&ok);
                                        items.push_back(info);
                                        info.clear();                    // 0x89154bc
                                    }
                                    groups.push_back(items);
                                    items.clear();                       // 0x89171b4
                                }
                                else if (token == "[/grow type]")
                                {
                                    // ORIG 0x81b5c8e：key 对默认构造后手写字段，再
                                    // make_pair<std::pair<int,int>, vector>（0x8921e92
                                    // 按值实例化）+ 转换 ctor（0x8921ed6）。
                                    std::pair<int, int> key;
                                    key.first = job;
                                    key.second = growIdx;
                                    out->m_rewardData.m_map24.insert(
                                        std::map<std::pair<int, int>,
                                            std::vector<std::vector<EventRewardItemInfo> >
                                        >::value_type(
                                            std::make_pair<std::pair<int, int>,
                                                std::vector<
                                                    std::vector<EventRewardItemInfo> >
                                            >(key, groups)));
                                    groups.clear();                      // 0x89172c5
                                    break;  // 回 give 层
                                }
                                // 其余 token：继续内层（items 已构造无副作用）
                            }
                        }
                        else if (token == "[/give group]")
                        {
                            break;      // ORIG 0x8917352 → 主循环
                        }
                        // 其余 token：ORIG 0x891734c → give 层继续
                    }
                    break;
                }
            }
            // b28d-21: ORIG 0891735b `movzbl __inited; xor $1; test; je 回主循环`
            // ⇒ 原码负极性（未建表则 leave+置位+回 guard 头重派发）。
            if (!__inited)
            {
                __init_lock.leave();        // ORIG 0x8917369 建表收尾重派发
                __inited = true;
                continue;
            }
            break;                  // 派发完成 → 主循环下一 token
        }
    }
    return true;                        // ORIG 0x8917388 EOF → 1（尾部共享块）
}

#include "InitScriptProducers_6/importEmotionTypeTagsScript.inc"


#include "InitScriptProducers_6/importEquipmentGradeSampleInfo.inc"

