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

int getGrowTypeIndex(const char* name)
{
    for (;;)
    {
        static GrowTypeMap __lookup;  // ORIG ...E8__lookup
        static SpinLock __init_lock;                             // ORIG ...E11__init_lock
        static bool __inited = false;                            // ORIG ...E8__inited
        int idx;
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;               // ORIG 088be81f jmp 088be650（回 guard 重检）
            }
            idx = 0;
        }
        else
        {
            GrowTypeMap::const_iterator it =
                __lookup.find(name);
            idx = (it != __lookup.end()) ? it->second : -1;
        }
        switch (idx)
        {
        // 建表模式（首调 !__inited，自 case 0 进入）逐 case 贯穿 insert 全表；
        // 派发模式（__inited）组内各码经 je 链汇聚共享结果槽后 return。
        case 0:
        case 0x8b5:
            if (__inited) return 0;
            __lookup.insert(GrowTypeMap::value_type("none", 0x8b5));
            /* fall through */
        case 0x8ba:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("weaponmaster", 0x8ba));
            /* fall through */
        case 0x8bb:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("nenmaster", 0x8bb));
            /* fall through */
        case 0x8bc:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("ranger", 0x8bc));
            /* fall through */
        case 0x8bd:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("elementalmaster", 0x8bd));
            /* fall through */
        case 0x8be:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("crusader", 0x8be));
            /* fall through */
        case 0x8bf:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("rogue", 0x8bf));
            /* fall through */
        case 0x8c1:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("elementalbomber", 0x8c1));
            /* fall through */
        case 0x8c7:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("soulbringer", 0x8c7));
            /* fall through */
        case 0x8c8:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("striker", 0x8c8));
            /* fall through */
        case 0x8c9:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("launcher", 0x8c9));
            /* fall through */
        case 0x8ca:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("summoner", 0x8ca));
            /* fall through */
        case 0x8cb:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("infighter", 0x8cb));
            /* fall through */
        case 0x8cc:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("necromancer", 0x8cc));
            /* fall through */
        case 0x8ce:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("glacialmaster", 0x8ce));
            /* fall through */
        case 0x8d4:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("berserker", 0x8d4));
            /* fall through */
        case 0x8d5:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("streetfighter", 0x8d5));
            /* fall through */
        case 0x8d6:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("mechanic", 0x8d6));
            /* fall through */
        case 0x8d7:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("battlemage", 0x8d7));
            /* fall through */
        case 0x8d8:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("exorcist", 0x8d8));
            /* fall through */
        case 0x8dd:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("asura", 0x8dd));
            /* fall through */
        case 0x8de:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("grappler", 0x8de));
            /* fall through */
        case 0x8df:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("spitfire", 0x8df));
            /* fall through */
        case 0x8e0:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("witch", 0x8e0));
            /* fall through */
        case 0x8e1:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("avenger", 0x8e1));
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            continue;                   // ORIG 088bf2a4 jmp 088be650 重派发
        }
        return 0xffff;                  // ORIG 未命中/未知码：local_24 初值
    }
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
// 所有 Scan 均为「先写入字段、后校验 ok」（fstp/mov 先于 ok 判定；
// [reward item prob] 三列前缀和亦同），与 ORIG 指令顺序一致。
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
            out.m_apcExpWeight = ScanFloat(&ok);    // fstps (+0x000)，float 位模式
            if (!ok) { return 0x1887; }
        }
        else if (token == "[reward exp weight]")
        {
            for (int i = 1;; ++i)                   // ORIG 下标从 1 开始
            {
                out.m_rewardExpWeight[i] = ScanFloat(&ok);   // fstps 0x4(%eax,%ebx,4)
                if (!ok) { break; }
            }
        }
        else if (token == "[reward card num]")
        {
            for (int i = 1;; ++i)
            {
                out.m_rewardCardNum[i] = ScanInt(&ok);       // (0xc8+i)*4+4 → +0x324+i*4
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward exp weight]")
        {
            for (int i = 1;; ++i)
            {
                out.m_illusionRewardExpWeight[i] = ScanFloat(&ok);  // (0x190+i)*4+4 → +0x644+i*4
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward card num]")
        {
            for (int i = 1;; ++i)
            {
                out.m_illusionRewardCardNum[i] = ScanInt(&ok);      // (0x258+i)*4+4 → +0x964+i*4
                if (!ok) { break; }
            }
        }
        else if (token == "[monster exp weight]")
        {
            out.m_monsterExpWeight = ScanFloat(&ok);  // fstps +0xc84
            if (!ok) { return 0x18b8; }
        }
        else if (token == "[reward item prob]")
        {
            int v1 = ScanInt(&ok);
            out.m_rewardItemProb[0] = v1;             // ORIG 先存储后校验（mov +0xc88 再判 ok）
            if (!ok) { return 0x18be; }
            int v2 = ScanInt(&ok);
            out.m_rewardItemProb[1] = out.m_rewardItemProb[0] + v2;
            if (!ok) { return 0x18c1; }
            int v3 = ScanInt(&ok);
            out.m_rewardItemProb[2] = out.m_rewardItemProb[1] + v3;
            if (!ok) { return 0x18c4; }
        }
        else if (token == "[reward gold weight]")
        {
            out.m_rewardGoldWeight = ScanFloat(&ok);  // fstps +0xc94
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
    // ORIG 双相派发（T 0x0895eda1）：函数级静态三件套位于主循环体内（ORIG
    // +0x8b/+0x107 每轮穿 guard），case 贯穿注册链按
    // [/create character gift]0x1ab→[apply job]0x1af→[item]0x1b5→
    // [new account event start time]0x1bc→[end time]0x1c2→[create title]0x1d2→
    // [create text]0x1d6→[/apply job]0x1da；执行形 0x1ab 段 = return true，
    // 时间转换 ret!=1 段 return false，EOF 出口 `movzbl -0x1d,%ebx` 读 ok 槽
    // （全函数无写入 ⇒ 恒 false）。
    // 收敛口径：orig=ours=637，助记符序列全等；残余仅 .o 未链接的 guard/静态
    // 对象/串字面量重定位显示（_ZZ…E8__lookup/11__init_lock/8__inited + 双
    // _ZGV guard 与 ORIG 同名，8 个 tag 串 .rodata 内容实测逐条一致），
    // 链接后按 AE 口径判等。
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                            // ORIG -0x108
    bool ok = false;                              // ORIG -0x1d（+0x4e 写，仅 EOF 出口读）
    createChracScript::rewardInfo info;           // ORIG -0x124
    int curJob = -1;                              // ORIG -0x128

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
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
            case 0x1ab:                      // [/create character gift]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[/create character gift]", 0x1ab));
                }
                else
                {
                    return true;              // ORIG +0x35c mov$1,%ebx
                }
                /* fall through（ORIG +0x35a jmp +0x366） */
            case 0x1af:                      // [apply job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[apply job]", 0x1af));
                }
                else
                {
                    std::string jobName;      // ORIG -0x130
                    ScanStr(&jobName);
                    curJob = getCharacterJobToEnumValue(jobName);
                    break;
                }
                /* fall through（+0x3c1 jmp +0x421） */
            case 0x1b5:                      // [item]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[item]", 0x1b5));
                }
                else
                {
                    int itemId = 0;           // ORIG -0x134（movl $0 先行）
                    ScanInt(&itemId);
                    info.m_items.push_back(itemId);
                    break;
                }
                /* fall through（+0x4ae jmp +0x4b3） */
            case 0x1bc:                      // [new account event start time]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[new account event start time]", 0x1bc));
                }
                else
                {
                    std::string str;          // ORIG -0x138
                    ScanStr(&str);
                    if (ConvertStringToTime(str, info.m_startTime) != 1)
                        return false;         // ORIG +0x597 mov$0,%ebx 出口
                    break;
                }
                /* fall through（+0x50e jmp +0x5e0） */
            case 0x1c2:                      // [new account event end time]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[new account event end time]", 0x1c2));
                }
                else
                {
                    std::string str;          // ORIG -0x13c
                    ScanStr(&str);
                    if (ConvertStringToTime(str, info.m_endTime) != 1)
                        return false;
                    break;
                }
                /* fall through（+0x63b jmp +0x70d） */
            case 0x1d2:                      // [create title]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create title]", 0x1d2));
                }
                else
                {
                    ScanStr(&info.m_title);   // ORIG info+0x14 直写
                    break;
                }
                /* fall through（+0x75c jmp +0x774） */
            case 0x1d6:                      // [create text]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create text]", 0x1d6));
                }
                else
                {
                    ScanStr(&info.m_text);    // ORIG info+0x18 直写
                    break;
                }
                /* fall through（+0x7c0 jmp +0x7d8） */
            case 0x1da:                      // [/apply job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/apply job]", 0x1da));
                }
                else
                {
                    if (curJob != -1)
                    {
                        std::map<int, createChracScript::rewardInfoList>::iterator it =
                            s->m_map0.find(curJob);
                        if (it == s->m_map0.end())
                        {
                            createChracScript::rewardInfoList list;
                            list.m_list.push_back(info);
                            s->m_map0.insert(std::make_pair(curJob, list));
                        }
                        else
                        {
                            it->second.m_list.push_back(info);
                        }
                    }
                    info.m_items.clear();     // ORIG +0x961..+0x9d8 复位六连
                    info.m_title.clear();
                    info.m_text.clear();
                    info.m_startTime = 0;
                    info.m_endTime = 0;
                    curJob = -1;
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_0895f783 公共尾。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return ok;                               // ORIG EOF 尾岛 movzbl -0x1d,%ebx
}

// ============================================================================
// importCharacterCreateCreatureGiftScript —— ORIG T 0x0896001a
//   bool (CharacterCreateCreatureGiftScript*, const char*)
// ORIG 双相派发（与 importTitleBookEtcScript/importAttendanceScript 同构）：
//   函数级静态 __lookup(map<const char*,int,less_TCHAR>)/__init_lock(SpinLock)/
//   __inited(bool) 三件套位于外层 token 循环体内（每轮穿过 guard 测试，
// ORIG +0x8e/+0x108），注册完成 SpinLock::leave + __inited=1 后 jmp +0x8e 以
// 当前令牌重查表派发。case 贯穿链按 [event job]→[start]→[end]→[/event job]→
// [change able creature lev]→[change infinity creature]→[client infinity
// creature] 顺序 insert(0x277/0x27d/0x281/0x285/0x28c/0x290/0x2a2)。
// 收敛口径：orig=ours=487，助记符序列全等、栈槽逐条一致；残余差异仅为
// .o 未链接的 guard/静态对象/串字面量重定位显示（.rodata 内容与 5 个局部
// 静态符号名 _ZZ…E8__lookup/11__init_lock/8__inited+双 guard 实测与 ORIG
// 一致），链接后按 AE 口径判等。
// ============================================================================
bool importCharacterCreateCreatureGiftScript(CharacterCreateCreatureGiftScript* s,
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

        // ORIG 重派发跳回 +0x8e（__lookup guard）：静态声明位于循环体内。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
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
    // ORIG T 0x0890e868（323 条；orig=ours=323，助记符序列仅 4 处 4.1 块放置
    // 顺序差：entry jmp/nop landing 前置与 EOF dtor 块位——跨版本 CFG 布局，
    // 无计数/调用/操作数缺口）。四 token 段 reward/quick 为**各自独立展开**
    // （ORIG 槽组 -0xbc/-0x28..-0x1c vs -0xc8/-0x18..-0xc 两套即证）；map/
    // multimap 提交为 make_pair(lvalue)+pair 转换 ctor+insert 三段 call 形
    // （P61/P123，value_type 直构会少一段）；[id] 节 ScanInt 失败 → break
    // 主循环 → return true；主循环外还有一个仅构造/析构的 dead string
    // （ORIG -0xa4）。
    std::string token;                 // ORIG -0xa0
    std::string unused;                // ORIG -0xa4（ctor/dtor 各一次，零引用）
    bool ok = false;                   // ORIG -0xa5

    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            goto loop_exit;                        // 共享 true 出口岛（ORIG +0x4b2）
        }

        if (token == "[id]")
        {
            int v = ScanInt(&ok);
            if (ok != true)
            {
                goto loop_exit;
            }
            out->m_field0 = v;                     // ORIG mov %edx,(%eax) → +0x00
        }
        else if (token == "[condition match]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // ORIG -0xac
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x30
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x2c
                if (ok != true) { break; }
                std::pair<int, int> value;         // 默认构造后 first/second 赋值（ORIG +0x14a 序）
                value.first = v1;
                value.second = v2;
                out->m_map8.insert(std::make_pair(key, value));
            }
        }
        else if (token == "[condition reward]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // -0xb8
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x28
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x24
                if (ok != true) { break; }
                int v3 = ScanInt(&ok);             // -0x20
                if (ok != true) { break; }
                int v4 = ScanInt(&ok);             // -0x1c
                if (ok != true) { break; }
                stConditionEventRewardInfo info;   // 默认构造（ORIG 0891b682 每轮）
                info.m_field0 = v1;
                info.m_field4 = v2;
                info.m_flag8 = (v3 != 0);          // cmpl$0;setne 直存
                info.m_flag9 = (v4 != 0);
                out->m_multiMap20.insert(std::make_pair(key, info));   // +0x20
            }
        }
        else if (token == "[condition quick party add reward]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // -0xc8
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x18
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x14
                if (ok != true) { break; }
                int v3 = ScanInt(&ok);             // -0x10
                if (ok != true) { break; }
                int v4 = ScanInt(&ok);             // -0xc
                if (ok != true) { break; }
                stConditionEventRewardInfo info;
                info.m_field0 = v1;
                info.m_field4 = v2;
                info.m_flag8 = (v3 != 0);
                info.m_flag9 = (v4 != 0);
                out->m_multiMap38.insert(std::make_pair(key, info));   // +0x38
            }
        }
    }
loop_exit:
    return true;
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
    // ORIG T 0x0890dbba（204 条；单出口 ebx 形——4.1 把所有失败/EOF 路径 merge
    // 到共享 dtor+return 链，4.4 的多 return 形会复制 3 组 string dtor ≈ +52。
    // 本形以 result 局部 + goto done 复现共享出口）。外层每轮消耗两个节头：
    // head1=[dblog buyitem]（flag 0）内层循环、head2=[dblog getitem]（flag 1）
    // 内层循环；内层 ScanInt 断流回下一节头/主循环顶；ScanStr/时间转换失败
    // 整体 false；两处 EOF 均 true。ConvertStringToTime 的 by-value 串参以块级
    // 临时物化、dtor 先于结果测试（ORIG +0x106..+0x11b 序）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    bool result = true;
    std::string token;                 // ORIG -0x20
    std::string str;                   // ORIG -0x24
    bool ok = false;                   // ORIG -0x25

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[dblog buyitem]")
        {
            for (;;)
            {
                stDBLogItem item;                  // ORIG memset(&item,0,0xc)
                memset(&item, 0, sizeof(item));
                int id = ScanInt(&ok);             // 结果先落槽再判（ORIG +0xb6）
                item.m_itemIdx = static_cast<unsigned int>(id);
                if (ok != true) { break; }
                if (!ScanStr(&str))
                {
                    result = false;
                    goto done;
                }
                bool convOk;
                {
                    std::string tmp(str);          // by-value 参物化（ORIG -0x1c）
                    convOk = (ConvertStringToTime(tmp, item.m_time) == 1);
                }
                if (!convOk)
                {
                    result = false;
                    goto done;
                }
                item.m_value = 0;                  // +0x08 memset 冗余重写 0
                out->push_back(item);
            }
        }

        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[dblog getitem]")
        {
            for (;;)
            {
                stDBLogItem item;
                memset(&item, 0, sizeof(item));
                int id = ScanInt(&ok);
                item.m_itemIdx = static_cast<unsigned int>(id);
                if (ok != true) { break; }
                if (!ScanStr(&str))
                {
                    result = false;
                    goto done;
                }
                bool convOk;
                {
                    std::string tmp(str);
                    convOk = (ConvertStringToTime(tmp, item.m_time) == 1);
                }
                if (!convOk)
                {
                    result = false;
                    goto done;
                }
                item.m_value = 1;                  // +0x08 写 1
                out->push_back(item);
            }
        }
    }
done:
    return result;
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
    // ORIG 0891826c：loadRDARScriptFile 先于任何局部构造（失败直接 ret 0x36f7，
    // 无 string 析构），随后才 basic_string() token（8918298）→ -0x14 置 0
    // （89182a3）→ ok=false（89182aa）。
    if (!loadRDARScriptFile("", path))
    {
        return 0x36f7;
    }

    std::string token;                              // ORIG -0x2c
    int nUnusedCounter = 0;                         // ORIG -0x14：仅 89182a3 置 0，全函数无其它引用
    bool ok = false;                                // ORIG -0x2d
    (void)nUnusedCounter;

    // ORIG 89182c9：ScanType 失败是 break 出外层循环（跳到 8918782 的
    // nop/mov $0x0,%ebx 尾块），不是就地 return。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[schedule]")
        {
            // ORIG 89182e6..8918318：5 个 std::string 在 [schedule] 分支入口
            // 一次性默认构造（-0x34/-0x38/-0x3c/-0x40/-0x44），循环体内复用，
            // 循环出口 89186a6.. 统一析构；不在每轮迭代内重建。
            std::string strStart;                   // -0x34
            std::string strEnd;                     // -0x38
            std::string strWeekday;                 // -0x3c
            std::string strStartTime;               // -0x40
            std::string strEndTime;                 // -0x44

            for (;;)
            {
                _DailySchedule entry;               // ORIG -0x6c
                memset(&entry, 0, sizeof(entry));   // 8918333 memset(...,0,0x24)

                // ORIG 891833e/891835b/891837e：第 1、3 枚 ScanInt 先落局部
                // （-0x10/-0xc）再回填字段，第 2 枚直接写 entry+0x04。
                int nEventId = ScanInt(&ok);        // -0x10
                if (!ok) { break; }                 // 三枚普通 ScanInt 失败仅回外层
                entry.m_type = ScanInt(&ok);        // +0x04（直接写字段）
                if (!ok) { break; }
                entry.m_eventId = nEventId;         // +0x00（8918372）
                int nIdx = ScanInt(&ok);            // -0xc
                if (!ok) { break; }
                entry.m_idx = nIdx;                 // +0x08（8918395）

                if (!ScanStr(&strStart))
                {
                    break;                          // 首串失败同样回外层（ORIG 89183ab）
                }
                if (!ScanStr(&strEnd))
                {
                    return 0x3721;
                }
                if (!ScanStr(&strWeekday))
                {
                    return 0x3723;
                }
                if (!ScanStr(&strStartTime))
                {
                    return 0x3725;
                }
                if (!ScanStr(&strEndTime))
                {
                    return 0x3727;
                }

                // ORIG 8918435/89184a4/8918513/891857e/89185fa：按值形参的
                // std::string 拷贝直接由实参构造（唯一一次 _ZNSsC1ERKSs），
                // 不存在额外的具名 tmp 中转。
                if (!ConvertStringToTime(strStart, *(long*)&entry.m_startSec))
                {
                    return 0x372a;                  // +0x0c
                }
                if (!ConvertStringToTime(strEnd, *(long*)&entry.m_endSec))
                {
                    return 0x372c;                  // +0x10
                }
                unsigned int wday = 0;              // ORIG -0x48（8918505）
                if (!ConvertWeekdayStringToInt(strWeekday, wday))
                {
                    return 0x3730;                  // +0x14
                }
                entry.m_wday = static_cast<int>(wday);
                // ORIG 8918589..89185a7：三个出参直接取 entry+0x18/0x19/0x1a
                if (!ConvertHHMMSSStringToInt(strStartTime, entry.m_startHour,
                                              entry.m_startMin, entry.m_startSec2))
                {
                    return 0x3735;
                }
                // ORIG 8918605..891861f：entry+0x1b/0x1c/0x1d
                if (!ConvertHHMMSSStringToInt(strEndTime, entry.m_endHour,
                                              entry.m_endMin, entry.m_endSec2))
                {
                    return 0x3737;
                }

                out.push_back(entry);               // 回 do 头继续下一记录
            }
        }
    }

    return 0;                                       // ORIG 8918783 mov $0x0,%ebx
}

// ============================================================================
// importDimensionActivationData —— ORIG T 0x89160f0
// ORIG 结构（/tmp/importDimAct.md 汇编+Ghidra C 双段实证）：
// 外层节名 = 函数级静态 __lookup map<char const*,int,less_TCHAR> +
// __init_lock(SpinLock) + __inited 双检贯穿 insert 建表链（5 条：
// "[play day count]"0x3488→"[coin info]"0x348d→"[section]"0x34b7→
// "[select group]"0x34ca→"[give group]"0x34ff），建表后 find 派发；
// coin info 难度 = 第二张静态 __lookup_0（4 条：[easy]0x3499→0、
// [medium]0x349e→1、[hard]0x34a3→2、[ultimate]0x34a8→3、未命中保持
// 入口 -1 且**照常**执行公共尾 insert），两表键均 less_TCHAR 大小写无关。
// 数据落点：[play day count]→out+0x00（ScanInt 返回值直写，不查 flag）；
// coinMap(+0x04) insert(coinType, DimensionCoinInfo{+0=首 ScanInt int、
// +4=二 ScanInt!=0 的 setne 字节})；[section] 装配 EventRewardSection 入
// m_rewardData.m_vec0(+0x1c)；[select group] 按 makeFairPVPEquipmentPackageKey
// (getCharacterJob,idx) 入 m_mapC(+0x28)；[give group] 按 (job,growIdx) 入
// m_map24(+0x40)。节内 ScanStr 失败 → return false；EOF(ScanType 失败) → true。
// ============================================================================
bool importDimensionActivationData(const char* path,
                                   DimensionActivationData* out)
{
    // ORIG 实证两张函数级静态表同为 map<char const*,int,less_TCHAR>。
    typedef std::map<const char*, int, less_TCHAR> DimSectionMap;
    typedef std::map<const char*, int, less_TCHAR> DimCoinDiffMap;

    bool ok = false;                    // ORIG local_1a5（ScanInt 出错 flag）
    std::string token;                  // ORIG local_1a0（节名）
    std::string sub;                    // ORIG local_1a4（子串）

    if (out == 0)
    {
        return false;                   // ORIG 0x89160fc 先于 load 判空
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)                            // ORIG LAB_0891615d 主循环
    {
        if (!ScanType(token, true))
        {
            return true;                // ORIG 0x8917388 EOF → 1
        }
        for (;;)                        // ORIG LAB_0891617e guard 重试
        {
            static DimSectionMap __lookup;      // ORIG ...E8__lookup
            static SpinLock __init_lock;        // ORIG ...E11__init_lock
            static bool __inited = false;       // ORIG ...E8__inited
            int idx;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;           // ORIG 0x891635e 回 guard 重检
                }
                idx = 0;
            }
            else
            {
                DimSectionMap::const_iterator it = __lookup.find(token.c_str());
                idx = (it != __lookup.end()) ? it->second : -1;
            }
            switch (idx)
            {
            case 0:
            case 0x3488:                // [play day count]
                if (__inited)
                {
                    out->m_field0 = ScanInt(&ok);  // ORIG 不查 flag
                    break;
                }
                __lookup.insert(
                    DimSectionMap::value_type("[play day count]", 0x3488));
                /* fall through */
            case 0x348d:                // [coin info]
                if (__inited)
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
                            if (!__inited_0)
                            {
                                __init_lock_0.enter();
                                if (__inited_0)
                                {
                                    __init_lock_0.leave();
                                    continue;
                                }
                                diff = 0;
                            }
                            else
                            {
                                DimCoinDiffMap::const_iterator it2 =
                                    __lookup_0.find(sub.c_str());
                                diff = (it2 != __lookup_0.end())
                                           ? it2->second : -1;
                            }
                            switch (diff)
                            {
                            case 0:
                            case 0x3499:
                                if (__inited_0) { coinType = 0; break; }
                                __lookup_0.insert(
                                    DimCoinDiffMap::value_type("[easy]", 0x3499));
                                /* fall through */
                            case 0x349e:
                                if (__inited_0) { coinType = 1; break; }
                                __lookup_0.insert(
                                    DimCoinDiffMap::value_type("[medium]", 0x349e));
                                /* fall through */
                            case 0x34a3:
                                if (__inited_0) { coinType = 2; break; }
                                __lookup_0.insert(
                                    DimCoinDiffMap::value_type("[hard]", 0x34a3));
                                /* fall through */
                            case 0x34a8:
                                if (__inited_0) { coinType = 3; break; }
                                __lookup_0.insert(
                                    DimCoinDiffMap::value_type("[ultimate]", 0x34a8));
                            }
                            if (__inited_0)
                            {
                                break;  // 未命中保持 -1，仍走公共尾 insert
                            }
                            __init_lock_0.leave();
                            __inited_0 = true;
                        }
                        coinInfo.m_field0 = ScanInt(&ok);        // -0x1b8
                        int used = ScanInt(&ok);                 // -0x1a8
                        coinInfo.m_field4 = (used != 0) ? 1 : 0; // setne 字节
                        // ORIG 0x8921dcd/0x8921e14：make_pair<int,DimensionCoinInfo>
                        // （按值实例化）+ value_type 转换 ctor 两级临时。
                        out->m_coinMap.insert(
                            std::map<int, DimensionCoinInfo>::value_type(
                                std::make_pair<int, DimensionCoinInfo>(
                                    coinType, coinInfo)));
                    }
                    break;
                }
                __lookup.insert(DimSectionMap::value_type("[coin info]", 0x348d));
                /* fall through */
            case 0x34b7:                // [section]
                if (__inited)
                {
                    for (;;)
                    {
                        EventRewardSection section;   // ctor 0x89154ee
                        section.m_field0 =
                            static_cast<unsigned char>(ScanInt(&ok));
                        if (ok)
                        {
                            break;      // ORIG 0x89169e8 flag 真 → 结束节块
                        }
                        section.m_field1 = ScanInt(static_cast<bool*>(0));
                        section.m_field2 = ScanInt(static_cast<bool*>(0));
                        int count = ScanInt(static_cast<bool*>(0));
                        for (int i = 0; i < count; ++i)
                        {
                            section.m_items.push_back(
                                static_cast<unsigned char>(
                                    ScanInt(static_cast<bool*>(0))));
                        }
                        out->m_rewardData.m_vec0.push_back(section);
                    }
                    break;
                }
                __lookup.insert(DimSectionMap::value_type("[section]", 0x34b7));
                /* fall through */
            case 0x34ca:                // [select group]
                if (__inited)
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
                            int first = 0;
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
                                    for (std::size_t k = 0;
                                         k < tokens.size(); ++k)
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
                __lookup.insert(
                    DimSectionMap::value_type("[select group]", 0x34ca));
                /* fall through */
            case 0x34ff:                // [give group]
                if (__inited)
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
                                    ScanInt(&ok);   // ORIG -0x1c 死存
                                    EventRewardItemInfo info;  // ctor 0x891548a 一次
                                    int value = 0;
                                    while (ScanInt(&value))
                                    {
                                        info.m_field0 = value;
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
                __lookup.insert(
                    DimSectionMap::value_type("[give group]", 0x34ff));
            }
            if (__inited)
            {
                break;                  // 派发完成 → 主循环下一 token
            }
            __init_lock.leave();        // ORIG 0x8917369 建表收尾重派发
            __inited = true;
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
                int v0 = ScanInt(&ok);
                if (ok != true) { break; }
                out->m_grade[0].push_back((unsigned int)v0);   // int->uint 转换临时槽（ORIG 两段）
            }
        }
        else if (token == "[grade_1]")
        {
            for (;;)
            {
                int v1 = ScanInt(&ok);
                if (ok != true) { break; }
                out->m_grade[1].push_back((unsigned int)v1);   // int->uint 转换临时槽（ORIG 两段）
            }
        }
        else if (token == "[grade_2]")
        {
            for (;;)
            {
                int v2 = ScanInt(&ok);
                if (ok != true) { break; }
                out->m_grade[2].push_back((unsigned int)v2);   // int->uint 转换临时槽（ORIG 两段）
            }
        }
        else if (token == "[grade_3]")
        {
            for (;;)
            {
                int v3 = ScanInt(&ok);
                if (ok != true) { break; }
                out->m_grade[3].push_back((unsigned int)v3);   // int->uint 转换临时槽（ORIG 两段）
            }
        }
        else if (token == "[grade_4]")
        {
            for (;;)
            {
                int v4 = ScanInt(&ok);
                if (ok != true) { break; }
                out->m_grade[4].push_back((unsigned int)v4);   // int->uint 转换临时槽（ORIG 两段）
            }
        }
    }
}
