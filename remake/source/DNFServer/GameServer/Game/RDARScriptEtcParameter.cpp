// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RDARScriptEtcParameter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CBoosterGage.h"        // BoosterGageData 真实布局 + BoosterPoint
#include "CDataManager.h"        // 目标成员类型：InvalidCharData/ImageCommunicationData
                                 // + m_moneyLimit/m_invalidChar/m_mapA928/m_mapA940/m_imageCommData
#include "PremiumServiceData.h"
#include "CEnvironment.h"       // SpinLock
#include "LogManager.h"
#include "CCompoundAvatar.h"   // ENUM_CHARACTERJOB
#include "NpcScript.h"         // getCharacterJob
#include <set>
#include "AdditionalEffectData.h"
#include "stNoviceCommand.h"
#include "sync_script.h"             // STEquipmentPartSetPiece / STEquipmentScript / EquipmentParameterInfo

// ---- 装备特效套装表（ORIG B 0x094f7148）----
// [SplitC batch10 · P2a 隔离件障碍清除] STEquipmentPartSetData 权威定义改由
// DataManagerInitDepends.h 提供（原 TU-local 镜像定义已删除；该类视图原与头一致）。
// 影子验证：键不变、四数逐位不变、符号集仅 +1 局部 static const（零 codegen 影响）。
#include "DataManagerInitDepends.h"
extern std::map<int, STEquipmentPartSetData> g_equipmentEffectSetMap_;
#include "RDARScriptEtcParameter/getSameStringIndex.inc"
extern const char* characterEquipmentTypeTags[26];

// ---- ORIG B 0x094f7148：装备特效套装表（importEquipmentEffectSetInfo 写入）----
std::map<int, STEquipmentPartSetData> g_equipmentEffectSetMap_;

// ---- ORIG B 0x094f7160：附加效果数据表（importAdditionalEffectListScript 写入）----
std::map<int, AdditionalEffectData> g_additionalEffectMap_;

#include "RDARScriptEtcParameter/getGrowTypeIndex.inc"

#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanStr
#include <string>
#include <map>
#include <cstring>

// ============================================================================
// stRangeElement 显式特化（ORIG 0x891f0ac/0x891f0f6 C1 双参构造 +
// 0x8948ff0/0x89491a6 空析构；CDataManager.h 冻结故在本 TU 特化）。
// ORIG importInvalidName push_back(stRangeElement<T>(lo,hi)) 右值形态
// （push_back(EOS2_) @0x891f0d4/0x891f122），依赖本特化提供双参构造。
// ============================================================================
template <>
struct InvalidCharData::stRangeElement<unsigned char>
{
    unsigned char m_min;
    unsigned char m_max;
    stRangeElement(unsigned char a, unsigned char b) : m_min(a), m_max(b) {}
};

template <>
struct InvalidCharData::stRangeElement<unsigned short>
{
    unsigned short m_min;
    unsigned short m_max;
    stRangeElement(unsigned short a, unsigned short b) : m_min(a), m_max(b) {}
};

#include "RDARScriptEtcParameter/importGoldLimitByLevel.inc"


#include "RDARScriptEtcParameter/importInvalidName.inc"


// ============================================================================
// importExperienceItemMapInfo —— ORIG 0x089136a4
// 报告 docs/class_func_reports/_lt_global_gt_/importExperienceItemMapInfo.md

#include "RDARScriptEtcParameter/importExperienceItemMapInfo.inc"


#include "RDARScriptEtcParameter/importBoosterGage.inc"


#include "RDARScriptEtcParameter/importImageCommunicationInfo.inc"


#include "RDARScriptEtcParameter/importPremiumServiceData.inc"
{
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    if (!loadRDARScriptFile("", "Etc/PremiumServiceEffect.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3044,
                              "CRITICAL ERROR : importPremiumServiceData load faild!");
        return 0;
    }
    char framePad[4];                             // [b28d-solo] P232 槽基探针
    std::string token;                            // ORIG -0xfc（先构造）
    PremiumServiceData data;                      // ORIG -0x150
    for (;;)
    {
        if (!ScanType(token, true))
            break;                                // ORIG 出口岛在函数尾（共享 leave）
        if (token == "[premium service]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                {
                scan_exit:
                    goto scan_done;
                }
            redo_lookup:
                static LookupMap __lookup;        // guard bss 0x094f71b8
                static SpinLock __init_lock;      // guard bss 0x094f71c0
                static bool __inited = false;     // bss 0x094f72d4
                int id;                           // ORIG -0x1c
                if (__inited)
                {
                    const char* key = token.c_str();     // ORIG -0xf4
                    LookupMap::const_iterator it = __lookup.find(key);
                    id = (it != __lookup.end()) ? it->second : -1;
                }
                else
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                        goto redo_lookup;
                    }
                    id = 0;
                }
                switch (id)
                {
                case 0:
                case 0x3059:                       // [type]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[type]", 0x3059));
                        }
                    else
                    {
                        data.m_type = ScanInt((bool*)0);
                        break;
                    }
                case 0x305c:                       // [icon index]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[icon index]", 0x305c));
                        }
                    else
                    {
                        data.m_iconIndex = ScanInt((bool*)0);
                        break;
                    }
                case 0x305f:                       // [icon explain]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[icon explain]", 0x305f));
                        }
                    else
                    {
                        ScanStr(&data.m_iconExplain);
                        break;
                    }
                case 0x3062:                       // [add equipment list]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[add equipment list]", 0x3062));
                        }
                    else
                    {
                        for (;;)
                        {
                            bool ok = false;      // ORIG 每轮 movb $0
                            int v = ScanInt(&ok);
                            if (!ok)
                                break;
                            data.m_addEquipmentList.push_back(v);
                        }
                        break;
                    }
                case 0x306e:                       // [main premium image index]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[main premium image index]", 0x306e));
                        }
                    else
                    {
                        data.m_mainImageIndex.first = ScanInt((bool*)0);
                        data.m_mainImageIndex.second = ScanInt((bool*)0);
                        break;
                    }
                case 0x3072:                       // [additional premium image index]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[additional premium image index]", 0x3072));
                        }
                    else
                    {
                        data.m_additionalImageIndex = ScanInt((bool*)0);
                        break;
                    }
                case 0x3076:                       // [add selectAble equipment list]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[add selectAble equipment list]", 0x3076));
                        }
                    else
                    {
                        for (;;)
                        {
                            bool ok = false;
                            int a = ScanInt(&ok);
                            if (!ok)
                                break;
                            int b = ScanInt(&ok);   // 复用 ok，不重置（ORIG +0x6b8）
                            if (!ok)
                                break;
                            data.m_selectAbleEquipmentList.insert(
                                std::make_pair(a, std::make_pair(false, b)));
                        }
                        break;
                    }
                case 0x3088:                       // [/premium service]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/premium service]", 0x3088));
                        }
                    else
                    {
                        out.push_back(data);
                        data.clear();
                        break;
                    }
                default:
                    break;
                }
                if (!__inited)                    // ORIG xor$1/test/je 回边形
                {
                    __init_lock.leave();
                    __inited = true;
                    goto redo_lookup;
                }
            }
        scan_done:
            ;
        }
    }
    return 1;
}
#include "RDARScriptEtcParameter/importCommonSkillIndexes.inc"


#include "RDARScriptEtcParameter/importCancelSkillIndices.inc"
    const char* path,
    std::map<std::pair<ENUM_CHARACTERJOB, int>, std::set<int> >& out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              // Q7S4: ORIG 0x08914dfe movl $0x8e103d8 = 完整前缀串（我方此前仅 "%s"）
                              __FUNCTION__, 0x3372,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    char framePad[4];                           // [b28d-solo] P232 槽基探针
    std::pair<ENUM_CHARACTERJOB, int> key;      // ORIG -0xb4 C2Ev + 直写两成员
    key.first = ENUM_CHARACTERJOB(0xb);         // -0xb4 movl $0xb
    key.second = -1;                            // -0xb0 movl $-1
    std::set<int> skills;                       // ORIG -0xcc
    std::string token;                          // ORIG -0xd0
    for (;;)
    {
        if (!ScanType(token, true))
            break;
    redo_lookup:
        static LookupMap __lookup;              // guard bss 0x094f71d8
        static SpinLock __init_lock;            // guard bss 0x094f71e0
        static bool __inited = false;           // bss 0x094f7294
        int id;                                 // ORIG -0x1c
        if (__inited)
        {
            const char* tk = token.c_str();     // ORIG -0xa8
            LookupMap::const_iterator it = __lookup.find(tk);
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto redo_lookup;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x3384:                            // [cancel skill]（无 parse）
            if (!__inited)
                            {
                                __lookup.insert(LookupMap::value_type("[cancel skill]", 0x3384));
                            }
                            else
                                break;
        case 0x3388:                            // [/cancel skill]（无 parse）
            if (!__inited)
                            {
                                __lookup.insert(LookupMap::value_type("[/cancel skill]", 0x3388));
                            }
                            else
                                break;
        case 0x338c:                            // [character job]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[character job]", 0x338c));
            }
            else
            {
                std::string jobName;            // ORIG -0xd8 块级
                if (!ScanStr(&jobName))
                    return 0;
                key.first = ENUM_CHARACTERJOB(getCharacterJob(jobName.c_str()));
                std::string growName;           // ORIG -0xdc
                if (!ScanStr(&growName))
                    return 0;
                key.second = getGrowTypeIndex(growName.c_str());
                bool ok;                        // ORIG -0xdd 无初始化
                for (;;)
                {
                    int v = ScanInt(&ok);       // ORIG -0xe4
                    if (!ok)
                    {
                        out.insert(std::map<std::pair<ENUM_CHARACTERJOB, int>,
                                           std::set<int> >::value_type(key, skills));
                        key.first = ENUM_CHARACTERJOB(0xb);
                        key.second = -1;
                        skills.clear();
                        break;
                    }
                    skills.insert(v);
                }
                break;
            }
        case 0x33aa:                            // [/character job]（无 parse）
            if (!__inited)
                            {
                                __lookup.insert(LookupMap::value_type("[/character job]", 0x33aa));
                            }
                            else
                                break;
        default:
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto redo_lookup;
        }
    }
    return 1;
}
#include "RDARScriptEtcParameter/importAdditionalEffectListScript.inc"

#include "RDARScriptEtcParameter/importNoviceCommandScript.inc"

#include "RDARScriptEtcParameter/importEquipmentEffectSetInfo.inc"

