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
// 验收：gap 5→1；P167 差集 11/11 归零；残余 1 行 = ORIG `mov $0,%eax;
// mov %eax,-0x10` 死初始化二段物化，4.4 恒折单条 movl（N18a 族）。
// ===========================================================================
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
    int level = 0;            // ORIG -0x14（movl $0 直存）
    int value = 0;            // ORIG -0x10（mov $0,%eax 二段；死初始化照抄 P171）
    bool ok = false;          // ORIG -0x19
    (void)value;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[gold limit from level]")
        {
            for (int j = 0; j <= 0x63; ++j)   // ORIG cmpl $0x63/setle（P2）
            {
                level = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG：LHS 地址先物化（ebx）→ ScanInt(0) → (%ebx) 直存
                out[level] = (unsigned int)ScanInt((bool*)0);
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
// 验收：P167 差集 ORIG-only=0；OURS-only 4×~stRangeElement = 4.1 对 user
// 空析构类 temp 消解 dtor 调用、4.4 判 non-trivial 必发 D1+EH cleanup
// （P71/P67 跨编译器结构性差异；ORIG D1/D2 W 0x8948ff0/0x89491a6 须保留，
// 不删用户析构）⇒ 标 N 封顶。
// ===========================================================================
int importInvalidName(char* path, InvalidCharData* out)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool okA;                               // ORIG -0x21..-0x26 无初始化（ScanInt 必写）
    bool okB;
    bool okC;
    bool okD;
    bool okE;
    bool okF;
    int lo;                                 // ORIG -0x10 跨块共享（含 one/two byte 值）
    int hi;                                 // ORIG -0xc 跨块共享
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[one byte range]")
        {
            for (;;)
            {
                lo = ScanInt(&okA);
                if (!okA)
                    break;
                hi = ScanInt(&okB);
                if (okA && okB)             // ORIG +0x99/+0xa1 双正向 test/je
                {
                    out->m_charRange.push_back(
                        InvalidCharData::stRangeElement<unsigned char>(
                            (unsigned char)lo, (unsigned char)hi));
                    continue;
                }
                break;
            }
        }
        else if (token == "[one byte]")
        {
            for (;;)
            {
                lo = ScanInt(&okC);
                if (!okC)
                    break;
                out->m_charList.push_back((unsigned char)lo);
            }
        }
        else if (token == "[two byte range]")
        {
            for (;;)
            {
                lo = ScanInt(&okD);
                if (!okD)
                    break;
                hi = ScanInt(&okE);
                if (okD && okE)             // ORIG +0x172/+0x17a 双正向 test/je
                {
                    out->m_shortRange.push_back(
                        InvalidCharData::stRangeElement<unsigned short>(
                            (unsigned short)lo, (unsigned short)hi));
                    continue;
                }
                break;
            }
        }
        else if (token == "[two byte]")
        {
            for (;;)
            {
                lo = ScanInt(&okF);
                if (!okF)
                    break;
                out->m_shortList.push_back((unsigned short)lo);
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
    bool ok;                      // ORIG -0x39 无初始化（ScanInt 必写）
    if (!ScanType(token, true))
        return 0;
    if (token == "[base table]")
    {
        for (;;)
        {
            int key = ScanInt(&ok);
            if (!ok)
                break;
            int val = ScanInt(&ok);
            if (!ok)
                return 0;
            // ORIG cmp $0x88b8/jle：有符号比较
            if (val > 0x88b8)
            {
                LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                      __FUNCTION__, 0x30b8,
                                      "ItemDictionary mappingTable MAX INDEX(40000) WARNING READY TO DB_TOOL");
                return 0;
            }
            // 依据 objdump 压栈序：second==false → (key,val)；true → (val,key)
            if (second)
                m.insert(std::make_pair(val, key));
            else
                m.insert(std::make_pair(key, val));
        }
        return 1;
    }
    return 0;
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
    std::string unused;   // ORIG -0x10：构造/析构但从未使用的第二 string（照抄）
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[max gage]")
        {
            // ORIG：store 先于 ok 检查（ScanInt 返回值 %eax→%edx 截 byte 直写 +0xc）
            bool okMax = false;                    // ORIG -0x11 独立槽
            out->m_cMaxBoosterGage = (unsigned char)ScanInt(&okMax);
            if (!okMax)
                break;
        }
        else if (token == "[sub gage]")
        {
            bool okSub = false;                    // ORIG -0x12 独立槽
            out->m_cFieldD = (unsigned char)ScanInt(&okSub);
            if (!okSub)
                break;
        }
        else if (token == "[reward multiple]")
        {
            bool okRw = false;                     // ORIG -0x13 独立槽
            out->m_cMultiboxCount = (unsigned char)ScanInt(&okRw);
            if (!okRw)
                break;
        }
        else if (token == "[booster point]")
        {
            BoosterPoint bp;                       // ORIG -0x19..-0x15 固定槽
            for (;;)
            {
                bool okBp = false;                 // ORIG 每轮 movb $0,-0x14
                bp.m_dwItemIdx = (unsigned long)ScanInt(&okBp);   // →-0x19
                if (!okBp)
                    break;
                bp.m_cPoint = (unsigned char)ScanInt(&okBp);      // al 直写 -0x15
                if (!okBp)
                    break;
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
// 验收：gap 12→1；差集 10/10 归零；残余 = ORIG 三态汇聚 ebx+终值 esi 双
// 寄存器物化（4.1 choose-without-store，N3/N76 族）。
// ===========================================================================
int importImageCommunicationInfo(const char* path, ImageCommunicationData* out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3233,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }
    // ORIG 0x08913dbc：循环体内每轮重置 isType(-0xd)/ok(-0xe)+构造 token(-0x14)、
    // 尾析构；ScanTypeOrInt 返回值落 -0xc 死槽（照抄 P171）。charge 段的
    // ScanInt 复用仍为 true 的 ok 槽（ORIG 不重置）、结果直写 *out 后查 ok。
    for (;;)
    {
        bool isType = false;
        bool ok = false;
        std::string token;
        int ret = ScanTypeOrInt(isType, token, ok, true);
        (void)ret;
        if (!ok)
            return 0;
        if (isType && token == "[charge]")
        {
            out->m_charge = ScanInt(&ok);
            if (!ok)
                return 0;
            return 1;
        }
        if (isType && token == "[/image communication]")
        {
            return 1;
        }
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
// importPremiumServiceData —— ORIG T 0x08912e47
// （docs/class_func_reports/_lt_global_gt_/importPremiumServiceData.md）
// 真形（ORIG 0x08912e47 汇编+Ghidra C 双段转写，P163/P165/P177/P178 族）：
// 外层字符串匹配 "[premium service]"，内层 while(ScanType) 每轮穿
// static 三件套（__lookup map<char const*,int,less_TCHAR> + __init_lock
// SpinLock + __inited，guard 声明位于重派发标签后）；未建表持锁 id=0 进
// switch，自 case 0（与 0x3059 共 leaf，P163）起逐 case 构造
// value_type(键, nId 具名槽) + insert 后**贯穿**下一 leaf，公共尾
// leave+__inited=1 后 jmp guard 以同 token 重查派发；建表后
// c_str→find→!=end 三目单点写 id，switch(id) 派发：
//   [type]0x3059→+0x00、[icon index]0x305c→+0x10、[icon explain]0x305f→
//   ScanStr(+0x14)、[add equipment list]0x3062→循环 push_back(+0x18)、
//   [main premium image index]0x306e→+0x04/+0x08 双 ScanInt、
//   [additional premium image index]0x3072→+0x0c、
//   [add selectAble equipment list]0x3076→双 ScanInt + insert(make_pair(a,
//     make_pair(false,b))) 嵌套实参形（+0x24 map）、
//   [/premium service]0x3088→out.push_back(data)+data.clear()。
// loadRDARScriptFile 失败记日志（0x3044）返 0，成功返 1。
// 验收（§6.2）：gap 158→1；P167 调用点差集 64/64 全命中；残余 1 insn =
// 4.4 在 token== 匹配后发射 `jmp <内层检查>` 中继 vs 4.1 fall-through
// （跨版本 nop;jmp 岛 vs jump-threading，N 类豁免）。
// ===========================================================================
int importPremiumServiceData(std::vector<PremiumServiceData>& out)
{
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    if (!loadRDARScriptFile("", "Etc/PremiumServiceEffect.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3044,
                              "CRITICAL ERROR : importPremiumServiceData load faild!");
        return 0;
    }
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
// ===========================================================================
// importCommonSkillIndexes —— ORIG T 0x08913f48
// （docs/class_func_reports/_lt_global_gt_/importCommonSkillIndexes.md）
// 清空两输出容器后按脚本填充："[common skill]" 后连续 ScanInt 进 vector<int>；
// "[except skill info]" 块内每轮 ScanStr(jobName)→getCharacterJob + 连续
// ScanInt 进 set<int>，插入 map<job,set>；loadRDARScriptFile 失败记日志返 0，
// 成功返 1。
// 验收：差集 35/35 归零；残余为 set/pair 临时链槽位序（N82）+ 块排布镜像。
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
    bool ok = false;                        // ORIG -0x55 函数级一次初始化
    for (;;)
    {
        if (!ScanType(token, true))
            return 1;
        if (token == "[common skill]")
        {
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
            std::set<int> skills;                       // ORIG -0x78 外层块构造一次
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (token == "[/except skill info]")
                    break;
                ENUM_CHARACTERJOB job = ENUM_CHARACTERJOB(0xb);   // ORIG 每轮 movl $0xb,-0x60
                skills.clear();
                std::string jobName;                    // ORIG -0x7c 每轮构造
                if (!ScanStr(&jobName))
                    return 0;
                job = ENUM_CHARACTERJOB(getCharacterJob(jobName.c_str()));
                bool okJob;                             // ORIG -0x7d 无初始化
                for (;;)
                {
                    int v = ScanInt(&okJob);
                    if (!okJob)
                        break;
                    skills.insert(v);
                }
                exceptSkill.insert(std::make_pair(job, skills));
            }
        }
        else if (token == "[/common skill]")
        {
        }
    }
}

// ===========================================================================
// importCancelSkillIndices —— ORIG T 0x08914dd1
// （docs/class_func_reports/_lt_global_gt_/importCancelSkillIndices.md）
// 真形（ORIG 0x08914dd1，同 importPremiumServiceData 双相派发族 P163/P177）：
// 外层 for(;;)+ScanType；每轮穿 static 三件套（__lookup map<char const*,int,
// less_TCHAR> + __init_lock SpinLock + __inited，声明位于 redo_lookup 标签后）；
// 未建表 enter 双检 id=0 进 switch（case 0 与 0x3384 共 leaf），自
// "[cancel skill]"0x3384 → "[/cancel skill]"0x3388 → "[character job]"0x338c →
// "[/character job]"0x33aa 逐 case 构造 value_type(键,字面量) + insert 后
// **贯穿**下一 leaf，公共尾 leave+__inited=1 重查派发。建表后仅 0x338c 有
// parse：jobName ScanStr 失败 return 0；key.first=getCharacterJob(c_str)；
// growName ScanStr 失败 return 0；key.second=getGrowTypeIndex；ScanInt 循环
// 进 skills，失败轮 out.insert(value_type(key,skills)) 直构形（无 make_pair
// 转换临时，OnlyOrigBatch14 口径）+ key=(0xb,-1) 重置 + skills.clear()。
// "[cancel skill]"/"[/cancel skill]"/"[/character job]" 命中均无动作（仅登记）。
// loadRDARScriptFile 失败记日志（0x3372）返 0，成功返 1。
// 验收：gap 52→1（413/412）；P167 差集 58/58 归零；残余=槽号平移+ctor/dtor
// 块序（N82/EH-merge 族）。
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
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
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
// ===========================================================================
// importAdditionalEffectListScript —— ORIG T 0x08907e3a
// （docs/class_func_reports/_lt_global_gt_/importAdditionalEffectListScript.md；
// 偏移经 objdump 复核：+0x00 index/+0x04 m_strings(animation)/+0x10 type/
// +0x14 fixX/+0x18/+0x1c/+0x20/+0x24/+0x28 各延迟项/+0x2c drawOver byte）
// 清空 g_additionalEffectMap_ 后按 "[effect]" 块解析各字段，
// "[/effect]" 时 g_additionalEffectMap_[index] = tmp。
// loadRDARScriptFile 失败记日志返 0，成功返 1。
// 验收：差集 40/40 归零；clean 225=225，残余为 clear/dtor 块序镜像
// （4.4 jump-threading 跨版本伪影，N 类）。
// ===========================================================================
int importAdditionalEffectListScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x172c, "%s", path);
        return 0;
    }
    std::string token;
    g_additionalEffectMap_.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            return 1;
        if (token == "[effect]")
        {
            AdditionalEffectData data;             // ORIG -0x40（每轮外层 body 构造）
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                          // EOF：仅析构 data，**不**写表
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
                std::string str;                   // ORIG -0x10 块级一次构造/尾析构
                for (;;)
                {
                    if (!ScanStr(&str))              // ORIG xor$1/test/jne 形
                        break;
                    data.m_strings.push_back(str);
                }
            }
            else if (token == "[/effect]")
            {
                g_additionalEffectMap_[data.m_id] = data;
                break;
            }
            }
        }
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
// 重建形 = ORIG 物理内联展开（P165 we-smaller 复原：8 个独立职业块、
// 各自 j/k 计数器，勿用 dst/rounds 参数化共享内层循环）。
// ===========================================================================
int importNoviceCommandScript(const char* path, stNoviceCommand& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x11cc;
    std::string token;
    for (int i = 0; i <= 0xa; ++i)   // ORIG cmpl $0xa/setle（P2）
        out.m_slots[i].clear();
    std::vector<int> row;
    int val;                          // ORIG 单槽 -0x60：各块共用
    for (;;)
    {
        if (!ScanType(token, true))
            break;                    // ORIG 出口岛在函数尾（共享 leave）
        if (token == "[gunner]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_GUNNER].push_back(row);
            }
        }
        else if (token == "[fighter]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_FIGHTER].push_back(row);
            }
        }
        else if (token == "[swordman]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_SWORDMAN].push_back(row);
            }
        }
        else if (token == "[at fighter]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_ATFIGHTER].push_back(row);
            }
        }
        else if (token == "[thief]")
        {
            for (int j = 0; j <= 0x5; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_THIEF].push_back(row);
            }
        }
        else if (token == "[mage]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_MAGE].push_back(row);
            }
        }
        else if (token == "[at gunner]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_ATGUNNER].push_back(row);
            }
        }
        else if (token == "[priest]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_PRIEST].push_back(row);
            }
        }
    }
    return 1;
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
// 验收：P167 差集 66/66 归零（op-> 13×、op[] 3×、find/end/clear/size/EPI
// 拷贝链逐点对应）；残余 = 循环出口多路 dtor 链块序与槽号
// （N82/EH merge 跨版本族）。
// ===========================================================================
int importEquipmentEffectSetInfo(const char* path, ENUM_MODULE_TYPE moduleType)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x16df, "%s", path);
        return 0;
    }
    std::string token;                    // ORIG 三 string 先构造（-0x34/-0x38/-0x3c）
    std::string name;
    std::string pieceName;
    g_equipmentEffectSetMap_.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[effect equipment part set]")
        {
            STEquipmentPartSetData data;
            data.m_field0 = ScanInt((bool*)0);
            ScanStr(&name);
            data.m_name4 = name;
            for (;;)
            {
                // ORIG：piece 每轮先构造（-0x64），ScanStr 失败也走析构；
                // piece.m_name 从不赋值（仅取 idx，-0x60=piece+4）
                STEquipmentPartSetPiece piece;
                if (!ScanStr(&pieceName))
                    break;
                piece.m_field4 = getSameStringIndex(pieceName.c_str(),
                                                    characterEquipmentTypeTags, 0x1a);
                data.m_pieces.push_back(piece);
                g_equipmentEffectSetMap_[data.m_field0] = data;
            }
        }
    }
    for (std::map<int, STEquipmentPartSetData>::iterator it =
             g_equipmentEffectSetMap_.begin();
         it != g_equipmentEffectSetMap_.end(); ++it)
    {
        // P157：ORIG 每次字段访问重调 _Rb_tree_iterator::operator->（共 13+ 次），
        // 禁缓存 it->second 引用；m_script 传参/删除前均重读。
        STEquipmentScript* pNew = new STEquipmentScript;
        it->second.m_script = pNew;
        if (!ImportEquipmentScript(it->second.m_script,
                                   it->second.m_name4.c_str(), false,
                                   (int)moduleType, it->first))
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  __FUNCTION__, 0x1712,
                                  "ERROR : \xc0\xe5\xba\xf1\xba\xce\xc0\xa7\xbc\xc2\xc6\xae \xb1\xb8\xbc\xba\xc7\xb0 %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
                                  it->second.m_name4.c_str());
            // ORIG 同一次 op-> 内读字段两遍（ebx 缓存）：具名指针缓存复现
            STEquipmentScript* pDel = it->second.m_script;
            if (pDel)
                delete pDel;                  // ORIG 不回写 m_script=0（P171 照抄）
            return 0;
        }
        // key=-1 档存在性检查（find 以 int& 具名槽传参，RS4_ 形）
        int kDel = -1;
        if (it->second.m_script->m_paramMap678.find(kDel) !=
            it->second.m_script->m_paramMap678.end())
        {
            int kNeg = -1;
            EquipmentParameterInfo tmp(
                it->second.m_script->m_paramMap678[kNeg]);   // op[] 引用拷贝构造（0x4bc 大 temp）
            it->second.m_script->m_paramMap678.clear();
            int nPieces = (int)it->second.m_pieces.size();
            it->second.m_script->m_paramMap678[nPieces] = tmp;
        }
    }
    return 1;
}
