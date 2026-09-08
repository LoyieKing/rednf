// ============================================================================
// df_game_r 还原 —— ScriptDepsB.cpp（脚本装载依赖 B 组，独占 TU）
//
// 逐报告转写自 docs/class_func_reports/_lt_global_gt_/ 对应 md
// （## 汇编 + ## 反编译 C 全文已核对）：
//   importEtcParameterScriptForServer  0x080966fa  ../RDARScriptEtcParameter.cpp 域
//   importGenuineUpgradeTable          0x08912336  同上
//   importSpPerLevelReferenceTable     0x08910505
//   loadAppendageType                  0x0889253b
//   loadValueType                      0x08892ac4
//   getTitleCollection                 0x08aa94e1
//   importRewards                      0x08aa8b79  （../RDARScriptTitle.cpp 域）
//   importTitleSection                 0x08aa8d65  （同上；三函数级静态）
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

// ============================================================================
// loadValueType —— ORIG 0x08892ac4（_Z13loadValueTypeRKSs）
// "+"=0、"%"=1、其他=2。
// ============================================================================
int loadValueType(const std::string& token)
{
    if (token == "+")
        return 0;
    if (token == "%")
        return 1;
    return 2;
}

// ============================================================================
// loadAppendageType —— ORIG 0x0889253b（_Z17loadAppendageTypeRKSs）
// 纯字符串→枚举映射链；比较顺序与 ORIG 级联完全一致，未匹配返回 -1。
// ============================================================================
int loadAppendageType(const std::string& token)
{
    if (token == "[physical attack]")                  return 0x00;
    if (token == "[magical attack]")                   return 0x01;
    if (token == "[physical defense]")                 return 0x03;
    if (token == "[magical defense]")                  return 0x02;
    if (token == "[equipment physical attack]")        return 0x04;
    if (token == "[equipment physical defense]")       return 0x05;
    if (token == "[jump power]")                       return 0x08;
    if (token == "[attack speed]")                     return 0x0a;
    if (token == "[move speed]")                       return 0x0b;
    if (token == "[cast speed]")                       return 0x0c;
    if (token == "[hp max]")                           return 0x0d;
    if (token == "[hp regen rate]")                    return 0x0e;
    if (token == "[magical critical hit rate]")        return 0x10;
    if (token == "[physical critical hit rate]")       return 0x0f;
    if (token == "[physical back attack critical rate]") return 0x11;
    if (token == "[magical back attack critical rate]")  return 0x12;
    if (token == "[fire element]")                     return 0x14;
    if (token == "[water element]")                    return 0x15;
    if (token == "[dark element]")                     return 0x16;
    if (token == "[light element]")                    return 0x17;
    if (token == "[stuck]")                            return 0x1b;
    if (token == "[mp regen rate]")                    return 0x1d;
    if (token == "[hit recovery]")                     return 0x1e;
    if (token == "[equipment magical attack]")         return 0x1f;
    if (token == "[equipment magical defense]")        return 0x20;
    if (token == "[good hit]")                         return 0x21;
    if (token == "[all active status resistance]")     return 0x1a;
    if (token == "[all element]")                      return 0x18;
    if (token == "[fire element attack]")              return 0x2a;
    if (token == "[water element attack]")             return 0x2b;
    if (token == "[dark element attack]")              return 0x2c;
    if (token == "[light element attack]")             return 0x2d;
    if (token == "[all element attack]")               return 0x2e;
    if (token == "[mp max]")                           return 0x30;
    if (token == "[separate attack]")                  return 0x35;
    if (token == "[antievil]")                         return 0x37;
    if (token == "[physical absolute damage]")         return 0x38;
    if (token == "[village moving speed]")             return 0x3f;
    if (token == "[magical absolute damage]")          return 0x3c;
    if (token == "[physical absolute defense]")        return 0x3d;
    if (token == "[magical absolute defense]")         return 0x3e;
    if (token == "[physical attack bonus]")            return 0x06;
    if (token == "[magical attack bonus]")             return 0x07;
    return -1;
}

// ============================================================================
// importEtcParameterScriptForServer —— ORIG 0x080966fa
// 报告 _lt_global_gt_/importEtcParameterScriptForServer.md：
//   loadRDARScriptFile("", "Etc/EtcParameter.etc") 失败→false；
//   do { ScanType(token,false); } while (token != "[damage fomula constant]");
//   随后按序 ScanFloat 写入 26 个 g_*Const_ 全局 → true。
//   （ORIG 另构造一个未使用局部 string local_20，无语义，略。）
// ============================================================================
// [b28d-32 N(P75/N89): 扫描器内联与装载序差（144/125） | ORIG 0x080966fa]
bool importEtcParameterScriptForServer(void)
{
    if (!loadRDARScriptFile("", "Etc/EtcParameter.etc"))
        return false;

    static const char* const kTarget = "[damage fomula constant]";
    std::string token;
    do
    {
        ScanType(token, false);
    } while (!(token == kTarget));

    g_physicalDamageRateConst_            = ScanFloat((bool*)0);
    g_physicalDamageAddConst_             = ScanFloat((bool*)0);
    g_physicalReduceRateConst_            = ScanFloat((bool*)0);
    g_physicalReduceAddConst_             = ScanFloat((bool*)0);
    g_physicalRandomBaseConst_            = ScanFloat((bool*)0);
    g_physicalRandomRateConst_            = ScanFloat((bool*)0);
    g_magicalDamageRateConst_             = ScanFloat((bool*)0);
    g_magicalDamageAddConst_              = ScanFloat((bool*)0);
    g_magicalReduceRateConst_             = ScanFloat((bool*)0);
    g_magicalReduceAddConst_              = ScanFloat((bool*)0);
    g_magicalRandomBaseConst_             = ScanFloat((bool*)0);
    g_magicalRandomRateConst_             = ScanFloat((bool*)0);
    g_commonElementBaseConst_             = ScanFloat((bool*)0);
    g_commonElementRateConst_             = ScanFloat((bool*)0);
    g_commonElementRateConstInPVP_        = ScanFloat((bool*)0);
    g_commonElementRateConstInLadderPVP_  = ScanFloat((bool*)0);
    g_hpIncreaseRateConst_                = ScanFloat((bool*)0);
    g_hpRegenRateConst_                   = ScanFloat((bool*)0);
    g_mpIncreaseRateConst_                = ScanFloat((bool*)0);
    g_mpRegenRateConst_                   = ScanFloat((bool*)0);
    g_activeStatusDamageRateConst_        = ScanFloat((bool*)0);
    g_activeStatusDefenseRateConst_       = ScanFloat((bool*)0);
    g_physicalDefenseAddConst_            = ScanFloat((bool*)0);
    g_magicalDefenseAddConst_             = ScanFloat((bool*)0);
    g_hpRegenRateAddRateConst_            = ScanFloat((bool*)0);
    g_mpRegenRateAddRateConst_            = ScanFloat((bool*)0);
    return true;
}

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

// ============================================================================
// importRewards —— ORIG 0x08aa8b79
// 报告 _lt_global_gt_/importRewards.md。appendage 命中→stRewardChangeStatus；
// 否则 [give emotion]→stRewardGiveEmotion；[send postal]→stRewardPostal；
// 其余令牌无副作用。恒返回 true（eax 流出）。
// 扫描顺序逐条对齐汇编：change=status 先存 appendage 再 Str/ValueType/Float；
// postal 先两个 int(+0x18/+0x1c) 后三个 string(+0xc/+0x10/+0x14)。
// ============================================================================
// [b28d-32 N7: stTitleRewardBasis D1 @0x08aa9652 间接虚调目标 ORIG %edx vs 4.4.7 恒 %eax（P230：4.4.6 后端可翻，路由表不改）]
int importRewards(std::vector<stTitleRewardBasis*>* out, std::string token)
{
    const int appendage = loadAppendageType(token);
    if (appendage != -1)
    {
        stRewardChangeStatus* reward = new stRewardChangeStatus();
        reward->m_appendage = appendage;                    // +0x0c
        std::string typeText;
        ScanStr(&typeText);
        reward->m_valueType = loadValueType(typeText);      // +0x10
        reward->m_value = ScanFloat((bool*)0);              // +0x14（fstps）
        out->push_back(reward);
        return 1;
    }

    if (token == "[give emotion]")
    {
        stRewardGiveEmotion* reward = new stRewardGiveEmotion();
        reward->m_emotionId = ScanInt((bool*)0);            // +0x0c
        out->push_back(reward);
        return 1;
    }

    if (token == "[send postal]")
    {
        stRewardPostal* reward = new stRewardPostal();
        reward->m_itemIdx = ScanInt((bool*)0);              // +0x18
        reward->m_addInfo = ScanInt((bool*)0);              // +0x1c
        ScanStr(&reward->m_sender);                         // +0x0c
        ScanStr(&reward->m_subject);                        // +0x10
        ScanStr(&reward->m_message);                        // +0x14
        out->push_back(reward);
    }
    return 1;
}

// ============================================================================
// importTitleSection —— ORIG 0x08aa8d65
// 报告 _lt_global_gt_/importTitleSection.md。三个函数级静态（ORIG 经
// __cxa_guard 初始化）：__lookup 键表 / __init_lock 自旋锁 / __inited 标志。
// 首个令牌触发级联登记（[/section]0x155→[/reward type]0x158→[section name]
// 0x15c→[section icon]0x15f→[frame type]0x166→[lock message]0x169→[name]
// 0x16e→[image path]0x172→[effect path]0x179），置位 __inited 后以当前令牌
// 重新查表派发。查表命中标签则解析对应字段；未命中即奖励行 → importRewards。
// 返回：ScanType 耗尽→true；[/reward type]→true；[/section]→false。
// ============================================================================
// [b28d-32 N(P75/N89): static map 惰性注册链与装载序差（461/475） | ORIG 0x08aa8d65]
bool importTitleSection(stTitleSection* section, int idx)
{
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    static LookupMap __lookup;      // ORIG _ZZ18importTitleSection..8__lookup
    static SpinLock __init_lock;     // ORIG ..11__init_lock
    static bool __inited = false;   // ORIG ..8__inited

    std::string token;

    section->m_startIdx = idx * 10;                    // +0x0c
    section->m_endIdx = section->m_startIdx + 10;      // +0x10

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

        int id;
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                // 双检竞争失败方：解锁后按查表路径重新派发当前令牌。
                __init_lock.leave();
                LookupMap::iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                // 首遍级联登记（顺序与 ORIG 插入次序一致）。
                __lookup.insert(std::make_pair("[/section]", 0x155));
                __lookup.insert(std::make_pair("[/reward type]", 0x158));
                __lookup.insert(std::make_pair("[section name]", 0x15c));
                __lookup.insert(std::make_pair("[section icon]", 0x15f));
                __lookup.insert(std::make_pair("[frame type]", 0x166));
                __lookup.insert(std::make_pair("[lock message]", 0x169));
                __lookup.insert(std::make_pair("[name]", 0x16e));
                __lookup.insert(std::make_pair("[image path]", 0x172));
                __lookup.insert(std::make_pair("[effect path]", 0x179));
                __init_lock.leave();
                __inited = true;
                // 以当前令牌重新走查表派发（ORIG jmp +0x72）。
                LookupMap::iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
        }
        else
        {
            LookupMap::iterator it = __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }

        switch (id)
        {
        case 0x155:                                    // [/section]
            return false;
        case 0x158:                                    // [/reward type]
            return true;
        case 0x15c:                                    // [section name]
            ScanStr(&section->m_sectionName);           // +0x04
            break;
        case 0x15f:                                    // [section icon]
            ScanStr(&section->m_icon);                  // +0x24
            section->m_iconId = ScanInt((bool*)0);      // +0x28
            break;
        case 0x166:                                    // [frame type]
            section->m_frameType = ScanInt((bool*)0);   // +0x2c
            break;
        case 0x169:                                    // [lock message]
            ScanStr(&section->m_lockMessage);           // +0x30
            break;
        case 0x16e:                                    // [name]
            ScanStr(&section->m_name);                  // +0x14
            break;
        case 0x172:                                    // [image path]
            ScanStr(&section->m_imagePath);             // +0x18
            section->m_imageParam = ScanInt((bool*)0);  // +0x1c
            break;
        case 0x179:                                    // [effect path]
            ScanStr(&section->m_effectPath);            // +0x20
            break;
        default:
            break;
        }

        // ORIG +0x6a3：__inited 后每个令牌（含标签）都会进入 importRewards；
        // 标签令牌因 loadAppendageType==-1 且非 emotion/postal 而无副作用。
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

// [b28d-32 N(P75/N89): 扫描器/STL 内联与装载序差（93/84） | ORIG 0x08910505]
int importSpPerLevelReferenceTable(const char* path, stSpPerLevelTable& table)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
        return 0x2948;

    while (ScanType(token, true))
    {
        if (token == "[sp table]")
        {
            table.reserve(200);
            unsigned int initial = 0;
            table.push_back(initial);                  // 等级 0 占位
            for (;;)
            {
                ScanInt(&ok);                          // 等级（丢弃，索引即位）
                if (!ok)
                    break;
                unsigned int value =
                    static_cast<unsigned int>(ScanInt(&ok));
                if (!ok)
                    break;
                table.push_back(value);
            }
        }
    }
    return 0;
}

// ============================================================================
// importGenuineUpgradeTable —— ORIG 0x08912336（../RDARScriptEtcParameter.cpp）
// 报告 _lt_global_gt_/importGenuineUpgradeTable.md。节分派：
//   [table]                    m_infos 清空后逐行 {int,int,int,level/10000f,float}
//                              以自增键插入 +0x158；
//   [separate upgrade max]     ScanInt(+0x004)，失败 logFormat(0x2f9e)；
//   [level]                    ScanInt(+0x000)，失败 logFormat(0x2fa5)；
//   [item weights by grade]    {key,int,int} 插入 +0x140；
//   [item weights by rarity]   6×float → +0x008 区（失败 logFormat(0x2fbe) 续扫）；
//   [stat weights by rarity]   6×float → +0x020 区（logFormat(0x2fc9)）；
//   [weapon weights]           11×6 float → +0x038 区（logFormat(0x2fd5)）；
//   [separate upgrade effect]  行循环 {r,g,b}→RGBA 打包入 +0x04，再
//                              {+0xc,+8,+0} 三 int，自增键插入 +0x170。
// loadRDARScriptFile 失败→false；其余路径均 true。
// ============================================================================
// [b28d-32 N(P75/N89): 大解析函数扫描器内联/装载序差（448/416） | ORIG 0x08912336]
int importGenuineUpgradeTable(const char* path,
                              GenuineDamageUpgradeTable* table)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    int level = 0;

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        if (token == "[table]")
        {
            int key = 1;
            table->m_infos.clear();
            for (;;)
            {
                GenuineDamageUpgradeInfo info;
                if (!ScanInt(&info.m_a))
                    break;
                if (!ScanInt(&info.m_b))
                    break;
                if (!ScanInt(&level))
                    break;
                info.m_levelRate =
                    static_cast<float>(level) / 10000.0f;
                if (!ScanFloat(&info.m_goldRate))
                    break;
                table->m_infos.insert(
                    std::make_pair(key, info));
                ++key;
            }
            continue;
        }

        if (token == "[separate upgrade max]")
        {
            if (!ScanInt(&table->m_maxLevel))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2f9e,
                    "importGenuineUpgradeTable() - [separate_upgrade max] false");
            }
            continue;
        }

        if (token == "[level]")
        {
            if (!ScanInt(&table->m_divisor))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2fa5,
                    "importGenuineUpgradeTable() - [level] false");
            }
            continue;
        }

        if (token == "[item weights by grade]")
        {
            for (;;)
            {
                int key = 0;
                GenuineDamageUpgradeMaterialInfo material;
                if (!ScanInt(&key))
                    break;
                if (!ScanInt(&material.m_materialItemIdx))
                    break;
                if (!ScanInt(&material.m_materialCount))
                    break;
                table->m_materials.insert(
                    std::make_pair(key, material));
            }
            continue;
        }

        if (token == "[item weights by rarity]")
        {
            for (int i = 0; i < 6; ++i)
            {
                if (!ScanFloat(&table->m_itemWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fbe,
                        "importGenuineUpgradeTable() - [item weights by rarity] false");
                }
            }
            continue;
        }

        if (token == "[stat weights by rarity]")
        {
            for (int i = 0; i < 6; ++i)
            {
                if (!ScanFloat(&table->m_statWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fc9,
                        "importGenuineUpgradeTable() - [stat weights by rarity] false");
                }
            }
            continue;
        }

        if (token == "[weapon weights]")
        {
            for (int a = 0; a < 11; ++a)
            {
                for (int b = 0; b < 6; ++b)
                {
                    if (!ScanFloat(&table->m_weaponWeights[a][b]))
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp",
                            __FUNCTION__, 0x2fd5,
                            "importGenuineUpgradeTable() - [weapon weights] false");
                    }
                }
            }
            continue;
        }

        if (token == "[separate upgrade effect]")
        {
            int key = 1;
            for (;;)
            {
                OutLineEFFECTInfoTemp effect;
                int r = 0;
                int g = 0;
                int b = 0;
                if (!ScanInt(&r))
                    break;
                if (!ScanInt(&g))
                    break;
                if (!ScanInt(&b))
                    break;
                // ORIG：(r&ff)|((g&ff)<<8)|((b&ff)<<16)|0xff000000 → +0x04。
                effect.m_b = static_cast<int>(
                    (static_cast<unsigned>(r) & 0xffu) |
                    ((static_cast<unsigned>(g) & 0xffu) << 8) |
                    ((static_cast<unsigned>(b) & 0xffu) << 16) |
                    0xff000000u);
                if (!ScanInt(&effect.m_d))              // +0x0c
                    break;
                if (!ScanInt(&effect.m_c))              // +0x08
                    break;
                if (!ScanInt(&effect.m_a))              // +0x00
                    break;
                table->m_effects.insert(
                    std::make_pair(key, effect));
                ++key;
            }
            continue;
        }
    }

    return true;
}
