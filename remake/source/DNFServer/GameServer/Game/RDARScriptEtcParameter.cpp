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
#include "sync_script.h"         // STEquipmentPartSetPiece 完整类型（含 STEquipmentScript.h）

#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanStr
#include <string>
#include <map>
#include <vector>
#include <cstring>

// ============================================================================
// importGoldLimitByLevel —— ORIG 0x088fdb69
// 报告 docs/class_func_reports/_lt_global_gt_/importGoldLimitByLevel.md
// 调用点（init.md 0x832…+0x2e4b）：CDataManager::init → this+0x6410
//   （CDataManager::m_moneyLimit[201]，int 数组）。
// 目标为 unsigned int*（报告 uint*），访问 out[level]。
// ============================================================================
int importGoldLimitByLevel(unsigned int* out)
{
    if (!out)
        return 0;

    // 清前 0x190（400）字节（与汇编 movl $0x190 一致；m_moneyLimit 数组更大，
    // 但 ORIG 仅清零 400 字节）。
    std::memset(out, 0, 0x190);

    if (!loadRDARScriptFile("", "Etc/GoldLimitByLevel.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              "importGoldLimitByLevel", 0xe3,
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
    if (!out)
        return 0;

    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool okA = false;
    bool okB = false;

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
                InvalidCharData::stRangeElement<unsigned char> elem;
                elem.m_min = (unsigned char)lo;
                elem.m_max = (unsigned char)hi;
                out->m_charRange.push_back(elem);
            }
        }
        else if (token == "[one byte]")
        {
            while (true)
            {
                int v = ScanInt(&okA);
                if (!okA)
                    break;
                out->m_charList.push_back((unsigned char)v);
            }
        }
        else if (token == "[two byte range]")
        {
            while (true)
            {
                int lo = ScanInt(&okA);
                if (!okA)
                    break;
                int hi = ScanInt(&okB);
                if (!okB)
                    break;
                InvalidCharData::stRangeElement<unsigned short> elem;
                elem.m_min = (unsigned short)lo;
                elem.m_max = (unsigned short)hi;
                out->m_shortRange.push_back(elem);
            }
        }
        else if (token == "[two byte]")
        {
            while (true)
            {
                int v = ScanInt(&okA);
                if (!okA)
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
                                  "importExperienceItemMapInfo", 0x30b8,
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
                              "importImageCommunicationInfo", 0x3233,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.", path);
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
// 装备部件套装（ORIG "../RDARScriptEtcParameter.cpp" 域）
//
// 类视图与 DataManagerInitDepends.h 权威声明逐字段一致；本 TU 不直接包含该头
// （其 importInvalidName bool 声明与本 TU 既有 int 本地声明冲突）。
// C1 0x0891b002 W：string@+0x04 / vector@+0x0c 默认构造 → m_field0=-1 → m_script=0
// D1 0x08907348 W：m_script 非空则 ~STEquipmentScript()+delete 并置 0
//                  → 成员逆序析构 ~vector(+0x0c) → ~string(+0x04)
// ===========================================================================
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

STEquipmentPartSetData::STEquipmentPartSetData()
{
    m_field0 = -1;      // ORIG c7 00 ff ff ff ff
    m_script = 0;       // ORIG c7 40 08 00 00 00 00
}

STEquipmentPartSetData::~STEquipmentPartSetData()
{
    if (m_script != 0)
        delete m_script;    // ORIG：~STEquipmentScript() + operator delete
    m_script = 0;           // ORIG movl $0x0,0x8(%eax)
    // m_pieces / m_name 由成员析构按逆序自动完成（与 ORIG 显式序列一致）。
}

// ORIG g_equipmentPartSetMap_ B 0x094f7130 (0x18)：全局装备部件套装表，
// 键=套装 id（脚本段首 ScanInt），值=STEquipmentPartSetData；
// 消费方 importEquipmentPartSetInfo（T 0x089073d9，本体待专属批次落地）。
std::map<int, STEquipmentPartSetData> g_equipmentPartSetMap_;
