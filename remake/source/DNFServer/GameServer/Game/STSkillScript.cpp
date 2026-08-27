// ============================================================================
// df_game_r 还原 —— STSkillScript（G5 数据/脚本域）
// 逐函数对照 docs/class_func_reports/STSkillScript.md（无独立报告，直接对照
// ORIG 反汇编：Clear 0x083750b4 / ctor 0x08375642 / dtor 0x0837a0fa，
// 嵌套类 SkillUseItem/SkillLevelFeature/STPassiveFeatureData/STKeyCommand
// 见 0x08a9b118 / 0x08a9b164 / 0x08a9b256 / 0x083660ec）。
// 脚本解析函数（loadRDARScriptFile/ScanType/ScanInt/ScanFloat）经
// DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include "STSkillScript.h"

#include <string.h>

#include <map>

#include "DNFLexWrapper.h"

// ===================== 前置声明（ORIG 自由函数，实现于各权威 TU） =====================

bool importCommand(STKeyCommand* command);                          // T 0x088c167a
bool processModuleTagSet(int* moduleId, int* subId, std::string* tag,
                         int type, bool flag);                      // T 0x08a97637

class STScriptFileListMirror
{
public:
    std::map<int, std::string> m_fileMap;             // +0x00（0x18 布局）
    int FindIndexByFullScan(const char* path) const;  // ORIG T 0x088bae56
};

extern std::map<int, std::string> g_staSkillScriptFileList[11];     // B 0x9500820

// ===================== 嵌套结构 =====================

SkillUseItem::SkillUseItem()
{
    clear();
}

SkillUseItem::~SkillUseItem()
{
}

void SkillUseItem::clear()
{
    m_index = -1;
    m_useLevel[0] = 0;
    m_useLevel[1] = 0;
}

SkillLevelFeature::SkillLevelFeature()
{
    clear();
}

SkillLevelFeature::SkillLevelFeature(const SkillLevelFeature& other)
    : m_str8(other.m_str8),
      m_vecC(other.m_vecC),
      m_vec18(other.m_vec18)
{
    m_field0[0] = other.m_field0[0];
    m_field0[1] = other.m_field0[1];
}

SkillLevelFeature::~SkillLevelFeature()
{
}

SkillLevelFeature& SkillLevelFeature::operator=(const SkillLevelFeature& other)
{
    if (this != &other)
    {
        m_field0[0] = other.m_field0[0];
        m_field0[1] = other.m_field0[1];
        m_str8 = other.m_str8;
        m_vecC = other.m_vecC;
        m_vec18 = other.m_vec18;
    }
    return *this;
}

void SkillLevelFeature::clear()
{
    m_field0[0] = 0;
    m_field0[1] = 0;
    m_str8.clear();
    m_vecC.clear();
    m_vec18.clear();
}

STPassiveFeatureData::STPassiveFeatureData()
{
    clear();
}

STPassiveFeatureData::~STPassiveFeatureData()
{
}

void STPassiveFeatureData::clear()
{
    m_enable = 1;
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
}

STKeyCommand::STKeyCommand()
{
    clear();
}

STKeyCommand::~STKeyCommand()
{
}

void STKeyCommand::clear()
{
    m_vec0.clear();
    m_vecC.clear();
    m_18 = 1;
    m_1c = 0;
    m_20 = 1;
}

// ===================== STSkillScript =====================

STSkillScript::STSkillScript()
{
    Clear();
}

STSkillScript::~STSkillScript()
{
}

void STSkillScript::Clear()
{
    m_field0 = 0;
    m_str4 = "";
    m_str8 = "";
    m_strc = "";
    m_14 = 0;
    m_vec18.clear();
    m_vec24.clear();
    m_30 = 1;
    m_34 = 1;
    m_38 = 0;
    m_3c = 1;
    m_40 = 1;
    m_vec44.clear();
    m_vec50.clear();
    m_5c = 0;
    m_60 = 0;
    for (int i = 0; i < 6; ++i)
    {
        m_arr64[i] = 0;
        m_arr7c[i] = 0;
        m_growType[i].m_0[0] = 0;
        m_growType[i].m_0[1] = 0;
        m_growType[i].m_8[0] = 0;
        m_growType[i].m_8[1] = 0;
    }
    m_vecf4.clear();
    for (int i = 0; i < 2; ++i)
    {
        m_f100[i] = 0.f;
    }
    for (int i = 0; i < 4; ++i)
    {
        m_i108[i] = 0;
    }
    m_arr138[0] = 0;
    m_arr138[1] = 0;
    m_118 = 0;
    m_useItems11c.clear();
    m_vec128.clear();
    m_134 = 1;
    m_vec140.clear();
    m_vec14c.clear();
    m_158 = 1;
    m_15c = 0;
    m_str160[0] = "";
    m_str160[1] = "";
    m_arr168[0] = 0;
    m_arr168[1] = 0;
    m_keyCommand170.clear();
    m_str194 = "";
    m_vec198.clear();
    m_1a4 = -1;
    m_vec1a8.clear();
    m_levelFeatures1b4.clear();
    m_levelFeatures1c0.clear();
    m_1cc = 0;
    m_1d0 = 0;
    m_1d4 = 0;
    m_1d5 = 0;
    m_1d8 = -1;
    m_vec1dc.clear();
    m_1e8 = 0;
    m_1ec = 0;
    m_1f0 = 0;
    m_1f4 = 0;
    m_vec1f8.clear();
    m_vec204.clear();
    m_vec210.clear();
    m_21c = 0;
    m_220 = 0;
    m_224 = -1;
    memset(m_228, 0, sizeof(m_228));
    m_228[3] = 1;
    m_23c = 0;
    m_240 = 0;
    m_244 = 0;
    m_248 = 0;
    m_254 = 0;
    m_250 = m_254;
    m_24c = m_250;
    m_258 = -1;
    m_25c = -1;
    m_260 = 0;
    m_264 = 0;
    m_passive268.clear();
    m_str274.clear();
    m_str278.clear();
    m_280 = 1;
    m_vec284.clear();
}


// ===================== ImportSkillScript 辅助（ORIG 静态域） =====================

// ---- ORIG static map<const char*,int>（guard 0x9500948 / 对象 0x9500958）

namespace
{

std::map<const char*, int> s_skillTagMap;
SpinLock s_skillTagLock;
bool s_skillTagReady = false;
const struct SkillTagEntry { int id; const char* name; } kSkillTags[kTagCount] =
{
    { 0x129, "[name]" },                          { 0x12c, "[name2]" },
    { 0x12f, "[explain]" },                       { 0x132, "[basic explain]" },
    { 0x135, "[basic property]" },                { 0x159, "[maximum level]" },

    { 0x15c, "[purchase cost]" },                 { 0x166, "[special purchase cost]" },
    { 0x172, "[required level]" },                { 0x175, "[required level range]" },
    { 0x178, "[required rank]" },                 { 0x17b, "[required guild level]" },
    { 0x17e, "[required guild level range]" },    { 0x181, "[pre required skill]" },
    { 0x18e, "[steel learning skill]" },          { 0x194, "[type]" },
    { 0x19b, "[skill class]" },                   { 0x19f, "[growtype fitness]" },
    { 0x1a4, "[second growtype fitness]" },       { 0x1ac, "[growtype maximum level]" },
    { 0x1b1, "[second growtype maximum level]" }, { 0x1b9, "[skill fitness growtype]" },
    { 0x1c4, "[skill fitness second growtype]" }, { 0x1cf, "[casting time]" },
    { 0x1d3, "[durability decrease rate]" },      { 0x1d6, "[consume MP]" },
    { 0x1dc, "[maintain MP]" },                   { 0x1e0, "[consume item]" },
    { 0x1ee, "[limit count]" },                   { 0x1f8, "[auto limit count reduce]" },
    { 0x1fb, "[cool time]" },                     { 0x1ff, "[league ban]" },
    { 0x202, "[skill under cooltime effect]" },   { 0x20d, "[skill under cooltime effect each]" },
    { 0x218, "[auto cooltime apply]" },           { 0x21b, "[weapon effect type]" },
    { 0x224, "[icon]" },                          { 0x22a, "[command key explain]" },
    { 0x22d, "[command]" },                       { 0x230, "[static data]" },
    { 0x23b, "[cooltime level info]" },           { 0x23f, "[level info]" },
    { 0x258, "[level property]" },                { 0x27c, "[shake screen]" },
    { 0x280, "[seal enable]" },                   { 0x283, "[active seal enable]" },
    { 0x287, "[physical magical change]" },       { 0x28b, "[skill preloading image]" },
    { 0x292, "[purchase gsp]" },                  { 0x29d, "[purchase gold]" },
    { 0x2a8, "[start cool time]" },               { 0x2ac, "[skill command advantage]" },
    { 0x2b1, "[is linked blast]" },               { 0x2b4, "[linked blast add point]" },
    { 0x2b7, "[linked blast dec point]" },        { 0x2bb, "[child skill index]" },
    { 0x2be, "[feature skill index]" },           { 0x2c1, "[feature skill type]" },
    { 0x2c4, "[special level up]" },              { 0x2e1, "[basic explain ex]" },
    { 0x2e4, "[explain ex]" },                    { 0x2e7, "[executable states]" },
    { 0x2fe, "[nut skill script]" },              { 0x319, "[command customizing]" },
    { 0x31d, "[combo set]" }
};

} // namespace

int ImportSkillScript(STSkillScript* script, const char* path,
                      ENUM_CHARACTERJOB job, int paramB, int iconIdx)
{
    if (!loadRDARScriptFile("Script/SkillScript", path))
    {
        return 0;                                   // 8a97955 → return ebx=0
    }
    script->Clear();

    std::string token;                              // -0x5d4
    std::string scanStrBuf;                         // -0x5d8
    int value = 0;                                  // -0x5dc
    bool okFlag = false;                            // -0x5dd
    std::vector<float> valuePool;                   // -0x5ec
    valuePool.reserve(0xfa0);

    if (!ScanType(token, true))
    {
        return FinalizeImport(script, job, iconIdx, path);   // 8a9ac51 失败尾部仍走后续修正
    }

    for (;;)                                        // 主循环头 8a979dc
    {
        // ---- 标签查找（静态表 + 双检 SpinLock 惰性初始化）----
        int tagId;
        if (s_skillTagReady)
        {
            std::map<const char*, int>::const_iterator it = s_skillTagMap.find(token.c_str());
            if (it != s_skillTagMap.end())
                tagId = it->second;
            else
                tagId = -1;                          // 未知标签 → 默认分支
        }
        else
        {
            s_skillTagLock.enter();
            if (!s_skillTagReady)
            {
                tagId = 0;                           // 未就绪：按 case 序注册本标签后落空
            }
            else
            {
                s_skillTagLock.leave();
                continue;                            // 重跑 find 分支（8a97bbc → 8a979dc 前）
            }
        }

        switch (tagId)
        {
        case 0:                                      // 未就绪路径：逐条注册（8a97f87 起）
            RegisterSkillTags();
            break;

        case TAG_NAME:                               // [name]
            ScanStr(&script->m_str4);
            break;

        case TAG_NAME2:                              // [name2]
            ScanStr(&script->m_str8);
            break;

        case TAG_EXPLAIN:                            // [explain]
            ScanStr(&script->m_strc);
            break;

        case TAG_BASIC_EXPLAIN:                      // [basic explain]
            ScanStr(&script->m_str10);
            break;

        case TAG_BASIC_PROPERTY:                     // [basic property]
        {
            SkillLevelFeature feat;
            feat.m_field0[0] = ScanInt(&okFlag);
            if (!okFlag)
                break;
            feat.m_field0[1] = ScanInt(0);
            ScanStr(&feat.m_str8);
            bool cont = true;
            while (cont)
            {
                value = ScanInt(&okFlag);
                if (okFlag)
                {
                    int value2 = ScanInt(0);
                    feat.m_vecC.push_back(std::pair<int, int>(value, value2));
                    float fValue = ScanFloat(0);
                    feat.m_vec18.push_back(fValue);
                }
                else
                {
                    cont = false;
                }
            }
            EraseOverlappingFeatures(&script->m_levelFeatures1b4,
                                     feat.m_field0[0], feat.m_field0[1]);
            script->m_levelFeatures1c0.push_back(feat);
            break;
        }

        case TAG_MAXIMUM_LEVEL:                      // [maximum level]
            script->m_14 = ScanInt(0);
            break;

        case TAG_PURCHASE_COST:                      // [purchase cost]
        {
            script->m_vec18.clear();
            ReadIntVecInto(script->m_vec18, &value, &okFlag);
            break;
        }

        case TAG_SPECIAL_PURCHASE_COST:              // [special purchase cost]
        {
            script->m_vec24.clear();
            ReadIntVecInto(script->m_vec24, &value, &okFlag);
            break;
        }

        case TAG_REQUIRED_LEVEL:                     // [required level]
            script->m_30 = ScanInt(0);
            break;

        case TAG_REQUIRED_LEVEL_RANGE:               // [required level range]
            script->m_34 = ScanInt(0);
            break;

        case TAG_REQUIRED_RANK:                      // [required rank]
            script->m_38 = ScanInt(0);
            break;

        case TAG_REQUIRED_GUILD_LEVEL:               // [required guild level]
            script->m_3c = ScanInt(0);
            break;

        case TAG_REQUIRED_GUILD_LEVEL_RANGE:         // [required guild level range]
            script->m_40 = ScanInt(0);
            break;

        case TAG_PRE_REQUIRED_SKILL:                 // [pre required skill]
        {
            script->m_vec44.clear();
            script->m_vec50.clear();
            bool cont = true;
            while (cont)
            {
                value = ScanInt(&okFlag);
                if (okFlag)
                {
                    int subVal = ScanInt(0);
                    int pushVal = value;
                    int pushSub = subVal;
                    script->m_vec44.push_back(pushVal);
                    script->m_vec50.push_back(pushSub);
                }
                else
                {
                    cont = false;
                }
            }
            break;
        }

        case TAG_STEEL_LEARNING_SKILL:               // [steel learning skill]
        {
            int enableVal = ScanInt(&okFlag);
            script->m_1e8 = (enableVal == 1) ? 1 : 0;
            script->m_1ec = ScanInt(0);
            script->m_1f0 = ScanInt(0);
            script->m_1f4 = ScanInt(0);
            break;
        }

        case TAG_TYPE:                               // [type]
            ScanStr(&scanStrBuf);
            if (scanStrBuf == "[active]")
                script->m_5c = 0;
            else if (scanStrBuf == "[passive]")
                script->m_5c = 1;
            break;                                   // 其它值不写（8a9ac17 落主循环尾）

        case TAG_SKILL_CLASS:                        // [skill class]
            script->m_60 = ScanInt(0);
            break;

        case TAG_GROWTYPE_FITNESS:                   // [growtype fitness]
            for (int i = 0; i <= 5; ++i)
                script->m_arr64[i][0] = ScanInt(0);  // dst = this+(i+0x18)*4+0x04
            break;

        case TAG_SECOND_GROWTYPE_FITNESS:            // [second growtype fitness]
            for (int i = 0; i <= 5; ++i)
                for (int j = 0; j <= 1; ++j)
                    script->m_growType[i].m_0[j] =
                        ScanInt(0);                  // dst = this+(i*16+j)+0x24+0x10
            break;

        case TAG_GROWTYPE_MAXIMUM_LEVEL:             // [growtype maximum level]
            for (int i = 0; i <= 5; ++i)
                script->m_arr7c[i][0] = ScanInt(0);  // dst = this+(i+0x1c)*4+0xc
            break;

        case TAG_SECOND_GROWTYPE_MAXIMUM_LEVEL:      // [second growtype maximum level]
            for (int i = 0; i <= 5; ++i)
                for (int j = 0; j <= 1; ++j)
                    script->m_growType[i].m_8[j] =
                        ScanInt(0);                  // dst = this+(i*16+j)+0x24+0x1c
            break;

        case TAG_SKILL_FITNESS_GROWTYPE:             // [skill fitness growtype]
            script->m_vecf4.clear();
            ReadIntVecInto(script->m_vecf4, &value, &okFlag);
            break;

        case TAG_SKILL_FITNESS_SECOND_GROWTYPE:      // [skill fitness second growtype]
            script->m_vec128.clear();
            ReadIntVecInto(script->m_vec128, &value, &okFlag);
            break;

        case TAG_CASTING_TIME:                       // [casting time]
            for (int i = 0; i <= 1; ++i)
                script->m_i108[i] = ScanInt(0);      // dst = this+(i+0x44)*4+0x00
            break;

        case TAG_DURABILITY_DECREASE_RATE:           // [durability decrease rate]
            script->m_118 = ScanInt(0);
            break;

        case TAG_CONSUME_MP:                         // [consume MP]
            for (int i = 0; i <= 1; ++i)
                script->m_f100[i] = ScanFloat(0);    // fstps this+i*0x40
            break;

        case TAG_MAINTAIN_MP:                        // [maintain MP]
            for (int i = 0; i <= 1; ++i)
                script->m_f100[i] = ScanFloat(0);    // fstps this+i*0x40+0x08（ORIG 同址别名写）
            break;

        case TAG_CONSUME_ITEM:                       // [consume item]
        {
            SkillUseItem item;
            script->m_useItems11c.clear();
            bool cont = true;
            while (cont)
            {
                item.m_index = ScanInt(&okFlag);
                if (okFlag)
                {
                    item.m_useLevel[0] = ScanInt(0);
                    item.m_useLevel[1] = ScanInt(0);
                    script->m_useItems11c.push_back(item);
                }
                else
                {
                    cont = false;
                }
            }
            break;
        }

        case TAG_LIMIT_COUNT:                        // [limit count]
        {
            script->m_vec140.clear();
            ReadIntVecInto(script->m_vec140, &value, &okFlag);
            break;
        }

        case TAG_AUTO_LIMIT_COUNT_REDUCE:            // [auto limit count reduce]
            script->m_134 = ScanInt(0);
            break;

        case TAG_COOL_TIME:                          // [cool time]
            for (int i = 0; i <= 1; ++i)
                script->m_i108[i + 19] = ScanInt(0); // dst = this+(i+0x4c)*4+0x08
            break;

        case TAG_LEAGUE_BAN:                         // [league ban]
            script->m_23c = ScanInt(0);
            break;

        case TAG_SKILL_UNDER_COOLTIME_EFFECT:        // [skill under cooltime effect]
        {
            script->m_vec140.clear();
            ReadIntVecInto(script->m_vec140, &value, &okFlag);
            break;
        }

        case TAG_SKILL_UNDER_COOLTIME_EFFECT_EACH:   // [skill under cooltime effect each]
        {
            script->m_vec14c.clear();
            ReadIntVecInto(script->m_vec14c, &value, &okFlag);
            break;
        }

        case TAG_AUTO_COOLTIME_APPLY:                // [auto cooltime apply]
            script->m_158 = ScanInt(0);
            break;

        case TAG_WEAPON_EFFECT_TYPE:                 // [weapon effect type]
            ScanStr(&scanStrBuf);
            if (scanStrBuf == "[none]")              // 0x8e29aea
                script->m_15c = 0;
            else if (scanStrBuf == "[physical]")     // 0x8e29af1
                script->m_15c = 1;
            else if (scanStrBuf == "[magical]")      // 0x8e29afc
                script->m_15c = 2;
            break;

        case TAG_ICON:                               // [icon]
        {
            char iconA[24];
            iconA[0] = 0;                            // 占位避免未用告警（ORIG 直接指针比较）
            (void)iconA;
            ScanStr(&script->m_str160[0]);
            script->m_arr168[0] = ScanInt(0);
            ScanStr(&script->m_str160[1]);
            script->m_arr168[1] = ScanInt(0);
            break;
        }

        case TAG_COMMAND_KEY_EXPLAIN:                // [command key explain]
            ScanStr(&script->m_str194);
            break;

        case TAG_COMMAND:                            // [command]
            importCommand(&script->m_keyCommand170);
            break;

        case TAG_STATIC_DATA:                        // [static data]
        {
            script->m_vec198.clear();
            ReadIntVecInto(script->m_vec198, &value, &okFlag);
            break;
        }

        case TAG_COOLTIME_LEVEL_INFO:                // [cooltime level info]
            script->m_1a4 = ScanInt(0);
            break;

        case TAG_LEVEL_INFO:                         // [level info]
        {
            script->m_vec1a8.clear();
            int rowCnt = ScanInt(0);                 // -0x40
            {
                std::vector<float> rowFiller;        // resize 第 2 参临时向量
                script->m_vec1a8.resize(rowCnt, rowFiller);
            }
            valuePool.clear();
            bool cont = true;
            while (cont)
            {
                float fValue = ScanFloat(&okFlag);
                if (okFlag)
                    valuePool.push_back(fValue);
                else
                    cont = false;
            }
            int poolSize = static_cast<int>(valuePool.size());
            for (int j = 0; j < rowCnt; ++j)         // 列优先填充：v[j + k*n]
            {
                std::vector<float>& row = script->m_vec1a8[j];
                int chunk = poolSize / rowCnt;       // 有符号除法（sar/idivl 形态）
                row.resize(chunk, 0.0f);
                int srcIdx = j;
                for (int k = 0; k < chunk; ++k)
                {
                    row[k] = valuePool[srcIdx];
                    srcIdx += rowCnt;
                }
            }
            break;
        }

        case TAG_LEVEL_PROPERTY:                     // [level property]
        {
            SkillLevelFeature feat;
            feat.m_field0[0] = ScanInt(&okFlag);
            if (!okFlag)
                break;
            feat.m_field0[1] = ScanInt(0);
            ScanStr(&feat.m_str8);
            bool cont = true;
            while (cont)
            {
                value = ScanInt(&okFlag);
                if (okFlag)
                {
                    int value2 = ScanInt(0);
                    feat.m_vecC.push_back(std::pair<int, int>(value, value2));
                    float fValue = ScanFloat(0);
                    feat.m_vec18.push_back(fValue);
                }
                else
                {
                    cont = false;
                }
            }
            EraseOverlappingFeatures(&script->m_levelFeatures1b4,
                                     feat.m_field0[0], feat.m_field0[1]);
            script->m_levelFeatures1b4.push_back(feat);
            break;
        }

        case TAG_SHAKE_SCREEN:                       // [shake screen]
            script->m_1cc = ScanInt(0);
            script->m_1d0 = ScanInt(0);
            break;

        case TAG_SEAL_ENABLE:                        // [seal enable]
            script->m_1d4 = (ScanInt(0) != 0) ? 1 : 0;
            break;

        case TAG_ACTIVE_SEAL_ENABLE:                 // [active seal enable]
            script->m_1d5 = (ScanInt(0) != 0) ? 1 : 0;
            break;

        case TAG_PHYSICAL_MAGICAL_CHANGE:            // [physical magical change]
            script->m_1d8 = ScanInt(0);
            break;

        case TAG_SKILL_PRELOADING_IMAGE:             // [skill preloading image]
        {
            std::string imgName;
            bool cont = true;
            while (cont)
            {
                script->m_vec1f8.push_back(imgName);
                cont = ScanStr(&imgName);            // 尾部余项不压入（jmp 8a9a10b）
            }
            break;
        }

        case TAG_PURCHASE_GSP:                       // [purchase gsp]
        {
            script->m_vec204.clear();
            ReadIntVecInto(script->m_vec204, &value, &okFlag);
            break;
        }

        case TAG_PURCHASE_GOLD:                      // [purchase gold]
        {
            script->m_vec210.clear();
            ReadIntVecInto(script->m_vec210, &value, &okFlag);
            break;
        }

        case TAG_START_COOL_TIME:                    // [start cool time]
            script->m_21c = ScanInt(0);
            script->m_220 = ScanInt(0);
            break;

        case TAG_SKILL_COMMAND_ADVANTAGE:            // [skill command advantage]
            script->m_240 = ScanInt(0);
            script->m_244 = ScanInt(0);
            break;

        case TAG_IS_LINKED_BLAST:                    // [is linked blast]
            script->m_248 = ScanInt(0);
            break;

        case TAG_LINKED_BLAST_ADD_POINT:             // [linked blast add point]
            script->m_24c = ScanInt(0);
            break;

        case TAG_LINKED_BLAST_DEC_POINT:             // [linked blast dec point]
            script->m_250 = ScanInt(0);
            break;

        case TAG_CHILD_SKILL_INDEX:                  // [child skill index]
            script->m_25c = ScanInt(0);
            break;

        case TAG_FEATURE_SKILL_INDEX:               // [feature skill index]
            script->m_258 = ScanInt(0);
            break;

        case TAG_FEATURE_SKILL_TYPE:                 // [feature skill type]
            script->m_260 = (ScanInt(0) != 0) ? 1 : 0;
            break;

        case TAG_SPECIAL_LEVEL_UP:                   // [special level up]
        {
            STPassiveFeatureData featData;
            script->m_passive268.clear();
            std::string opToken;                     // -0x610
            bool cont = true;
            while (cont)
            {
                int enableVal = ScanInt(&okFlag);    // -0x24
                if (!okFlag)
                {
                    featData.clear();                // OK 后收尾字段复用 featData
                    break;                           // ebx=0 结束（8a9a80c 处判 ebx 出循环）
                }
                bool keepEnable = (enableVal == -1);
                if (!keepEnable)
                    featData.m_enable = 0;           // 仅 -1 保持 ctor 初值 1
                featData.m_4 = ScanInt(0);           // -0x61c
                ScanStr(&opToken);
                featData.m_c = 0;                    // 默认 '+' 分支值
                if (opToken == "+")                  // 0x8e29cb5
                    featData.m_c = 0;
                else if (opToken == "-")             // 0x8e29cb7
                    featData.m_c = 1;
                else if (opToken == "%")             // 0x8e29cb9
                    featData.m_c = 2;
                featData.m_c = ScanInt(0);           // -0x614：数值段直接覆盖符号序号槽
                script->m_passive268.push_back(featData);
                cont = true;                         // ebx=1 继续
            }
            break;
        }

        case TAG_BASIC_EXPLAIN_EX:                   // [basic explain ex]
            ScanStr(&script->m_str278);
            break;

        case TAG_EXPLAIN_EX:                         // [explain ex]
            ScanStr(&script->m_str274);
            break;

        case TAG_EXECUTABLE_STATES:                  // [executable states]
        {
            std::string unused;                      // -0x624（每轮构造/析构）
            bool cont = true;
            while (cont)
            {
                value = ScanInt(&okFlag);
                if (okFlag)
                {
                    int pushVal = value;
                    script->m_vec284.push_back(pushVal);
                }
                else
                {
                    cont = false;
                }
            }
            break;
        }

        case TAG_NUT_SKILL_SCRIPT:                   // [nut skill script]
            ScanStr(&script->m_str278);
            break;

        case TAG_COMMAND_CUSTOMIZING:                // [command customizing]
            script->m_280 = (ScanInt(0) != 0) ? 1 : 0;
            break;

        case TAG_COMBO_SET:                          // [combo set]
        {
            bool first = true;
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                {
                    script->m_290 = 1;
                    break;
                }
                if (first)
                {
                    script->m_vec294.clear();        // only 头条清空 vector
                    first = false;
                }
                int pushVal = value;
                script->m_vec294.push_back(pushVal);
            }
            break;
        }

        default:                                     // 8a9ab85（含 tagId==-1 未知标签）
            break;
        }

        if (!s_skillTagReady)                        // 8a9ac24 循环尾
        {
            s_skillTagLock.leave();
            s_skillTagReady = true;
            continue;                                // jmp 8a979dc 重新读下一个 token
        }
        break;                                       // 已就绪：退出 switch 循环
    }

    if (!s_skillTagReady)                            // 8a9ab85 分支入口
    {
        FinishBodyParse(script);
        FinishModuleTags(script, token, path, static_cast<int>(job));
        return 1;
    }

    return FinalizeImport(script, job, iconIdx, path);
}
