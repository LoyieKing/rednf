// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventRewardQuickPartyB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include <string>

#include "CDataManager.h"             // eventReward / itemData
#include "CQuickPartyRewardManager.h" // namespace QuickParty / QuickPartyData / import 声明
#include "CEnvironment.h"             // less_TCHAR / SpinLock
#include "DNFLexWrapper.h"            // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "StrikerScript.h"            // ContentsType

// ===========================================================================
// 1) eventReward 解析 helper 族（ORIG 非静态成员；this 不参与逻辑）
// ---------------------------------------------------------------------------
// itemData 真实布局见 itemData.h（+0/+1/+2 三字节 + +0x04/+0x08 int，
// sizeof 0x0c）；ORIG 以 movb/movzbl 访问 +0/+1/+2 字节、movl 访问 +4/+8。
// ===========================================================================


#include "EventRewardQuickPartyB2/getSubKeyType.inc"


#include "EventRewardQuickPartyB2/getSubKeyAction.inc"


#include "EventRewardQuickPartyB2/getOptionType.inc"


#include "EventRewardQuickPartyB2/defaultOption.inc"


// ===========================================================================
// 2) ContentsType（ORIG 全局类，静态成员）
// ===========================================================================

#include "EventRewardQuickPartyB2/getString.inc"


#include "EventRewardQuickPartyB2/getType.inc"


// ===========================================================================
// 3) QuickParty 脚本导入族
// ===========================================================================

namespace QuickParty
{

namespace
{

// importQuickPartyScript 段名 id（ORIG __lookup 表值实证）。
enum
{
    QPSEC_QUICK_PARTY_END    = 0x3e,   // [/quick party]
    QPSEC_UNABLE             = 0x41,   // [unable to quickparty]
    QPSEC_LEVEL_SECTION      = 0x4d,   // [level section]
    QPSEC_BASIC_REWARD       = 0x59,   // [basic reward]
    QPSEC_RANDOM_REWARD      = 0x8f,   // [random reward]
    QPSEC_RANDOM_REWARD_PROB = 0xb8,   // [random reward prob]
    QPSEC_REQUIRE_PARTY_NUM  = 0xce    // [require party num]
};

}  // unnamed namespace

// ORIG T 0x08a6e100：脚本 buff 名 → RandomBuffType；未知 → 11（0xb）。
// [SplitC 2026-09-11 batch4 · TU 迁移] 定义已迁出至 RDARScriptQuickParty.cpp（ORIG GI 键对齐）；
//   本 TU 调用点保留，取声明引用（namespace QuickParty 内）。
RandomBuffType getRandomBuffTypeByName(const std::string& name);   // 定义见 RDARScriptQuickParty.cpp

#include "EventRewardQuickPartyB2/importQuickPartyScript.inc"
{
    if (!loadRDARScriptFile("", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    token.clear();                                    // ORIG：C1 后紧跟 Ss::clear
    std::string value;
    value.clear();
    bool ok = false;
    int lv = 0;                                       // ORIG -0xf8：全函数复用
    LevelData entry;                                  // ORIG -0x100：函数级，两节复用

    int id;

    for (; ScanType(token, true); )                   // P6S6: 主扫描失败 → false
    {
    re_lookup:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

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
                goto re_lookup;                       // 双检命中：重查表
            }
            id = 0;                                   // 首遍哨兵 → 注册链
        }

        switch (id)
        {
        case 0:                                       // 首遍注册哨兵（与 0x3e 同块）
        case QPSEC_QUICK_PARTY_END:                   // [/quick party] → 成功
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type(
                    "[/quick party]", (int)QPSEC_QUICK_PARTY_END));
            }
            else
            {
                return true;
            }
            // 首遍：继续登记下一段

        case QPSEC_UNABLE:                            // 单扫描：值即入列元素
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type(
                    "[unable to quickparty]", (int)QPSEC_UNABLE));
            }
            else
            {
                for (;;)
                {
                    lv = ScanInt(&ok);
                    if (!ok)
                        break;
                    data->m_dungeonIdxList.push_back(lv);
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_LEVEL_SECTION:                     // 单扫描：值即入列元素
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type(
                    "[level section]", (int)QPSEC_LEVEL_SECTION));
            }
            else
            {
                for (;;)
                {
                    lv = ScanInt(&ok);
                    if (!ok)
                        break;
                    data->m_vec00.push_back(lv);
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_BASIC_REWARD:
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type(
                    "[basic reward]", (int)QPSEC_BASIC_REWARD));
            }
            else
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                        break;
                    if (token == "[/basic reward]")
                        break;

                    if (token == "[increase gold card]")
                    {
                        for (;;)                      // 门扫描弃值，体读「档」
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entry.m_value4 = lv;
                            ok = ScanStr(&value);
                            entry.m_field0 = (ok && value == "%") ? 1 : 0;
                            data->m_basicReward.m_levelDatas.push_back(entry);
                        }
                    }
                    else if (token == "[hell ratio]")
                    {
                        ok = ScanStr(&value);
                        if (!ok)
                            break;                    // ORIG：退出节回主循环
                        lv = ScanInt((bool*)0);
                        data->m_basicReward.m_field18 = lv;   // +0x30
                        lv = ScanInt((bool*)0);
                        data->m_basicReward.m_field1c = lv;   // +0x34
                        LevelDataHell entryHell;
                        for (;;)                      // 门扫描弃值，体读两 int
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entryHell.m_field0 = lv;
                            lv = ScanInt((bool*)0);
                            entryHell.m_value4 = lv;
                            data->m_basicReward.m_hellDatas.push_back(entryHell);
                        }
                    }
                    // 其余令牌：忽略并继续节内扫描
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_RANDOM_REWARD:
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type(
                    "[random reward]", (int)QPSEC_RANDOM_REWARD));
            }
            else
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                        break;
                    if (token == "[/random reward]")
                        break;

                    if (token == "[reward]")
                    {
                        ok = ScanStr(&value);
                        if (!ok)
                            break;
                        LevelDataRandomReward lrr;    // ctor：+0=0xb、串/向量清空
                        lrr.m_field0 = getRandomBuffTypeByName(value);
                        ScanStr(&lrr.m_name);         // 返回值被忽略（ORIG 同）

                        for (;;)                      // 门扫描弃值，体读「档」
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entry.m_value4 = lv;
                            ok = ScanStr(&value);
                            entry.m_field0 = (ok && value == "%") ? 1 : 0;
                            lrr.m_levelDatas.push_back(entry);
                        }

                        // ORIG：value_type 引用构造（非 make_pair）
                        data->m_randomRewards.insert(
                            std::map<RandomBuffType, LevelDataRandomReward>::value_type(
                                lrr.m_field0, lrr));
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_RANDOM_REWARD_PROB:
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type(
                    "[random reward prob]", (int)QPSEC_RANDOM_REWARD_PROB));
            }
            else
            {
                for (;;)
                {
                    ok = ScanStr(&value);
                    if (!ok)
                        break;
                    const int idx = (int)getRandomBuffTypeByName(value);
                    if (idx > 10)                     // ORIG jle 实证上界
                        continue;
                    for (int i = 0; i < 7; ++i)
                    {
                        lv = ScanInt((bool*)0);
                        data->m_probs[idx][i] = lv;
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_REQUIRE_PARTY_NUM:
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type(
                    "[require party num]", (int)QPSEC_REQUIRE_PARTY_NUM));
            }
            else
            {
                while (ScanStr(&value))
                {
                    int dungeonType = -1;
                    if (value == "[normal]")       dungeonType = 0;
                    else if (value == "[ancient]") dungeonType = 1;
                    else break;

                    if (!ScanStr(&value))
                        break;

                    int diff = -1;
                    if      (value == "[easy]")     diff = 0;
                    else if (value == "[medium]")   diff = 1;
                    else if (value == "[hard]")     diff = 2;
                    else if (value == "[ultimate]") diff = 3;
                    else break;

                    if (!ScanInt(&lv))
                        break;

                    // ORIG 双边界检查（js/jg 实证）：type∈{0,1}、diff∈{0..4}
                    if (dungeonType >= 0 && dungeonType <= 1 &&
                        diff >= 0 && diff <= 4)
                    {
                        data->m_minCounts[dungeonType][diff] = lv;
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        default:                                      // 未知令牌（id==-1）：跳过
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto re_lookup;                           // 首遍登记完 → 重查表
        }
    }

    return ok;
}

}  // namespace QuickParty
