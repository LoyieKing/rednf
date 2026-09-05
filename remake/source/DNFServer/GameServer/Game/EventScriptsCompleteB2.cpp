// EventScriptsCompleteB2.cpp —— 事件脚本/升级支持域独占恢复 TU（EventScriptsCompleteB2 批次）。
//
// 覆盖符号（全部经 ORIG nm 实证存在，地址随各项标注）：
//   A. 10 个事件脚本真实 ctor（W）+ ExchangeRandomItemRewardScript::reset（W）
//   B. LevelupSupportSciprt / LevelupSupport2ndScript 的 C1/D1（T）
//   C. importActionMailScript（T 0x0897f147）
//   D. 10 个 import*EventScript（T）
//
// 证据等级：
//   - LevelupSupportSciprt/LevelupSupport2ndScript C1/D1：docs/class_func_reports/
//     LevelupSupportSciprt/ 与 LevelupSupport2ndScript/ 下对应 .md 报告已完整阅读
//     （## 汇编 + ## 反编译 C），按报告转写。
//   - 其余函数无独立报告 → 按 source/toolchain 规则用 ORIG objdump 反汇编逐指令
//     转写；编辑前已完整阅读同类/调用者报告：
//     InGameEventScriptLoader/importInGameEventScript.md（调用者，全文）、
//     EventClassify__C*/README.md 全部、EventClassify__CObjectBringUp/
//     verifyGiveBringUpReward.md（ObjectBringUpRule 字段语义）、以及同域已交付
//     EventLevelupA2.cpp（基类/邮件动作/装载器定义）。各函数头部均已标记。
//
// 解析器形态说明：ORIG 各 import 函数内的「static map<tag,id> + SpinLock +
// __cxa_guard」是 GCC 对函数级静态查表的一次性构建代码；构建完成前首个 token
// 走建表-重派发路径，可观测行为与直接 token 比较完全一致（表内容为纯常量，
// 无副作用）。本仓既有惯例（InitScriptProducers_1/6.cpp、StrikerWorldDropB2.cpp）
// 均将其转写为直接 token 比较，本文件沿用。

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "DNFLexWrapper.h"             // loadRDARScriptFile/ScanType/ScanInt/ScanStr
#include "CEnvironment.h"              // less_TCHAR / SpinLock（__lookup 双相派发依赖）
#include "InGameEventScripts.h"        // InGameEventScript 域权威声明（本批次补字段）
#include "LevelupSupportEventManger.h" // LevelupSupportSciprt / LevelupSupport2ndScript

// CDataManager.cpp 提供（ORIG T 0x089bbc0f，声明亦见 InitScriptProducers_1.cpp）。
int getCharacterJobToEnumValue(std::string& job);

// ============================================================================
// A. 事件脚本 ctor（ctor 内显式语句顺序严格对照各自反汇编）
// ============================================================================

// ORIG W 0x089861da：基类 → string+0xc → map+0x10 → mail+0x28。
// +0x08 字节故意不初始化（is_recv_check_item 读此字节；[recv check db table]
// 未出现时保持未定值，忠实保留）。
LevelUpRewardScript::LevelUpRewardScript() {}

// ORIG W 0x08985eec：string ctor → 五个标量写入 → Action_SendMail ctor →
// str.clear → mail.reset。标量经初始化列表写在 string 与 mail 的隐式构造之间，
// 复现 ORIG 交错顺序。
UsedFatigueGiveItemScript::UsedFatigueGiveItemScript()
    : m_userType(0),
      m_usedFatigue(0),
      m_charcEventFlag(0),
      m_jobType(-1),
      m_fatigueLimit(-1)
{
    m_dbTableName.clear();
    m_mail.reset();
}

// ORIG W 0x0898597a：全成员构造 → str(+8).clear → vec(+0x10).clear →
// mail.reset → int+0xc=0 → int+0x24=0。
AttendanceScript::AttendanceScript()
{
    m_dbTableName.clear();
    m_attends.clear();
    m_mail.reset();
    m_field0c = 0;
    m_field24 = 0;
}

// ORIG W 0x08985fb8。
PcBangPlayTimeScript::PcBangPlayTimeScript()
{
    m_dbTableName.clear();
    m_playTime     = 0;
    m_numberOfTime = 1;
    m_startTag     = 0x20;
    m_endTag       = 0x20;
}

// ORIG W 0x089858da：基类 → string+0x8 → mail+0xc → str.clear → mail.reset。
AccountFirstLoginScript::AccountFirstLoginScript()
{
    m_dbTableName.clear();
    m_mail.reset();
}

// ORIG W 0x08985ae0：字节+8=0 → vec<pair>+0xc clear → vec<int>+0x18 clear →
// mail+0x24 reset。
void ExchangeRandomItemRewardScript::reset()
{
    m_flag08 = 0;
    m_pairs0c.clear();
    m_ints18.clear();
    m_mail.reset();
}

// ORIG W 0x08985b1a：基类 → vec<pair>+0xc → vec<int>+0x18 → mail+0x24 → reset()。
ExchangeRandomItemRewardScript::ExchangeRandomItemRewardScript()
{
    reset();
}

// ORIG W 0x08985be2：基类 → map<char,uint>+0x8 → mail+0x20 → mail.reset。
AvatarDisjointRandomRewardScript::AvatarDisjointRandomRewardScript()
{
    m_mail.reset();
}

// ORIG W 0x08985c74：基类 → string+0x8 → 字节+c=0 → str.clear。
FatigueQuantityScript::FatigueQuantityScript()
{
    m_userType = 0;
    m_dbTableName.clear();
}

// ORIG W 0x08985e0a：基类 → string+0x8 → vec<int>+0x10 →
// vector<pair<vector<uint>,mail>>+0x1c → vector<pair<Rule,mail>>+0x28 →
// str.clear → 字节+c=0。
ObjectBringUpScript::ObjectBringUpScript()
{
    m_dbTableName.clear();
    m_exchangeRuleFlag = 0;
}

// ORIG W 0x08986134：基类 → vec<int>+0x8 → vec<int>+0x14 → string+0x20。
AddItemCountDBScript::AddItemCountDBScript() {}

// ObjectBringUpRule：ORIG W 0x08985dd4（调 reset）/ T 0x08985de8（三 int = -1）。
ObjectBringUpScript::ObjectBringUpRule::ObjectBringUpRule() { reset(); }
void ObjectBringUpScript::ObjectBringUpRule::reset()
{
    m_rewardStartCount    = -1;
    m_rewardEndCount      = -1;
    m_rewardMutipleNumber = -1;
}
// PurchaseBonusRule：ORIG W 0x08985ce6（调 reset）/ T 0x08985cfa（三 int = -1）。
PurchaseCashItemBonusScript::PurchaseBonusRule::PurchaseBonusRule() { reset(); }
void PurchaseCashItemBonusScript::PurchaseBonusRule::reset()
{
    m_purchaseStartCount    = -1;
    m_purchaseEndCount      = -1;
    m_purchaseMutipleNumber = -1;
}

// ORIG W 0x08985d1c：基类 → string+0x8 → set+0xc → vector<pair<Rule,mail>>+0x24
// → string+0x8 clear（无 mail.reset，忠实保留）。
PurchaseCashItemBonusScript::PurchaseCashItemBonusScript()
{
    m_dbTableName.clear();
}

// ============================================================================
// B. LevelupSupportSciprt / LevelupSupport2ndScript C1/D1（报告转写）
// ============================================================================

// 报告 LevelupSupportSciprt.md @0x08ab5124：仅构造 +0x00 的
// map<int, vector<ItemInfo>*>。
LevelupSupportSciprt::LevelupSupportSciprt() {}

// 报告 ~LevelupSupportSciprt.md @0x08ab5138：仅析构该 map。
LevelupSupportSciprt::~LevelupSupportSciprt() {}

// 报告 LevelupSupport2ndScript.md @0x08ab54be：构造 +0x04 map、+0x1c/+0x20
// 两 string；+0x00 m_jobType 故意不初始化（忠实保留）。
LevelupSupport2ndScript::LevelupSupport2ndScript() {}

// 报告 ~LevelupSupport2ndScript.md @0x08ab5530：逆序析构 +0x20/+0x1c string、
// +0x04 map（与声明序相反的隐式顺序一致）。
LevelupSupport2ndScript::~LevelupSupport2ndScript() {}

// ============================================================================
// C. importActionMailScript —— T 0x0897f147（1656 字节）
//    [objdump·报告缺失] 转写。标签：[inven]/[job](8 职业)/[detail]/[/send mail]。
// ============================================================================
bool importActionMailScript(Action_SendMail* mail)
{
    bool result = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[inven]")
        {
            // mov %dl,0x1c：直发标记字节。
            mail->m_field1c = static_cast<unsigned char>(ScanInt((bool*)0));
        }
        else if (token == "[job]")
        {
            // 节内循环：每职业两整数(itemIdx,count)插入 m_jobMap[职业序号]；
            // [/job] 返回主循环；未知 token 跳过；ScanType 失败回主循环。
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                int jobKey;
                if (token == "[swordman]")   jobKey = 0;
                else if (token == "[fighter]")   jobKey = 1;
                else if (token == "[gunner]")    jobKey = 2;
                else if (token == "[mage]")      jobKey = 3;
                else if (token == "[priest]")    jobKey = 4;
                else if (token == "[atgunner]")  jobKey = 5;
                else if (token == "[thief]")     jobKey = 6;
                else if (token == "[atfighter]") jobKey = 7;
                else if (token == "[/job]")
                    break;
                else
                    continue;
                const int itemIdx = ScanInt((bool*)0);
                const int count   = ScanInt((bool*)0);
                std::pair<int, int> value(itemIdx, count);
                mail->m_jobMap.insert(
                    std::make_pair(jobKey, value));
            }
        }
        else if (token == "[detail]")
        {
            mail->m_field18 = 0;                                   // dword+0x18 = 0
            mail->m_itemIdx = ScanInt((bool*)0);                   // +0x20
            mail->m_addInfo = ScanInt((bool*)0);                   // +0x24
            mail->m_gold    = ScanInt((bool*)0);                   // +0x28
            mail->m_coin    = static_cast<unsigned char>(ScanInt((bool*)0));  // byte+0x2c
            // js/cmp $0xf：bit7 置位或 >0xf 时清零。
            if ((mail->m_coin & 0x80) != 0 || mail->m_coin > 0x0f)
                mail->m_coin = 0;
            ScanStr(&mail->m_field30);                             // 标题 key
            ScanStr(&mail->m_field34);                             // 正文 key
        }
        else if (token == "[/send mail]")
        {
            result = true;
            break;
        }
    }
    return result;
}

// ============================================================================
// D. import*EventScript ×10
// ============================================================================

// ---- importFatigueQuantityEventScript T 0x08982fc6（1248B）----
// [objdump·报告缺失]。标签：[/event]=返回真、[title]→+0、[explain]→+4、
// [db table]→+8、[user type]→byte+0xc。ORIG 成功结束路径的 ok 标志恒 0
// （仅 init 后从未置 1），故 token 耗尽必返回 false —— 忠实保留。
bool importFatigueQuantityEventScript(FatigueQuantityScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool result = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return true;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[db table]")
            ScanStr(&script->m_dbTableName);
        else if (token == "[user type]")
            script->m_userType = static_cast<unsigned char>(ScanInt((bool*)0));
    }
    return result;
}

// ---- importAccountFirstLoginEventScript T 0x0897f7bf（1242B）----
// [objdump·报告缺失]。标签：[/event]、[title]、[explain]、[db table]、[send mail]。
bool importAccountFirstLoginEventScript(AccountFirstLoginScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool result = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return true;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[db table]")
            ScanStr(&script->m_dbTableName);
        else if (token == "[send mail]")
            importActionMailScript(&script->m_mail);
    }
    return result;
}

// ---- importAddItemCountDBEventScript T 0x0897fc99（1480B）----
// [objdump·报告缺失]。[item index]/[quest index] 用 ScanInt(&ok) 且仅在成功时
// push_back；ok 即最终返回值（init 0）。
bool importAddItemCountDBEventScript(AddItemCountDBScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool ok = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return true;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[db table]")
            ScanStr(&script->m_dbTableName);
        else if (token == "[item index]")
        {
            const int v = ScanInt(&ok);
            if (ok)
                script->m_itemIdxs.push_back(v);
        }
        else if (token == "[quest index]")
        {
            const int v = ScanInt(&ok);
            if (ok)
                script->m_questIds.push_back(v);
        }
    }
    return ok;
}

// ---- importPcBangPlayTimeScript T 0x0898512c（1661B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importPcBangPlayTimeScript.md
// （双段完整阅读）。同 importAttendanceScript 双相派发形：8 个 (键,id) 注册链
// +0x2d3→+0x33a→+0x3a7→+0x417→+0x47b→+0x4e2→+0x549→+0x5ae，公共尾 +0x611。
// id：[/event]0x5d2 [title]0x5d7 [explain]0x5da [db table]0x5dd [play time]
// 0x5e0 [number of time]0x5e3 [start tag]0x5e6 [end tag]0x5e9。[play time]
// +0xc(int)/[number of time]+0x10(byte mov %dl)/[start tag]+0x12(word mov
// %dx)/[end tag]+0x14(word) 均 ScanInt(NULL) 直存；-0x1d bool 槽初值 0 仅作
// ScanType 失败返回（+0x63e movzbl -0x1d），此外从不写。
bool importPcBangPlayTimeScript(PcBangPlayTimeScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;          // ORIG -0x1d：仅 ScanType 失败路径读取返回

    for (;;)
    {
        if (!ScanType(token, true))
        {
        scan_exit:
            goto scan_done;
        }

    redo_lookup:
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int key;
        if (__inited)
        {
            LookupMap::const_iterator it = __lookup.find(token.c_str());
            key = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                        // +0x226
            if (__inited)
            {
                __init_lock.leave();                    // +0x23d
                goto redo_lookup;                       // +0x243 → guard 重查
            }
            key = 0;                                    // +0x248 注册链入口
        }

        switch (key)
        {
            case 0x5d2:                                  // [/event]（+0x2d3）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x5d2));
                }
                else
                {
                    return true;                        // +0x330 mov $1,%ebx
                }
            }
            /* fall through */
            case 0x5d7:                                  // [title]（+0x33a）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x5d7));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0x5da:                                  // [explain]（+0x3a7）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x5da));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0x5dd:                                  // [db table]（+0x417）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x5dd));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);
                    break;
                }
            }
            /* fall through */
            case 0x5e0:                                  // [play time]（+0x47b）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[play time]", 0x5e0));
                }
                else
                {
                    script->m_playTime = ScanInt((bool*)0);
                    break;
                }
            }
            /* fall through */
            case 0x5e3:                                  // [number of time]（+0x4e2）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[number of time]", 0x5e3));
                }
                else
                {
                    script->m_numberOfTime =
                        static_cast<unsigned char>(ScanInt((bool*)0));
                    break;
                }
            }
            /* fall through */
            case 0x5e6:                                  // [start tag]（+0x549）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[start tag]", 0x5e6));
                }
                else
                {
                    script->m_startTag =
                        static_cast<unsigned short>(ScanInt((bool*)0));
                    break;
                }
            }
            /* fall through */
            case 0x5e9:                                  // [end tag]（+0x5ae）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[end tag]", 0x5e9));
                }
                else
                {
                    script->m_endTag =
                        static_cast<unsigned short>(ScanInt((bool*)0));
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                    // +0x626
                __inited = true;                        // +0x62b
                goto redo_lookup;                       // +0x632 → guard 重派发
            }
            goto scan_exit;                             // +0x638 → ScanType 回边
    }
scan_done:
    return result;
}

// ---- importExchangeRandomItemRewardScript T 0x08981e0f（1701B）----
// [objdump·报告缺失]。[material] 节：先 ScanType 一次（失败直接返回假）；若该
// token 为 [exchange rule] 则读规则字节（失败回主循环），随后成对读 uint 直到
// ScanInt 失败；非规则 token 直接进入同一整数对循环。[random reward item]
// 单值列；ok 共享变量即最终返回值。
bool importExchangeRandomItemRewardScript(ExchangeRandomItemRewardScript* script,
                                          char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool ok = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return true;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[material]")
        {
            if (!ScanType(token, true))
                return false;                     // ORIG 直接返回假
            if (token == "[exchange rule]")
            {
                script->m_flag08 =
                    static_cast<unsigned char>(ScanInt(&ok));
                if (ok)
                {
                    for (;;)
                    {
                        unsigned int first  = 0;
                        unsigned int second = 0;
                        first = static_cast<unsigned int>(ScanInt(&ok));
                        if (!ok)
                            break;
                        second = static_cast<unsigned int>(ScanInt(&ok));
                        if (!ok)
                            break;
                        script->m_pairs0c.push_back(
                            std::pair<int, int>(
                                static_cast<int>(first),
                                static_cast<int>(second)));
                    }
                }
            }
            else
            {
                // 非规则标签：直接进入整数对循环（首个 ScanInt 失败即回主循环）
                for (;;)
                {
                    unsigned int first  = 0;
                    unsigned int second = 0;
                    first = static_cast<unsigned int>(ScanInt(&ok));
                    if (!ok)
                        break;
                    second = static_cast<unsigned int>(ScanInt(&ok));
                    if (!ok)
                        break;
                    script->m_pairs0c.push_back(
                        std::pair<int, int>(static_cast<int>(first),
                                            static_cast<int>(second)));
                }
            }
        }
        else if (token == "[random reward item]")
        {
            for (;;)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_ints18.push_back(v);
            }
        }
        else if (token == "[send mail]")
            importActionMailScript(&script->m_mail);
    }
    return ok;
}

// ---- importAvatarDisjointRandomRewardScript T 0x089824b4（1429B）----
// [objdump·报告缺失]。[random reward item]：(char)ScanInt、(uint)ScanInt 成对，
// 插入 map<char,uint>+0x8；任一失败回主循环。
bool importAvatarDisjointRandomRewardScript(AvatarDisjointRandomRewardScript* script,
                                            char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool ok = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return true;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[random reward item]")
        {
            for (;;)
            {
                char         key = 0;
                unsigned int cnt = 0;
                key = static_cast<char>(ScanInt(&ok));
                if (!ok)
                    break;
                cnt = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok)
                    break;
                script->m_map08.insert(
                    std::pair<char, unsigned int>(key, cnt));
            }
        }
        else if (token == "[send mail]")
            importActionMailScript(&script->m_mail);
    }
    return ok;
}

// ---- importAttendanceScript T 0x08981539（2262B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAttendanceScript.md（汇编+
// Ghidra C 双段完整阅读）。ORIG 为「函数级静态 __lookup(map<const char*,int,
// less_TCHAR>)/__init_lock(SpinLock)/__inited(bool) 三件套 + case 贯穿建表链」
// 双相派发（同 importTitleBookEtcScript 形）：首令牌以 key=0 进入注册链，自
// case 0x26f（与 case 0 合并，+0x306）起逐 case insert 全部 12 个 (键,id)
// （+0x306→+0x36d→+0x3da→+0x44a→+0x4ba→+0x52e→+0x59e→+0x60e→+0x6d1→+0x736→
// +0x7ad→+0x80b），落公共尾（+0x867）leave+置位后回 guard 重派发同一令牌。
// id：[/event]0x26f [title]0x271 [explain]0x274 [db table]0x277 [fatigue]0x27a
// [reward explain]0x299 [final reward explain]0x29c [reward]0x29f
// [final day condition]0x2ad [final reward]0x2b0 [send mail]0x2c7
// [msg overbtn]0x2cb。[reward] 节为 pair 默认构造+ScanInt(&ok) 成对循环
// （+0x65c..+0x6ca，ok 即 -0x121 返回值共享槽）；其余数值节 ScanInt(NULL)
// 直存；ScanType 失败返回 ok（+0x893 movzbl -0x121）。
bool importAttendanceScript(AttendanceScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
        {
        scan_exit:
            goto scan_done;
        }

    redo_lookup:
        // 三静态位于外层 token 循环体首；ORIG 重派发 jmp +0x66 = guard 检查点，
        // goto 边每轮穿过 guard 测试。__inited 零初始化无 guard。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int key;
        if (__inited)
        {
            LookupMap::const_iterator it = __lookup.find(token.c_str());
            key = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                        // +0x229
            if (__inited)
            {
                __init_lock.leave();                    // +0x240
                goto redo_lookup;                       // +0x246 → guard 重查
            }
            key = 0;                                    // +0x24b 注册链入口
        }

        switch (key)
        {
            case 0x26f:                                  // [/event]（+0x306）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x26f));
                }
                else
                {
                    return true;                        // +0x363 mov $1,%ebx
                }
            }
            /* fall through */
            case 0x271:                                  // [title]（+0x36d）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x271));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0x274:                                  // [explain]（+0x3da）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x274));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0x277:                                  // [db table]（+0x44a）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x277));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);
                    break;
                }
            }
            /* fall through */
            case 0x27a:                                  // [fatigue]（+0x4ba）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[fatigue]", 0x27a));
                }
                else
                {
                    script->m_field0c = ScanInt((bool*)0);
                    break;
                }
            }
            /* fall through */
            case 0x299:                                  // [reward explain]（+0x52e）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[reward explain]", 0x299));
                }
                else
                {
                    ScanStr(&script->m_str70);
                    break;
                }
            }
            /* fall through */
            case 0x29c:                                  // [final reward explain]（+0x59e）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[final reward explain]", 0x29c));
                }
                else
                {
                    ScanStr(&script->m_str74);
                    break;
                }
            }
            /* fall through */
            case 0x29f:                                  // [reward]（+0x60e）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[reward]", 0x29f));
                }
                else
                {
                    for (;;)
                    {
                        std::pair<int, int> attend;     // +0x65c 每轮默认构造
                        attend.first = ScanInt(&ok);    // +0x673
                        if (!ok)
                            break;                      // +0x68a → 公共尾
                        attend.second = ScanInt(&ok);   // +0x697
                        if (!ok)
                            break;
                        script->m_attends.push_back(attend);   // +0x6c5
                    }
                    break;
                }
            }
            /* fall through */
            case 0x2ad:                                  // [final day condition]（+0x6d1）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[final day condition]", 0x2ad));
                }
                else
                {
                    script->m_field24 = ScanInt((bool*)0);
                    break;
                }
            }
            /* fall through */
            case 0x2b0:                                  // [final reward]（+0x736）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[final reward]", 0x2b0));
                }
                else
                {
                    script->m_pair1c.first  = ScanInt((bool*)0);   // +0x1c
                    script->m_pair1c.second = ScanInt((bool*)0);   // +0x20
                    break;
                }
            }
            /* fall through */
            case 0x2c7:                                  // [send mail]（+0x7ad）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[send mail]", 0x2c7));
                }
                else
                {
                    importActionMailScript(&script->m_mail);
                    break;
                }
            }
            /* fall through */
            case 0x2cb:                                  // [msg overbtn]（+0x80b）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[msg overbtn]", 0x2cb));
                }
                else
                {
                    ScanStr(&script->m_str28);
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                    // +0x87c
                __inited = true;                        // +0x881
                goto redo_lookup;                       // +0x888 → guard 重派发
            }
            goto scan_exit;                             // +0x88e → ScanType（岛回跳）
    }
scan_done:
    return ok;
}

// ---- importUsedFatigueGiveItemEventScript T 0x0898491b（2065B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importUsedFatigueGiveItemEventScript.md
// （双段完整阅读）。同双相派发形：10 个 (键,id) 注册链 +0x2ec→+0x355→+0x3c2→
// +0x432→+0x4a2→+0x531→+0x5af→+0x609→+0x6b5→+0x72f，公共尾 +0x7a2。
// id：[/event]0x58f [title]0x592 [explain]0x595 [db table]0x598 [user type]
// 0x59c [used fatigue]0x5a1 [charc event]0x5a7 [apply job type]0x5aa
// [used fatigue limit]0x5af [send mail]0x5b5。result(-0xf9) 初值 true；
// [/event] 返回 result 本体（+0x349 movzbl -0xf9，非恒真）；[user type]
// ScanInt(&result) 字节存 +0xc、[used fatigue] int 存 +0x10、[used fatigue
// limit] int 存 +0x1c，三者失败即 mov $0,%ebx 返回假（+0x527/+0x5a5/+0x725）；
// [charc event] 置字节 +0x14=1；[apply job type] 局部 string → ScanStr →
// getCharacterJobToEnumValue 存 +0x18；[send mail] 失败仅把 result 置假并继续。
bool importUsedFatigueGiveItemEventScript(UsedFatigueGiveItemScript* script,
                                          char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = true;

    for (;;)
    {
        if (!ScanType(token, true))
        {
        scan_exit:
            goto scan_done;
        }

    redo_lookup:
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int key;
        if (__inited)
        {
            LookupMap::const_iterator it = __lookup.find(token.c_str());
            key = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                        // +0x229
            if (__inited)
            {
                __init_lock.leave();                    // +0x240
                goto redo_lookup;                       // +0x246 → guard 重查
            }
            key = 0;                                    // +0x24b 注册链入口
        }

        switch (key)
        {
            case 0x58f:                                  // [/event]（+0x2ec）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x58f));
                }
                else
                {
                    return result;                      // +0x349 movzbl -0xf9
                }
            }
            /* fall through */
            case 0x592:                                  // [title]（+0x355）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x592));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0x595:                                  // [explain]（+0x3c2）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x595));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0x598:                                  // [db table]（+0x432）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x598));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);
                    break;
                }
            }
            /* fall through */
            case 0x59c:                                  // [user type]（+0x4a2）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[user type]", 0x59c));
                }
                else
                {
                    script->m_userType =
                        static_cast<unsigned char>(ScanInt(&result));
                    if (!result)
                        return false;                   // +0x527
                    break;
                }
            }
            /* fall through */
            case 0x5a1:                                  // [used fatigue]（+0x531）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[used fatigue]", 0x5a1));
                }
                else
                {
                    script->m_usedFatigue = ScanInt(&result);
                    if (!result)
                        return false;                   // +0x5a5
                    break;
                }
            }
            /* fall through */
            case 0x5a7:                                  // [charc event]（+0x5af）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[charc event]", 0x5a7));
                }
                else
                {
                    script->m_charcEventFlag = 1;       // +0x600 movb $1
                    break;
                }
            }
            /* fall through */
            case 0x5aa:                                  // [apply job type]（+0x609）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[apply job type]", 0x5aa));
                }
                else
                {
                    std::string jobName;                // +0x660 局部 ctor
                    ScanStr(&jobName);
                    script->m_jobType =
                        getCharacterJobToEnumValue(jobName);
                    break;                              // +0x695 dtor → 公共尾
                }
            }
            /* fall through */
            case 0x5af:                                  // [used fatigue limit]（+0x6b5）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[used fatigue limit]", 0x5af));
                }
                else
                {
                    script->m_fatigueLimit = ScanInt(&result);
                    if (!result)
                        return false;                   // +0x725
                    break;
                }
            }
            /* fall through */
            case 0x5b5:                                  // [send mail]（+0x72f）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[send mail]", 0x5b5));
                }
                else
                {
                    // 失败仅置假 result 并继续主循环（+0x792 movb $0,-0xf9）。
                    if (!importActionMailScript(&script->m_mail))
                        result = false;
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                    // +0x7b7
                __inited = true;                        // +0x7bc
                goto redo_lookup;                       // +0x7c3 → guard 重派发
            }
            goto scan_exit;                             // +0x7c9 → ScanType 回边
    }
scan_done:
    return result;
}

// ---- importObjectBringUpEventScript T 0x08983e48（2771B）----
// [objdump·报告缺失]。四数据节：
//   [material item]：先 [exchange rule] 字节（ScanInt 失败→回主循环），随后整数
//                    列入 m_ints10 直到 ScanInt 失败；ScanType 失败→返回假。
//   [eventitem send]：循环收 [get eventitem fatigue] 整数到局部 vector<uint>；
//                    [send mail] 构造局部 mail 并 make_pair 推入 +0x1c（解析
//                    失败→退出本节回主循环）；[/eventitem send] 或 ScanType
//                    失败退出本节；未知 token 忽略继续。
//   [send]：进入后一直吞 token 到 EOF；[reward start/end count]、
//          [reward mutiple number] 填 Rule；[send mail] 在三字段均非 -1 时推入
//          +0x28，随后 rule.reset() 继续节内循环（解析失败同样留在主流程）。
bool importObjectBringUpEventScript(ObjectBringUpScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool result = false;   // 兼作节内 ScanInt 的 ok（ORIG 同一栈槽）
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return true;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[db table]")
            ScanStr(&script->m_dbTableName);
        else if (token == "[material item]")
        {
            if (!ScanType(token, true))
                return false;
            if (token == "[exchange rule]")
            {
                script->m_exchangeRuleFlag =
                    static_cast<unsigned char>(ScanInt(&result));
                if (result)
                {
                    for (;;)
                    {
                        int idx = ScanInt(&result);
                        if (!result)
                            break;                    // 回主循环
                        script->m_materialItemIdxs.push_back(idx);
                    }
                }
            }
            else
            {
                // 非规则标签：直接尝试整数收集（首个失败即回主循环）
                for (;;)
                {
                    int idx = ScanInt(&result);
                    if (!result)
                        break;
                    script->m_materialItemIdxs.push_back(idx);
                }
            }
        }
        else if (token == "[eventitem send]")
        {
            std::vector<unsigned int> fatigues;
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                            // 退节回主循环
                if (token == "[get eventitem fatigue]")
                {
                    const unsigned int v =
                        static_cast<unsigned int>(ScanInt(&result));
                    if (!result)
                        continue;
                    fatigues.push_back(v);
                }
                else if (token == "[send mail]")
                {
                    Action_SendMail mail;
                    mail.reset();
                    if (!importActionMailScript(&mail))
                        break;                        // 失败：弃对、退节回主循环
                    script->m_ruleMails1c.push_back(
                        std::pair<std::vector<unsigned int>, Action_SendMail>(
                            fatigues, mail));
                }
                else if (token == "[/eventitem send]")
                    break;                            // 退节回主循环
            }
        }
        else if (token == "[send]")
        {
            ObjectBringUpScript::ObjectBringUpRule rule;
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                            // EOF → 主循环（再失败即退出）
                if (token == "[reward start count]")
                {
                    rule.m_rewardStartCount = ScanInt(&result);
                    if (!result)
                        break;                        // 回主循环
                }
                else if (token == "[reward end count]")
                {
                    rule.m_rewardEndCount = ScanInt(&result);
                    if (!result)
                        break;
                }
                else if (token == "[reward mutiple number]")
                {
                    rule.m_rewardMutipleNumber = ScanInt(&result);
                    if (!result)
                        break;
                }
                else if (token == "[send mail]")
                {
                    Action_SendMail mail;
                    mail.reset();
                    const bool mailOk = importActionMailScript(&mail);
                    if (mailOk && rule.m_rewardStartCount != -1 &&
                        rule.m_rewardEndCount != -1 &&
                        rule.m_rewardMutipleNumber != -1)
                    {
                        script->m_sendMails28.push_back(
                            std::pair<ObjectBringUpScript::ObjectBringUpRule,
                                      Action_SendMail>(rule, mail));
                    }
                    rule.reset();                     // 无论成败均复位续扫
                }
            }
        }
    }
    return result;
}

// ---- importLevelUpRewardScript T 0x08980261（4824B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importLevelUpRewardScript.md
// （ORIG 汇编 + Ghidra 反编译 C 两段已完整阅读）。标签主循环 4 个：
// [recv check db table]/[level up]/[send mail]/[level up reward end]。
// ORIG 反汇编（报告 /tmp/LevelUpReward.md，已完整读 ## 汇编 + ## 反编译 C）：
// [level up] 节内是 11 个职业标签的线性 operator== 比较链，GCC 对每个分支
// 复制一整块解析代码（ScanType 子标签 + ScanInt×4 + ScanStr×2 + push_back +
// pair(char&,cond&) 构造 + map::insert + pair 析构），故 ORIG 函数体达 4824B、
// 调用 164 个。本文件按同构形态逐分支展开转写。
// 控制流要点（全部经汇编实证）：
//   - 节顶 ScanType 失败（EOF）→ 直接回主循环头（不建局部对象）；
//   - 职业分支内子标签 ScanType 失败 → ebx=0 → 销毁局部、退出本节回主循环
//     （不是返回假！此前重建误写 return false，已修正）；
//   - [/level up] → ebx=0 退出本节；未知 token → ebx=1 继续节内；
//   - 职业命中后子 token 不比较、直接丢弃；cond.m_filter[0]=立即数职业号，
//     filter[1]=(char)ScanInt(&ok)；reward 循环三 ScanInt 短路检查（任一失败
//     即跳出到 insert），随后两 ScanStr + push_back；
//   - insert 键经 pair<char&,Condition_LevelUp&> 模板推导（ORIG 符号
//     _ZNSt4pairIKi17Condition_LevelUpEC2IRcRS1_EEOT_OT0_），故键实参必须是
//     char 左值（本地 char 变量持有 filter[0] 值）；
//   - ok(=result) 为跨全部分支共享的局部 bool，ScanInt 全写它，且
//     [level up reward end] 置真后作为返回值。
bool importLevelUpRewardScript(LevelUpRewardScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    std::string token;
    bool result = false;
    script->m_dbTableName = "";
    script->m_flag08 = 0;

    // for(;;)+头侧 break 形态：令首 ScanType 调用停在循环体前（ORIG 主循环
    // test 位于 +0x67 循环头；while(cond) 形式会被 GCC 4.4 旋转至环尾）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[recv check db table]")
        {
            ScanStr(&script->m_dbTableName);
            script->m_flag08 = 1;
        }
        else if (token == "[level up]")
        {
            bool next = true;
            while (next)
            {
                if (!ScanType(token, true))
                    break;                    // ORIG 节顶 EOF：回主循环头

                Condition_LevelUp cond;
                Condition_LevelUp::Reward_Item item;

                if (token == "[swordman]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 0;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;         // ORIG ebx=0：退出本节回主循环
                }
                else if (token == "[fighter]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 1;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[gunner]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 2;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[mage]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 3;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[priest]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 4;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[at gunner]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 5;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[thief]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 6;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[at fighter]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 7;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[at mage]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 8;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[demonic swordman]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 9;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[creator mage]")
                {
                    if (ScanType(token, true))
                    {
                        cond.m_filter[0] = 0xa;
                        cond.m_filter[1] = static_cast<unsigned char>(ScanInt(&result));
                        for (;;)
                        {
                            item.m_level = ScanInt(&result);
                            if (!result) break;
                            item.m_itemIndex = ScanInt(&result);
                            if (!result) break;
                            item.m_itemCount = ScanInt(&result);
                            if (!result) break;
                            ScanStr(&item.m_title);
                            ScanStr(&item.m_content);
                            cond.m_items.push_back(item);
                        }
                        char key = static_cast<char>(cond.m_filter[0]);
                        std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                        script->m_conditions.insert(entry);
                    }
                    else
                        next = false;
                }
                else if (token == "[/level up]")
                {
                    next = false;
                }
                // 其余未知 token：next 保持真 → 销毁局部并继续本节（ORIG 同）
            }
        }
        else if (token == "[send mail]")
        {
            importActionMailScript(&script->m_mail);
        }
        else if (token == "[level up reward end]")
        {
            result = true;
            break;
        }
    }
    return result;
}
// ---- importPurchaseCashItemBonusEventScript T 0x089834a6（2404B）----
// [objdump·报告缺失]。主循环 8 标签：[/event]=返回真、[title]/[explain]/
// [db table] 三 ScanStr、[item idx] 循环 ScanInt(&ok) 成功才 insert 进 set+0xc、
// [item idx start]=局部 start 直存（ScanInt(NULL)）、[item idx end] 读 count 后
// 将 [start,count] 区间逐值 insert（start==0 或 count==0 或解析失败均跳过）、
// [send] 节。[send] 节内标签用直接 strcmp（不入 static map）：三个
// [purchase * count] 数值节 ScanInt(&ok) 失败继续节内、成功即退出节回主循环；
// [send mail] 失败（且节内未曾成功）退出节回主循环、成功则三字段均非 -1 时
// push_back 进 +0x24 并 rule.reset() 后继续节内（ORIG ebx 标志跨迭代保持）；
// 未知 token 忽略继续；节内 ScanType 失败回主循环。ok 共享变量即最终返回值
//（token 耗尽返回 ok 当前值）。
bool importPurchaseCashItemBonusEventScript(PurchaseCashItemBonusScript* script,
                                            char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool ok = false;
    int  start = 0;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return true;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[db table]")
            ScanStr(&script->m_dbTableName);
        else if (token == "[item idx]")
        {
            for (;;)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_set0c.insert(static_cast<unsigned int>(v));
            }
        }
        else if (token == "[item idx start]")
        {
            start = ScanInt((bool*)0);
        }
        else if (token == "[item idx end]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                continue;                 // 回主循环
            if (start == 0 || count == 0)
                continue;                 // ORIG 两处跳过路径均回主循环
            while (start <= count)
            {
                script->m_set0c.insert(static_cast<unsigned int>(start));
                ++start;
            }
        }
        else if (token == "[send]")
        {
            PurchaseCashItemBonusScript::PurchaseBonusRule rule;
            bool sendOk = false;          // ORIG ebx 标志：节内曾成功处理 [send mail]
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                // 节内 EOF：回主循环
                if (token == "[purchase start count]")
                {
                    rule.m_purchaseStartCount = ScanInt(&ok);
                    if (ok)
                        break;            // 成功退出节回主循环
                }
                else if (token == "[purchase end count]")
                {
                    rule.m_purchaseEndCount = ScanInt(&ok);
                    if (ok)
                        break;
                }
                else if (token == "[purchase mutiple number]")
                {
                    rule.m_purchaseMutipleNumber = ScanInt(&ok);
                    if (ok)
                        break;
                }
                else if (token == "[send mail]")
                {
                    Action_SendMail mail;
                    mail.reset();
                    ok = importActionMailScript(&mail);
                    if (ok)
                    {
                        if (rule.m_purchaseStartCount != -1 &&
                            rule.m_purchaseEndCount != -1 &&
                            rule.m_purchaseMutipleNumber != -1)
                        {
                            script->m_ruleMails24.push_back(
                                std::pair<PurchaseCashItemBonusScript::PurchaseBonusRule,
                                          Action_SendMail>(rule, mail));
                        }
                        rule.reset();     // 无论是否入表均复位
                        sendOk = true;    // 置标志：此后失败仍留在节内（ORIG ebx）
                    }
                    else if (!sendOk)
                        break;            // 首败且未成功过：退出节回主循环
                }
                // 其余未知 token：忽略继续节内（ORIG 同）
            }
        }
    }
    return ok;
}
