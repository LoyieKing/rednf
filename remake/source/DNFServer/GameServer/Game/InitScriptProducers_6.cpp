// ============================================================================
// InitScriptProducers_6 —— CDataManager::init（ORIG 0x08358332..0x0835f1d5，
// docs/class_func_reports/CDataManager/init.md）依赖的脚本装载自由函数·第 6 批。
//
// 本 TU 独家提供下列 ORIG T 符号的真实定义（逐指令转写自 df_game_r，
// 各函数 ORIG 地址见实现处注释；节名键串取自 .rodata 实测）：
//   importAvatarRouletteList            0x089116eb (+ Detail 0x089115b1)
//   AvatarRoulette::{C1,D1,clear}       0x08911b98 / 0x0837e484 / 0x08911cf6
//   importBloodDungeonScript            0x08909016
//   importCharacLinkSystemScript        0x08909529
//   importCharacterCreateCreatureGiftScript 0x0896001a
//   importCharacterCreateGiftScript     0x0895eda1
//   importConditionEventChkDungeonScript 0x08913891
//   importConditionEventScript          0x0890e868
//   importDBLogItem                     0x0890dbba
//   importDailyScheduleEtcScript        0x0891826c
//   importDeathTowerScript              0x089089f3
//   importDimensionActivationData       0x089160f0
//   importEmotionTypeTagsScript         0x0890de85
//   importEquipmentGradeSampleInfo      0x0890e18d
// 及其共享叶子依赖（ORIG 均为 T，源码此前无任何定义，已广播认领）：
//   ConvertStringToTime                 0x088c2341
//   ConvertHHMMSSStringToInt            0x088c25df
//   ConvertWeekdayStringToInt           0x088c284a
//   getCharacterJob                     0x088be57e
//   getGrowTypeIndex                    0x088be63d
//   makeFairPVPEquipmentPackageKey      0x088945ad
//   tokenizeStr                         0x088945e2
//   getCharacterJobToEnumValue          0x089bbc0f
//
// 统一模式：loadRDARScriptFile("", path) → ScanType 分派节名 → ScanInt/
// ScanFloat/ScanStr 字段回填。ORIG 中 string→分支 的静态 map<const char*,int>
// （__lookup/__init_lock/__inited BSS 桩）是编译器对「按行号分派」的缓存 lowering，
// 观测语义等价于顺序串比较——重建一律采用 if/else 链，不引入全局可变状态。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <utility>
#include <vector>
#include <map>

#include "CDataManager.h"             // 脚本类型权威布局（stDeathTower_t 等）
#include "sync_script.h"             // STEquipmentPartSetPiece/STEquipmentScript
                                     // （DataManagerInitDepends.h 前置要求）
#include "DataManagerInitDepends.h"   // 本批 import* 声明 + AvatarRoulette 完整视图
#include "DNFLexWrapper.h"            // loadRDARScriptFile/unload/Scan*/ScanTypeOrInt

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

// ----------------------------------------------------------------------------
// ConvertStringToTime —— ORIG T 0x088c2341。"YYYY-MM-DD HH:MM:SS"（19 字符）
// → mktime。substr(0,4)/(5,2)/(8,2)/(11,2)/(14,2)/(17,2) 各 atoi 后直接写入
// 栈上 struct tm 对应槽位（tm_sec..tm_year；wday/yday/isdst 不初始化，ORIG 如此，
// mktime 会归一化）。长度非 19 返回 false；成功写 out 并返回 true。
// ----------------------------------------------------------------------------
bool ConvertStringToTime(std::string s, long& out)
{
    if (s.size() != 0x13)
    {
        return false;
    }
    struct tm t;
    t.tm_sec = atoi(s.substr(17, 2).c_str());   // -0x4c
    t.tm_min = atoi(s.substr(14, 2).c_str());   // -0x48
    t.tm_hour = atoi(s.substr(11, 2).c_str());  // -0x44
    t.tm_mday = atoi(s.substr(8, 2).c_str());   // -0x40
    t.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;   // -0x3c
    t.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;  // -0x38（sub $0x76c）
    // tm_wday/tm_yday/tm_isdst 保持未初始化（与 ORIG 栈槽一致，mktime 归一化）
    out = mktime(&t);
    return true;
}

// ----------------------------------------------------------------------------
// ConvertHHMMSSStringToInt —— ORIG T 0x088c25df。"HH:MM:SS"（8 字符）：
// h=atoi(substr(0,2))、m=atoi(substr(3,2))、s=atoi(substr(6,2))。
// 长度非 8 返回 false。
// ----------------------------------------------------------------------------
bool ConvertHHMMSSStringToInt(std::string s, unsigned char& h,
                              unsigned char& m, unsigned char& sec)
{
    if (s.size() != 8)
    {
        return false;
    }
    h = static_cast<unsigned char>(atoi(s.substr(0, 2).c_str()));
    m = static_cast<unsigned char>(atoi(s.substr(3, 2).c_str()));
    sec = static_cast<unsigned char>(atoi(s.substr(6, 2).c_str()));
    return true;
}

// ----------------------------------------------------------------------------
// ConvertWeekdayStringToInt —— ORIG T 0x088c284a。SUN..SAT→0..6、ALL→7；
// 未命中返回 false 且不触碰 out（ORIG 仅在命中路径写 (%edx)）。
// ----------------------------------------------------------------------------
bool ConvertWeekdayStringToInt(std::string s, unsigned int& out)
{
    if (s == "SUN") { out = 0; return true; }
    if (s == "MON") { out = 1; return true; }
    if (s == "TUE") { out = 2; return true; }
    if (s == "WED") { out = 3; return true; }
    if (s == "THU") { out = 4; return true; }
    if (s == "FRI") { out = 5; return true; }
    if (s == "SAT") { out = 6; return true; }
    if (s == "ALL") { out = 7; return true; }
    return false;
}

// ----------------------------------------------------------------------------
// tokenizeStr —— ORIG T 0x088945e2。先求首/尾锚点并 clear 输出容器，随后
// while(end!=npos || start!=npos) 循环 push_back(substr(start,end-start)) 并
// 推进双锚点。循环条件为「或」（88946cd 处 jne/je 组合实证），与常规实现不同。
// ----------------------------------------------------------------------------
void tokenizeStr(const std::string& s, std::vector<std::string>& out,
                 const std::string& delims)
{
    std::size_t start = s.find_first_not_of(delims, 0);
    std::size_t end = s.find_first_of(delims, start);
    out.clear();
    while (end != std::string::npos || start != std::string::npos)
    {
        out.push_back(s.substr(start, end - start));
        start = s.find_first_not_of(delims, end);
        end = s.find_first_of(delims, start);
    }
}

// ----------------------------------------------------------------------------
// getCharacterJob —— ORIG T 0x088be57e（定义在 SyncScriptDeps.cpp，本 TU 仅声明
// 引用）。全局作业名表 g_characterJobNames（ORIG .data 0x93f4d40，12 项实测）：
// "[swordman]".."[creator mage]" 共 11 项 + "[all]"；i=0..10 命中返回 i，否则 11。
// ----------------------------------------------------------------------------
int getCharacterJob(const char* name);

// ----------------------------------------------------------------------------
// makeFairPVPEquipmentPackageKey —— ORIG T 0x088945ad（定义在
// InitScriptProducers_2.cpp，归属协商结果，本 TU 仅声明引用）：
// 两参各截 short 入 4 字节槽后整槽返回 ((short)b<<16)|(short)a。
// ----------------------------------------------------------------------------
int makeFairPVPEquipmentPackageKey(int a, int b);

// ----------------------------------------------------------------------------
// getCharacterJobToEnumValue —— ORIG T 0x089bbc0f（定义在 CDataManager.cpp，
// 本 TU 仅声明引用）。std::string== 链：swordman..creator mage→0..10、
// common→11、其余 -1。
// ----------------------------------------------------------------------------
int getCharacterJobToEnumValue(std::string& s);

// ----------------------------------------------------------------------------
// getGrowTypeIndex —— ORIG T 0x088be63d（0xc7b 字节，定义归本 TU）。ORIG 的静态
// map<const char*,int> 缓存（0x949b430）lazy-insert 分支链实测展开后语义为下表
// （组内同名共享一个结果写槽，88bf27b 等共享块实证）：
// "none"→0；7 个一转名→1；7 个二转名→2；5 名→3；5 名→4；未知→0xffff。
// ----------------------------------------------------------------------------
int getGrowTypeIndex(const char* name)
{
    static const char* const kGrowTypeGroup1[] =
    {
        "weaponmaster", "nenmaster", "ranger", "elementalmaster",
        "crusader", "rogue", "elementalbomber"
    };
    static const char* const kGrowTypeGroup2[] =
    {
        "soulbringer", "striker", "launcher", "summoner", "infighter",
        "necromancer", "glacialmaster"
    };
    static const char* const kGrowTypeGroup3[] =
    {
        "berserker", "streetfighter", "mechanic", "battlemage", "exorcist"
    };
    static const char* const kGrowTypeGroup4[] =
    {
        "asura", "grappler", "spitfire", "witch", "avenger"
    };

    if (name == 0)
    {
        return 0xffff;
    }
    if (strcmp(name, "none") == 0)
    {
        return 0;
    }
    for (int i = 0; i < 7; ++i)
    {
        if (strcmp(name, kGrowTypeGroup1[i]) == 0) { return 1; }
    }
    for (int i = 0; i < 7; ++i)
    {
        if (strcmp(name, kGrowTypeGroup2[i]) == 0) { return 2; }
    }
    for (int i = 0; i < 5; ++i)
    {
        if (strcmp(name, kGrowTypeGroup3[i]) == 0) { return 3; }
    }
    for (int i = 0; i < 5; ++i)
    {
        if (strcmp(name, kGrowTypeGroup4[i]) == 0) { return 4; }
    }
    return 0xffff;
}

// ============================================================================
// AvatarRoulette 生命周期（ORIG C1 0x08911b98 / D1 0x0837e484 / clear 0x8911cf6）
// ============================================================================

AvatarRoulette::AvatarRoulette()
{
    // 两数组（m_avatars[11]/m_stikers[11]）由成员默认构造完成（ORIG 循环
    // 调 vector C1Ev 等价），随后调 clear。
    clear();
}

AvatarRoulette::~AvatarRoulette()
{
    // ORIG D1：逆序析构 m_stikers[10]..[0]、再 m_avatars[10]..[0]
    // （编译器生成的数组析构等价于此）。
}

// clear —— ORIG 0x8911cf6：j=0..10 双层清空 avatar 数组；stiker 数组不动。
void AvatarRoulette::clear()
{
    for (int j = 0; j <= 10; ++j)
    {
        std::vector<std::vector<std::pair<unsigned long, int> > >& bucket =
            m_avatars[j];
        for (std::size_t k = 0; k < bucket.size(); ++k)
        {
            bucket[k].clear();
        }
        bucket.clear();
    }
}

// ============================================================================
// importAvatarRouletteListDetail —— ORIG T 0x089115b1
//   bool (AvatarRoulette&, std::string&, ENUM_CHARACTERJOB)
// 内层装载器：至多 9 次 ScanTypeOrInt 取数值（遇 TYPE 提前终止），累计入局部
// pairs{value,3}；pairs 非空时整体 push 进 m_avatars[job]，再 ScanInt 一枚
// sticker push 进 m_stikers[job]。恒返回 true。
// ============================================================================
bool importAvatarRouletteListDetail(AvatarRoulette& out,
                                    std::string& token,
                                    ENUM_CHARACTERJOB job)
{
    std::vector<std::pair<unsigned long, int> > pairs;
    bool isType = false;
    bool isInt = false;

    for (int i = 0; i <= 8; ++i)
    {
        int value = ScanTypeOrInt(isType, token, isInt, true);
        if (isType)
        {
            break;   // TYPE token 已留在 token 中供上层判节尾
        }
        pairs.push_back(std::make_pair(static_cast<unsigned long>(value), 3));
    }

    const int jobIdx = static_cast<int>(job);
    if (!pairs.empty())
    {
        out.m_avatars[jobIdx].push_back(pairs);   // &out[job*0xc+0x14]（+0x14 数组基址实证）
        int sticker = ScanInt((bool*)0);
        out.m_stikers[jobIdx].push_back(sticker); // &out[job*0xc+0x98]
    }
    return true;
}

// ============================================================================
// importAvatarRouletteList —— ORIG T 0x089116eb
//   bool (char* path, AvatarRoulette&)
// [avatar coin info]/[rare clone avatar rate]/[rare avatar rate]/
// [upper avatar rate]/[normal avatar rate] 五个标量 + 十个 job 节
// ([swordman]..[DemonicSwordman])，每节 do{Detail}while(token==[/节名])。
// EOF（ScanTypeOrInt 非型且返 0）或 load 失败之外的出口恒为 true。
// ============================================================================
bool importAvatarRouletteList(char* path, AvatarRoulette& out)
{
    bool isType = false;
    bool isInt = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    out.clear();

    for (;;)
    {
        int ret = ScanTypeOrInt(isType, token, isInt, true);
        if (!isInt && ret == 0)
        {
            return true;   // ORIG 0x8911b61
        }

        if (token == "[avatar coin info]")
        {
            out.m_avatarCoinInfo = ScanInt((bool*)0);
        }
        else if (token == "[rare clone avatar rate]")
        {
            out.m_rareCloneAvatarRate = ScanInt((bool*)0);
        }
        else if (token == "[rare avatar rate]")
        {
            out.m_rareAvatarRate = ScanInt((bool*)0);
        }
        else if (token == "[upper avatar rate]")
        {
            out.m_upperAvatarRate = ScanInt((bool*)0);
        }
        else if (token == "[normal avatar rate]")
        {
            out.m_normalAvatarRate = ScanInt((bool*)0);
        }
        else if (token == "[swordman]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)0);
            } while (token == "[/swordman]");
        }
        else if (token == "[fighter]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)1);
            } while (token == "[/fighter]");
        }
        else if (token == "[gunner]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)2);
            } while (token == "[/gunner]");
        }
        else if (token == "[mage]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)3);
            } while (token == "[/mage]");
        }
        else if (token == "[priest]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)4);
            } while (token == "[/priest]");
        }
        else if (token == "[at_gunner]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)5);
            } while (token == "[/at_gunner]");
        }
        else if (token == "[thief]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)6);
            } while (token == "[/thief]");
        }
        else if (token == "[at_fighter]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)7);
            } while (token == "[/at_fighter]");
        }
        else if (token == "[at_mage]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)8);
            } while (token == "[/at_mage]");
        }
        else if (token == "[DemonicSwordman]")
        {
            do
            {
                importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)9);
            } while (token == "[/DemonicSwordman]");
        }
    }
}

// ============================================================================
// importDeathTowerScript —— ORIG T 0x089089f3
//   int (const char*, stDeathTower_t&)，0=成功，否则 init.md Fail(%d) 行号码。
// 八个节：单值节带错误码；四个 200 表节从下标 1 开始连续 ScanFloat/ScanInt
// 直至失败（ORIG 无边界检查，忠实保留）；未知节名 → 0x18cd 错误。
// ============================================================================
int importDeathTowerScript(const char* path, stDeathTower_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x187e;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 0;
        }

        if (token == "[apc exp weight]")
        {
            float v = ScanFloat(&ok);
            out.m_field0 = v;                       // fstps (+0x000)，float 位模式
            if (!ok) { return 0x1887; }
        }
        else if (token == "[reward exp weight]")
        {
            for (int i = 1;; ++i)                   // ORIG 下标从 1 开始
            {
                float v = ScanFloat(&ok);
                out.m_rateA[i] = v;
                if (!ok) { break; }
            }
        }
        else if (token == "[reward card num]")
        {
            for (int i = 1;; ++i)
            {
                int v = ScanInt(&ok);
                out.m_cntB[i] = v;                  // (0xc8+i)*4+4 → +0x324+i*4
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward exp weight]")
        {
            for (int i = 1;; ++i)
            {
                float v = ScanFloat(&ok);
                out.m_rateC[i] = v;                 // (0x190+i)*4+4 → +0x644+i*4
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward card num]")
        {
            for (int i = 1;; ++i)
            {
                int v = ScanInt(&ok);
                out.m_cntD[i] = v;                  // (0x258+i)*4+4 → +0x964+i*4
                if (!ok) { break; }
            }
        }
        else if (token == "[monster exp weight]")
        {
            float v = ScanFloat(&ok);
            out.m_fieldc84 = v;                     // fstps +0xc84
            if (!ok) { return 0x18b8; }
        }
        else if (token == "[reward item prob]")
        {
            int v1 = ScanInt(&ok);
            if (!ok) { return 0x18be; }
            out.m_rewardProbC88[0] = v1;
            int v2 = ScanInt(&ok);
            if (!ok) { return 0x18c1; }
            out.m_rewardProbC88[1] = out.m_rewardProbC88[0] + v2;
            int v3 = ScanInt(&ok);
            if (!ok) { return 0x18c4; }
            out.m_rewardProbC88[2] = out.m_rewardProbC88[1] + v3;
        }
        else if (token == "[reward gold weight]")
        {
            float v = ScanFloat(&ok);
            out.m_floatc94 = v;                     // fstps +0xc94
            if (!ok) { return 0x18ca; }
        }
        else
        {
            return 0x18cd;                          // ORIG 未知节名即失败
        }
    }
}

// ============================================================================
// importBloodDungeonScript —— ORIG T 0x08909016
//   int (const char*, stBloodDungeon_t&)，0=成功。
// [ultimate reward prob] 以 map find 判重：key 已存在 → 0x196e 错误。
// ============================================================================
int importBloodDungeonScript(const char* path, stBloodDungeon_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x191c;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 0;
        }

        if (token == "[reward exp weight]")
        {
            for (int i = 0;; ++i)                   // 本表下标从 0 开始
            {
                float v = ScanFloat(&ok);
                out.m_rate[i] = v;
                if (!ok) { break; }
            }
        }
        else if (token == "[exp standard monster cnt]")
        {
            int v = ScanInt(&ok);
            out.m_field320 = v;
            if (!ok) { return 0x192f; }
        }
        else if (token == "[reward item prob]")
        {
            int v1 = ScanInt(&ok);
            if (!ok) { return 0x1936; }
            out.m_field324[0] = v1;
            int v2 = ScanInt(&ok);
            if (!ok) { return 0x1939; }
            out.m_field324[1] = out.m_field324[0] + v2;
            int v3 = ScanInt(&ok);
            if (!ok) { return 0x193c; }
            out.m_field324[2] = out.m_field324[1] + v3;
        }
        else if (token == "[reward gold weight]")
        {
            float v = ScanFloat(&ok);
            out.m_field330 = v;
            if (!ok) { return 0x1942; }
        }
        else if (token == "[reward exp inout count weight]")
        {
            for (;;)
            {
                float v = ScanFloat(&ok);
                if (!ok) { break; }
                out.m_vec334.push_back(v);
            }
        }
        else if (token == "[ultimate difficulty point]")
        {
            out.m_ultDiffPoint344 = ScanInt((bool*)0);
            out.m_ultPointTotal348 = ScanInt((bool*)0);
        }
        else if (token == "[ultimate reward prob]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);
                if (!ok) { break; }
                STUltimateRewardProb prob;
                prob.m_field0 = ScanInt((bool*)0);
                prob.m_field4 = ScanInt((bool*)0);

                std::map<int, STUltimateRewardProb>& table = out.m_map358;
                std::map<int, STUltimateRewardProb>::iterator it =
                    table.find(key);
                if (it != table.end())
                {
                    return 0x196e;                  // ORIG key 重复即失败
                }
                table.insert(
                    std::map<int, STUltimateRewardProb>::value_type(key, prob));
            }
        }
    }
}

// ============================================================================
// importCharacLinkSystemScript —— ORIG T 0x08909529
//   int (const char*, stCharacLinkSystem_t&)，0=成功。
// [link level] 写 +0x00；四个 bonus 节各自选定目标 vector 后 do-while 连续装
// 4×ScanInt 记录；未匹配节名静默跳过（ORIG al=0 → 回外层）。
// ============================================================================
int importCharacLinkSystemScript(const char* path, stCharacLinkSystem_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x19ab;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 0;
        }

        if (token == "[link level]")
        {
            int v = ScanInt(&ok);
            out.m_field0 = v;
            if (!ok) { return 0x19b7; }
        }
        else if (token == "[low link character exp bonus]" ||
                 token == "[low link character gold bonus]" ||
                 token == "[high link character exp bonus]" ||
                 token == "[high link character gold bonus]")
        {
            std::vector<stCharacLinkSystemData>* target = 0;
            if (token == "[low link character exp bonus]")
            {
                target = &out.m_vec4;               // +0x04
            }
            else if (token == "[low link character gold bonus]")
            {
                target = &out.m_vec10;              // +0x10
            }
            else if (token == "[high link character exp bonus]")
            {
                target = &out.m_vec1c;              // +0x1c
            }
            else
            {
                target = &out.m_vec28;              // +0x28
            }

            for (;;)
            {
                stCharacLinkSystemData data;        // ORIG ctor 0x891b762 四连清零
                data.m_field0 = ScanInt(&ok);
                if (!ok) { break; }                 // ORIG 此路径跳过 dtor，无观测差异（trivial dtor）
                data.m_field4 = ScanInt(&ok);
                if (!ok) { return 0x19d5; }
                data.m_field8 = ScanInt(&ok);
                if (!ok) { return 0x19da; }
                data.m_fieldc = ScanInt(&ok);
                if (!ok) { return 0x19df; }
                target->push_back(data);
            }
        }
    }
}

// ============================================================================
// importCharacterCreateGiftScript —— ORIG T 0x0895eda1
//   bool (createChracScript*, const char*)
// [apply job] 记忆当前 job（getCharacterJobToEnumValue）；[/apply job] 把累积的
// rewardInfo 挂入 m_map0[job] 并复位累积器；[/create character gift] 返回 true；
// EOF 未遇结束符 → false；时间串转换失败 → false。
// ============================================================================
bool importCharacterCreateGiftScript(createChracScript* s, const char* path)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == 0)
    {
        return false;
    }

    createChracScript::rewardInfo info;
    int curJob = -1;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return false;                           // ORIG 出口读恒为 0 的标志字节
        }

        if (token == "[/create character gift]")
        {
            return true;                            // ORIG 0x895f0fd
        }
        else if (token == "[apply job]")
        {
            std::string jobName;
            ScanStr(&jobName);
            curJob = getCharacterJobToEnumValue(jobName);
        }
        else if (token == "[item]")
        {
            int itemId = 0;
            ScanInt(&itemId);                       // ScanIntPi 变体
            info.m_items.push_back(itemId);         // ri+0x00 vector<int>
        }
        else if (token == "[new account event start time]")
        {
            std::string str;
            ScanStr(&str);
            std::string tmp(str);
            if (!ConvertStringToTime(tmp, *(long*)&info.m_startTime))
            {
                return false;                       // long 与 int 同宽（ia32）
            }
        }
        else if (token == "[new account event end time]")
        {
            std::string str;
            ScanStr(&str);
            std::string tmp(str);
            if (!ConvertStringToTime(tmp, *(long*)&info.m_endTime))
            {
                return false;
            }
        }
        else if (token == "[create title]")
        {
            ScanStr(&info.m_title);                 // ScanStr 直写 +0x14
        }
        else if (token == "[create text]")
        {
            ScanStr(&info.m_text);                  // ScanStr 直写 +0x18
        }
        else if (token == "[/apply job]")
        {
            if (curJob != -1)
            {
                std::map<int, createChracScript::rewardInfoList>::iterator it =
                    s->m_map0.find(curJob);
                if (it == s->m_map0.end())
                {
                    createChracScript::rewardInfoList list;
                    list.m_list.push_back(info);
                    s->m_map0.insert(
                        std::map<int, createChracScript::rewardInfoList>::
                            value_type(curJob, list));
                }
                else
                {
                    it->second.m_list.push_back(info);
                }
            }
            // 复位累积器（ORIG 0x895f735..0x895f780）
            info.m_items.clear();
            info.m_title.clear();
            info.m_text.clear();
            info.m_startTime = 0;
            info.m_endTime = 0;
            curJob = -1;
        }
    }
}

// ============================================================================
// importCharacterCreateCreatureGiftScript —— ORIG T 0x0896001a
//   bool (CharacterCreateCreatureGiftScript*, const char*)
// [event job]/[start]/[end][/event job] 装配 map<int,eventJob>；
// [change able creature lev] 单值；[change infinity creature] 向 +0x1c、
// [client infinity creature] 向 +0x34 的 map<int,int> 成对 insert。
// ============================================================================
bool importCharacterCreateCreatureGiftScript(CharacterCreateCreatureGiftScript* s,
                                             const char* path)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == 0)
    {
        return false;
    }

    int curJob = -1;
    CharacterCreateCreatureGiftScript::eventJob eventJobItem;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;                            // ORIG EOF → true
        }

        if (token == "[event job]")
        {
            std::string jobName;
            ScanStr(&jobName);
            curJob = getCharacterJobToEnumValue(jobName);
        }
        else if (token == "[start]")
        {
            ScanInt(&eventJobItem.m_minLevel);      // ScanIntPi
        }
        else if (token == "[end]")
        {
            ScanInt(&eventJobItem.m_maxLevel);
        }
        else if (token == "[/event job]")
        {
            s->m_eventJobMap.insert(
                std::map<int, CharacterCreateCreatureGiftScript::eventJob>::
                    value_type(curJob, eventJobItem));
            eventJobItem.m_minLevel = -1;           // ORIG 复位三连
            eventJobItem.m_maxLevel = -1;
            curJob = -1;
        }
        else if (token == "[change able creature lev]")
        {
            ScanInt(&s->m_changeAbleCreatureLev);   // ScanIntPi 直写 +0x18
        }
        else if (token == "[change infinity creature]")
        {
            for (;;)
            {
                int first = ScanInt(&ok);
                if (!ok) { break; }
                int second = ScanInt(&ok);
                if (!ok) { break; }
                s->m_clientChangeMap.insert(        // +0x1c
                    std::map<int, int>::value_type(first, second));
            }
        }
        else if (token == "[client infinity creature]")
        {
            for (;;)
            {
                int first = ScanInt(&ok);
                if (!ok) { break; }
                int second = ScanInt(&ok);
                if (!ok) { break; }
                s->m_infinityChangeMap.insert(      // +0x34
                    std::map<int, int>::value_type(first, second));
            }
        }
    }
}

// ============================================================================
// importConditionEventChkDungeonScript —— ORIG T 0x08913891
//   bool (const char*, ConditionLevelChkDungeon*)
// [apply level] 写 +0x0c；[unable dungeon] 清空后 do-while push_back；
// 无 unloadRDARScriptFile（ORIG 如此）；out 为空指针时返回 false。
// ============================================================================
bool importConditionEventChkDungeonScript(const char* path,
                                          ConditionLevelChkDungeon* out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[apply level]")
        {
            int v = ScanInt(&ok);
            out->m_applyLevel = v;                  // +0x0c
        }
        else if (token == "[unable dungeon]")
        {
            out->m_unableDungeons.clear();
            for (;;)
            {
                int dungeonIdx = ScanInt(&ok);
                if (!ok) { break; }
                out->m_unableDungeons.push_back(dungeonIdx);
            }
        }
    }
}

// ============================================================================
// importConditionEventScript —— ORIG T 0x0890e868
//   bool (const char*, stConditionEventInfo*)
// [id]→+0x00；[condition match]→map+0x08（key,(v1,v2)）；
// [condition reward]→multimap+0x20；[condition quick party add reward]→
// multimap+0x38。两条 reward 节均扫 1 key+4 值，末两值 setne 成 bool。
// ============================================================================
bool importConditionEventScript(const char* path, stConditionEventInfo* out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[id]")
        {
            int v = ScanInt(&ok);
            out->m_field0 = v;                      // mov %eax,(%eax) → +0x00
        }
        else if (token == "[condition match]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);
                if (!ok) { break; }
                int v1 = ScanInt(&ok);
                if (!ok) { break; }
                int v2 = ScanInt(&ok);
                if (!ok) { break; }
                out->m_map8.insert(
                    std::map<int, std::pair<int, int> >::value_type(
                        key, std::make_pair(v1, v2)));
            }
        }
        else if (token == "[condition reward]" ||
                 token == "[condition quick party add reward]")
        {
            bool quickParty = (token == "[condition quick party add reward]");
            for (;;)
            {
                int key = ScanInt(&ok);
                if (!ok) { break; }
                int v1 = ScanInt(&ok);
                if (!ok) { break; }
                int v2 = ScanInt(&ok);
                if (!ok) { break; }
                int v3 = ScanInt(&ok);
                if (!ok) { break; }
                int v4 = ScanInt(&ok);
                if (!ok) { break; }
                stConditionEventRewardInfo info;
                info.m_field0 = v1;
                info.m_field4 = v2;
                info.m_flag8 = (v3 != 0);           // setne
                info.m_flag9 = (v4 != 0);
                if (quickParty)
                {
                    out->m_multiMap38.insert(       // +0x38
                        std::multimap<int, stConditionEventRewardInfo>::
                            value_type(key, info));
                }
                else
                {
                    out->m_multiMap20.insert(       // +0x20
                        std::multimap<int, stConditionEventRewardInfo>::
                            value_type(key, info));
                }
            }
        }
    }
}

// ============================================================================
// importDBLogItem —— ORIG T 0x0890dbba
//   bool (char*, std::vector<stDBLogItem>*)
// 外层每轮消耗两个节头：第一 ScanType 匹配 [dblog buyitem]（flag=0 批量）、
// 第二匹配 [dblog getitem]（flag=1）；记录={id,ConvertStringToTime(str),flag}，
// id ScanInt 失败即节尾。任一 ScanStr/时间转换失败 → 整体 false。
// ============================================================================
bool importDBLogItem(char* path, std::vector<stDBLogItem>* out)
{
    bool ok = false;
    std::string token;
    std::string str;

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        int flag = 0;
        if (token != "[dblog buyitem]")
        {
            // 未命中 buyitem 时 ORIG 直接进入第二个节头判定（同轮二次 ScanType）
            ;
        }

        if (token == "[dblog buyitem]")
        {
            flag = 0;
            for (;;)
            {
                stDBLogItem item;                   // memset(this,0,0xc)
                memset(&item, 0, sizeof(item));
                int id = ScanInt(&ok);
                item.m_itemIdx = static_cast<unsigned int>(id);
                if (!ok) { break; }
                if (!ScanStr(&str))
                {
                    return false;
                }
                std::string tmp(str);
                if (!ConvertStringToTime(tmp, *(long*)&item.m_time))
                {
                    return false;
                }
                item.m_value = flag;                // -0x2c=0（memset 冗余写）
                out->push_back(item);
            }
        }

        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[dblog getitem]")
        {
            flag = 1;
            for (;;)
            {
                stDBLogItem item;
                memset(&item, 0, sizeof(item));
                int id = ScanInt(&ok);
                item.m_itemIdx = static_cast<unsigned int>(id);
                if (!ok) { break; }                 // ORIG 此路径回最外层
                if (!ScanStr(&str))
                {
                    return false;
                }
                std::string tmp(str);
                if (!ConvertStringToTime(tmp, *(long*)&item.m_time))
                {
                    return false;
                }
                item.m_value = flag;                // -0x38=1
                out->push_back(item);
            }
        }
    }
}

// ============================================================================
// importDailyScheduleEtcScript —— ORIG T 0x0891826c
//   int (const char*, std::vector<_DailySchedule>&)，0=成功。
// [schedule] 内 do-while 连续装配 _DailySchedule（0x24）：3×ScanInt +
// 5×ScanStr（后四者失败带 0x3721..0x3737 行号码）+ 两次 ConvertStringToTime
// （+0x0c/+0x10）+ weekday（+0x14）+ 两组 ConvertHHMMSSStringToInt（+0x18..）。
// ============================================================================
int importDailyScheduleEtcScript(const char* path,
                                 std::vector<_DailySchedule>& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x36f7;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 0;
        }

        if (token == "[schedule]")
        {
            for (;;)
            {
                _DailySchedule entry;               // memset(this,0,0x24)
                memset(&entry, 0, sizeof(entry));

                entry.m_eventId = ScanInt(&ok);     // +0x00
                if (!ok) { break; }                 // 三枚普通 ScanInt 失败仅回外层
                entry.m_type = ScanInt(&ok);        // +0x04
                if (!ok) { break; }
                entry.m_idx = ScanInt(&ok);         // +0x08
                if (!ok) { break; }

                std::string strStart;
                if (!ScanStr(&strStart))
                {
                    break;                          // 首串失败同样回外层（ORIG 89183ab）
                }
                std::string strEnd;
                if (!ScanStr(&strEnd))
                {
                    return 0x3721;
                }
                std::string strWeekday;
                if (!ScanStr(&strWeekday))
                {
                    return 0x3723;
                }
                std::string strStartTime;
                if (!ScanStr(&strStartTime))
                {
                    return 0x3725;
                }
                std::string strEndTime;
                if (!ScanStr(&strEndTime))
                {
                    return 0x3727;
                }

                {
                    std::string tmp(strStart);
                    if (!ConvertStringToTime(tmp, *(long*)&entry.m_startSec))
                    {
                        return 0x372a;              // +0x0c
                    }
                }
                {
                    std::string tmp(strEnd);
                    if (!ConvertStringToTime(tmp, *(long*)&entry.m_endSec))
                    {
                        return 0x372c;              // +0x10
                    }
                }
                {
                    unsigned int wday = 0;
                    std::string tmp(strWeekday);
                    if (!ConvertWeekdayStringToInt(tmp, wday))
                    {
                        return 0x3730;              // +0x14
                    }
                    entry.m_wday = static_cast<int>(wday);
                }
                {
                    std::string tmp(strStartTime);
                    unsigned char h = 0;
                    unsigned char m = 0;
                    unsigned char sec = 0;
                    if (!ConvertHHMMSSStringToInt(tmp, h, m, sec))
                    {
                        return 0x3735;
                    }
                    entry.m_startHour = h;          // +0x18
                    entry.m_startMin = m;           // +0x19
                    entry.m_startSec2 = sec;        // +0x1a
                }
                {
                    std::string tmp(strEndTime);
                    unsigned char h = 0;
                    unsigned char m = 0;
                    unsigned char sec = 0;
                    if (!ConvertHHMMSSStringToInt(tmp, h, m, sec))
                    {
                        return 0x3737;
                    }
                    entry.m_endHour = h;            // +0x1b
                    entry.m_endMin = m;             // +0x1c
                    entry.m_endSec2 = sec;          // +0x1d
                }

                out.push_back(entry);               // 回 do 头继续下一记录
            }
        }
    }
}

// ============================================================================
// importDimensionActivationData —— ORIG T 0x89160f0
//   bool (const char*, DimensionActivationData*)
// [play day count]→+0x00；[coin info]{easy|medium→0,hard→1,ultimate→3} 键入
// coinMap(+0x04)；[section] 装配 EventRewardSection 入 m_rewardData.m_vec0(+0x1c)；
// [select group] 按 makeFairPVPEquipmentPackageKey(getCharacterJob,idx) 入
// m_rewardData.m_mapC(+0x28)；[give group] 按 ((job,growIdx)) 入
// m_rewardData.m_map24(+0x40)。ScanStr 失败 → false。
// ============================================================================
bool importDimensionActivationData(const char* path,
                                   DimensionActivationData* out)
{
    bool ok = false;
    std::string token;
    std::string sub;

    if (out == 0)
    {
        return false;                               // ORIG 先于 load 判空
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[play day count]")
        {
            int v = ScanInt(&ok);
            out->m_field0 = v;                      // +0x00
        }
        else if (token == "[coin info]")
        {
            for (;;)
            {
                if (!ScanStr(&sub))
                {
                    break;                          // 回外层节循环
                }
                int diff = 0;
                if (sub == "[easy]" || sub == "[medium]")
                {
                    diff = 0;
                }
                else if (sub == "[hard]")
                {
                    diff = 1;
                }
                else if (sub == "[ultimate]")
                {
                    diff = 3;
                }
                else
                {
                    continue;                       // 未知子键：ORIG 落到公共尾（等价跳过）
                }

                int count = ScanInt(&ok);           // -0x1b8
                if (!ok) { break; }
                int used = ScanInt(&ok);            // -0x1a8
                if (!ok) { break; }

                DimensionCoinInfo coinInfo;         // ctor 0x891bcb6 清零
                coinInfo.m_field0 = count;
                coinInfo.m_field4 = (used != 0) ? 1 : 0;   // setne 字节
                out->m_coinMap.insert(
                    std::map<int, DimensionCoinInfo>::value_type(diff, coinInfo));
            }
        }
        else if (token == "[section]")
        {
            for (;;)
            {
                EventRewardSection section;         // ctor 0x89154ee
                section.m_field0 = ScanInt(&ok);
                if (!ok) { break; }                 // 首字段失败回外层
                section.m_field1 = ScanInt((bool*)0);
                section.m_field2 = ScanInt((bool*)0);
                int count = ScanInt(&ok);
                for (int i = 0; i < count; ++i)
                {
                    unsigned char item =
                        static_cast<unsigned char>(ScanInt((bool*)0));
                    section.m_items.push_back(item);   // vector<uchar> @+0x04
                }
                out->m_rewardData.m_vec0.push_back(section);
            }
        }
        else if (token == "[select group]")
        {
            if (!ScanStr(&sub))
            {
                return false;
            }
            int job = getCharacterJob(sub.c_str());
            int idx = ScanInt((bool*)0);
            int packageKey = makeFairPVPEquipmentPackageKey(job, idx);

            std::vector<std::vector<EventRewardItemInfo> > groups;
            for (;;)
            {
                if (!ScanType(token, true))
                {
                    break;                          // EOF → 回外层
                }
                if (token != "[package selection]")
                {
                    if (token == "[/select group]")
                    {
                        out->m_rewardData.m_mapC.insert(
                            std::map<int,
                                std::vector<std::vector<EventRewardItemInfo> > >::
                                value_type(packageKey, groups));
                        break;                      // 回外层
                    }
                    continue;                       // 其他 token 忽略（ORIG 回内层头）
                }

                std::vector<EventRewardItemInfo> items;
                int first = 0;
                if (!ScanInt(&first))
                {
                    groups.push_back(items);        // 空批也入组（ORIG 0x8916e5e）
                    continue;
                }
                for (;;)
                {
                    EventRewardItemInfo info;       // ctor 0x891548a
                    info.m_field0 = first;
                    info.m_short8 =
                        static_cast<short>(ScanInt((bool*)0));   // %ax 半字写
                    info.m_field4 = ScanInt((bool*)0);
                    std::string growTypes;
                    ScanStr(&growTypes);            // 返回值被忽略（ORIG 0x8916c45）
                    {
                        std::string tmp(growTypes.c_str());
                        std::vector<std::string> tokens;
                        tokenizeStr(tmp, tokens, ",");
                        for (std::size_t k = 0; k < tokens.size(); ++k)
                        {
                            int growIdx = getGrowTypeIndex(tokens.at(k).c_str());
                            info.m_growTypes.push_back(growIdx);
                        }
                    }
                    items.push_back(info);
                    if (!ScanInt(&first))
                    {
                        break;                      // 本包终止
                    }
                }
                groups.push_back(items);
            }
        }
        else if (token == "[give group]")
        {
            if (!ScanStr(&sub))
            {
                return false;
            }
            int job = getCharacterJob(sub.c_str());

            for (;;)
            {
                if (!ScanType(token, true))
                {
                    break;                          // EOF → 回外层
                }
                if (token != "[grow type]")
                {
                    if (token == "[/give group]")
                    {
                        break;
                    }
                    continue;
                }

                if (!ScanStr(&sub))
                {
                    break;                          // ORIG 0x891735a 回外层
                }
                int growIdx = getGrowTypeIndex(sub.c_str());

                std::vector<std::vector<EventRewardItemInfo> > groups;
                for (;;)
                {
                    if (!ScanType(token, true))
                    {
                        break;
                    }
                    std::vector<EventRewardItemInfo> items;
                    if (token != "[package selection]")
                    {
                        if (token == "[/grow type]")
                        {
                            out->m_rewardData.m_map24.insert(
                                std::map<std::pair<int, int>,
                                    std::vector<std::vector<EventRewardItemInfo> > >::
                                    value_type(std::make_pair(job, growIdx),
                                               groups));
                            groups.clear();         // ORIG 0x89172c5 显式 clear
                            break;                  // 回 give-group 层
                        }
                        continue;                   // items 构造在比较前（ORIG 0x89170d5），无副作用
                    }

                    ScanInt(&ok);                   // -0x1c 死存（ORIG 保留形态）
                    for (;;)
                    {
                        EventRewardItemInfo info;   // ctor 后逐条覆盖
                        int value = 0;
                        if (!ScanInt(&value))
                        {
                            break;
                        }
                        info.m_field0 = value;
                        info.m_short8 =
                            static_cast<short>(ScanInt(&ok));
                        info.m_field4 = ScanInt(&ok);
                        items.push_back(info);
                        info.clear();               // ORIG 0x89154bc
                    }
                    groups.push_back(items);
                    items.clear();                  // ORIG 0x89171b4
                }
            }
        }
    }
}

// ============================================================================
// importEmotionTypeTagsScript —— ORIG T 0x0890de85
//   bool (const char*, stEmotionTypeTagsInfo*)
// [EMOTION INFO] 内：tag 入 m_tags(+0x00) 且记入 stOneEmotionData.m_0；
// 随后 4 值/3 布尔/字符串族填充元素后 push 入 m_data(+0x0c)；$END 结束
// m_18 字符串列表。任一 Scan 失败 → false。
// ============================================================================
bool importEmotionTypeTagsScript(const char* path, stEmotionTypeTagsInfo* out)
{
    bool ok = false;
    std::string token;
    std::string str;

    if (out == 0)
    {
        return false;                               // ORIG 0x890decb 判空
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[EMOTION INFO]")
        {
            stOneEmotionData data;                  // ctor 0x891b560 + clear
            int tag = ScanInt(&ok);
            if (!ok) { return false; }
            out->m_tags.push_back(tag);
            data.m_0 = tag;

            data.m_4 = ScanInt(&ok);
            if (!ok) { return false; }
            if (!ScanStr(&data.m_8))
            {
                return false;
            }
            if (!ScanStr(&data.m_c))
            {
                return false;
            }
            int v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_10 = (v == 1) ? 1 : 0;           // sete
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_11 = (v == 1) ? 1 : 0;
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_12 = (v == 1) ? 1 : 0;

            if (!ScanStr(&data.m_14))
            {
                return false;
            }
            for (;;)
            {
                if (!ScanStr(&str))
                {
                    break;                          // 列表终止（含 $END 缺失情形）
                }
                if (str == "$END")
                {
                    break;
                }
                data.m_18.push_back(str);
            }
            out->m_data.push_back(data);
        }
    }
}

// ============================================================================
// importEquipmentGradeSampleInfo —— ORIG T 0x0890e18d
//   bool (const char*, stEquipmentGradeSample*)
// [grade_0]..[grade_4] 五节分别向五个 vector<unsigned int> push（push_backEOj
// 右值重载，值序一致）；out 判空在 load 之后（ORIG 0x890e1d3）。
// ============================================================================
bool importEquipmentGradeSampleInfo(const char* path,
                                    stEquipmentGradeSample* out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (out == 0)
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[grade_0]")
        {
            for (;;)
            {
                unsigned int v = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) { break; }
                out->m_grade0.push_back(v);
            }
        }
        else if (token == "[grade_1]")
        {
            for (;;)
            {
                unsigned int v = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) { break; }
                out->m_grade1.push_back(v);
            }
        }
        else if (token == "[grade_2]")
        {
            for (;;)
            {
                unsigned int v = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) { break; }
                out->m_grade2.push_back(v);
            }
        }
        else if (token == "[grade_3]")
        {
            for (;;)
            {
                unsigned int v = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) { break; }
                out->m_grade3.push_back(v);
            }
        }
        else if (token == "[grade_4]")
        {
            for (;;)
            {
                unsigned int v = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) { break; }
                out->m_grade4.push_back(v);
            }
        }
    }
}
