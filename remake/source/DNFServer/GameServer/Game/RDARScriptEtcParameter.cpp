// ============================================================================
// df_game_r 还原 —— RDARScriptEtcParameter.cpp 域（自由脚本 producer）
//
// CDataManager::init（0x08358332..0x0835f1d5）调用的若干自由脚本装载函数。
// 本节逐一转写自 docs/class_func_reports/_lt_global_gt_/ 对应报告：
//   importGoldLimitByLevel       0x088fdb69
//   importInvalidName            0x08908338
//   importExperienceItemMapInfo  0x089136a4
//   importBoosterGage            0x08913a7e
//   importImageCommunicationInfo 0x08913dbc
//
// 每个函数返回 int：0=失败（文件加载失败/键缺失/阈值超限），1=成功。
// 数据写入由调用方（CDataManager::init）以 this+offset 传入目标成员地址。
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
// STEquipmentPartSetData 类视图与 DataManagerInitDepends.h 权威声明一致
// （+0x00 int / +0x04 string / +0x08 STEquipmentScript* / +0x0c vector<piece>），
// 成员函数定义归 InitScriptProducers_2.cpp（importEquipmentPartSetInfo 生产 TU）。
class STEquipmentPartSetData
{
public:
    STEquipmentPartSetData();
    ~STEquipmentPartSetData();

    int m_field0;                                   // +0x00（ctor 置 -1）
    std::string m_name4;                            // +0x04
    STEquipmentScript* m_script;                    // +0x08（dtor delete）
    std::vector<STEquipmentPartSetPiece> m_pieces;  // +0x0c
};

extern std::map<int, STEquipmentPartSetData> g_equipmentEffectSetMap_;
int getSameStringIndex(const char* s, const char** table, int count);
extern const char* characterEquipmentTypeTags[26];

// ---- ORIG B 0x094f7148：装备特效套装表（importEquipmentEffectSetInfo 写入）----
std::map<int, STEquipmentPartSetData> g_equipmentEffectSetMap_;

// ---- ORIG B 0x094f7160：附加效果数据表（importAdditionalEffectListScript 写入）----
std::map<int, AdditionalEffectData> g_additionalEffectMap_;

// ---- 跨 TU 依赖声明 ----
int getGrowTypeIndex(const char* name);  // InitScriptProducers_6.cpp（ORIG T 0x088be63d）

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
    ~stRangeElement() {}
};

template <>
struct InvalidCharData::stRangeElement<unsigned short>
{
    unsigned short m_min;
    unsigned short m_max;
    stRangeElement(unsigned short a, unsigned short b) : m_min(a), m_max(b) {}
    ~stRangeElement() {}
};

// ============================================================================
// importGoldLimitByLevel —— ORIG 0x088fdb69
// 报告 docs/class_func_reports/_lt_global_gt_/importGoldLimitByLevel.md
// 调用点（init.md 0x832…+0x2e4b）：CDataManager::init → this+0x6410
//   （CDataManager::m_moneyLimit[201]，int 数组）。
// 目标为 unsigned int*（报告 uint*），访问 out[level]。
// ============================================================================
int importGoldLimitByLevel(unsigned int* out)
{
    // 清前 0x190（400）字节（与汇编 movl $0x190 一致；m_moneyLimit 数组更大，
    // 但 ORIG 仅清零 400 字节）。ORIG 0x088fdb69 无空指针检查（0x8fdb71 直接 memset）。
    std::memset(out, 0, 0x190);

    if (!loadRDARScriptFile("", "Etc/GoldLimitByLevel.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xe3,
                              "CRITICAL ERROR : \xec\xa0\x84\xec\xa0\x90\xeb\xb6\x84 \xea\xb8\x88\xec\x95\xa1 \xec\x86\x8c\xec\xa7\x80\xec\xa0\x9c\xed\x95\x9c \xec\xa0\x95\xeb\xb3\xb4 \xeb\xa1\x9c\xeb\x93\x9c \xec\x8b\xa4\xed\x8c\xa8!");
        return 0;
    }

    std::string token;
    int count = 0;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[gold limit from level]")
        {
            count = 0;
            while (count < 100)
            {
                int level = ScanInt(&ok);
                if (!ok)
                    break;
                int value = ScanInt((bool*)0);   // 第二值（不关心成功标志）
                out[level] = (unsigned int)value;
                ++count;
            }
        }
    }
    return 1;
}

// ============================================================================
// importInvalidName —— ORIG 0x08908338
// 报告 docs/class_func_reports/_lt_global_gt_/importInvalidName.md
// 调用点（init.md +0x2279）：CDataManager::init → this+0x86c4
//   （CDataManager::m_invalidChar，InvalidCharData）。
// 布局：+0x00 vector<unsigned char> m_charList
//       +0x0c vector<stRangeElement<unsigned char>> m_charRange
//       +0x18 vector<unsigned short> m_shortList
//       +0x24 vector<stRangeElement<unsigned short>> m_shortRange
// ============================================================================
int importInvalidName(char* path, InvalidCharData* out)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool okA = false;
    bool okB = false;
    bool okC = false;
    bool okD = false;
    bool okE = false;
    bool okF = false;

    while (ScanType(token, true))
    {
        if (token == "[one byte range]")
        {
            while (true)
            {
                int lo = ScanInt(&okA);
                if (!okA)
                    break;
                int hi = ScanInt(&okB);
                if (!okB)
                    break;
                out->m_charRange.push_back(
                    InvalidCharData::stRangeElement<unsigned char>(
                        (unsigned char)lo, (unsigned char)hi));
            }
        }
        else if (token == "[one byte]")
        {
            while (true)
            {
                int v = ScanInt(&okC);
                if (!okC)
                    break;
                out->m_charList.push_back((unsigned char)v);
            }
        }
        else if (token == "[two byte range]")
        {
            while (true)
            {
                int lo = ScanInt(&okD);
                if (!okD)
                    break;
                int hi = ScanInt(&okE);
                if (!okE)
                    break;
                out->m_shortRange.push_back(
                    InvalidCharData::stRangeElement<unsigned short>(
                        (unsigned short)lo, (unsigned short)hi));
            }
        }
        else if (token == "[two byte]")
        {
            while (true)
            {
                int v = ScanInt(&okF);
                if (!okF)
                    break;
                out->m_shortList.push_back((unsigned short)v);
            }
        }
    }
    return 1;
}

// ============================================================================
// importExperienceItemMapInfo —— ORIG 0x089136a4
// 报告 docs/class_func_reports/_lt_global_gt_/importExperienceItemMapInfo.md

// ============================================================================
// importExperienceItemMapInfo —— ORIG 0x089136a4
// 报告 docs/class_func_reports/_lt_global_gt_/importExperienceItemMapInfo.md
// 调用点（init.md +0x181c / +0x1891）：
//   init → this+0xa928（m_mapA928），bool=0
//   init → this+0xa940（m_mapA940），bool=1
// 阈值：第二个值 > 0x88b8（35000）时 log 报错返回 0。
// OBJDUMP 复核（0x89137b2 true 分支 / 0x8913801 false 分支）：
//   true (param_2==true, -0x4c(%ebp)!=0):
//     make_pair(&local_28 {first=-0x44(level), second=-0x48(val)})  → insert
//   false:
//     make_pair(&local_10 {first=-0x48(val), second=-0x44(level)}) → insert
//   即 second==true → insert(val,key)；second==false → insert(key⋯ 实际以
//   实参压栈序为准；下述实现按 false 存 (key,val)、true 存 (val,key)。
// 注意报告 Ghidra 的 make_pair 两分支实参顺序相反，以 objdump 为准：
//   false 分支把 -0x48(val) 放 first、-0x44(level) 放 second；true 分支相反。
// ============================================================================
int importExperienceItemMapInfo(std::map<int, int, std::less<int>,
                                std::allocator<std::pair<int const, int> > >& m,
                                bool second)
{
    m.clear();

    if (!loadRDARScriptFile("Etc/ItemDictionary/", "(R)baseItemDictionaryTable.etc"))
        return 0;

    std::string token;
    bool ok = false;

    if (!ScanType(token, true))
        return 0;
    if (token != "[base table]")
        return 0;

    while (true)
    {
        int key = ScanInt(&ok);
        if (!ok)
            break;
        int val = ScanInt(&ok);
        if (!ok)
            return 0;

        // 阈值检查：val > 0x88b8（35000）
        if ((unsigned)val > 0x88b8u)
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  __FUNCTION__, 0x30b8,
                                  "ItemDictionary mappingTable MAX INDEX(40000) WARNING READY TO DB_TOOL");
            return 0;
        }

        // 依据 objdump：second==false → (key,val)；second==true → (val,key)
        if (second)
            m.insert(std::make_pair(val, key));
        else
            m.insert(std::make_pair(key, val));
    }
    return 1;
}

// ============================================================================
// importBoosterGage —— ORIG 0x08913a7e
// 报告 docs/class_func_reports/_lt_global_gt_/importBoosterGage.md
// 调用点（init.md +0x2412）：CDataManager::init → this+0x8770
//   （CDataManager::m_boosterGage，BoosterGageData）。
// 布局：+0x00 vector<BoosterPoint> m_BoosterPoint
//       +0x0c unsigned char m_cMaxBoosterGage
//       +0x0d unsigned char m_cFieldD
//       +0x0e unsigned char m_cMultiboxCount
// BoosterPoint：+0x00 unsigned long m_dwItemIdx / +0x04 unsigned char m_cPoint
// ============================================================================
int importBoosterGage(char* path, BoosterGageData* out)
{
    if (!out)
        return 0;

    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[max gage]")
        {
            int v = ScanInt(&ok);
            if (!ok)
                break;
            out->m_cMaxBoosterGage = (unsigned char)v;
        }
        else if (token == "[sub gage]")
        {
            int v = ScanInt(&ok);
            if (!ok)
                break;
            out->m_cFieldD = (unsigned char)v;
        }
        else if (token == "[reward multiple]")
        {
            int v = ScanInt(&ok);
            if (!ok)
                break;
            out->m_cMultiboxCount = (unsigned char)v;
        }
        else if (token == "[booster point]")
        {
            while (true)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    break;
                int point = ScanInt(&ok);
                if (!ok)
                    break;
                BoosterPoint bp;
                bp.m_dwItemIdx = (unsigned long)idx;
                bp.m_cPoint = (unsigned char)point;
                out->m_BoosterPoint.push_back(bp);
            }
        }
    }
    return 1;
}

// ============================================================================
// importImageCommunicationInfo —— ORIG 0x08913dbc
// 报告 docs/class_func_reports/_lt_global_gt_/importImageCommunicationInfo.md
// 调用点（init.md +0x0966）：CDataManager::init → this+0xb4c4
//   （CDataManager::m_imageCommData，ImageCommunicationData，写 +0x00 int）。
// 段键："[charge]" 写首字；"[/image communication]" 结束返回 1。
// ============================================================================
int importImageCommunicationInfo(const char* path, ImageCommunicationData* out)
{
    if (!out)
        return 0;

    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3233,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }

    std::string token;
    bool isType = false;
    bool isInt = false;

    // 报告 do-while：每次 ScanTypeOrInt 得 token（返回值=解析标量，不能作为
    // 成功标志；用 isInt 出参判定）。isInt==false（EOF/非法）→ 返回 0。
    //   isType && "[charge]"           → 写首字，返回 1。
    //   isType && "[/image communication]" → 返回 1（结束段）。
    //   其它 type/数值 token            → 继续循环。
    while (true)
    {
        ScanTypeOrInt(isType, token, isInt, true);
        if (!isInt)
            return 0;
        if (isType && token == "[charge]")
        {
            bool ok = false;
            int v = ScanInt(&ok);
            if (!ok)
                return 0;
            out->m_charge = v;
            return 1;
        }
        if (isType && token == "[/image communication]")
        {
            return 1;
        }
        // 其它 token：继续。
    }
}

// ===========================================================================
// 装备部件套装（ORIG "../RDARScriptEtcParameter.cpp" 域）——本 TU 不再持有。
//
// 唯一定义归 InitScriptProducers_2.cpp（importEquipmentPartSetInfo 生产 TU，
// DataManagerInitDepends.h 权威声明指定）：STEquipmentPartSetData 类视图 +
// C1 0x0891b002 W / D1 0x08907348 T + g_equipmentPartSetMap_（B 0x094f7130）。
// 本文件此前与该 TU 三重重复（wave22 链接 multiple definition），已删除；
// 本 TU 无任何消费点，无需前向声明。
// ===========================================================================
// ===========================================================================
// importPremiumServiceData —— ORIG T 0x08912e47
// （docs/class_func_reports/_lt_global_gt_/importPremiumServiceData.md）
// 双阶段：首次调用（__inited==0，SpinLock 双检）建 token→偏移查表
// （[type]0x3059/[icon index]0x305c/[icon explain]0x305f/[add equipment list]
// 0x3062/[main premium image index]0x306e/[additional premium image index]
// 0x3072/[add selectAble equipment list]0x3076/[/premium service]0x3088），
// 置 __inited=1；后续调用查表分派写入 PremiumServiceData 对应成员，
// "[/premium service]" 时 push_back 并 clear 临时对象。
// loadRDARScriptFile 失败记日志返 0，成功返 1。
// ===========================================================================
int importPremiumServiceData(std::vector<PremiumServiceData>& out)
{
    if (!loadRDARScriptFile("", "Etc/PremiumServiceEffect.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3044,
                              "CRITICAL ERROR : importPremiumServiceData load faild!");
        return 0;
    }
    static std::map<const char*, int> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    PremiumServiceData data;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        __init_lock.enter();
        if (!__inited)
        {
            __lookup["[type]"] = 0x3059;
            __lookup["[icon index]"] = 0x305c;
            __lookup["[icon explain]"] = 0x305f;
            __lookup["[add equipment list]"] = 0x3062;
            __lookup["[main premium image index]"] = 0x306e;
            __lookup["[additional premium image index]"] = 0x3072;
            __lookup["[add selectAble equipment list]"] = 0x3076;
            __lookup["[/premium service]"] = 0x3088;
            __inited = true;
            __init_lock.leave();
            continue;
        }
        __init_lock.leave();
        std::map<const char*, int>::const_iterator it = __lookup.find(token.c_str());
        int off = (it == __lookup.end()) ? -1 : it->second;
        switch (off)
        {
        case 0x3059:
            data.m_type = ScanInt((bool*)0);
            break;
        case 0x305c:
            data.m_iconIndex = ScanInt((bool*)0);
            break;
        case 0x305f:
            ScanStr(&data.m_iconExplain);
            break;
        case 0x3062:
        {
            bool ok = false;
            for (;;)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                data.m_addEquipmentList.push_back(v);
            }
            break;
        }
        case 0x306e:
            data.m_mainImageIndex.first = ScanInt((bool*)0);
            data.m_mainImageIndex.second = ScanInt((bool*)0);
            break;
        case 0x3072:
            data.m_additionalImageIndex = ScanInt((bool*)0);
            break;
        case 0x3076:
        {
            bool ok = false;
            for (;;)
            {
                int a = ScanInt(&ok);
                if (!ok)
                    break;
                int b = ScanInt(&ok);
                if (!ok)
                    break;
                data.m_selectAbleEquipmentList[a] = std::make_pair(false, b);
            }
            break;
        }
        case 0x3088:
            out.push_back(data);
            data.clear();
            break;
        default:
            break;
        }
    }
    return 1;
}
// ===========================================================================
// importCommonSkillIndexes —— ORIG T 0x08913f48
// （docs/class_func_reports/_lt_global_gt_/importCommonSkillIndexes.md）
// 清空两输出容器后按脚本填充："[common skill]" 后连续 ScanInt 进 vector<int>；
// "[except skill info]" 块内每轮 ScanStr(jobName)→getCharacterJob + 连续
// ScanInt 进 set<int>，插入 map<job,set>；loadRDARScriptFile 失败记日志返 0，
// 成功返 1。
// ===========================================================================
int importCommonSkillIndexes(
    const char* path,
    std::vector<int>& commonSkill,
    std::map<ENUM_CHARACTERJOB, std::set<int> >& exceptSkill)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x329a, "%s", path);
        return 0;
    }
    commonSkill.clear();
    exceptSkill.clear();
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return 1;
        if (token == "[common skill]")
        {
            bool ok = false;
            for (;;)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                commonSkill.push_back(v);
            }
        }
        else if (token == "[except skill info]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (token == "[/except skill info]")
                    break;
                std::set<int> skills;
                std::string jobName;
                if (!ScanStr(&jobName))
                    return 0;
                ENUM_CHARACTERJOB job = ENUM_CHARACTERJOB(getCharacterJob(jobName.c_str()));
                bool ok = false;
                for (;;)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                        break;
                    skills.insert(v);
                }
                exceptSkill.insert(std::make_pair(job, skills));
            }
        }
    }
}

// ===========================================================================
// importCancelSkillIndices —— ORIG T 0x08914dd1
// （docs/class_func_reports/_lt_global_gt_/importCancelSkillIndices.md）
// 双阶段（SpinLock 双检 + static lookup 表）：
//   "[cancel skill]"0x3384 / "[/cancel skill]"0x3388 /
//   "[character job]"0x338c / "[/character job]"0x33aa；
// 首次调用建表后置 __inited=1；后续 "[character job]" 块内
// ScanStr(job)→getCharacterJob、ScanStr(grow)→getGrowTypeIndex、
// 连续 ScanInt 进 set<int>，insert map<pair<job,grow>,set> 后重置
// key=(0xb,-1)、set.clear()。loadRDARScriptFile 失败记日志返 0，成功返 1。
// ===========================================================================
int importCancelSkillIndices(
    const char* path,
    std::map<std::pair<ENUM_CHARACTERJOB, int>, std::set<int> >& out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3372, "%s", path);
        return 0;
    }
    static std::map<const char*, int> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    std::string token;
    std::pair<ENUM_CHARACTERJOB, int> key(ENUM_CHARACTERJOB(0xb), -1);
    std::set<int> skills;
    for (;;)
    {
        if (!ScanType(token, true))
            return 1;
        __init_lock.enter();
        if (!__inited)
        {
            __lookup["[cancel skill]"] = 0x3384;
            __lookup["[/cancel skill]"] = 0x3388;
            __lookup["[character job]"] = 0x338c;
            __lookup["[/character job]"] = 0x33aa;
            __inited = true;
            __init_lock.leave();
            continue;
        }
        __init_lock.leave();
        std::map<const char*, int>::const_iterator it = __lookup.find(token.c_str());
        int off = (it == __lookup.end()) ? -1 : it->second;
        if (off == 0x338c)
        {
            std::string jobName;
            std::string growName;
            if (!ScanStr(&jobName))
                return 0;
            key.first = ENUM_CHARACTERJOB(getCharacterJob(jobName.c_str()));
            if (!ScanStr(&growName))
                return 0;
            key.second = getGrowTypeIndex(growName.c_str());
            bool ok = false;
            for (;;)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                skills.insert(v);
            }
            out.insert(std::make_pair(key, skills));
            key.first = ENUM_CHARACTERJOB(0xb);
            key.second = -1;
            skills.clear();
        }
    }
}
// ===========================================================================
// importAdditionalEffectListScript —— ORIG T 0x08907e3a
// （docs/class_func_reports/_lt_global_gt_/importAdditionalEffectListScript.md；
// 偏移经 objdump 复核：+0x00 index/+0x04 m_strings(animation)/+0x10 type/
// +0x14 fixX/+0x18/+0x1c/+0x20/+0x24/+0x28 各延迟项/+0x2c drawOver byte）
// 清空 g_additionalEffectMap_ 后按 "[effect]" 块解析各字段，
// "[/effect]" 时 g_additionalEffectMap_[index] = tmp。
// loadRDARScriptFile 失败记日志返 0，成功返 1。
// ===========================================================================
int importAdditionalEffectListScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x172c, "%s", path);
        return 0;
    }
    g_additionalEffectMap_.clear();
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return 1;
        if (token != "[effect]")
            continue;
        AdditionalEffectData data;
        for (;;)
        {
            if (!ScanType(token, true))
                break;
            if (token == "[index]")
                data.m_id = ScanInt((bool*)0);
            else if (token == "[type]")
                data.m_mode = ScanInt((bool*)0);
            else if (token == "[draw over]")
                data.m_flag = (unsigned char)(ScanInt((bool*)0) != 0);
            else if (token == "[fix position x]")
                data.m_count = ScanInt((bool*)0);
            else if (token == "[custom ani delay]")
                data.m_min = ScanInt((bool*)0);
            else if (token == "[custom ani delay in town]")
                data.m_max = ScanInt((bool*)0);
            else if (token == "[default term in town]")
                data.m_valueA = ScanInt((bool*)0);
            else if (token == "[default term]")
                data.m_valueB = ScanInt((bool*)0);
            else if (token == "[dash term]")
                data.m_valueC = ScanInt((bool*)0);
            else if (token == "[animation]")
            {
                std::string str;
                while (ScanStr(&str))
                    data.m_strings.push_back(str);
            }
            else if (token == "[/effect]")
                break;
        }
        g_additionalEffectMap_[data.m_id] = data;
    }
}
// ===========================================================================
// importNoviceCommandScript —— ORIG T 0x0890402a
// （docs/class_func_reports/_lt_global_gt_/importNoviceCommandScript.md）
// 先清 0xb 个 vector<vector<int>> 槽；按职业 token 逐轮 push_back：
// gunner→+0x18(10 轮)/fighter→+0x0c(8)/swordman→+0x00(8)/
// at fighter→+0x54(8)/thief→+0x48(6)/mage→+0x24(10)/
// at gunner→+0x3c(10)/priest→+0x30(10)，每轮内层 10 个 ScanInt。
// loadRDARScriptFile 失败返 0x11cc，成功返 1。
// ===========================================================================
int importNoviceCommandScript(const char* path, stNoviceCommand& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x11cc;
    for (int i = 0; i < 0xb; ++i)
        reinterpret_cast<std::vector<std::vector<int> >*>(&out)[i].clear();
    std::string token;
    std::vector<int> row;
    for (;;)
    {
        if (!ScanType(token, true))
            return 1;
        std::vector<std::vector<int> >* dst = 0;
        int rounds = 0;
        if (token == "[gunner]")          { dst = &out.m_gunner; rounds = 10; }
        else if (token == "[fighter]")    { dst = &out.m_fighter; rounds = 8; }
        else if (token == "[swordman]")   { dst = &out.m_swordman; rounds = 8; }
        else if (token == "[at fighter]") { dst = &out.m_atFighter; rounds = 8; }
        else if (token == "[thief]")      { dst = &out.m_thief; rounds = 6; }
        else if (token == "[mage]")       { dst = &out.m_mage; rounds = 10; }
        else if (token == "[at gunner]")  { dst = &out.m_atGunner; rounds = 10; }
        else if (token == "[priest]")     { dst = &out.m_priest; rounds = 10; }
        if (dst == 0)
            continue;
        for (int r = 0; r < rounds; ++r)
        {
            row.clear();
            for (int c = 0; c < 10; ++c)
                row.push_back(ScanInt((bool*)0));
            dst->push_back(row);
        }
    }
}
// ===========================================================================
// importEquipmentEffectSetInfo —— ORIG T 0x0890793f
// （docs/class_func_reports/_lt_global_gt_/importEquipmentEffectSetInfo.md）
// 阶段一：解析 "[effect equipment part set]" 块 → STEquipmentPartSetData
//   （m_field0=ScanInt、m_name4=ScanStr、m_pieces 每件
//   getSameStringIndex(characterEquipmentTypeTags,0x1a)），写入
//   g_equipmentEffectSetMap_[id]；阶段二：遍历生成 STEquipmentScript 并
//   ImportEquipmentScript(script, name, false, moduleType, id)，失败记日志
//   （0x1712）删除后返 0；m_paramMap678 若含 key=-1 档则复制后清空，
//   以套装件数为 key 重存。loadRDARScriptFile 失败记日志（0x16df）返 0。
// ===========================================================================
int importEquipmentEffectSetInfo(const char* path, ENUM_MODULE_TYPE moduleType)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x16df, "%s", path);
        return 0;
    }
    g_equipmentEffectSetMap_.clear();
    std::string token;
    std::string name;
    std::string pieceName;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token != "[effect equipment part set]")
            continue;
        STEquipmentPartSetData data;
        data.m_field0 = ScanInt((bool*)0);
        ScanStr(&name);
        data.m_name4 = name;
        for (;;)
        {
            if (!ScanStr(&pieceName))
                break;
            STEquipmentPartSetPiece piece;
            piece.m_name = pieceName;
            piece.m_field4 = getSameStringIndex(pieceName.c_str(),
                                                characterEquipmentTypeTags, 0x1a);
            data.m_pieces.push_back(piece);
            g_equipmentEffectSetMap_[data.m_field0] = data;
        }
    }
    for (std::map<int, STEquipmentPartSetData>::iterator it =
             g_equipmentEffectSetMap_.begin();
         it != g_equipmentEffectSetMap_.end(); ++it)
    {
        STEquipmentScript* script = new STEquipmentScript;
        it->second.m_script = script;
        if (!ImportEquipmentScript(script, it->second.m_name4.c_str(), false,
                                   (int)moduleType, it->first))
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  __FUNCTION__, 0x1712, "%s",
                                  it->second.m_name4.c_str());
            delete script;
            it->second.m_script = 0;
            return 0;
        }
        std::map<int, EquipmentParameterInfo>::iterator found =
            script->m_paramMap678.find(-1);
        if (found != script->m_paramMap678.end())
        {
            EquipmentParameterInfo tmp = found->second;
            script->m_paramMap678.clear();
            script->m_paramMap678[(int)it->second.m_pieces.size()] = tmp;
        }
    }
    return 1;
}
