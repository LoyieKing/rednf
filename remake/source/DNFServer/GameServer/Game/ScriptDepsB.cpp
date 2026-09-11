#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScriptDepsB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ScriptDepsB.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"           // stTitleBookInfo（m_map18 权威布局）
#include "CEnvironment.h"           // less_TCHAR / SpinLock
#include "CItemUpgrade_Separate.h"  // GenuineDamageUpgradeTable 及元素 POD
#include "CTitleBook.h"             // stTitleSection / stTitleRewardBasis 族
#include "DNFLexWrapper.h"          // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"

// ============================================================================
// Etc/EtcParameter.etc 常量定义（ORIG BSS：g_physicalDamageRateConst_ 等，
// nm 实测 B 0x093f5d10..0x093f5d83；本 TU 为唯一定义点）
// ============================================================================
float g_physicalDamageRateConst_;
float g_physicalDamageAddConst_;
float g_physicalReduceRateConst_;
float g_physicalReduceAddConst_;
float g_physicalRandomBaseConst_;
float g_physicalRandomRateConst_;
float g_magicalDamageRateConst_;
float g_magicalDamageAddConst_;
float g_magicalReduceRateConst_;
float g_magicalReduceAddConst_;
float g_magicalRandomBaseConst_;
float g_magicalRandomRateConst_;
float g_commonElementBaseConst_;
float g_commonElementRateConst_;
float g_commonElementRateConstInPVP_;
float g_commonElementRateConstInLadderPVP_;
float g_hpIncreaseRateConst_;
float g_hpRegenRateConst_;
float g_mpIncreaseRateConst_;
float g_mpRegenRateConst_;
float g_activeStatusDamageRateConst_;
float g_activeStatusDefenseRateConst_;
float g_physicalDefenseAddConst_;
float g_magicalDefenseAddConst_;
float g_hpRegenRateAddRateConst_;
float g_mpRegenRateAddRateConst_;

#include "ScriptDepsB/loadValueType.inc"


#include "ScriptDepsB/loadAppendageType.inc"


#include "ScriptDepsB/importEtcParameterScriptForServer.inc"


// ============================================================================
// getTitleCollection —— ORIG 0x08aa94e1（_Z18getTitleCollectionP15stTitleBookInfoi）
// find(tab)；== end → 返回 0；否则返回 &m_map18[tab]。
// ============================================================================
std::vector<stTitleElement>* getTitleCollection(stTitleBookInfo* book, int tab)
{
    // b28c-l23 根因（P0/P183）：ORIG 无 map 引用局部槽——三处成员访问都是
    // `mov 0x8(%ebp),%eax; lea 0x18(%eax),%eX` 就地折叠寻址（@0x08aa94f1/0x08aa9508/
    // 0x08aa953b）；我方具名 `collection` 引用使 4.4 把基址落槽（-0xc）并在每次
    // 实参装载时回读，槽号整体平移（-0x10→-0x14、-0xc→-0x10）。改为逐点直写表达式。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        book->m_map18.find(tab);
    if (it == book->m_map18.end())
        return 0;
    return &book->m_map18[tab];
}

#include "ScriptDepsB/importRewards.inc"


#include "ScriptDepsB/importTitleSection.inc"
{
    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    std::string deadToken;          // ORIG -0xec（仅 ctor/dtor，P171 照抄）

    section->m_startIdx = idx * 10;                    // +0x0c
    section->m_endIdx = section->m_startIdx + 10;      // +0x10

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

    dispatch:                       // ORIG +0x72：重派发当前令牌（不重扫）
        static LookupMap __lookup;      // ORIG _ZZ18importTitleSection..8__lookup
        static SpinLock __init_lock;     // ORIG ..11__init_lock
        static bool __inited = false;   // ORIG ..8__inited

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
                // 双检竞争失败方：解锁后重走查表（当前令牌，不重扫）。
                __init_lock.leave();
                goto dispatch;
            }
            id = 0;                     // 首遍：id=0 驱动级联注册链
        }

        switch (id)
        {
        case 0:
        case 0x155:                                    // [/section]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/section]", 0x155));
            else
                return false;
            // fall through（注册链）
        case 0x158:                                    // [/reward type]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/reward type]", 0x158));
            else
                return true;
            // fall through
        case 0x15c:                                    // [section name]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[section name]", 0x15c));
            else
            {
                ScanStr(&section->m_sectionName);       // +0x04
                break;
            }
            // fall through
        case 0x15f:                                    // [section icon]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[section icon]", 0x15f));
            else
            {
                ScanStr(&section->m_icon);              // +0x24
                section->m_iconId = ScanInt((bool*)0);  // +0x28
                break;
            }
            // fall through
        case 0x166:                                    // [frame type]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[frame type]", 0x166));
            else
            {
                section->m_frameType = ScanInt((bool*)0);  // +0x2c
                break;
            }
            // fall through
        case 0x169:                                    // [lock message]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[lock message]", 0x169));
            else
            {
                ScanStr(&section->m_lockMessage);       // +0x30
                break;
            }
            // fall through
        case 0x16e:                                    // [name]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[name]", 0x16e));
            else
            {
                ScanStr(&section->m_name);              // +0x14
                break;
            }
            // fall through
        case 0x172:                                    // [image path]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[image path]", 0x172));
            else
            {
                ScanStr(&section->m_imagePath);         // +0x18
                section->m_imageParam = ScanInt((bool*)0);  // +0x1c
                break;
            }
            // fall through
        case 0x179:                                    // [effect path]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[effect path]", 0x179));
            else
            {
                ScanStr(&section->m_effectPath);        // +0x20
                break;
            }
            // fall through（注册链尾）
        default:
            break;
        }

        // ORIG +0x6a3 公共尾：注册链上 leave+置位并回循环头重派发；
        // __inited 后每个令牌（含标签）都会进入 importRewards。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto dispatch;
        }

        importRewards(&section->m_rewards, token);
    }
}

// ============================================================================
// importSpPerLevelReferenceTable —— ORIG 0x08910505
// 报告 _lt_global_gt_/importSpPerLevelReferenceTable.md。
// stSpPerLevelTable 在 ORIG 中为空壳派生（W ctor/dtor 与 vector<unsigned> 一致，
// sizeof 0xc）；此处与 CDataManagerMethods.cpp 保持逐 token 相同的局部定义以
// 产生一致的 mangled 形参 R17stSpPerLevelTable。
// 语义：loadRDARScriptFile("", path) 失败→0x2948；ScanType 循环中命中
// "[sp table]" 段则 reserve(200)、先压入 0 号项，随后每行 ScanInt 两枚——
// 仅第二枚入表（首枚为等级位），任一枚失败回到外层令牌循环。成功返回 0。
// ============================================================================
struct stSpPerLevelTable : public std::vector<unsigned int> {};

#include "ScriptDepsB/importSpPerLevelReferenceTable.inc"


#include "ScriptDepsB/importGenuineUpgradeTable.inc"

