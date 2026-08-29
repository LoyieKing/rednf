// ============================================================================
// EventRewardQuickPartyB2.cpp — 独占 TU（B2 批：eventReward / QuickParty /
// ContentsType 叶子符号恢复）。
//
// 覆盖 ORIG 符号（df_game_r nm 实证，全部 T；重建侧此前无任何定义）：
//   _ZN11eventReward13getSubKeyTypeERSs                                      0x080ef0c0
//   _ZN11eventReward13defaultOptionEcR8itemData                               0x080ef25c
//   _ZN12ContentsType9getStringENS_1TE                                        0x08ac3434
//   _ZN12ContentsType7getTypeEPKc                                             0x08ac34a4
//   _ZN10QuickParty23getRandomBuffTypeByNameERKSs                             0x08a6e100
//   _ZN10QuickParty22importQuickPartyScriptEPKcPNS_14QuickPartyDataE          0x08a6e282
//
// 语义来源（每函数已完整阅读报告 ##汇编 + ##反编译 C 后逐指令转写）：
//   docs/class_func_reports/eventReward/{getSubKeyType,getSubKeyAction,
//                                        getOptionType,defaultOption}.md
//   docs/class_func_reports/ContentsType/{getString,getType}.md
//   docs/class_func_reports/QuickParty/{getRandomBuffTypeByName,
//                                        importQuickPartyScript}.md
// 注：PowerParameterScript::clear（W 0x0837187a）按 Main 归属由
//     CharacterAIA2.cpp 提供，本 TU 不涉及。
// ============================================================================

#include <map>
#include <string>

#include "CDataManager.h"             // eventReward / itemData
#include "CQuickPartyRewardManager.h" // namespace QuickParty / QuickPartyData / import 声明
#include "CEnvironment.h"             // less_TCHAR / SpinLock
#include "DNFLexWrapper.h"            // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "StrikerScript.h"            // ContentsType

// ===========================================================================
// 1) eventReward 解析 helper 族（ORIG 非静态成员；this 不参与逻辑）
// ---------------------------------------------------------------------------
// itemData 真实布局见 itemData.h（+0/+1/+2 三字节 + +0x04/+0x08 int，
// sizeof 0x0c）；ORIG 以 movb/movzbl 访问 +0/+1/+2 字节、movl 访问 +4/+8。
// ===========================================================================


// ORIG T 0x080ef0c0：[sub type] 行名 → 子键类型。
char eventReward::getSubKeyType(std::string& token)
{
    if (token == "common")  return 0;
    if (token == "level")   return 1;
    if (token == "fatigue") return 2;
    return 0;
}

// ORIG T 0x080ef128：[/sub key] 行名 → 发放动作（equip=2 / inven=1 / postal=3）。
char eventReward::getSubKeyAction(std::string& token)
{
    if (token == "equip")  return 2;
    if (token == "inven")  return 1;
    if (token == "postal") return 3;
    return 0;
}

// ORIG T 0x080ef190：[option] 区选项名 → itemData 字节域写入。
// 返回 false 当且仅当 "upgrade" 的整数扫描失败；未知名返回 true 且不写。
bool eventReward::getOptionType(std::string& token, itemData& out)
{
    bool ok = true;
    if (token == "unseal")
        out.m_seal = 0;
    else if (token == "seal")
        out.m_seal = 1;
    else if (token == "upgrade")
    {
        // ok 初值 true；ScanInt 自身写失败标志（ORIG 无预置 false）
        const int level = ScanInt(&ok);
        out.m_upgrade = (unsigned char)level;
        if (!ok)
            return false;
    }
    else if (token == "unsealROI")
        out.m_roi = 0;
    else if (token == "sealROI")
        out.m_roi = 1;
    return ok;
}

// ORIG W 0x080ef25c：动作 2（postal）时重置 ROI 域并置 m_count=1；
// 尾部统一保证强化字节非 0（低字节为 0 → 置 1）。
void eventReward::defaultOption(char action, itemData& out)
{
    switch (action)                        // ORIG：movsbl 后与 2 比较
    {
    case 2:
        out.m_count = 1;                     // *(int*)(d+8) = 1
        out.m_seal = 0;   // +0x01 = 0
        out.m_roi = 0;    // +0x02 = 0
        break;
    default:
        break;
    }
    if (out.m_upgrade == 0)   // movzbl/test
        out.m_upgrade = 1;    // +0x00 = 1
}

// ===========================================================================
// 2) ContentsType（ORIG 全局类，静态成员）
// ===========================================================================

// ORIG T 0x08ac3434：T(1..12) → 段名字符串；越界/0 → ""（跳转表 ja 0xc 实证）。
const char* ContentsType::getString(ContentsType::T type)
{
    switch ((int)type)
    {
    case 1:  return "[normal]";
    case 2:  return "[ancient]";
    case 3:  return "[dimension]";
    case 4:  return "[tower]";
    case 5:  return "[infinite]";
    case 6:  return "[ultimate]";
    case 7:  return "[despair]";
    case 8:  return "[tournament]";
    case 9:  return "[pvp]";
    case 10: return "[fair pvp]";
    case 11: return "[warroom]";
    case 12: return "[assault]";
    default: return "";
    }
}

// ORIG T 0x08ac34a4：线性 i=0..12，双构串后 operator==（name 串 vs getString 串），
// 命中返回 i；穷尽未命中返回 0。
int ContentsType::getType(const char* name)
{
    for (int i = 0; i <= 12; ++i)
    {
        std::string expected(getString((ContentsType::T)i));
        std::string current(name);
        if (current == expected)
            return i;
    }
    return 0;
}

// ===========================================================================
// 3) QuickParty 脚本导入族
// ===========================================================================

namespace QuickParty
{

namespace
{

// importQuickPartyScript 段名 id（ORIG __lookup 表值实证）。
enum
{
    QPSEC_QUICK_PARTY_END    = 0x3e,   // [/quick party]
    QPSEC_UNABLE             = 0x41,   // [unable to quickparty]
    QPSEC_LEVEL_SECTION      = 0x4d,   // [level section]
    QPSEC_BASIC_REWARD       = 0x59,   // [basic reward]
    QPSEC_RANDOM_REWARD      = 0x8f,   // [random reward]
    QPSEC_RANDOM_REWARD_PROB = 0xb8,   // [random reward prob]
    QPSEC_REQUIRE_PARTY_NUM  = 0xce    // [require party num]
};

}  // unnamed namespace

// ORIG T 0x08a6e100：脚本 buff 名 → RandomBuffType；未知 → 11（0xb）。
RandomBuffType getRandomBuffTypeByName(const std::string& name)
{
    RandomBuffType type = (RandomBuffType)0xb;
    if      (name == "[increase exp]")       type = RANDOM_BUFF_INCREASE_EXP;
    else if (name == "[increase stats]")     type = RANDOM_BUFF_INCREASE_STATS;
    else if (name == "[increase speed]")     type = RANDOM_BUFF_INCREASE_SPEED;
    else if (name == "[increase dropprob]")  type = RANDOM_BUFF_INCREASE_DROPPROB;
    else if (name == "[increase allattack]") type = RANDOM_BUFF_INCREASE_ALLATTACK;
    else if (name == "[increase allresist]") type = RANDOM_BUFF_INCREASE_ALLRESIST;
    else if (name == "[increase broken]")    type = RANDOM_BUFF_INCREASE_BROKEN;
    else if (name == "[increase mobhp]")     type = RANDOM_BUFF_INCREASE_MOBHP;
    else if (name == "[increase antidevil]") type = RANDOM_BUFF_INCREASE_ANTIDEVIL;
    else if (name == "[increase coinlimit]") type = RANDOM_BUFF_INCREASE_COINLIMIT;
    else if (name == "[return ticket]")      type = RANDOM_BUFF_RETURN_TICKET;
    return type;
}

// ORIG T 0x08a6e282。双遍结构（ORIG 函数级静态 __lookup/__init_lock/__inited，
// SpinLock 双检建表）：首令牌触发 7 个 (段名,id) 登记后回到派发块重新查表，
// 当前令牌被正常处理（与 importTitleBookEtcScript 同构）。主循环仅以
// [/quick party]（返回 true）或主扫描失败（返回 false）退出；节内子扫描失败
// 一律回到主循环继续取令牌；未知令牌（id==-1）跳过。
//
// 各节写入目标（ORIG 偏移 ↔ QuickPartyData 具名字段，CQuickPartyRewardManager.h）：
//   [level section]        → +0x00 m_vec00（vector<int>）
//   [unable to quickparty] → +0x0c m_dungeonIdxList（vector<int>）
//   [increase gold card]   → +0x18 BasicReward.m_levelDatas（LevelData{率%,档}）
//   [hell ratio]           → +0x30/0x34 m_basicReward.m_field18/m_field1c、
//                            +0x24 BasicReward.m_hellDatas（LevelDataHell）
//   [reward]               → +0x38 m_randomRewards map insert（键=buff 类型）
//   [random reward prob]   → +0x50 m_probs[type][i]（i<7；type≤10 才写）
//   [require party num]    → m_minCounts[dungeonType][diff]
//                            （QuickPartyData +0x1a0 区，行 stride 0x10；
//                            ORIG 寻址 (type*4+diff+0x68)*4 完全一致）
bool importQuickPartyScript(const char* path, QuickPartyData* data)
{
    static std::map<const char*, int, less_TCHAR> s_lookup;
    static SpinLock s_initLock;
    static bool s_inited = false;

    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string value;
    bool ok = false;

    while (true)
    {
        if (!ScanType(token, true))
            break;                                    // 主扫描失败 → false

        if (!s_inited)
        {
            s_initLock.enter();
            if (!s_inited)
            {
                typedef std::pair<const char*, int> SecEntry;
                s_lookup.insert(SecEntry("[/quick party]",
                                         (int)QPSEC_QUICK_PARTY_END));
                s_lookup.insert(SecEntry("[unable to quickparty]",
                                         (int)QPSEC_UNABLE));
                s_lookup.insert(SecEntry("[level section]",
                                         (int)QPSEC_LEVEL_SECTION));
                s_lookup.insert(SecEntry("[basic reward]",
                                         (int)QPSEC_BASIC_REWARD));
                s_lookup.insert(SecEntry("[random reward]",
                                         (int)QPSEC_RANDOM_REWARD));
                s_lookup.insert(SecEntry("[random reward prob]",
                                         (int)QPSEC_RANDOM_REWARD_PROB));
                s_lookup.insert(SecEntry("[require party num]",
                                         (int)QPSEC_REQUIRE_PARTY_NUM));
            }
            s_initLock.leave();
        }

        std::map<const char*, int, less_TCHAR>::const_iterator it =
            s_lookup.find(token.c_str());
        const int id = (it != s_lookup.end()) ? it->second : -1;

        switch (id)
        {
        case QPSEC_QUICK_PARTY_END:                   // [/quick party] → 成功
            return true;

        case QPSEC_UNABLE:                            // 单扫描：值即入列元素
            for (;;)
            {
                const int lv = ScanInt(&ok);
                if (!ok)
                    break;
                data->m_dungeonIdxList.push_back(lv);
            }
            break;

        case QPSEC_LEVEL_SECTION:                     // 单扫描：值即入列元素
            for (;;)
            {
                const int lv = ScanInt(&ok);
                if (!ok)
                    break;
                data->m_vec00.push_back(lv);
            }
            break;

        case QPSEC_BASIC_REWARD:
            while (ScanType(token, true))
            {
                if (token == "[/basic reward]")
                    break;

                if (token == "[increase gold card]")
                {
                    for (;;)                          // 门扫描弃值，体读「档」
                    {
                        ScanInt(&ok);
                        if (!ok)
                            break;
                        LevelData entry;
                        entry.m_value4 = ScanInt((bool*)0);
                        ok = ScanStr(&value);
                        entry.m_field0 = (ok && value == "%") ? 1 : 0;
                        data->m_basicReward.m_levelDatas.push_back(entry);
                    }
                }
                else if (token == "[hell ratio]")
                {
                    if (!ScanStr(&value))
                        break;                        // ORIG：退出节回主循环
                    data->m_basicReward.m_field18 = ScanInt((bool*)0);  // +0x30
                    data->m_basicReward.m_field1c = ScanInt((bool*)0);  // +0x34
                    for (;;)                          // 门扫描弃值，体读两 int
                    {
                        ScanInt(&ok);
                        if (!ok)
                            break;
                        LevelDataHell entry;
                        entry.m_field0 = ScanInt((bool*)0);
                        entry.m_value4 = ScanInt((bool*)0);
                        data->m_basicReward.m_hellDatas.push_back(entry);
                    }
                }
                // 其余令牌：忽略并继续节内扫描
            }
            break;

        case QPSEC_RANDOM_REWARD:
            while (ScanType(token, true))
            {
                if (token == "[/random reward]")
                    break;
                if (token != "[reward]")
                    continue;

                if (!ScanStr(&value))
                    break;                            // ORIG：退出节回主循环

                LevelDataRandomReward lrr;            // ctor：+0=0xb、串/向量清空
                lrr.m_field0 = (int)getRandomBuffTypeByName(value);
                ScanStr(&lrr.m_name);                 // 返回值被忽略（ORIG 同）

                for (;;)                              // 门扫描弃值，体读「档」
                {
                    ScanInt(&ok);
                    if (!ok)
                        break;
                    LevelData entry;
                    entry.m_value4 = ScanInt((bool*)0);
                    ok = ScanStr(&value);
                    entry.m_field0 = (ok && value == "%") ? 1 : 0;
                    lrr.m_levelDatas.push_back(entry);
                }

                // pair<RBT,LDRR> 栈拷贝后 insert，插入结果被忽略（ORIG 同）
                data->m_randomRewards.insert(
                    std::make_pair((RandomBuffType)lrr.m_field0, lrr));
            }
            break;

        case QPSEC_RANDOM_REWARD_PROB:
            for (;;)
            {
                if (!ScanStr(&value))
                    break;
                const int idx = (int)getRandomBuffTypeByName(value);
                if (idx > 10)                         // ORIG jle 实证上界
                    continue;
                for (int i = 0; i < 7; ++i)
                    data->m_probs[idx][i] = ScanInt((bool*)0);
            }
            break;

        case QPSEC_REQUIRE_PARTY_NUM:
            while (ScanStr(&value))
            {
                int dungeonType;
                if (value == "[normal]")       dungeonType = 0;
                else if (value == "[ancient]") dungeonType = 1;
                else break;

                if (!ScanStr(&value))
                    break;

                int diff;
                if      (value == "[easy]")     diff = 0;
                else if (value == "[medium]")   diff = 1;
                else if (value == "[hard]")     diff = 2;
                else if (value == "[ultimate]") diff = 3;
                else break;

                int requireNum = 0;
                if (!ScanInt(&requireNum))
                    break;

                // ORIG 双边界检查（js/jg 实证）：type∈{0,1}、diff∈{0..4}
                if (dungeonType >= 0 && dungeonType <= 1 && diff >= 0 && diff <= 4)
                {
                    data->m_minCounts[dungeonType][diff] = requireNum;
                }
            }
            break;

        default:                                      // 未知令牌（id==-1）：跳过
            break;
        }
    }

    return ok;
}

}  // namespace QuickParty
