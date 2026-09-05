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
// __cxa_guard」是函数级静态惰性建表（map 构造/SpinLock 构造各带 guard +
// __cxa_atexit，__inited 零初始化 bool）；构建完成前首个 token 以 key=0 进入
// case 贯穿注册链，表尾释放锁置位后回 guard 重派发同一 token。batch-9 起，
// importAttendanceScript / importPcBangPlayTimeScript /
// importUsedFatigueGiveItemEventScript / importExchangeRandomItemRewardScript
// 四函数按报告逐指令还原该真实双相形（不再用直接 token 比较简化形）；其余
// 同族函数暂保留简化形（各自报告到位后同法收敛）。回边布局技巧：外循环头
// 写 `if (!ScanType) { scan_exit: goto scan_done; }` + 公共尾 `goto
// scan_exit;`，可阻止 4.4 循环旋转、复现 ORIG 4.1 入口直落 + 尾部回跳岛。

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
// ---- importActionMailScript T 0x0897f147（1656B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importActionMailScript.md（双段
// 完整阅读）。此函数主循环用 token 串比较链（非 __lookup 表——ORIG 无
// 惰性建表段，实证汇编 +0x4a 起即 operator== 链）。[job] 节内循环：
// ScanType 失败跳回主循环头（+0x0bb）；每轮先默认构造 pair<int,int> kv
// （-0xbc/-0xb8 跨职业共享，+0x0c9 在比较链之前），命中职业则
// job(-0xb4)=立即数 + kv.first/kv.second = ScanInt(&ok)×2 +
// value_type(job,kv) 引用实参构造 + map::insert（P134 嵌套形）。
// [detail] coin 校验为 signed char <0 || 15< 双段（js/jle，+0x5e6/+0x5f1）。
// 残差归因（N 类）：[job] 8 职业分支已按 ORIG 物理复制+具名 entry/res 独立
// 槽（计数 378/380），但 4.4 栈槽分配器按 SSA 序落位（token 在浅端 -0xc，
// ORIG 4.1 在深端 -0xac），全函数槽号文本错位不可由声明序翻转复现（P112）；
// 比较链 je/jne 朝向为 setcc-fold 族。
bool importActionMailScript(Action_SendMail* mail)
{
    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[inven]")
        {
            // +0x064 ScanInt(&ok)，字节存 +0x1c。
            mail->m_field1c = static_cast<unsigned char>(ScanInt(&ok));
        }
        else if (token == "[job]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                      // +0x0bb → 主循环头
                // P160 逐指令：ORIG 8 职业分支物理复制，每分支 value_type 临时与
                // insert sret 各占独立槽（frame 0xd0；-0xa0/-0x8c/../-0x14 与
                // -0xa8/-0x94/../-0x1c）。4.4 只为具名变量固定分槽，故逐分支具名。
                std::pair<int, int> kv;         // +0x0c9 每轮默认构造（槽共享）
                int jobKey;                     // -0xb4（各分支共写同槽）
                typedef std::map<int, std::pair<int, int> > JobMap;
                if (token == "[swordman]")          // +0x0df
                {
                    jobKey = 0;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry0(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res0 =
                        mail->m_jobMap.insert(entry0);
                    (void)res0;
                }
                else
                if (token == "[fighter]")          // +0x174
                {
                    jobKey = 1;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry1(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res1 =
                        mail->m_jobMap.insert(entry1);
                    (void)res1;
                }
                else
                if (token == "[gunner]")          // +0x209
                {
                    jobKey = 2;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry2(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res2 =
                        mail->m_jobMap.insert(entry2);
                    (void)res2;
                }
                else
                if (token == "[mage]")          // +0x295
                {
                    jobKey = 3;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry3(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res3 =
                        mail->m_jobMap.insert(entry3);
                    (void)res3;
                }
                else
                if (token == "[priest]")          // +0x321
                {
                    jobKey = 4;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry4(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res4 =
                        mail->m_jobMap.insert(entry4);
                    (void)res4;
                }
                else
                if (token == "[atgunner]")          // +0x3ad
                {
                    jobKey = 5;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry5(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res5 =
                        mail->m_jobMap.insert(entry5);
                    (void)res5;
                }
                else
                if (token == "[thief]")          // +0x439
                {
                    jobKey = 6;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry6(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res6 =
                        mail->m_jobMap.insert(entry6);
                    (void)res6;
                }
                else
                if (token == "[atfighter]")          // +0x4c5
                {
                    jobKey = 7;
                    kv.first = ScanInt(&ok);
                    kv.second = ScanInt(&ok);
                    std::pair<const int, std::pair<int, int> > entry7(jobKey, kv);
                    std::pair<JobMap::iterator, bool> res7 =
                        mail->m_jobMap.insert(entry7);
                    (void)res7;
                }
                else
                {
                    if (token == "[/job]")
                        break;                  // +0x55e -> 主循环头
                    // 未知 token：自然回节内头
                }
            }
        }
        else if (token == "[detail]")
        {
            mail->m_field18 = 0;                                   // +0x584
            mail->m_itemIdx = ScanInt(&ok);                        // +0x20
            mail->m_addInfo = ScanInt(&ok);                        // +0x24
            mail->m_gold    = ScanInt(&ok);                        // +0x28
            mail->m_coin    = static_cast<unsigned char>(ScanInt(&ok));  // +0x2c
            // +0x5e6 js / +0x5ef cmp 0xf jle：signed char 出界清零。
            if (static_cast<char>(mail->m_coin) < 0 ||
                static_cast<char>(mail->m_coin) > 15)
            {
                mail->m_coin = 0;
            }
            ScanStr(&mail->m_field30);                             // 标题 key
            ScanStr(&mail->m_field34);                             // 正文 key
        }
        else if (token == "[/send mail]")
        {
            return true;                        // +0x639 mov $1,%ebx
        }
    }
    return false;                               // ScanType EOF：+0x40 mov $0,%ebx
}

// ============================================================================
// D. import*EventScript ×10
// ============================================================================

// ---- importFatigueQuantityEventScript T 0x08982fc6（1248B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importFatigueQuantityEventScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→LAB(title)→explain→
// db table→user type，公共尾 +0x474。id：[/event]0x478 [title]0x47a
// [explain]0x47d [db table]0x480 [user type]0x483。[user type] ScanInt(NULL)
// 字节截断值先物化存 +0xc（+0x46c mov %eax,%edx / mov %dl）；-0x1d bool 槽
// 初值 0 仅作 ScanType 失败返回（+0x4a1 movzbl -0x1d），此外从不写。
bool importFatigueQuantityEventScript(FatigueQuantityScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;

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
        case 0x478:                                     // [/event]
        case 0:
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/event]", 0x478));
            }
            else
            {
                return true;
            }
        }
        /* fall through */
        case 0x47a:                                     // [title]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[title]", 0x47a));
            }
            else
            {
                ScanStr(&script->m_field00);
                break;
            }
        }
        /* fall through */
        case 0x47d:                                     // [explain]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[explain]", 0x47d));
            }
            else
            {
                ScanStr(&script->m_field04);
                break;
            }
        }
        /* fall through */
        case 0x480:                                     // [db table]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[db table]", 0x480));
            }
            else
            {
                ScanStr(&script->m_dbTableName);
                break;
            }
        }
        /* fall through */
        case 0x483:                                     // [user type]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[user type]", 0x483));
            }
            else
            {
                script->m_userType =
                    static_cast<unsigned char>(ScanInt((bool*)0));
                break;
            }
        }
        }

        if (!__inited)
        {
            __init_lock.leave();                        // +0x489
            __inited = true;                            // +0x48e
            goto redo_lookup;                           // +0x495 → guard 重派发
        }
        goto scan_exit;                                 // +0x49b → ScanType 回边
    }
scan_done:
    return result;
}

// ---- importAccountFirstLoginEventScript T 0x0897f7bf（1242B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAccountFirstLoginEventScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→LAB(0x fab4)→(fb12)→
// (fb73)→(fbd1)，公共尾 +0x46e。id：[/event]0xc4 [title]0xc6 [explain]0xc9
// [db table]0xcc [send mail]0xcf；mail 存 +0xc；-0x1d bool 槽仅作 ScanType
// 失败返回（+0x49b）。
bool importAccountFirstLoginEventScript(AccountFirstLoginScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;

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
        case 0xc4:                                      // [/event]
        case 0:
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/event]", 0xc4));
            }
            else
            {
                return true;
            }
        }
        /* fall through */
        case 0xc6:                                      // [title]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[title]", 0xc6));
            }
            else
            {
                ScanStr(&script->m_field00);
                break;
            }
        }
        /* fall through */
        case 0xc9:                                      // [explain]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[explain]", 0xc9));
            }
            else
            {
                ScanStr(&script->m_field04);
                break;
            }
        }
        /* fall through */
        case 0xcc:                                      // [db table]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[db table]", 0xcc));
            }
            else
            {
                ScanStr(&script->m_dbTableName);
                break;
            }
        }
        /* fall through */
        case 0xcf:                                      // [send mail]
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[send mail]", 0xcf));
            }
            else
            {
                importActionMailScript(&script->m_mail);   // +0xc
                break;
            }
        }
        }

        if (!__inited)
        {
            __init_lock.leave();                        // +0x483
            __inited = true;                            // +0x488
            goto redo_lookup;                           // +0x48f → guard 重派发
        }
        goto scan_exit;                                 // +0x495 → ScanType 回边
    }
scan_done:
    return result;
}

// ---- importAddItemCountDBEventScript T 0x0897fc99（1480B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAddItemCountDBEventScript.md
// （双段完整阅读）。双相派发同族：6 键注册链 +0x2a8→+0x30f→+0x36d→+0x3ce→
// +0x42f→+0x4c4，公共尾 +0x559。id：[/event]0xe9 [title]0xeb [explain]0xee
// [db table]0xf1 [item index]0xf4 [quest index]0xff。[item index]/[quest
// index] 均为「槽清 0→ScanInt(&ok)→失败回主循环→push_back→重循环」的收集
// 循环（+0x47d/+0x512，此前简化形漏循环=语义缺口），vec 分别 +0x8/+0x14；
// [db table] 串存 +0x20。ok(-0xa9) 共享即返回值。
bool importAddItemCountDBEventScript(AddItemCountDBScript* script, char const* path)
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
        case 0xe9:                                      // [/event]（+0x2a8）
        case 0:
        {
            if (!__inited)
    {
                __lookup.insert(LookupMap::value_type("[/event]", 0xe9));
        }
            else
        {
                return true;                            // +0x305
            }
        }
        /* fall through */
        case 0xeb:                                      // [title]（+0x30f）
        {
            if (!__inited)
        {
                __lookup.insert(LookupMap::value_type("[title]", 0xeb));
            }
            else
            {
            ScanStr(&script->m_field00);
                break;
            }
        }
        /* fall through */
        case 0xee:                                      // [explain]（+0x36d）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[explain]", 0xee));
            }
            else
            {
            ScanStr(&script->m_field04);
                break;
            }
        }
        /* fall through */
        case 0xf1:                                      // [db table]（+0x3ce）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[db table]", 0xf1));
            }
            else
            {
                ScanStr(&script->m_dbTableName);       // +0x20
                break;
            }
        }
        /* fall through */
        case 0xf4:                                      // [item index]（+0x42f）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[item index]", 0xf4));
            }
            else
            {
                for (;;)
                {
                    int v = 0;                          // -0xb4 每轮清 0
                    v = ScanInt(&ok);
                    if (ok)
                        script->m_itemIdxs.push_back(v);   // +0x8
                    else
                        break;                          // +0x4a4 → 公共尾
                }
                break;
            }
        }
        /* fall through */
        case 0xff:                                      // [quest index]（+0x4c4）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[quest index]", 0xff));
            }
            else
            {
                for (;;)
                {
                    int v = 0;                          // -0xb8 每轮清 0
                    v = ScanInt(&ok);
                    if (ok)
                        script->m_questIds.push_back(v);   // +0x14
                    else
                        break;                          // +0x539 → 公共尾
                }
                break;
            }
        }
        }

        if (!__inited)
        {
            __init_lock.leave();                        // +0x56e
            __inited = true;                            // +0x573
            goto redo_lookup;                           // +0x57a → guard 重派发
        }
        goto scan_exit;                                 // +0x580 → ScanType 回边
    }
scan_done:
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
                    // ORIG +0x4d5 值先物化经 %edx 再重载 this 存 +0xc；4.4 对
                    // register 局部固定分配 %ebx（+0x4d5 `mov %eax,%edx` vs 本
                    // 形 `mov %eax,%ebx`，等长等助记符仅寄存器名差 = N7/N18a/P78
                    // 结构性天花板 → NEAR 豁免，ORIG 0x08985601..+0x4da 实证；
                    // 普通赋值会被 4.4 折为 LHS 地址先行形反而 DIFF，uint 中转
                    // 则多一条 `mov %ebx,%edx`）。
                    register int v = ScanInt((bool*)0);
                    script->m_playTime = v;
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
// 报告 docs/class_func_reports/_lt_global_gt_/importExchangeRandomItemRewardScript.md
// （双段完整阅读）。双相派发同族：6 个 (键,id) 注册链 +0x2a8→+0x30f→+0x376→
// +0x3d7→+0x540→+0x5d7，公共尾 +0x636。id：[/event]0x2e8 [title]0x2ea
// [explain]0x2ed [material]0x2f0 [random reward item]0x308 [send mail]0x312。
// [material] 节（+0x428）：先 ScanType 一次（失败 mov $0,%ebx 直返假，+0x445）；
// 该 token 经 std::operator== 与 "[exchange rule]" 直比（+0x460，非查表）；
// 相等则 ScanInt(&ok) 字节存 +0x8，失败跳公共尾回主循环；两路均汇入 uint
// 整数对循环（+0x491：槽先清 0，make_pair<unsigned&,unsigned&> 引用对临时
// （P61/P81）→ pair<int,int>::pair<unsigned,unsigned> 转换构造 →
// vector<pair<int,int>>::push_back）。[random reward item]（+0x58e）int 槽每轮
// 清 0 后 ScanInt(&ok)，成功 push_back 至 +0x18；ok 共享变量即返回值（+0x663）。
bool importExchangeRandomItemRewardScript(ExchangeRandomItemRewardScript* script,
                                          char const* path)
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
        case 0x2e8:                                     // [/event]（+0x2a8）
        case 0:
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/event]", 0x2e8));
            }
            else
            {
                return true;                            // +0x305 mov $1,%ebx
            }
        }
        /* fall through */
        case 0x2ea:                                     // [title]（+0x30f）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[title]", 0x2ea));
            }
            else
            {
                ScanStr(&script->m_field00);
                break;
            }
        }
        /* fall through */
        case 0x2ed:                                     // [explain]（+0x376）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[explain]", 0x2ed));
            }
            else
            {
                ScanStr(&script->m_field04);
                break;
            }
        }
        /* fall through */
        case 0x2f0:                                     // [material]（+0x3d7）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[material]", 0x2f0));
            }
            else
            {
                if (!ScanType(token, true))
                    return false;                       // +0x445 直接返回假
                if (token == "[exchange rule]")          // +0x460 串比较（非查表）
                {
                    script->m_flag08 = static_cast<unsigned char>(
                        ScanInt(&ok));
                    // 规则读失败：跳过整数对循环直回公共尾（+0x48b→+0x636）。
                    if (!ok)
                        break;
                }
                // 非规则 token 与规则读取成功两路汇入同一整数对循环
                // （+0x467 je +0x491 / +0x48b fall-through，ORIG 物理单份）。
                for (;;)
                {
                    unsigned int first  = 0;
                    unsigned int second = 0;
                    first = ScanInt(&ok);
                    if (!ok)
                        break;
                    second = ScanInt(&ok);
                    if (!ok)
                        break;
                    // 引用对临时（-0x4c）+ pair&lt;int,int&gt; 转换临时（-0x54）均为
                    // 内联实参 sret 槽（P134），不命名局部。
                    script->m_pairs0c.push_back(
                        std::pair<int, int>(std::make_pair(first, second)));
                }
                break;
            }
        }
        /* fall through */
        case 0x308:                                     // [random reward item]（+0x540）
        {
            if (!__inited)
            {
                __lookup.insert(
                    LookupMap::value_type("[random reward item]", 0x308));
            }
            else
            {
                for (;;)
                {
                    int v = 0;                          // -0xcc 每轮清 0
                    v = ScanInt(&ok);
                    if (!ok)
                        break;
                    script->m_ints18.push_back(v);      // +0x18 vector<int>
                }
                break;
            }
        }
        /* fall through */
        case 0x312:                                     // [send mail]（+0x5d7）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[send mail]", 0x312));
            }
            else
            {
                importActionMailScript(&script->m_mail);   // +0x24
                break;
            }
        }
        }

        if (!__inited)
        {
            __init_lock.leave();                        // +0x64b
            __inited = true;                            // +0x650
            goto redo_lookup;                           // +0x657 → guard 重派发
        }
        goto scan_exit;                                 // +0x65d → ScanType 回边
    }
scan_done:
    return ok;
}

// ---- importAvatarDisjointRandomRewardScript T 0x089824b4（1429B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAvatarDisjointRandomRewardScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→+0x304→+0x365→+0x3c6→
// +0x4ca，公共尾 +0x526。id：[/event]0x32e [title]0x330 [explain]0x333
// [random reward item]0x336 [send mail]0x346。[random reward item]（+0x417）：
// char/uint 双槽每轮先清 0，key=(char)ScanInt 字节截断存槽（+0x436 mov %al），
// 成功对经 make_pair&lt;char&amp;,unsigned&amp;&gt; 引用对临时+value_type 转换+
// map::insert 三段（P134 嵌套实参形），任一 ScanInt 失败回主循环。
bool importAvatarDisjointRandomRewardScript(AvatarDisjointRandomRewardScript* script,
                                            char const* path)
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
        case 0x32e:                                     // [/event]（+0x29d）
        case 0:
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/event]", 0x32e));
            }
            else
            {
                return true;                            // +0x2fa
            }
        }
        /* fall through */
        case 0x330:                                     // [title]（+0x304）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[title]", 0x330));
            }
            else
            {
            ScanStr(&script->m_field00);
                    break;
            }
        }
        /* fall through */
        case 0x333:                                     // [explain]（+0x365）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[explain]", 0x333));
            }
            else
            {
            ScanStr(&script->m_field04);
                    break;
            }
        }
        /* fall through */
        case 0x336:                                     // [random reward item]（+0x3c6）
        {
            if (!__inited)
            {
                __lookup.insert(
                    LookupMap::value_type("[random reward item]", 0x336));
            }
            else
            {
            for (;;)
{
                    char         rewardKey = 0;         // -0xb5 每轮清 0
                    unsigned int cnt = 0;               // -0xbc
                    rewardKey = static_cast<char>(ScanInt(&ok));
                if (!ok)
                        break;                          // +0x44a → 公共尾
                cnt = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok)
                        break;                          // +0x46e → 公共尾
                    // 引用对临时(-0x38)+value_type 转换(-0x40)+insert(-0x48)
                    // 三段 sret 内联实参形（P134/P61）。
                script->m_map08.insert(
                        std::make_pair(rewardKey, cnt));
            }
                break;
            }
        }
        /* fall through */
        case 0x346:                                     // [send mail]（+0x4ca）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[send mail]", 0x346));
            }
            else
            {
                importActionMailScript(&script->m_mail);   // +0x20
                break;
            }
        }
        }

        if (!__inited)
        {
            __init_lock.leave();                        // +0x53b
            __inited = true;                            // +0x540
            goto redo_lookup;                           // +0x547 → guard 重派发
        }
        goto scan_exit;                                 // +0x54d → ScanType 回边
    }
scan_done:
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
// 报告 docs/class_func_reports/_lt_global_gt_/importObjectBringUpEventScript.md
// （双段完整阅读）。双相派发同族：7 键注册链 +0x2bf→+0x326→+0x393→+0x403→
// +0x473→+0x58b→+0x626，公共尾 +0xa64。id：[/event]0x509 [title]0x50b
// [explain]0x50e [db table]0x511 [material item]0x515 [eventitem send]0x529
// [send]0x54c。[material item]（+0x4d3）：节内 ScanType 失败 mov $0,%ebx 直返
// 假（+0x4f5）；[exchange rule] 字节存 +0xc，读失败退出节回主循环（+0x536）；
// 两路汇入同一 int 收集循环（+0x53c 槽清 0，失败回主循环）。[eventitem send]
// （+0x5eb）：局部 vector<uint> fatigues；[get eventitem fatigue] 为内层收集
// 循环（+0x637），ScanInt 失败回节内头重 ScanType（+0x664，非退节）；
// [send mail] 成功 pair(vector,mail) 值临时 push +0x1c 后 fatigues.clear()
// （+0x754）并继续节内，失败退节（+0x786 je→退）；[/eventitem send] 或
// ScanType 失败或未知 token 后判定退节/继续如汇编；节退一律 dtor fatigues。
// [send]（+0x832）：局部 ObjectBringUpRule rule；三个 [reward * ] 数值节
// ScanInt(&ok) 成功→回节内头、失败→直接公共尾（rule 不清理）；[send mail]
// 失败仅退节不复位 rule（+0x997），成功三字段均 !=-1 时 push +0x28、
// rule.reset() 后回节内（+0xa58）；ScanType 失败直接公共尾。此前简化形漏
// 「fatigue 内层循环/clear 复位/mail 失败不复位 rule」三处语义，已按 ORIG 修正。
bool importObjectBringUpEventScript(ObjectBringUpScript* script, char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;   // 兼作节内 ScanInt 的 ok（ORIG 同槽 -0x165）
    // P31 dead local（switch 前）：复现 ORIG 帧 0x1dc 与 mail/rule/
    // fatigues 深端槽号（-0x1cc/-0x188/-0x17c）。token/iterator 交错槽
    // 序（ORIG conv2 -0x158 vs 4.4 -0x168 区）属跨版本栈槽分配器差异
    // （P112/N76 族），无法由合法源码复现，残差按 N 类标注。

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
        int deadB;             // 槽位对齐 dead（浅端）

        switch (key)
        {
        case 0x509:                                     // [/event]（+0x2bf）
        case 0:
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/event]", 0x509));
            }
            else
            {
                return true;                            // +0x31c
            }
        }
        /* fall through */
        case 0x50b:                                     // [title]（+0x326）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[title]", 0x50b));
            }
            else
            {
            ScanStr(&script->m_field00);
                        break;
            }
        }
        /* fall through */
        case 0x50e:                                     // [explain]（+0x393）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[explain]", 0x50e));
            }
            else
            {
            ScanStr(&script->m_field04);
                        break;
            }
        }
        /* fall through */
        case 0x511:                                     // [db table]（+0x403）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[db table]", 0x511));
            }
            else
            {
            ScanStr(&script->m_dbTableName);
                        break;
            }
        }
        /* fall through */
        case 0x515:                                     // [material item]（+0x473）
        {
            if (!__inited)
            {
                __lookup.insert(
                    LookupMap::value_type("[material item]", 0x515));
            }
            else
            {
                if (!ScanType(token, true))
                    return false;                       // +0x4f0 直返假
                if (token == "[exchange rule]")          // +0x50b 串直比
                {
                script->m_exchangeRuleFlag =
                    static_cast<unsigned char>(ScanInt(&result));
                    // 失败退出节回主循环（+0x536 jne 公共尾）。
                    if (result != true)
                        break;
                }
                // 两路汇入同一整数收集循环（物理单份，+0x53c）。
                for (;;)
                {
                    int idx = 0;                        // -0x1c 每轮清 0
                    idx = ScanInt(&result);
                    if (result != true)
                        break;                          // +0x563 → 公共尾
                    script->m_materialItemIdxs.push_back(idx);
                }
                break;
            }
        }
        /* fall through */
        case 0x529:                                     // [eventitem send]（+0x58b）
        {
            if (!__inited)
            {
                __lookup.insert(
                    LookupMap::value_type("[eventitem send]", 0x529));
            }
            else
            {
                std::vector<unsigned int> fatigues;     // -0x178 节局部
            send_sec_head:;
                for (;;)
                {
                if (!ScanType(token, true))
                        break;              // EOF → 退节（+0x7af→+0x7b0）
                if (token == "[get eventitem fatigue]")
                    {
                for (;;)
{
                            unsigned int f = 0;         // -0x17c 每轮清 0
                            f = static_cast<unsigned int>(ScanInt(&result));
                            if (result != true)
                                goto send_sec_head;     // 失败回节内头（+0x664）
                            fatigues.push_back(f);
                    }
                    }
                else if (token == "[send mail]")
                    {
                        Action_SendMail mail;           // -0x1cc ctor+reset
                    mail.reset();
                        result = importActionMailScript(&mail);
                        if (result == true)
    {
                    script->m_ruleMails1c.push_back(
                                std::pair<std::vector<unsigned int>,
                                          Action_SendMail>(fatigues, mail));
                            fatigues.clear();           // +0x754 节内复位
                            continue;                   // +0x78a 回节内头
                }
                        break;                          // 失败退节（+0x786）
                    }
                else if (token == "[/eventitem send]")
                        break;                          // +0x7ad 退节
                    // 未知 token：+0x7a7 回节内头继续
            else
                        goto send_sec_head;
                }
                break;
            }
        }
        /* fall through */
        case 0x54c:                                     // [send]（+0x626）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[send]", 0x54c));
            }
            else
            {
                ObjectBringUpScript::ObjectBringUpRule rule;   // -0x18c
                for (;;)
                {
                if (!ScanType(token, true))
                        break;                          // EOF → +0xa64 公共尾
                if (token == "[reward start count]")
                    {
                    rule.m_rewardStartCount = ScanInt(&result);
                        if (result == true)
                            continue;                   // +0x8aa→节内头
                        break;                          // 失败 → 公共尾
                    }
                else if (token == "[reward end count]")
                    {
                    rule.m_rewardEndCount = ScanInt(&result);
                        if (result == true)
                        continue;
                        break;
                    }
                else if (token == "[reward mutiple number]")
                    {
                    rule.m_rewardMutipleNumber = ScanInt(&result);
                        if (result == true)
                        continue;
                        break;
                    }
                else if (token == "[send mail]")
                    {
                        Action_SendMail mail;           // ctor + reset（+0x962/970）
                    mail.reset();
                        result = importActionMailScript(&mail);
                        if (result == true)
        {
                            if (rule.m_rewardStartCount != -1 &&
                        rule.m_rewardEndCount != -1 &&
                        rule.m_rewardMutipleNumber != -1)
            {
                        script->m_sendMails28.push_back(
                                    std::pair<
                                        ObjectBringUpScript::ObjectBringUpRule,
                                      Action_SendMail>(rule, mail));
            }
                            rule.reset();               // 仅成功路径复位（+0xa24）
                            continue;                   // +0xa58 回节内头
                }
                        break;           // 失败退节，rule 不复位（+0x997 直落）
                    }
                    // 未知 token：+0x953 回节内头继续
            else
                        continue;
                }
                break;
            }
        }
        }

        if (!__inited)
        {
            __init_lock.leave();                        // +0xa79
            __inited = true;                            // +0xa7e
            goto redo_lookup;                           // +0xa85 → guard 重派发
        }
        goto scan_exit;                                 // +0xa8b → ScanType 回边
            }
scan_done:
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
// 残差归因（N 类）：ORIG(4.1) 把 token(-0x140) 与 11 个职业分支各自的
// cond/ri/临时槽在深区按活跃性交错复用（帧 0x170），4.4 分配器为每变量
// 固定独立槽（帧 0x1a0、token -0x68），几乎每条含槽指令文本错位——跨版本
// 栈槽分配器差异（P112/N76 族），无法由合法 C++ 复现；语义与指令序列已
// 逐分支对照 ORIG 汇编一致。
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
// 报告 docs/class_func_reports/_lt_global_gt_/importPurchaseCashItemBonusEventScript.md
// （双段完整阅读）。双相派发同族：8 键注册链 +0x2e0→+0x347→+0x3b4→+0x424→
// +0x494→+0x54a→+0x5be→+0x6aa，公共尾 +0x933。id：[/event]0x4a1 [title]0x4a3
// [explain]0x4a6 [db table]0x4a9 [item idx]0x4ac [item idx start]0x4b6
// [item idx end]0x4b9 [send]0x4c6。函数级 uint start(-0x13c) 初值 0（+0x45）。
// [item idx]（+0x4f1）uint 槽清 0→ScanInt(&ok)→set<unsigned>+0xc insert，
// 失败回主循环；[item idx start] ScanInt(NULL) 存 start；[item idx end]
// （+0x61e）uint end=ScanInt(&ok)，ok&&start!=0&&end!=0 时
// `for(;start<=end;++start) set.insert(start)`（+0x695 setbe 无符号界）。
// [send] 节（+0x6fe）：局部 rule ctor 后节内 while(ScanType)，四标签经
// std::operator== 直比（不入查表）：三个 [purchase * count] ScanInt(&ok)
// 成功→节内 continue、失败→退出节回主循环（+0x776/+0x778，此前简化形方向
// 写反=语义缺口，现按 ORIG 修正）；[send mail] 局部 mail ctor+reset，
// importActionMailScript 失败立即退出节（rule 不复位保留原值，+0x861..868），
// 成功则三字段均 !=-1 时 push_back(pair(rule,mail) 值临时+~pair)、随后
// rule.reset() 后回节内（+0x924 test ebx/jne +0x71b）。未知 token 忽略
// 继续；节内 ScanType 失败退出节回主循环。ok(-0x135) 共享即返回值（+0x960）。
// 残差归因（N 类，等计数 563=563）：4 节 ok 检查点 je/jne 朝向（4.1 先
// xor 物化再反判 vs 4.4 直接 test 反转目标，setcc-fold 族）、节头 4 枚
// 前跳 nop;jmp 岛（+0x70e..+0x717，jump-threading 族）、pair 构造实参
// %eax↔%edx（N7）、make_pair sret 后 `sub $0x4` 伴随（P75 族）均不可由
// 合法源码翻正；[item idx end] 的 `xor;test;jne` 已用 != true 形复现。
bool importPurchaseCashItemBonusEventScript(PurchaseCashItemBonusScript* script,
                                            char const* path)
{
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;
    unsigned int start = 0;          // -0x13c：函数级，[item idx start] 覆写

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
            __init_lock.enter();                        // +0x233
            if (__inited)
            {
                __init_lock.leave();                    // +0x24a
                goto redo_lookup;                       // +0x250 → guard 重查
            }
            key = 0;                                    // +0x255 注册链入口
        }

        switch (key)
        {
        case 0x4a1:                                     // [/event]（+0x2e0）
        case 0:
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/event]", 0x4a1));
            }
            else
            {
                return true;                            // +0x33d
            }
        }
        /* fall through */
        case 0x4a3:                                     // [title]（+0x347）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[title]", 0x4a3));
            }
            else
            {
            ScanStr(&script->m_field00);
                    break;
            }
        }
        /* fall through */
        case 0x4a6:                                     // [explain]（+0x3b4）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[explain]", 0x4a6));
            }
            else
            {
            ScanStr(&script->m_field04);
                        break;
            }
        }
        /* fall through */
        case 0x4a9:                                     // [db table]（+0x424）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[db table]", 0x4a9));
            }
            else
            {
                ScanStr(&script->m_dbTableName);        // +0x8
                        break;
            }
        }
        /* fall through */
        case 0x4ac:                                     // [item idx]（+0x494）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[item idx]", 0x4ac));
            }
            else
            {
            for (;;)
                {
                    unsigned int v = 0;                 // -0x144 每轮清 0
                    v = static_cast<unsigned int>(ScanInt(&ok));
                    if (ok != true)                     // +0x50f xor 物化形
                        break;                          // +0x51e → 公共尾
                    script->m_set0c.insert(v);          // set<unsigned>+0xc
                }
                break;
            }
        }
        /* fall through */
        case 0x4b6:                                     // [item idx start]（+0x54a）
        {
            if (!__inited)
            {
                __lookup.insert(
                    LookupMap::value_type("[item idx start]", 0x4b6));
            }
            else
            {
                start = static_cast<unsigned int>(ScanInt((bool*)0));
                break;
            }
        }
        /* fall through */
        case 0x4b9:                                     // [item idx end]（+0x5be）
        {
            if (!__inited)
            {
                __lookup.insert(
                    LookupMap::value_type("[item idx end]", 0x4b9));
            }
            else
            {
                unsigned int end = 0;                   // 槽清 0 后 ScanInt
                end = static_cast<unsigned int>(ScanInt(&ok));
                if (ok != true)                         // +0x642 xor/jne 直退
                    break;                              // 失败 → 公共尾
                if (start != 0)                         // +0x650
                {
                    if (end != 0)
                    {
                        for (; start <= end; ++start)
                            script->m_set0c.insert(start);
                    }
                }
                break;
            }
        }
        /* fall through */
        case 0x4c6:                                     // [send]（+0x6aa）
        {
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[send]", 0x4c6));
            }
            else
            {
            PurchaseCashItemBonusScript::PurchaseBonusRule rule;
                for (;;)
                {
                if (!ScanType(token, true))
                        break;              // 节内 EOF → 公共尾（+0x932）
                if (token == "[purchase start count]")
                    {
                    rule.m_purchaseStartCount = ScanInt(&ok);
                        if (!ok)
                            break;                      // 失败退出节（+0x778）
                        continue;                       // 成功回节内头
                    }
                else if (token == "[purchase end count]")
                    {
                    rule.m_purchaseEndCount = ScanInt(&ok);
                        if (!ok)
                            break;
                        continue;
                    }
                else if (token == "[purchase mutiple number]")
                    {
                    rule.m_purchaseMutipleNumber = ScanInt(&ok);
                        if (!ok)
                            break;
                        continue;
                    }
                else if (token == "[send mail]")
                    {
                        Action_SendMail mail;            // +0x82e ctor
                        mail.reset();                    // +0x83c
                        // 返回值先物化回 ok 槽再判（+0x84f mov %al,-0x135 /
                        // +0x855 movzbl 读回形）。
                        ok = importActionMailScript(&mail);
                        if (ok != true)                  // +0x861 xor 物化形
                        {
                            break;          // 失败即退出节，rule 不复位（+0x863）
                        }
                        else
                        {
                            if (rule.m_purchaseStartCount != -1 &&
                                rule.m_purchaseEndCount != -1 &&
                                rule.m_purchaseMutipleNumber != -1)
                    {
                                script->m_ruleMails24.push_back(
                                    std::pair<
                                        PurchaseCashItemBonusScript::PurchaseBonusRule,
                                        Action_SendMail>(rule, mail));
                    }
                            rule.reset();                // +0x8e7
                        }
                    }
                }
                break;
            }
        }
        }

        if (!__inited)
        {
            __init_lock.leave();                        // +0x948
            __inited = true;                            // +0x94d
            goto redo_lookup;                           // +0x954 → guard 重派发
        }
        goto scan_exit;                                 // +0x95a → ScanType 回边
    }
scan_done:
    return ok;
}
