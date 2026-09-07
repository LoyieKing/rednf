// ============================================================================
// df_game_r 还原 —— STSkillScript（G5 数据/脚本域）
// 逐函数对照 docs/class_func_reports/STSkillScript.md（嵌套类布局）与
// docs/class_func_reports/_lt_global_gt_/ImportSkillScript.md（ImportSkillScript
// ORIG 汇编 0x08a9792f..0x08a9af4e + Ghidra C 全量）。Clear 0x083750b4 /
// ctor 0x08375642 / dtor 0x0837a0fa，嵌套类见 0x08a9b118 / 0x08a9b164 /
// 0x08a9b256 / 0x083660ec。
// 脚本解析函数（loadRDARScriptFile/ScanType/ScanInt/ScanFloat）经
// DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "STSkillScript.h"

#include <map>

#include "DNFLexWrapper.h"
#include "CEnvironment.h"   // class SpinLock
#include "NpcScript.h"      // class STScriptFileList（0x18 布局）
#include "Quest.h"             // enum ENUM_CHARACTERJOB

// ===================== 前置声明（ORIG 自由函数，实现于各权威 TU） =====================

// （RegisterSkillTags / FinalizeImport 为 ORIG ImportSkillScript 内联段，无独立符号，
//   已分别内联到主循环注册分支与尾部修正，禁止以函数形式编造。）

bool importCommand(STKeyCommand* command);                          // T 0x088c167a
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
                         int type, bool flag);                      // T 0x08a97637

extern char g_szSkillScriptBaseDirectory[0x100];                   // B 0x9500720（InitScriptProducers_4.cpp）
extern std::map<int, std::string> g_staSkillScriptFileList[11];     // B 0x9500820


// ---- ORIG ImportSkillScript 尾部修正段（8a9ac50..8a9af42）内联于 ImportSkillScript，
//      ORIG 无独立 FinalizeImport 符号；对应 back()/size()/empty() 等调用点须落在
//      ImportSkillScript 切片内，故直接内联在函数尾部 finalize 标签处，禁止拆独立函数。
// 汇编证据：docs/class_func_reports/_lt_global_gt_/ImportSkillScript.md 2862-3096 行 +
// Ghidra C（max level 夹紧 / 价格向量 back() 补齐 / 图标索引定位）。

// ===================== 嵌套结构 =====================

SkillUseItem::SkillUseItem()
{
    clear();
}

void SkillUseItem::clear()
{
    m_index = -1;
    for (int i = 0; i <= 1; ++i)
        m_useLevel[i] = 0;
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

// TAG_* 常量：与 kSkillTags 表 id 一一对应（ORIG ImportSkillScript switch case 实证）
enum
{
    TAG_NAME = 0x129,
    TAG_NAME2 = 0x12c,
    TAG_EXPLAIN = 0x12f,
    TAG_BASIC_EXPLAIN = 0x132,
    TAG_BASIC_PROPERTY = 0x135,
    TAG_MAXIMUM_LEVEL = 0x159,
    TAG_PURCHASE_COST = 0x15c,
    TAG_SPECIAL_PURCHASE_COST = 0x166,
    TAG_REQUIRED_LEVEL = 0x172,
    TAG_REQUIRED_LEVEL_RANGE = 0x175,
    TAG_REQUIRED_RANK = 0x178,
    TAG_REQUIRED_GUILD_LEVEL = 0x17b,
    TAG_REQUIRED_GUILD_LEVEL_RANGE = 0x17e,
    TAG_PRE_REQUIRED_SKILL = 0x181,
    TAG_STEEL_LEARNING_SKILL = 0x18e,
    TAG_TYPE = 0x194,
    TAG_SKILL_CLASS = 0x19b,
    TAG_GROWTYPE_FITNESS = 0x19f,
    TAG_SECOND_GROWTYPE_FITNESS = 0x1a4,
    TAG_GROWTYPE_MAXIMUM_LEVEL = 0x1ac,
    TAG_SECOND_GROWTYPE_MAXIMUM_LEVEL = 0x1b1,
    TAG_SKILL_FITNESS_GROWTYPE = 0x1b9,
    TAG_SKILL_FITNESS_SECOND_GROWTYPE = 0x1c4,
    TAG_CASTING_TIME = 0x1cf,
    TAG_DURABILITY_DECREASE_RATE = 0x1d3,
    TAG_CONSUME_MP = 0x1d6,
    TAG_MAINTAIN_MP = 0x1dc,
    TAG_CONSUME_ITEM = 0x1e0,
    TAG_LIMIT_COUNT = 0x1ee,
    TAG_AUTO_LIMIT_COUNT_REDUCE = 0x1f8,
    TAG_COOL_TIME = 0x1fb,
    TAG_LEAGUE_BAN = 0x1ff,
    TAG_SKILL_UNDER_COOLTIME_EFFECT = 0x202,
    TAG_SKILL_UNDER_COOLTIME_EFFECT_EACH = 0x20d,
    TAG_AUTO_COOLTIME_APPLY = 0x218,
    TAG_WEAPON_EFFECT_TYPE = 0x21b,
    TAG_ICON = 0x224,
    TAG_COMMAND_KEY_EXPLAIN = 0x22a,
    TAG_COMMAND = 0x22d,
    TAG_STATIC_DATA = 0x230,
    TAG_COOLTIME_LEVEL_INFO = 0x23b,
    TAG_LEVEL_INFO = 0x23f,
    TAG_LEVEL_PROPERTY = 0x258,
    TAG_SHAKE_SCREEN = 0x27c,
    TAG_SEAL_ENABLE = 0x280,
    TAG_ACTIVE_SEAL_ENABLE = 0x283,
    TAG_PHYSICAL_MAGICAL_CHANGE = 0x287,
    TAG_SKILL_PRELOADING_IMAGE = 0x28b,
    TAG_PURCHASE_GSP = 0x292,
    TAG_PURCHASE_GOLD = 0x29d,
    TAG_START_COOL_TIME = 0x2a8,
    TAG_SKILL_COMMAND_ADVANTAGE = 0x2ac,
    TAG_IS_LINKED_BLAST = 0x2b1,
    TAG_LINKED_BLAST_ADD_POINT = 0x2b4,
    TAG_LINKED_BLAST_DEC_POINT = 0x2b7,
    TAG_CHILD_SKILL_INDEX = 0x2bb,
    TAG_FEATURE_SKILL_INDEX = 0x2be,
    TAG_FEATURE_SKILL_TYPE = 0x2c1,
    TAG_SPECIAL_LEVEL_UP = 0x2c4,
    TAG_BASIC_EXPLAIN_EX = 0x2e1,
    TAG_EXPLAIN_EX = 0x2e4,
    TAG_EXECUTABLE_STATES = 0x2e7,
    TAG_NUT_SKILL_SCRIPT = 0x2fe,
    TAG_COMMAND_CUSTOMIZING = 0x319,
    TAG_COMBO_SET = 0x31d,
};

// ORIG 全局实证：map<PKc,i> 全部用 less_TCHAR（内容比较）——
// find(token.c_str()) 为动态指针，默认 std::less 指针比较恒 miss。
// 65 张标签条目改为 ImportSkillScript 内函数局部静态 __lookup 的逐条 insert
// （ORIG __lookup 是函数局部 static，编译器 __cxa_guard 包裹，见主函数）。
typedef std::map<const char*, int, less_TCHAR> SkillTagMap;

} // namespace

int ImportSkillScript(STSkillScript* script, const char* path,
                      ENUM_CHARACTERJOB job, int paramB, int iconIdx)
{
    if (!loadRDARScriptFile(g_szSkillScriptBaseDirectory, path))   // 8a97942 实参 &g_szSkillScriptBaseDirectory
    {
        return 0;                                   // 8a97955 → return ebx=0
    }
    script->Clear();

    // ---- ORIG 4.1 槽位序（ebp 相对，浅→深，0x08a9792f 全函数汇编实证）：
    // -0x1c finIdx / -0x1d moduleRet / -0x24 enVal / -0x28 srcIdx / -0x2c kIdx /
    // -0x30 chunk / -0x34 row / -0x38 jIdx / -0x3c poolSize / -0x40 rowCnt /
    // -0x44..-0x68 各 case 循环 i/j / -0x6c tagId / -0x70.. guard 临时区 /
    // -0x5b0.. -0x5c0 段（guard 尾部+find 临时）/ -0x5d4 token ... -0x5ec pool。
    // 4.4 按“先声明→浅”分配、块作用域临时会共享槽，故 C89 风格集中声明防共享。
    int finIdx;                                     // finalize 夹紧循环 i（-0x1c）
    bool moduleRet;                                 // default 死存返回值（-0x1d，8a9abd9）
    int enVal;                                      // special level up enableVal（-0x24）
    int srcIdx;                                     // level info 源下标（-0x28）
    int kIdx;                                       // level info 内层 k（-0x2c）
    int chunk;                                      // level info 列块（-0x30）
    int jIdx;                                       // level info 外层 j（-0x38）
    int poolSize;                                   // level info pool.size()（-0x3c）
    int rowCnt;                                     // level info 行数（-0x40）
    int coolIdx;                                    // cool time i（-0x44）
    int maintainIdx;                                // maintain MP i（-0x48）
    int mpIdx;                                      // consume MP i（-0x4c）
    int castIdx;                                    // casting time i（-0x50）
    int sMaxJ;                                      // second growtype maximum j（-0x54）
    int sMaxI;                                      // second growtype maximum i（-0x58）
    int maxIdx;                                     // growtype maximum i（-0x5c）
    int sFitJ;                                      // second growtype fitness j（-0x60）
    int sFitI;                                      // second growtype fitness i（-0x64）
    int fitIdx;                                     // growtype fitness i（-0x68）
    int tagId;                                      // 标签 id（-0x6c）

    std::string token;                              // -0x5d4
    std::string scanStrBuf;                         // -0x5d8
    int value = 0;                                  // -0x5dc
    bool okFlag = false;                            // -0x5dd
    std::vector<float> valuePool;                   // -0x5ec
    valuePool.reserve(0xfa0);

read_token:                                         // 8a979bb：唯一 ScanType 调用点（首读+续读共用）
    if (!ScanType(token, true))
        goto finalize;                              // 8a979d6 jne 8a9ac50（尾部修正段）

    // ORIG __lookup/__init_lock 声明在 read_token 之后（8a979dc 起两个 __cxa_guard
    // 检查块每轮循环都重新执行）——静态声明必须落在 loop 区域内，否则 guard 序
    // 只出现一次，指令流与 ORIG 不符。__inited 常量初始化无 guard。
    static SkillTagMap __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;


loop_head:                                          // 8a979dc：guard(__lookup) → guard(__init_lock) → __inited 判定
    if (__inited)                                   // 8a97ad0 movzbl __inited; test; je 8a97b98
    {
        // ORIG 8a97adf..8a97b93：两臂汇聚到 %eax（mov $-1,%eax / second→eax）后
        // 单点 store -0x6c —— 三目形（P47）。
        SkillTagMap::const_iterator it = __lookup.find(token.c_str());
        tagId = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();                        // 8a97b9f SpinLock::enter()
        if (__inited)                               // 8a97ba4 复查：他线程已注册
        {
            __init_lock.leave();                    // 8a97bb6 SpinLock::leave()
            goto loop_head;                         // 8a97bbc jmp 8a979dc（同 token 重跑）
        }
        // 首轮 tagId=0 → 直入 switch 决策树（8a97bc1 movl $0,-0x6c(%ebp)）：
        // 65 张标签经各 case 头部的惰性 insert 链逐条注册（ORIG 8a97f87 起
        // case 物理序 = 标签 id 升序 = tree je 目标序），末位 [combo set] 落
        // default 头（8a9ab85 `if(__inited)` 短路）→ 主循环尾统一 leave。
        tagId = 0;
    }


    switch (tagId)
    {
        case 0:                                     // 8a97c31 test%eax je 8a97f87：首轮 tagId=0 与
        case TAG_NAME:                              // [name]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[name]", 0x129));
                goto L_SKLTAG_TAG_NAME2;
            }
            ScanStr(&script->m_str4);
            break;

        L_SKLTAG_TAG_NAME2:
        case TAG_NAME2:                              // [name2]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[name2]", 0x12c));
                goto L_SKLTAG_TAG_EXPLAIN;
            }
            ScanStr(&script->m_str8);
            break;

        L_SKLTAG_TAG_EXPLAIN:
        case TAG_EXPLAIN:                            // [explain]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[explain]", 0x12f));
                goto L_SKLTAG_TAG_BASIC_EXPLAIN;
            }
            ScanStr(&script->m_strc);
            break;

        L_SKLTAG_TAG_BASIC_EXPLAIN:
        case TAG_BASIC_EXPLAIN:                      // [basic explain]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[basic explain]", 0x132));
                goto L_SKLTAG_TAG_BASIC_PROPERTY;
            }
            ScanStr(&script->m_str10);
            break;

        L_SKLTAG_TAG_BASIC_PROPERTY:
        case TAG_BASIC_PROPERTY:                     // [basic property]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[basic property]", 0x135));
                goto L_SKLTAG_TAG_MAXIMUM_LEVEL;
            }
        {
            // ORIG 8a981a7..：先 ScanInt 落 value 槽（-0x5dc）→ okFlag 判失败直跳主循环尾
            // （8a981c7 jne 8a9ac14 岛），feat ctor（-0x650）在判后；字段 [0] 经 value 槽两步存。
            value = ScanInt(&okFlag);
            if (!okFlag)
                break;
            SkillLevelFeature feat;
            feat.m_field0[0] = value;
            feat.m_field0[1] = ScanInt((bool*)0);
            ScanStr(&feat.m_str8);
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;                           // 8a9822a jne 8a982b1（出循环进 erase）
                int value2 = ScanInt((bool*)0);
                feat.m_vecC.push_back(std::pair<int, int>(value, value2));
                float fValue = ScanFloat((bool*)0);
                feat.m_vec18.push_back(fValue);
            }
            // ORIG 内联重叠擦除（8a982b2..8a983a9）：删除与 [feat.f0, feat.f1] 相交项
            for (std::vector<SkillLevelFeature>::iterator it =
                     script->m_levelFeatures1b4.begin();
                 it != script->m_levelFeatures1b4.end();)
            {
                // ORIG 8a982d5..：&& 短路双条件直接物化 %eax（mov$1/mov$0/test%al），
                // 无 bool 栈槽——写成单一 if(A && B)。
                if (((*it).m_field0[1] >= feat.m_field0[0]) &&
                    ((*it).m_field0[0] <= feat.m_field0[1]))
                    it = script->m_levelFeatures1b4.erase(it);
                else
                    it++;
            }
            script->m_levelFeatures1c0.push_back(feat);
            break;
        }

        L_SKLTAG_TAG_MAXIMUM_LEVEL:
        case TAG_MAXIMUM_LEVEL:                      // [maximum level]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[maximum level]", 0x159));
                goto L_SKLTAG_TAG_PURCHASE_COST;
            }
            script->m_14 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_PURCHASE_COST:
        case TAG_PURCHASE_COST:                      // [purchase cost]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[purchase cost]", 0x15c));
                goto L_SKLTAG_TAG_SPECIAL_PURCHASE_COST;
            }
        {
            script->m_vec18.clear();                 // 8a984cd
            for (;;)                                 // ORIG 内联读取向量（8a984db..）
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;                           // 失败→主循环尾 8a9ac24
                script->m_vec18.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_SPECIAL_PURCHASE_COST:
        case TAG_SPECIAL_PURCHASE_COST:              // [special purchase cost]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[special purchase cost]", 0x166));
                goto L_SKLTAG_TAG_REQUIRED_LEVEL;
            }
        {
            script->m_vec24.clear();
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;
                script->m_vec24.push_back(value);
            }
            break;
        }



        L_SKLTAG_TAG_REQUIRED_LEVEL:
        case TAG_REQUIRED_LEVEL:                     // [required level]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[required level]", 0x172));
                goto L_SKLTAG_TAG_REQUIRED_LEVEL_RANGE;
            }
            script->m_30 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_REQUIRED_LEVEL_RANGE:
        case TAG_REQUIRED_LEVEL_RANGE:               // [required level range]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[required level range]", 0x175));
                goto L_SKLTAG_TAG_REQUIRED_RANK;
            }
            script->m_34 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_REQUIRED_RANK:
        case TAG_REQUIRED_RANK:                      // [required rank]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[required rank]", 0x178));
                goto L_SKLTAG_TAG_REQUIRED_GUILD_LEVEL;
            }
            script->m_38 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_REQUIRED_GUILD_LEVEL:
        case TAG_REQUIRED_GUILD_LEVEL:               // [required guild level]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[required guild level]", 0x17b));
                goto L_SKLTAG_TAG_REQUIRED_GUILD_LEVEL_RANGE;
            }
            script->m_3c = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_REQUIRED_GUILD_LEVEL_RANGE:
        case TAG_REQUIRED_GUILD_LEVEL_RANGE:         // [required guild level range]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[required guild level range]", 0x17e));
                goto L_SKLTAG_TAG_PRE_REQUIRED_SKILL;
            }
            script->m_40 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_PRE_REQUIRED_SKILL:
        case TAG_PRE_REQUIRED_SKILL:                 // [pre required skill]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[pre required skill]", 0x181));
                goto L_SKLTAG_TAG_STEEL_LEARNING_SKILL;
            }
        {
            script->m_vec44.clear();
            script->m_vec50.clear();
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;                           // 8a988aa je 走失败路（nop; jmp loop_end）
                // ORIG 8a988b2 vec44.push_back(value)（-0x5dc 命名左值 → RKi 实参）；
                // 8a988d6/8e2 vec50.push_back(v2)：值先落 -0x490 命名槽再取址 ⇒ 同为
                // const int&（RKi，0808e1ee 调用体），非右值 EOi 实例。
                script->m_vec44.push_back(value);
                int v2 = ScanInt((bool*)0);
                script->m_vec50.push_back(v2);
            }
            break;
        }

        L_SKLTAG_TAG_STEEL_LEARNING_SKILL:
        case TAG_STEEL_LEARNING_SKILL:               // [steel learning skill]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[steel learning skill]", 0x18e));
                goto L_SKLTAG_TAG_TYPE;
            }
        {
            // ORIG 8a98953：ScanInt(NULL)（非 &okFlag），结果 cmp $1 → sete %dl → 1 字节
            // 直写 +0x1e8（char 字段）；无中间命名槽。
            script->m_1e8 = (ScanInt((bool*)0) == 1);
            script->m_1ec = ScanInt((bool*)0);
            script->m_1f0 = ScanInt((bool*)0);
            script->m_1f4 = ScanInt((bool*)0);
            break;
        }

        L_SKLTAG_TAG_TYPE:
        case TAG_TYPE:                               // [type]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[type]", 0x194));
                goto L_SKLTAG_TAG_SKILL_CLASS;
            }
            ScanStr(&scanStrBuf);
            if (scanStrBuf == "[active]")
                script->m_5c = 0;
            else if (scanStrBuf == "[passive]")
                script->m_5c = 1;
            break;                                   // 其它值不写（8a9ac17 落主循环尾）

        L_SKLTAG_TAG_SKILL_CLASS:
        case TAG_SKILL_CLASS:                        // [skill class]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[skill class]", 0x19b));
                goto L_SKLTAG_TAG_GROWTYPE_FITNESS;
            }
            script->m_60 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_GROWTYPE_FITNESS:
        case TAG_GROWTYPE_FITNESS:                   // [growtype fitness]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[growtype fitness]", 0x19f));
                goto L_SKLTAG_TAG_SECOND_GROWTYPE_FITNESS;
            }
            // ORIG 8a98b5f fold = `lea 0x18(%ebx),%ecx; mov %eax,0x4(%edx,%ecx,4)`
            // （m_arr64@0x64 = 4 + 0x18*4，4.4 与 4.1 折叠同形）
            for (fitIdx = 0; fitIdx <= 5; ++fitIdx)
                script->m_arr64[fitIdx] = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_SECOND_GROWTYPE_FITNESS:
        case TAG_SECOND_GROWTYPE_FITNESS:            // [second growtype fitness]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[second growtype fitness]", 0x1a4));
                goto L_SKLTAG_TAG_GROWTYPE_MAXIMUM_LEVEL;
            }
            for (sFitI = 0; sFitI <= 5; ++sFitI)
                for (sFitJ = 0; sFitJ <= 1; ++sFitJ)
                    script->m_growType[sFitI].m_0[sFitJ] =
                        ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_GROWTYPE_MAXIMUM_LEVEL:
        case TAG_GROWTYPE_MAXIMUM_LEVEL:             // [growtype maximum level]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[growtype maximum level]", 0x1ac));
                goto L_SKLTAG_TAG_SECOND_GROWTYPE_MAXIMUM_LEVEL;
            }
            for (maxIdx = 0; maxIdx <= 5; ++maxIdx)
                script->m_arr7c[maxIdx] = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_SECOND_GROWTYPE_MAXIMUM_LEVEL:
        case TAG_SECOND_GROWTYPE_MAXIMUM_LEVEL:      // [second growtype maximum level]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[second growtype maximum level]", 0x1b1));
                goto L_SKLTAG_TAG_SKILL_FITNESS_GROWTYPE;
            }
            for (sMaxI = 0; sMaxI <= 5; ++sMaxI)
                for (sMaxJ = 0; sMaxJ <= 1; ++sMaxJ)
                    script->m_growType[sMaxI].m_8[sMaxJ] =
                        ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_SKILL_FITNESS_GROWTYPE:
        case TAG_SKILL_FITNESS_GROWTYPE:             // [skill fitness growtype]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[skill fitness growtype]", 0x1b9));
                goto L_SKLTAG_TAG_SKILL_FITNESS_SECOND_GROWTYPE;
            }
        {
            script->m_vec1dc.clear();                // ORIG 目标 +0x1dc（8a98dd8）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;                           // 失败→主循环尾
                script->m_vec1dc.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_SKILL_FITNESS_SECOND_GROWTYPE:
        case TAG_SKILL_FITNESS_SECOND_GROWTYPE:      // [skill fitness second growtype]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[skill fitness second growtype]", 0x1c4));
                goto L_SKLTAG_TAG_CASTING_TIME;
            }
        {
            script->m_vecf4.clear();                 // 目标 +0xf4（8a98e8a..8a98ec2）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;
                script->m_vecf4.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_CASTING_TIME:
        case TAG_CASTING_TIME:                       // [casting time]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[casting time]", 0x1cf));
                goto L_SKLTAG_TAG_DURABILITY_DECREASE_RATE;
            }
            // ORIG 8a98f57 `lea 0x44(%ebx),%ecx; mov %eax,(%edx,%ecx,4)`：int 视图基址
            // +0x110（m_dw110，STSkillScript.h 同存储 union 字段）。
            for (castIdx = 0; castIdx <= 1; ++castIdx)
                script->m_dw110[castIdx] = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_DURABILITY_DECREASE_RATE:
        case TAG_DURABILITY_DECREASE_RATE:           // [durability decrease rate]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[durability decrease rate]", 0x1d3));
                goto L_SKLTAG_TAG_CONSUME_MP;
            }
            script->m_118 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_CONSUME_MP:
        case TAG_CONSUME_MP:                         // [consume MP]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[consume MP]", 0x1d6));
                goto L_SKLTAG_TAG_MAINTAIN_MP;
            }
            for (mpIdx = 0; mpIdx <= 1; ++mpIdx)
                script->m_f100[mpIdx] = ScanFloat((bool*)0);
            break;

        L_SKLTAG_TAG_MAINTAIN_MP:
        case TAG_MAINTAIN_MP:                        // [maintain MP]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[maintain MP]", 0x1dc));
                goto L_SKLTAG_TAG_CONSUME_ITEM;
            }
            // ORIG 8a990f2 `lea 0x40(%ebx),%edx; fstps 0x8(%eax,%edx,4)`：float 视图基址
            // +0x108（m_f108 union 字段）。
            for (maintainIdx = 0; maintainIdx <= 1; ++maintainIdx)
                script->m_f108[maintainIdx] = ScanFloat((bool*)0);
            break;
        L_SKLTAG_TAG_CONSUME_ITEM:
        case TAG_CONSUME_ITEM:                       // [consume item]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[consume item]", 0x1e0));
                goto L_SKLTAG_TAG_LIMIT_COUNT;
            }
        {
            SkillUseItem item;
            script->m_useItems11c.clear();
            for (;;)
            {
                item.m_index = ScanInt(&okFlag);
                if (!okFlag)
                    break;                           // 8a991ab je：失败直落 nop;jmp loop_end 岛
                item.m_useLevel[0] = ScanInt((bool*)0);
                item.m_useLevel[1] = ScanInt((bool*)0);
                script->m_useItems11c.push_back(item);
            }
            break;
        }

        L_SKLTAG_TAG_LIMIT_COUNT:
        case TAG_LIMIT_COUNT:                        // [limit count]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[limit count]", 0x1ee));
                goto L_SKLTAG_TAG_AUTO_LIMIT_COUNT_REDUCE;
            }
        {
            script->m_vec128.clear();                // ORIG 目标 +0x128（8a99254）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;                           // 失败→主循环尾
                script->m_vec128.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_AUTO_LIMIT_COUNT_REDUCE:
        case TAG_AUTO_LIMIT_COUNT_REDUCE:            // [auto limit count reduce]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[auto limit count reduce]", 0x1f8));
                goto L_SKLTAG_TAG_COOL_TIME;
            }
            script->m_134 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_COOL_TIME:
        case TAG_COOL_TIME:                          // [cool time]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[cool time]", 0x1fb));
                goto L_SKLTAG_TAG_LEAGUE_BAN;
            }
            for (coolIdx = 0; coolIdx <= 1; ++coolIdx)
                script->m_arr138[coolIdx] = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_LEAGUE_BAN:
        case TAG_LEAGUE_BAN:                         // [league ban]（ORIG 0x1ff case：写 +0x23c）
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[league ban]", 0x1ff));
                goto L_SKLTAG_TAG_SKILL_UNDER_COOLTIME_EFFECT;
            }
            script->m_23c = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_SKILL_UNDER_COOLTIME_EFFECT:
        case TAG_SKILL_UNDER_COOLTIME_EFFECT:        // [skill under cooltime effect]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[skill under cooltime effect]", 0x202));
                goto L_SKLTAG_TAG_SKILL_UNDER_COOLTIME_EFFECT_EACH;
            }
        {
            script->m_vec140.clear();                // 目标 +0x140（8a99487）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;
                script->m_vec140.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_SKILL_UNDER_COOLTIME_EFFECT_EACH:
        case TAG_SKILL_UNDER_COOLTIME_EFFECT_EACH:   // [skill under cooltime effect each]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[skill under cooltime effect each]", 0x20d));
                goto L_SKLTAG_TAG_AUTO_COOLTIME_APPLY;
            }
        {
            script->m_vec14c.clear();                // 目标 +0x14c（8a99571）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;
                script->m_vec14c.push_back(value);
            }
            break;
        }


        L_SKLTAG_TAG_AUTO_COOLTIME_APPLY:
        case TAG_AUTO_COOLTIME_APPLY:                // [auto cooltime apply]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[auto cooltime apply]", 0x218));
                goto L_SKLTAG_TAG_WEAPON_EFFECT_TYPE;
            }
            script->m_158 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_WEAPON_EFFECT_TYPE:
        case TAG_WEAPON_EFFECT_TYPE:                 // [weapon effect type]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[weapon effect type]", 0x21b));
                goto L_SKLTAG_TAG_ICON;
            }
            ScanStr(&scanStrBuf);
            if (scanStrBuf == "[none]")              // 0x8e29aea
                script->m_15c = 0;
            else if (scanStrBuf == "[physical]")     // 0x8e29af1
                script->m_15c = 1;
            else if (scanStrBuf == "[magical]")      // 0x8e29afc
                script->m_15c = 2;
            break;

        L_SKLTAG_TAG_ICON:
        case TAG_ICON:                               // [icon]（8a99755：ScanStr/+0x168、ScanStr(+0x164)/+0x16c）
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[icon]", 0x224));
                goto L_SKLTAG_TAG_COMMAND_KEY_EXPLAIN;
            }
            ScanStr(&script->m_str160[0]);
            script->m_arr168[0] = ScanInt((bool*)0);
            ScanStr(&script->m_str160[1]);
            script->m_arr168[1] = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_COMMAND_KEY_EXPLAIN:
        case TAG_COMMAND_KEY_EXPLAIN:                // [command key explain]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[command key explain]", 0x22a));
                goto L_SKLTAG_TAG_COMMAND;
            }
            ScanStr(&script->m_str194);
            break;

        L_SKLTAG_TAG_COMMAND:
        case TAG_COMMAND:                            // [command]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[command]", 0x22d));
                goto L_SKLTAG_TAG_STATIC_DATA;
            }
            importCommand(&script->m_keyCommand170);
            break;

        L_SKLTAG_TAG_STATIC_DATA:
        case TAG_STATIC_DATA:                        // [static data]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[static data]", 0x230));
                goto L_SKLTAG_TAG_COOLTIME_LEVEL_INFO;
            }
        {
            script->m_vec198.clear();                // 目标 +0x198（8a998e8）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;
                script->m_vec198.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_COOLTIME_LEVEL_INFO:
        case TAG_COOLTIME_LEVEL_INFO:                // [cooltime level info]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[cooltime level info]", 0x23b));
                goto L_SKLTAG_TAG_LEVEL_INFO;
            }
            script->m_1a4 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_LEVEL_INFO:
        case TAG_LEVEL_INFO:                         // [level info]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[level info]", 0x23f));
                goto L_SKLTAG_TAG_LEVEL_PROPERTY;
            }
        {
            script->m_vec1a8.clear();
            int rowCnt = ScanInt((bool*)0);                 // -0x40
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

        L_SKLTAG_TAG_LEVEL_PROPERTY:
        case TAG_LEVEL_PROPERTY:                     // [level property]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[level property]", 0x258));
                goto L_SKLTAG_TAG_SHAKE_SCREEN;
            }
        {
            // ORIG 8a99c04..：与 [basic property] 同构（value 槽两步存 + ctor 后置于 ok 判）
            value = ScanInt(&okFlag);
            if (!okFlag)
                break;
            SkillLevelFeature feat;
            feat.m_field0[0] = value;
            feat.m_field0[1] = ScanInt((bool*)0);
            ScanStr(&feat.m_str8);
            for (;;)
            {
                value = ScanInt(&okFlag);
            if (!okFlag)
                break;
                    int value2 = ScanInt((bool*)0);
                feat.m_vecC.push_back(std::pair<int, int>(value, value2));
                    float fValue = ScanFloat((bool*)0);
                feat.m_vec18.push_back(fValue);
            }
            // ORIG 内联重叠擦除（[level property] 同 [basic property] 段）
            for (std::vector<SkillLevelFeature>::iterator it =
                     script->m_levelFeatures1b4.begin();
                 it != script->m_levelFeatures1b4.end();)
            {
                // ORIG 8a99d0e..：同 [basic property] erase 形
                if (((*it).m_field0[1] >= feat.m_field0[0]) &&
                    ((*it).m_field0[0] <= feat.m_field0[1]))
                    it = script->m_levelFeatures1b4.erase(it);
                else
                    it++;
            }
            script->m_levelFeatures1b4.push_back(feat);
            break;
        }

        L_SKLTAG_TAG_SHAKE_SCREEN:
        case TAG_SHAKE_SCREEN:                       // [shake screen]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[shake screen]", 0x27c));
                goto L_SKLTAG_TAG_SEAL_ENABLE;
            }
            script->m_1cc = ScanInt((bool*)0);
            script->m_1d0 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_SEAL_ENABLE:
        case TAG_SEAL_ENABLE:                        // [seal enable]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[seal enable]", 0x280));
                goto L_SKLTAG_TAG_ACTIVE_SEAL_ENABLE;
            }
            script->m_1d4 = (ScanInt((bool*)0) != 0) ? 1 : 0;
            break;

        L_SKLTAG_TAG_ACTIVE_SEAL_ENABLE:
        case TAG_ACTIVE_SEAL_ENABLE:                 // [active seal enable]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[active seal enable]", 0x283));
                goto L_SKLTAG_TAG_PHYSICAL_MAGICAL_CHANGE;
            }
            script->m_1d5 = (ScanInt((bool*)0) != 0) ? 1 : 0;
            break;

        L_SKLTAG_TAG_PHYSICAL_MAGICAL_CHANGE:
        case TAG_PHYSICAL_MAGICAL_CHANGE:            // [physical magical change]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[physical magical change]", 0x287));
                goto L_SKLTAG_TAG_SKILL_PRELOADING_IMAGE;
            }
            script->m_1d8 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_SKILL_PRELOADING_IMAGE:
        case TAG_SKILL_PRELOADING_IMAGE:             // [skill preloading image]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[skill preloading image]", 0x28b));
                goto L_SKLTAG_TAG_PURCHASE_GSP;
            }
        {
            std::string imgName;
            while (ScanStr(&imgName))                // ORIG：先 ScanStr 后 push（8a9a0d7..）
                script->m_vec1f8.push_back(imgName);
            break;
        }

        L_SKLTAG_TAG_PURCHASE_GSP:
        case TAG_PURCHASE_GSP:                       // [purchase gsp]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[purchase gsp]", 0x292));
                goto L_SKLTAG_TAG_PURCHASE_GOLD;
            }
        {
            script->m_vec204.clear();                // 目标 +0x204（purchase gsp）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;
                script->m_vec204.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_PURCHASE_GOLD:
        case TAG_PURCHASE_GOLD:                      // [purchase gold]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[purchase gold]", 0x29d));
                goto L_SKLTAG_TAG_START_COOL_TIME;
            }
        {
            script->m_vec210.clear();                // 目标 +0x210（purchase gold）
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                    break;
                script->m_vec210.push_back(value);
            }
            break;
        }

        L_SKLTAG_TAG_START_COOL_TIME:
        case TAG_START_COOL_TIME:                    // [start cool time]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[start cool time]", 0x2a8));
                goto L_SKLTAG_TAG_SKILL_COMMAND_ADVANTAGE;
            }
            script->m_21c = ScanInt((bool*)0);
            script->m_220 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_SKILL_COMMAND_ADVANTAGE:
        case TAG_SKILL_COMMAND_ADVANTAGE:            // [skill command advantage]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[skill command advantage]", 0x2ac));
                goto L_SKLTAG_TAG_IS_LINKED_BLAST;
            }
            script->m_240 = ScanInt((bool*)0);
            script->m_244 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_IS_LINKED_BLAST:
        case TAG_IS_LINKED_BLAST:                    // [is linked blast]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[is linked blast]", 0x2b1));
                goto L_SKLTAG_TAG_LINKED_BLAST_ADD_POINT;
            }
            script->m_248 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_LINKED_BLAST_ADD_POINT:
        case TAG_LINKED_BLAST_ADD_POINT:             // [linked blast add point]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[linked blast add point]", 0x2b4));
                goto L_SKLTAG_TAG_LINKED_BLAST_DEC_POINT;
            }
            script->m_24c = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_LINKED_BLAST_DEC_POINT:
        case TAG_LINKED_BLAST_DEC_POINT:             // [linked blast dec point]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[linked blast dec point]", 0x2b7));
                goto L_SKLTAG_TAG_CHILD_SKILL_INDEX;
            }
            script->m_250 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_CHILD_SKILL_INDEX:
        case TAG_CHILD_SKILL_INDEX:                  // [child skill index]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[child skill index]", 0x2bb));
                goto L_SKLTAG_TAG_FEATURE_SKILL_INDEX;
            }
            script->m_25c = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_FEATURE_SKILL_INDEX:
        case TAG_FEATURE_SKILL_INDEX:               // [feature skill index]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[feature skill index]", 0x2be));
                goto L_SKLTAG_TAG_FEATURE_SKILL_TYPE;
            }
            script->m_258 = ScanInt((bool*)0);
            break;

        L_SKLTAG_TAG_FEATURE_SKILL_TYPE:
        case TAG_FEATURE_SKILL_TYPE:                 // [feature skill type]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[feature skill type]", 0x2c1));
                goto L_SKLTAG_TAG_SPECIAL_LEVEL_UP;
            }
            script->m_260 = (ScanInt((bool*)0) != 0) ? 1 : 0;
            break;

        L_SKLTAG_TAG_SPECIAL_LEVEL_UP:
        case TAG_SPECIAL_LEVEL_UP:                   // [special level up]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[special level up]", 0x2c4));
                goto L_SKLTAG_TAG_BASIC_EXPLAIN_EX;
            }
        {
            script->m_passive268.clear();            // 8a9a6d4 clear(+0x268)
            std::string opToken;                     // -0x610（每轮复用，ORIG 每轮 string ctor/dtor）
            bool cont = true;                        // ebx
            while (cont)                             // ORIG do{ obj ctor; ... }while(ebx)（8a9a6d9..8a9a822）
            {
                STPassiveFeatureData featData;       // -0x620：每轮 ctor 重置 enable=1 / m_4=0 / m_8=0 / m_c=0
                enVal = ScanInt(&okFlag);            // -0x24
                if (!okFlag)                         // 8a9a70d：失败 ebx=0 出循环
                {
                    cont = false;
                    break;
                }
                if (enVal != -1)                     // 8a9a71e：仅 -1 保持 ctor 初值 enable=1
                    featData.m_enable = 0;           // movb $0,-0x620
                featData.m_4 = ScanInt(&okFlag);     // -0x61c（8a9a734 传 &ok，值不查）
                ScanStr(&opToken);
                // ORIG 8a9a74d/75e 先比 "+"（==则显式写 0），再 "-"，再 "%"，
                // 三者皆非则保留 ctor 初值（无 else）；共 3 处 string operator==。
                if (opToken == "+")                  // 8a9a75e == → movl $0,-0x618
                    featData.m_8 = 0;
                else if (opToken == "-")             // 8a9a784 == → 1
                    featData.m_8 = 1;
                else if (opToken == "%")             // 8a9a7aa == → 2
                    featData.m_8 = 2;
                featData.m_c = ScanInt((bool*)0);    // -0x614（8a9a7c4：数值槽）
                script->m_passive268.push_back(featData);
                cont = true;                         // ebx=1 继续
            }
            break;
        }

        L_SKLTAG_TAG_BASIC_EXPLAIN_EX:
        case TAG_BASIC_EXPLAIN_EX:                   // [basic explain ex]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[basic explain ex]", 0x2e1));
                goto L_SKLTAG_TAG_EXPLAIN_EX;
            }
            ScanStr(&script->m_str278);
            break;

        L_SKLTAG_TAG_EXPLAIN_EX:
        case TAG_EXPLAIN_EX:                         // [explain ex]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[explain ex]", 0x2e4));
                goto L_SKLTAG_TAG_EXECUTABLE_STATES;
            }
            ScanStr(&script->m_str274);
            break;

        L_SKLTAG_TAG_EXECUTABLE_STATES:
        case TAG_EXECUTABLE_STATES:                  // [executable states]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[executable states]", 0x2e7));
                goto L_SKLTAG_TAG_NUT_SKILL_SCRIPT;
            }
        {
            // ORIG 8a9a96b..：string 临时（-0x624）每轮 ctor/dtor；cont 走 4.1 callee-saved
            // %ebx（mov $0/$1,%ebx + test%ebx 后测 do-while）。
            bool cont;
            do
            {
                std::string unused;
                value = ScanInt(&okFlag);
                if (!okFlag)
                    cont = false;
                else
                {
                    script->m_vec284.push_back(value);
                    cont = true;
                }
            } while (cont);
            break;
        }

        L_SKLTAG_TAG_NUT_SKILL_SCRIPT:
        case TAG_NUT_SKILL_SCRIPT:                   // [nut skill script]（ORIG 0x2fe：ScanStr(+0x27c)）
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[nut skill script]", 0x2fe));
                goto L_SKLTAG_TAG_COMMAND_CUSTOMIZING;
            }
            ScanStr(&script->m_str27c);
            break;

        L_SKLTAG_TAG_COMMAND_CUSTOMIZING:
        case TAG_COMMAND_CUSTOMIZING:                // [command customizing]
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[command customizing]", 0x319));
                goto L_SKLTAG_TAG_COMBO_SET;
            }
            script->m_280 = (ScanInt((bool*)0) != 0) ? 1 : 0;
            break;

        L_SKLTAG_TAG_COMBO_SET:
        case TAG_COMBO_SET:                          // [combo set]（ORIG 8a9ab36：无 clear，直接 push）
            if (!__inited)
            {
                __lookup.insert(SkillTagMap::value_type("[combo set]", 0x31d));
                break;  // ORIG: 末位 insert 后 jmp default 头 8a9ab34->8a9ab85
            }
            for (;;)
            {
                value = ScanInt(&okFlag);
                if (!okFlag)
                {
                    script->m_290 = 1;
                    break;
                }
                script->m_vec294.push_back(value);
            }
            break;

        default:                                     // 8a9ab85（含 tagId==-1 未知标签）
            // ORIG 8a9ab85 头守卫：首轮 tagId=0 由 [name] 块承接，default 仅在
            // 注册链尾被借道（8a9ab34 jmp 8a9ab85）→ 未注册直接短路主循环尾。
            if (!__inited)
                break;
            // switch 仅在 __inited 后到达（未就绪时上面注册块已置 __inited=true），
            // 故 ORIG 8a9ab85 的 `if(__inited)` 短路恒真，直接执行 module 处理。
            // processModuleTagSet(int&,int&,string*,type,bool)：_Z19processModuleTagSetRiS_PSsib
            // type 实参 = 形参 4（0x14(%ebp)），非 job（汇编 8a9abb0/8a9abb3 实证）
            int moduleA = -1;                        // -0x628
            int moduleB = -1;                        // -0x62c
            moduleRet = processModuleTagSet(moduleA, moduleB, &token, paramB, true);
            if (moduleA != -1)
                script->m_228[moduleA] = 1;          // +0x228+a（8a9abf0）
            if (moduleB != -1)
                script->m_224 = moduleB;             // +0x224（8a9ac0c）
            break;
    }

    // 主循环尾（8a9ac24）：注册链走完（仍 !__inited）→ leave + 置位 + 同 token
    // 重入 loop_head（8a9ac45 jmp 8a979dc）；已注册态 break 落 8a9ac4a → read_token。
    if (!__inited)
    {
        __init_lock.leave();                        // 8a9ac39 SpinLock::leave()
        __inited = true;                            // 8a9ac3e movb $1,__inited
        goto loop_head;                             // 8a9ac45 jmp 8a979dc
    }
    goto read_token;                                // 8a9ac4b jmp 8a979bb：读取下一 token

finalize:                                           // 8a9ac50 尾部修正段（内联，非独立符号）
    // [growtype maximum level] 表逐项夹紧最大等级（8a9ac5a..8a9aca4，i<=5 setle）
    for (finIdx = 0; finIdx <= 5; ++finIdx)
    {
        if (script->m_14 < script->m_arr7c[finIdx])
        {
            printf("SKILL ERROR : 만렙이 배울 수 있는 레벨 미만 [%s]\n", path);
            script->m_14 = script->m_arr7c[finIdx];
        }
    }
    // 购买价格向量补齐（8a9acc8..ae7f）：ORIG vec18/vec24/vec128 的 empty() 在
    // **每轮**条件位重新求值（9acc8/9ad27/9ae2d call empty → xor$1/test/je 出环；
    // 非空再 size()<m_14 物化比较）⇒ 双条件短路 while 形。
    while (!script->m_vec18.empty() &&
           script->m_vec18.size() < static_cast<unsigned>(script->m_14))
        script->m_vec18.push_back(script->m_vec18.back());
    while (!script->m_vec24.empty() &&
           script->m_vec24.size() < static_cast<unsigned>(script->m_14))
        script->m_vec24.push_back(script->m_vec24.back());
    if (script->m_vec204.size() != 0)
        while (script->m_vec204.size() < static_cast<unsigned>(script->m_14))
            script->m_vec204.push_back(script->m_vec204.back());
    if (script->m_vec210.size() != 0)
        while (script->m_vec210.size() < static_cast<unsigned>(script->m_14))
            script->m_vec210.push_back(script->m_vec210.back());
    while (!script->m_vec128.empty() &&
           script->m_vec128.size() < static_cast<unsigned>(script->m_14))
        script->m_vec128.push_back(script->m_vec128.back());

    // job==0xb 特判 -1；否则 iconIdx==-1 时经技能脚本文件表全量扫描定位（8a9ae7f..ae90）
    if (job == 0xb)
    {
        script->m_field0 = -1;
    }
    else
    {
        if (iconIdx == -1)
            iconIdx = reinterpret_cast<STScriptFileList*>(
                          &g_staSkillScriptFileList[job])->FindIndexByFullScan(path);
        script->m_field0 = iconIdx;
    }
    return 1;
}
