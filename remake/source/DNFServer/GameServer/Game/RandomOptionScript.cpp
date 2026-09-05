// ============================================================================
// df_game_r 还原 —— RandomOptionScript（G5 random_option 域）
// 逐函数对照 docs/class_func_reports/RandomOptionScript.md：
//   clear 0x0837261e / ctor 0x08372904 / dtor 0x0837c9e8
//   getPrefixGrade 0x08a73a80 / getPrefix 0x08a73956 / 
//   getOptionModificationCost 0x08a738a0 / importRandomOptionScript 0x08a70a90
// ============================================================================

#include "RandomOptionScript.h"

#include <string.h>

#include "DNFLexWrapper.h"
#include "CCubeStatistic.h"   // getItemGroupNameTable()
#include "LogManager.h"       // logFormat（ORIG duplicate-key 报错）

void OPTION_VALUE_RATIO::clear()
{
    m_vec0.clear();
    m_c = 0;
}

RandomOptionGive::RandomOptionGive()
{
    clear();
}

RandomOptionGive::RandomOptionGive(const RandomOptionGive& other)
    : m_vec0(other.m_vec0)
{
    m_c = other.m_c;
}

RandomOptionGive::~RandomOptionGive()
{
}

void RandomOptionGive::clear()
{
    m_vec0.clear();
    m_c = 0;
}

RandomOptionScript::RandomOptionScript()
{
    clear();
}

RandomOptionScript::~RandomOptionScript()
{
}

void RandomOptionScript::clear()
{
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_map48.clear();
    m_baseItems60.clear();
    m_map78.clear();
    m_map90.clear();
    m_veca8.clear();
    m_mapb4.clear();
    m_mapcc.clear();
    m_ratioE4.clear();
    m_mapf4.clear();
    m_costVec10c.clear();
    m_map118.clear();
    m_map130.clear();
    // ORIG 0x08372648/0x083726a2：+0x130 clear 后、+0x160 clear 前，
    // 依序 set(&+0x90,&+0xcc) / set(&+0x118,&+0x130)。
    m_providerNormal148.set(&m_map90, &m_mapcc);
    m_providerModified154.set(&m_map118, &m_map130);
    m_map160.clear();
    m_map178.clear();
    m_map190.clear();
    m_prefixNormal.clear();
    m_prefixPremium.clear();
    // ORIG 0x083838c4 起：+0x1d8 vector clear 后直接 +0x1e8 map clear，
    // 中间的 m_str1e4（ORIG C1/D1 实证 +0x1e4 string）不参与 clear()。
    m_choosePrefix1d8.clear();
    m_map1e8.clear();
    m_set200.clear();
    m_set218.clear();
}

// ORIG 0x08a73a80（非 const，_ZN mangled；循环前缓存 vector 指针，
// 命中存 result 后 break 汇合返回——对齐 ORIG -O0 布局）。
int RandomOptionScript::getPrefixGrade(int level)
{
    int result = 0;
    std::vector<ChoosePrefix>& vec = m_choosePrefix1d8;
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        if (level >= vec[i].m_min && level < vec[i].m_max)
        {
            result = vec[i].m_grade - 1;
            break;
        }
    }
    return result;
}

// ORIG 0x08a73956 RandomOptionScript::getPrefix(int,int,bool)
// 按 flag 选择 m_prefixNormal(+0x1a8) / m_prefixPremium(+0x1c0) map，
// 以第一个 int(prefix) 为 key find；返回向量下标
// getPrefixGrade(grade)（flag 时 +3）处的字符串。
std::string RandomOptionScript::getPrefix(int prefix, int grade, bool flag)
{
    std::string out;
    const std::map<int, std::vector<std::string> >& map =
        flag ? m_prefixPremium : m_prefixNormal;
    int idx = getPrefixGrade(grade);
    if (flag)
    {
        idx += 3;
    }
    std::map<int, std::vector<std::string> >::const_iterator it =
        map.find(prefix);
    if (it != map.end() && idx >= 0 && idx < (int)it->second.size())
    {
        out = it->second[idx];
    }
    return out;
}

int RandomOptionScript::getOptionModificationCost(short level, ENUM_RARITY rarity) const
{
    for (std::vector<OPTION_MODIFICATION_COST>::const_iterator it =
             m_costVec10c.begin();
         it != m_costVec10c.end(); ++it)
    {
        if (level <= (*it).m_level)   // ORIG 用 operator*（de）
        {
            if (rarity == RARITY_RARE)
            {
                return (*it).m_cost2;
            }
            if (rarity == RARITY_UNIQUE)
            {
                return (*it).m_cost3;
            }
        }
    }
    return -1;
}

// ===================== importRandomOptionScript =====================
// ORIG 0x08a70a90（0x2e10 字节脚本解析器，2754 条反汇编逐段核对）。
// 主循环 ScanType 按段名分发，各段把脚本数值填入对应成员表；
// 返回出错行号（成功 0）——与调用方 CDataManager::init
// `const int err = importRandomOptionScript(...); if (err != 0)` 消费一致。
// ORIG 无 script.clear()（调用方 CDataManager 于加载前已 clear）。
//
// 段名 → 目标表（+ 成员偏移）：
//   [random application]        → m_map0(+0x00)
//   [level limit]               → m_map18(+0x18)
//   [part type]                 → m_map30(+0x30)
//   [base item]                 → m_map48(+0x48)
//   [option type]               → m_map90(+0x90)
//   [quantity ratio]            → m_veca8(+0xa8)
//   [option quantity]           → m_mapb4(+0xb4)
//   [choose option group]       → m_mapcc(+0xcc)
//   [modified option selection] → m_map130(+0x130)
//   [modified option type]      → m_map118(+0x118)
//   [option modification]       → m_costVec10c(+0x10c)
//   [option value ratio]        → m_ratioE4(+0xe4)
//   [option group]              → m_map160(+0x160)
//   [postfix]                   → m_prefixNormal(+0x1a8)/m_prefixPremium(+0x1c0)
//   [choose postfix]            → m_choosePrefix1d8(+0x1d8)
//   [break seal cost]           → m_map178(+0x178)
//   [option text]               → 丢弃（ORIG 仅 ScanInt+ScanStr 消费行）
//   [different weight]          → m_mapf4(+0xf4)
//   [changed postfix]           → m_str1e4(+0x1e4)
//   [postfix grade modification]→ m_map1e8(+0x1e8)
//   [unable to modify postfix grade] → m_set200(+0x200)
//   [gradeless postfix]         → m_set218(+0x218)

// ============================================================================
// 键拼装视图（batch-10 P167 反向差集复核）：ORIG 侧这些 map 键不是「先存若干
// 临时量再移位或拼」，而是**每次 ScanInt 后直接把字节写进键对象自己的槽**，
// 最后以 u64/int 视图作 make_pair 实参：
//   [option quantity]        int  键 -0x4a4：`mov %al,-0x4a4/-0x4a3/-0x4a2/-0x4a1`
//     （报告 ORIG +0xECA..+0xF05；值侧 `mov %ax,-0x4a8/-0x4a6` = m_minA/m_maxB 直存）
//   [choose option group]    u64  键 -0x4b0：`mov %al,-0x4b0/-0x4af/-0x4ae`、
//     `mov %eax,-0x4ac`（高 32）、`mov %al,-0x4ad`（+0x1393.. 区段），
//     make_pair 实参型 `Ry`（unsigned long long&）⇒ 同对象 u64 视图
//   [modified option selection] u64 键 -0x4e0：同形（-0x4e0/-0x4df/-0x4de/
//     -0x4dc(dword)/-0x4dd）
//   [break seal cost]        int  键 -0x588：`mov %al,-0x588`、`mov %ax,-0x586`、
//     `mov %al,-0x587`（+2 word / +1 byte 混序，ORIG 逐条照抄）
// 故用 union 视图复现「具名成员 + 常量偏移直存」形（范式库 P180）；移位或链在
// -O0 会多出 40+ insns/站（本函数 we-bigger 221 的主因）。
// ============================================================================

namespace
{

union OptionQuantityKeyView
{
    int m_key;                      // map<int, OptionQuantityValue> 键
    struct
    {
        unsigned char m_b0;         // +0x00（`mov %al,-0x4a4`）
        unsigned char m_b1;         // +0x01
        unsigned char m_b2;         // +0x02
        unsigned char m_b3;         // +0x03
    } m_bytes;
};
static_assert(sizeof(OptionQuantityKeyView) == 4, "ORIG int 键 4 字节");

union OptionGroupChoiceKeyView
{
    unsigned long long m_key;       // map<u64, OptionGroupChoiceValue> 键
    struct
    {
        unsigned char m_b0;         // +0x00
        unsigned char m_b1;         // +0x01
        unsigned char m_b2;         // +0x02（getItemGroupNameTable 值）
        unsigned char m_b3;         // +0x03
        unsigned int m_hi;          // +0x04（`mov %eax,-0x4ac`）
    } m_bytes;
};
static_assert(sizeof(OptionGroupChoiceKeyView) == 8, "ORIG u64 键 8 字节");

union BreakSealCostKeyView
{
    int m_key;
    struct
    {
        unsigned char m_b0;         // +0x00（`mov %al,-0x588`）
        unsigned char m_b1;         // +0x01（`mov %al,-0x587`）
        unsigned short m_w2;        // +0x02（`mov %ax,-0x586`）
    } m_bytes;
};
static_assert(sizeof(BreakSealCostKeyView) == 4, "ORIG break seal int 键 4 字节");

}   // namespace

int importRandomOptionScript(RandomOptionScript& script, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x11;
    }

    std::string line;
    bool ok = false;
    while (ScanType(line, true))
    {
        int v;

        if (line == "[random application]")
        {
            RandomApplicationKey key;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x27;
                }
                RandomApplicationValue val;
                val.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2b;
                }
                val.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2f;
                }
                if (!script.m_map0.insert(std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x33,
                        "importRandomOptionScript - random_application_ duplicate key error");
                    return 0x34;
                }
            }
        }
        else if (line == "[level limit]")
        {
            RandomApplicationKey key;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x44;
                }
                LevelLimitDecisionValue val;
                val.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x48;
                }
                val.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x4c;
                }
                if (!script.m_map18.insert(std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x50,
                        "importRandomOptionScript - level_limit_ duplicate key error");
                    return 0x51;
                }
            }
        }
        else if (line == "[part type]")
        {
            // ORIG 每行三字段：partType itemGroupName weight；循环回 0x8a70e28
            // 重新读 partType。weight 读失败 → return 0x6e（非跳出）。
            std::string name;
            int partType = 0;
            int itemGroup = 0;
            int weight = 0;
            for (;;)
            {
                partType = ScanInt(&ok);
                if (!ok)
                {
                    break;   // 段首字段失败 → 回主循环（ORIG esi=1）
                }
                name.clear();
                if (!ScanStr(&name))
                {
                    return 0x64;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(name);
                if (git == getItemGroupNameTable().end())
                {
                    return 0x68;
                }
                itemGroup = git->second;
                weight = ScanInt(&ok);
                if (!ok)
                {
                    return 0x6e;
                }
                std::map<int, PartSelectonValue>::iterator mit =
                    script.m_map30.find(partType);
                if (mit != script.m_map30.end())
                {
                    // ORIG 0x8a70fdb/0x8a71032：以 (itemGroup, weight) 两紧邻 int 槽
                    // 直接作 PartSelectonField（+0x00/+0x04）推入两 vector。
                    PartSelectonField field;
                    field.m_partType = itemGroup;   // +0x00
                    field.m_weight = weight;        // +0x04
                    mit->second.m_vec10.push_back(field);
                    mit->second.m_fieldC += weight;
                    mit->second.m_vec0.push_back(field);
                }
                else
                {
                    PartSelectonValue val;
                    PartSelectonField field;
                    field.m_partType = itemGroup;   // +0x00
                    field.m_weight = weight;        // +0x04
                    val.m_vec10.push_back(field);
                    val.m_vec0.push_back(field);
                    val.m_fieldC = weight;
                    // ORIG 此分支 insert 不检查 duplicate。
                    script.m_map30.insert(std::make_pair(partType, val));
                }
            }
        }
        else if (line == "[base item]")
        {
            RandomApplicationKey key;
            BaseItemSelectionValue val;
            int itemCount = 0;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x94;
                }
                itemCount = ScanInt(&ok);
                if (!ok)
                {
                    return 0x98;
                }
                val.clear();
                for (int i = 0; i < itemCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x9f;
                    }
                    val.m_fieldC += v;
                    val.m_vec0.push_back((unsigned int)val.m_fieldC);
                }
                if (!script.m_map48.insert(std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0xa8,
                        "importRandomOptionScript - base_item_selection_ duplicate key error");
                    return 0xa9;
                }
            }
        }
        else if (line == "[option type]")
        {
            RandomApplicationKey key;
            RandomOptionGive give;
            int optionType = 0;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                optionType = ScanInt(&ok);
                if (!ok)
                {
                    return 0xbc;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xc0;
                }
                give.clear();
                for (int i = 0; i <= 2; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0xc7;
                    }
                    give.m_c += v;
                    // ORIG 0x8a71575：m_c 仅累加，vector 推入的是本次输入原值
                    // （-0x334 = -0x48 的副本），与 [base item] 推累计和不同。
                    give.m_vec0.push_back((unsigned int)v);
                }
                std::map<RandomApplicationKey,
                         std::map<int, RandomOptionGive> >::iterator it =
                    script.m_map90.find(key);
                if (it != script.m_map90.end())
                {
                    if (!it->second.insert(std::make_pair(optionType, give)).second)
                    {
                        LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                              __FUNCTION__, 0xd3,
                            "importRandomOptionScript - option_type_ duplicate key error");
                        return 0xd4;
                    }
                }
                else
                {
                    std::map<int, RandomOptionGive> newMap;
                    newMap.insert(std::make_pair(optionType, give));
                    script.m_map90.insert(std::make_pair(key, newMap));
                }
            }
        }
        else if (line == "[quantity ratio]")
        {
            for (;;)
            {
                QuantityRatio ratio;
                ratio.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ratio.m_weight4 = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xea;
                }
                script.m_veca8.push_back(ratio);
            }
        }
        else if (line == "[option quantity]")
        {
            OptionQuantityValue val;         // ORIG -0x4a8（m_minA/m_maxB 逐 word 直存）
            OptionQuantityKeyView keyView;   // ORIG -0x4a4（逐 byte 直存，无移位或链）
            for (;;)
            {
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xfb;
                }
                keyView.m_bytes.m_b2 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xff;
                }
                keyView.m_bytes.m_b3 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x103;
                }
                val.m_minA = static_cast<short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x107;
                }
                val.m_maxB = static_cast<short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x10b;
                }
                if (!script.m_mapb4.insert(std::make_pair(keyView.m_key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x10f,
                        "importRandomOptionScript - option_quantity_ duplicate key error");
                    return 0x110;
                }
            }
        }
        else if (line == "[choose option group]")
        {
            OptionGroupChoiceValue choice;
            std::string name;
            int groupCount = 0;
            OptionGroupChoiceKeyView keyView;   // ORIG -0x4b0：逐字节/双字直存
            for (;;)
            {
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x125;
                }
                name.clear();
                if (!ScanStr(&name))
                {
                    return 0x129;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(name);
                if (git == getItemGroupNameTable().end())
                {
                    return 0x12d;
                }
                keyView.m_bytes.m_b2 = static_cast<unsigned char>(git->second);
                // ORIG +0x1289：ScanInt 结果整 dword 直存键 +4（无中间量）
                keyView.m_bytes.m_hi = ScanInt(&ok);
                if (!ok)
                {
                    return 0x133;
                }
                keyView.m_bytes.m_b3 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x137;
                }
                groupCount = ScanInt(&ok);   // ORIG +0x12e8：直存计数槽
                if (!ok)
                {
                    return 0x13b;
                }
                choice.clear();
                for (int i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x142;
                    }
                    choice.m_vec0.push_back(v);
                }
                for (int i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x14b;
                    }
                    choice.m_vecC.push_back((unsigned int)v);
                    choice.m_field18 += v;
                }
                // 键已按 ORIG 序逐段写入（b0/b1/b2/hi/b3），此处仅取 u64 视图
                if (!script.m_mapcc.insert(std::make_pair(keyView.m_key, choice)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x153,
                        "importRandomOptionScript - option_group_choice_ duplicate key error");
                    return 0x154;
                }
            }
        }
        else if (line == "[modified option selection]")
        {
            OptionGroupChoiceValue choice;
            std::string name;
            int groupCount = 0;
            OptionGroupChoiceKeyView keyView;   // ORIG -0x4e0：逐字节/双字直存
            for (;;)
            {
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x16a;
                }
                name.clear();
                if (!ScanStr(&name))
                {
                    return 0x16e;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(name);
                if (git == getItemGroupNameTable().end())
                {
                    return 0x172;
                }
                keyView.m_bytes.m_b2 = static_cast<unsigned char>(git->second);
                keyView.m_bytes.m_hi = ScanInt(&ok);   // ORIG +0x1358：dword 直存键 +4
                if (!ok)
                {
                    return 0x178;
                }
                keyView.m_bytes.m_b3 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x17c;
                }
                groupCount = ScanInt(&ok);             // ORIG +0x1380：直存计数槽
                if (!ok)
                {
                    return 0x180;
                }
                choice.clear();
                for (int i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x187;
                    }
                    choice.m_vec0.push_back(v);
                }
                for (int i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x190;
                    }
                    choice.m_vecC.push_back((unsigned int)v);
                    choice.m_field18 += v;
                }
                // 键已按 ORIG 序（b0/b1/b2/hi/b3）逐段写入，取 u64 视图入表
                if (!script.m_map130.insert(std::make_pair(keyView.m_key, choice)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x198,
                        "importRandomOptionScript - modified_option_group_choice_ duplicate key error");
                    return 0x199;
                }
            }
        }
        else if (line == "[modified option type]")
        {
            RandomApplicationKey key;
            RandomOptionGive give;
            int optionType = 0;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                optionType = ScanInt(&ok);
                if (!ok)
                {
                    return 0x1ac;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x1b0;
                }
                give.clear();
                for (int i = 0; i <= 2; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x1b7;
                    }
                    give.m_c += v;
                    // ORIG 0x08a7259a：m_c 仅累加，vector 推入的是本次输入原值
                    // （-0x1b4 = -0x2c 的副本），与 [base item] 推累计和不同。
                    give.m_vec0.push_back((unsigned int)v);
                }
                std::map<RandomApplicationKey,
                         std::map<int, RandomOptionGive> >::iterator it =
                    script.m_map118.find(key);
                if (it != script.m_map118.end())
                {
                    if (!it->second.insert(std::make_pair(optionType, give)).second)
                    {
                        LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                              __FUNCTION__, 0x1c3,
                            "importRandomOptionScript - modified_option_type_ duplicate key error");
                        return 0x1c4;
                    }
                }
                else
                {
                    std::map<int, RandomOptionGive> newMap;
                    newMap.insert(std::make_pair(optionType, give));
                    script.m_map118.insert(std::make_pair(key, newMap));
                }
            }
        }
        else if (line == "[option modification]")
        {
            for (;;)
            {
                OPTION_MODIFICATION_COST cost;
                cost.m_level = static_cast<short>(ScanInt(&ok));   // ORIG：word 直存 +0x00
                if (!ok)
                {
                    break;
                }
                cost.m_cost2 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x1da;
                }
                cost.m_cost3 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x1de;
                }
                script.m_costVec10c.push_back(cost);
            }
        }
        else if (line == "[option value ratio]")
        {
            // ORIG 0x8a72a15/~0x8a72ac1：ratio 对象（+0 scaleRange0/+4 scaleBase4/+8 weight8）
            // 于段入口清零构造一次；行内首 float 写 +4、次 float 写 +0、int 写 +8。
            for (;;)
            {
                float base = ScanFloat(&ok);
                if (!ok)
                {
                    break;
                }
                float range = ScanFloat(&ok);
                if (!ok)
                {
                    break;
                }
                v = ScanInt(&ok);
                if (!ok)
                {
                    return 0x1f5;
                }
                OptionValueRatio ratio;
                ratio.m_scaleBase4 = base;    // +0x04（行内首 float）
                ratio.m_scaleRange0 = range;  // +0x00（行内次 float）
                ratio.m_weight8 = (unsigned int)v;
                script.m_ratioE4.m_c += v;
                script.m_ratioE4.m_vec0.push_back(ratio);
            }
        }
        else if (line == "[option group]")
        {
            OptionGroupValue value;
            int type = ScanInt(&ok);
            if (!ok)
            {
                return 0x203;
            }
            for (;;)
            {
                // ORIG 1939-1975：两次 ScanInt 结果**直接落进 g 的字段槽**
                // （-0x56c = g.m_type0、-0x568 = g.m_weight04），界检读回 g.m_type0，
                // 累计项取 g.m_weight4 —— 全程无 weight/v 中转量。
                OptionGroup g;
                g.m_type0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (g.m_type0 <= 0 || g.m_type0 > 0xff)
                {
                    return 0x20c;
                }
                g.m_weight4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x210;
                }
                value.m_vec0.push_back(g);
                value.m_fieldC += g.m_weight4;
            }
            if (!script.m_map160.insert(std::make_pair(type, value)).second)
            {
                LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                      __FUNCTION__, 0x218,
                    "importRandomOptionScript - option_group_ duplicate key error");
                return 0x219;
            }
        }
        else if (line == "[postfix]")
        {
            int count = ScanInt(&ok);   // ORIG：直存计数槽（无 v 中转）
            if (!ok)
            {
                return 0;
            }
            std::string str;
            for (;;)
            {
                int grade = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::vector<std::string> vec;
                for (int i = 0; i < count; ++i)
                {
                    str.clear();
                    if (!ScanStr(&str))
                    {
                        return 0;
                    }
                    vec.push_back(str);
                }
                script.m_prefixNormal[grade] = vec;
                vec.clear();
                for (int i = 0; i < count; ++i)
                {
                    str.clear();
                    if (!ScanStr(&str))
                    {
                        return 0;
                    }
                    vec.push_back(str);
                }
                script.m_prefixPremium[grade] = vec;
            }
        }
        else if (line == "[choose postfix]")
        {
            for (;;)
            {
                int minVal = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int maxVal = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                // ORIG 0x8a7301f：ChoosePrefix 临时对象按右值 push_back（EOS0_ 移动重载）。
                script.m_choosePrefix1d8.push_back(ChoosePrefix(minVal, maxVal, v));
            }
        }
        else if (line == "[break seal cost]")
        {
            for (;;)
            {
                int k1 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int k2 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x28f;
                }
                int k3 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x293;
                }
                int cost = ScanInt(&ok);
                if (!ok)
                {
                    return 0x297;
                }
                // key 字节序：byte0=k1, byte1=k3, ushort@idx2=k2
                // （ORIG 槽 -0x588/-0x587/-0x586）→ k1 | k3<<8 | k2<<16。
                int key = (int)((unsigned int)(unsigned char)k1 |
                                ((unsigned int)(unsigned char)k3 << 8) |
                                ((unsigned int)(unsigned short)k2 << 16));
                if (!script.m_map178.insert(std::make_pair(key, cost)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x29b,
                        "importRandomOptionScript - break_seal_cost_ duplicate key error");
                    return 0x29c;
                }
            }
        }
        else if (line == "[option text]")
        {
            std::string str;
            for (;;)
            {
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                str.clear();
                if (!ScanStr(&str))
                {
                    return 0x2ab;
                }
                // ORIG：读到的文本行不写入任何成员（丢弃）。
            }
        }
        else if (line == "[different weight]")
        {
            for (;;)
            {
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                unsigned char k0 = (unsigned char)v;
                v = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2ba;
                }
                unsigned char k1 = (unsigned char)v;
                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2be;
                }
                int v3 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2c2;
                }
                if (v3 <= v2)
                {
                    return 0x2c5;
                }
                if (v3 > 0x64 || v3 < 0)
                {
                    return 0x2c8;
                }
                if (v2 > 0x64 || v2 < 0)
                {
                    return 0x2cb;
                }
                short key = (short)(((unsigned short)k1 << 8) | k0);
                DifferentWeightValue val;
                val.m_field0 = v2;
                val.m_field4 = v3;
                if (!script.m_mapf4.insert(std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x2cf,
                        "importRandomOptionScript - different_weight_ duplicate key error");
                    return 0x2d0;
                }
            }
        }
        else if (line == "[changed postfix]")
        {
            std::string str;
            ScanStr(&str);
            script.m_str1e4 = str;
        }
        else if (line == "[postfix grade modification]")
        {
            unsigned char key = 0;
            for (;;)
            {
                key = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                v = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2eb;
                }
                // ORIG 0x08a73522：此行 ScanInt 结果仅消费、不入 ResetValue（丢弃）。
                int minVal = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2ef;
                }
                int maxVal = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2f3;
                }
                int weight = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2f7;
                }
                ResetValue rv;
                rv.m_min = minVal;       // +0x00
                rv.m_max = maxVal;       // +0x04
                rv.m_weight8 = weight;   // +0x08
                std::map<unsigned char, ResetValues>::iterator it =
                    script.m_map1e8.find(key);
                if (it != script.m_map1e8.end())
                {
                    it->second.m_count += weight;
                    it->second.m_values.push_back(rv);
                }
                else
                {
                    ResetValues rvs;
                    rvs.m_count = weight;   // ORIG 0x08a73678：not-found 直接置本行 weight
                    rvs.m_values.push_back(rv);
                    script.m_map1e8.insert(std::make_pair(key, rvs));
                }
            }
        }
        else if (line == "[unable to modify postfix grade]")
        {
            for (;;)
            {
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script.m_set200.insert((unsigned long)v);
            }
        }
        else if (line == "[gradeless postfix]")
        {
            for (;;)
            {
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script.m_set218.insert((unsigned long)v);
            }
        }
    }
    return 0;
}
