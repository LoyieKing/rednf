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
// [objdump·报告缺失]。[play time]+0xc(int)/[number of time]+0x10(byte)/
// [start tag]+0x12(word)/[end tag]+0x14(word) 均 ScanInt(NULL) 直存。
bool importPcBangPlayTimeScript(PcBangPlayTimeScript* script, char const* path)
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
        else if (token == "[play time]")
            script->m_playTime = ScanInt((bool*)0);
        else if (token == "[number of time]")
            script->m_numberOfTime = static_cast<unsigned char>(ScanInt((bool*)0));
        else if (token == "[start tag]")
            script->m_startTag = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[end tag]")
            script->m_endTag = static_cast<unsigned short>(ScanInt((bool*)0));
    }
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
// [objdump·报告缺失]。[reward] 为 pair 循环（ScanInt(&ok) 失败回主循环），
// 其余数值节均 ScanInt(NULL) 直存；ok 共享变量即返回值。
bool importAttendanceScript(AttendanceScript* script, char const* path)
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
        else if (token == "[fatigue]")
            script->m_field0c = ScanInt((bool*)0);
        else if (token == "[reward explain]")
            ScanStr(&script->m_str70);
        else if (token == "[final reward explain]")
            ScanStr(&script->m_str74);
        else if (token == "[reward]")
        {
            for (;;)
            {
                std::pair<int, int> attend;
                attend.first = ScanInt(&ok);
                if (!ok)
                    break;
                attend.second = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_attends.push_back(attend);
            }
        }
        else if (token == "[final day condition]")
            script->m_field24 = ScanInt((bool*)0);
        else if (token == "[final reward]")
        {
            script->m_pair1c.first  = ScanInt((bool*)0);   // +0x1c
            script->m_pair1c.second = ScanInt((bool*)0);   // +0x20
        }
        else if (token == "[send mail]")
            importActionMailScript(&script->m_mail);
        else if (token == "[msg overbtn]")
            ScanStr(&script->m_str28);
    }
    return ok;
}

// ---- importUsedFatigueGiveItemEventScript T 0x0898491b（2065B）----
// [objdump·报告缺失]。result 初值 true；三个数值节 ScanInt 失败直接返回假；
// [charc event] 置字节 +0x14=1；[apply job type] 经
// getCharacterJobToEnumValue 存 +0x18；[send mail] 失败仅把 result 置假并继续。
bool importUsedFatigueGiveItemEventScript(UsedFatigueGiveItemScript* script,
                                          char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;
    bool result = true;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[/event]")
            return result;
        else if (token == "[title]")
            ScanStr(&script->m_field00);
        else if (token == "[explain]")
            ScanStr(&script->m_field04);
        else if (token == "[db table]")
            ScanStr(&script->m_dbTableName);
        else if (token == "[user type]")
        {
            script->m_userType = static_cast<unsigned char>(ScanInt(&result));
            if (!result)
                return false;
        }
        else if (token == "[used fatigue]")
        {
            script->m_usedFatigue = ScanInt(&result);
            if (!result)
                return false;
        }
        else if (token == "[charc event]")
        {
            script->m_charcEventFlag = 1;
        }
        else if (token == "[apply job type]")
        {
            std::string jobName;
            ScanStr(&jobName);
            script->m_jobType = getCharacterJobToEnumValue(jobName);
        }
        else if (token == "[used fatigue limit]")
        {
            script->m_fatigueLimit = ScanInt(&result);
            if (!result)
                return false;
        }
        else if (token == "[send mail]")
        {
            if (!importActionMailScript(&script->m_mail))
                result = false;
        }
    }
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
// [objdump·报告缺失]。前置：默认 dbTableName=""、flag08=0。[recv check db table]
// 读表名并置 flag08=1。[level up] 节内逐 job 标签解析：job 键字节（filter[0]）
// 由标签决定 0..0xa，filter[1]=(char)首 ScanInt；随后 (level,itemIdx,count,
// title,content) 三 int + 两 string 循环直至 ScanInt 失败，整块 insert 进
// m_conditions（键=int(filter[0])）；[/level up] 结束节；节内 ScanType 失败
// 直接返回假。[level up reward end] → 返回真。

bool importLevelUpRewardScript(LevelUpRewardScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    script->m_dbTableName = "";
    script->m_flag08 = 0;

    bool result = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[recv check db table]")
        {
            ScanStr(&script->m_dbTableName);
            script->m_flag08 = 1;
        }
        else if (token == "[level up]")
        {
            bool inSection = true;
            while (inSection)
            {
                if (!ScanType(token, true))
                    break;                    // ORIG 回主循环头（EOF 再失败即返回）

                Condition_LevelUp cond;
                Condition_LevelUp::Reward_Item item;

                // 职业标签链（顺序与 ORIG 比较树一致；键=职业序号 0..0xa）
                int job;
                if      (token == "[swordman]")         job = 0;
                else if (token == "[fighter]")          job = 1;
                else if (token == "[gunner]")           job = 2;
                else if (token == "[mage]")             job = 3;
                else if (token == "[priest]")           job = 4;
                else if (token == "[at gunner]")        job = 5;
                else if (token == "[thief]")            job = 6;
                else if (token == "[at fighter]")       job = 7;
                else if (token == "[at mage]")          job = 8;
                else if (token == "[demonic swordman]") job = 9;
                else if (token == "[creator mage]")     job = 0xa;
                else job = -1;

                if (job >= 0)
                {
                    // 命中职业标签：再消费一个子标签（ORIG 不比较、直接丢弃；
                    // 失败直接返回假）。
                    if (!ScanType(token, true))
                        return false;
                    cond.m_filter[0] = static_cast<unsigned char>(job);
                    cond.m_filter[1] =
                        static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level     = ScanInt(&result);
                        if (!result)
                            break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result)
                            break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result)
                            break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    // 键=filter[0]（char&→int），值=cond 拷贝；插入后继续本节
                    //（ORIG 跳过 [/level up] 比较直接回节内扫描）。
                    std::map<int, Condition_LevelUp>::value_type entry(
                        static_cast<int>(static_cast<char>(cond.m_filter[0])),
                        cond);
                    script->m_conditions.insert(entry);
                }
                else if (token == "[/level up]")
                {
                    inSection = false;
                }
                // 其余未知 token：销毁局部并继续本节（ORIG 同）
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
