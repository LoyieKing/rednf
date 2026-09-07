// df_game_r Game/ InGameEventScript 装载域权威类型声明。
//
// 本头为以下类型的唯一权威定义来源（EventLevelupA2 批次）：
//   ::Action_SendMail        —— 全局作用域邮件动作（ORIG nm 实证无命名空间前缀：
//                               _ZN15Action_SendMail5resetEv / 8set_itemEii /
//                               C1Ev @0x088e40ce / D1Ev @0x088e4160；
//                               且 _ZN13EventClassify15CEventActionMng...R15Action_SendMailb
//                               的参数亦为全局型）。
//   Condition_LevelUp        —— 升级奖励条件（含嵌套 Reward_Item）。
//   InGameEventScript        —— 全部事件脚本的非多态基类。
//   各 *Script               —— importInGameEventScript 按 0x194/0x65/0x56/… 分派的
//                               具体脚本对象。
//
// 布局证据等级说明：
//   [报告] docs/class_func_reports/ 反编译实证；
//   [objdump·报告缺失] 该函数/类型无独立报告，按 ORIG 地址反汇编逐指令转写
//   （调用者报告已完整阅读：check_recv_level.md/set_reward_item.md/
//   set_event_data.md/get_db_table_name.md/importInGameEventScript.md/
//   process_action_send_mail.md，及 process_coin_send/process_gold_mail_send/
//   process_mail_send 中对 +0x2c 字节的 movzbl 读法交叉印证）。

#ifndef DNF_GAME_INGAME_EVENT_SCRIPTS_H
#define DNF_GAME_INGAME_EVENT_SCRIPTS_H

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// ============================================================================
// ::Action_SendMail —— 事件系统邮件发送数据（全局作用域）。sizeof = 0x44。
// 布局三重互证：process_action_send_mail.md[报告]（+0x00 map size/find、
// +0x20/+0x24/+0x28/+0x1c 读取、+0x38 push_back）、reset()[objdump 0x080ec026]、
// set_reward_item.md 对 mail+0x30/+0x34 的 string 赋值。
// ============================================================================
struct Action_SendMail
{
    Action_SendMail();                       // ORIG W 0x088e40ce（装载批次定义）
    ~Action_SendMail();                      // ORIG W 0x088e4160（装载批次定义）
    void reset();                            // ORIG W 0x080ec026（定义 EventLevelupA2.cpp）
    void set_item(int itemIndex, int addInfo);  // ORIG W 0x080ec08c（同上）

    std::map<int, std::pair<int, int> > m_jobMap;  // +0x00 按职业查 itemIdx/addInfo
                                                   //       （reset 清空；不 clear 时保留）
    int           m_field18;                        // +0x18（reset 写 0）
    unsigned char m_field1c;                        // +0x1c 非0=直发不走 DB 邮件
    int           m_itemIdx;                        // +0x20 物品代码（set_item 第1参）
    int           m_addInfo;                        // +0x24 addInfo/数量（set_item 第2参）
    int           m_gold;                           // +0x28 >0 走 process_gold_mail_send
    unsigned char m_coin;                           // +0x2c 三份 process_* 报告均 movzbl
                                                  //       字节读、reset 仅清此字节
    unsigned char m_pad2d[3];                       // +0x2d..0x2f（无任何访问证据）
    std::string   m_field30;                        // +0x30 邮件标题 key（set_reward_item 写）
    std::string   m_field34;                        // +0x34 邮件正文 key（set_reward_item 写）
    std::vector<int> m_vec38;                       // +0x38 发奖记录（reset 不清空，忠实）
};                                                  // 0x3c+0xc = 0x44

// ============================================================================
// Condition_LevelUp —— tw 级升级奖励条件表 value 类型。
// C1 [objdump·报告缺失] W 0x0898605a：仅构造 +0x04 vector；
// D1 W 0x08986070：仅析构该 vector。+0x00 四字节由解析器写入，
// 其中 +1 字节被 check_recv_level/set_reward_item 以有符号字节与
// growType 比较（0xff=-1 通配）；其余三字节无访问证据。
// ============================================================================
struct Condition_LevelUp
{
    struct Reward_Item
    {
        Reward_Item();     // ORIG W 0x08986086：构造 +0xc/+0x10 两个 string
        ~Reward_Item();    // ORIG W 0x089860d2

        int         m_level;      // +0x00 与角色等级比较（==/<= 双模式）
        int         m_itemIndex;  // +0x04 → Action_SendMail::set_item 第1参
        int         m_itemCount;  // +0x08 → set_item 第2参（写入 mail.m_addInfo）
        std::string m_title;      // +0x0c → mail.m_field30
        std::string m_content;    // +0x10 → mail.m_field34
    };                            // sizeof = 0x14

    Condition_LevelUp();          // ORIG W 0x0898605a（装载批次定义）
    ~Condition_LevelUp();         // ORIG W 0x08986070

    unsigned char m_filter0;      // +0x00 解析器写入（职业号）；
    unsigned char m_filter1;      // +0x01 growType 过滤（0xff 通配）；
                                  // ORIG 拷贝构造 0x0898673e 对两成员逐个
                                  // movzbl/mov %dl 交替复制 ⇒ 独立标量成员
                                  // 而非数组（4.4 数组形折叠 movzwl 不可翻）
    std::vector<Reward_Item> m_items;  // +0x04

    typedef std::vector<Reward_Item> ItemVector;   // 便捷别名
};                                    // sizeof = 0x10

// ============================================================================
// InGameEventScript —— 全部事件脚本的非多态基类（无虚函数、无虚表）。
// 失败路径由 importInGameEventScript 直接调基类 D1 后 operator delete。
// C1 [objdump·报告缺失] W 0x08985864：默认构造两个 string 后各自 clear()。
// D1 W 0x08110e96：逆序析构两个 string。两者均定义于 EventLevelupA2.cpp 内。
// ============================================================================
class InGameEventScript
{
public:
    InGameEventScript();    // ORIG W 0x08985864
    ~InGameEventScript();   // ORIG W 0x08110e96

    std::string m_field00;  // +0x00
    std::string m_field04;  // +0x04
};                          // sizeof = 0x8

// ============================================================================
// 各具体事件脚本类。尺寸均以 importInGameEventScript 的 operator new 实参实证；
// 构造/析构声明先行，函数体（含各成员构造顺序）归 InGameEventScript 装载批次，
// 各 ctor 反汇编要点已注于类注释供其直接转写。
// ============================================================================

// 事件记录类型 0x194（new 0x6c）。ctor W 0x089861da：基类 → string+0xc →
// map+0x10 → Action_SendMail+0x28；注意 +0x08 字节故意不初始化（忠实保留）。
class LevelUpRewardScript : public InGameEventScript
{
public:
    LevelUpRewardScript();
    ~LevelUpRewardScript();

    unsigned char m_flag08;                         // +0x08（is_recv_check_item 读此字节）
    std::string   m_dbTableName;                    // +0x0c（get_db_table_name 返回处）
    std::map<int, Condition_LevelUp> m_conditions;  // +0x10
    Action_SendMail m_mail;                         // +0x28（get_mail_data 返回处）
};                                                  // 0x10+0x18+0x44 = 0x6c

// 0x65（new 100=0x64）。ctor W 0x08985eec：基类 → string+0x8 → 字节+c=0 →
// int+0x10=0 → 字节+0x14=0 → int+0x18=-1 → int+0x1c=-1 → Action_SendMail+0x20
// → string clear → mail.reset。字段名取自 importUsedFatigueGiveItemEventScript
// 解析标签（objdump·报告缺失，调用者 importInGameEventScript.md 已完整阅读）。
class UsedFatigueGiveItemScript : public InGameEventScript
{
public:
    UsedFatigueGiveItemScript();
    ~UsedFatigueGiveItemScript();

    std::string   m_dbTableName;   // +0x08（[db table]）
    unsigned char m_userType;      // +0x0c（[user type]；C1 写 0）
    unsigned char m_pad0d[3];      // +0x0d..0x0f
    int           m_usedFatigue;   // +0x10（[used fatigue]；C1 写 0）
    unsigned char m_charcEventFlag;  // +0x14（[charc event] 置 1；C1 写 0）
    unsigned char m_pad15[3];      // +0x15..0x17
    int           m_jobType;       // +0x18（[apply job type]；C1 写 -1）
    int           m_fatigueLimit;  // +0x1c（[used fatigue limit]；C1 写 -1）
    Action_SendMail m_mail;        // +0x20
};                                 // 0x20+0x44 = 0x64

// 0x56（new 0x78）。ctor W 0x0898597a：基类 → string+0x8 → vec<pair>+0x10 →
// pair+0x1c → string+0x28 → Action_SendMail+0x2c → string+0x70 → string+0x74 →
// string/vec clear、mail.reset、+0xc=0、+0x24=0。
class AttendanceScript : public InGameEventScript
{
public:
    AttendanceScript();
    ~AttendanceScript();

    std::string m_dbTableName;                   // +0x08
    int         m_field0c;                       // +0x0c（C1 写 0）
    std::vector<std::pair<int, int> > m_attends; // +0x10
    std::pair<int, int> m_pair1c;                // +0x1c
    int         m_field24;                       // +0x24（C1 写 0）
    std::string m_str28;                         // +0x28
    Action_SendMail m_mail;                      // +0x2c
    std::string m_str70;                         // +0x70
    std::string m_str74;                         // +0x74
};                                               // 0x74+0x4 = 0x78

// 0x5c（new 0x18）。ctor W 0x08985fb8：基类 → string+0x8 → clear → int+0xc=0 →
// 字节+0x10=1 → word+0x12=0x20 → word+0x14=0x20。字段名取自
// importPcBangPlayTimeScript 解析标签（objdump·报告缺失）。
class PcBangPlayTimeScript : public InGameEventScript
{
public:
    PcBangPlayTimeScript();
    ~PcBangPlayTimeScript();

    std::string    m_dbTableName;   // +0x08（[db table]）
    int            m_playTime;      // +0x0c（[play time]；C1 写 0）
    unsigned char  m_numberOfTime;  // +0x10（[number of time]；C1 写 1）
    unsigned char  m_pad11;         // +0x11
    unsigned short m_startTag;      // +0x12（[start tag]；C1 写 0x20）
    unsigned short m_endTag;        // +0x14（[end tag]；C1 写 0x20）
    unsigned char  m_pad16[2];      // +0x16..0x17
};                                 // 0x18

// 0x191/0x192/0x19c 共用（new 0x50）。ctor W 0x089858da：基类 → string+0x8 →
// Action_SendMail+0xc → string clear → mail.reset。
class AccountFirstLoginScript : public InGameEventScript
{
public:
    AccountFirstLoginScript();
    ~AccountFirstLoginScript();

    std::string m_dbTableName;  // +0x08
    Action_SendMail m_mail;     // +0x0c
};                              // 0x0c+0x44 = 0x50

// 0x196（new 0x68）。ctor W 0x08985b1a：基类 → vec<pair>+0xc → vec<int>+0x18 →
// Action_SendMail+0x24 → this->reset()。reset W 0x08985ae0：字节+8=0、两 vec
// clear、mail.reset。
class ExchangeRandomItemRewardScript : public InGameEventScript
{
public:
    ExchangeRandomItemRewardScript();
    ~ExchangeRandomItemRewardScript();

    void reset();                                   // ORIG W 0x08985ae0

    unsigned char m_flag08;                         // +0x08（reset 写 0）
    std::vector<std::pair<int, int> > m_pairs0c;    // +0x0c
    std::vector<int> m_ints18;                      // +0x18
    Action_SendMail m_mail;                         // +0x24
};                                                  // 0x24+0x44 = 0x68

// 0x197（new 100=0x64）。ctor W 0x08985be2：基类 → map<char,uint>+0x8 →
// Action_SendMail+0x20 → mail.reset。
class AvatarDisjointRandomRewardScript : public InGameEventScript
{
public:
    AvatarDisjointRandomRewardScript();
    ~AvatarDisjointRandomRewardScript();

    std::map<char, unsigned int> m_map08;  // +0x08
    Action_SendMail m_mail;                // +0x20
};                                         // 0x20+0x44 = 0x64

// 0x199（new 0x10）。ctor W 0x08985c74：基类 → string+0x8 → 字节+c=0 → clear。
// +0xc 字节由 importFatigueQuantityEventScript 的 [user type] 写入（objdump·报告缺失）。
class FatigueQuantityScript : public InGameEventScript
{
public:
    FatigueQuantityScript();
    ~FatigueQuantityScript();

    std::string   m_dbTableName;  // +0x08（[db table]）
    unsigned char m_userType;     // +0x0c（[user type]；C1 写 0）
    unsigned char m_pad0d[3];     // +0x0d..0xf
};                                // 0x10

// 0x19b 之前段（new 0x30）。ctor W 0x08985d1c：基类 → string+0x8 →
// set<uint>+0xc → vector<pair<PurchaseBonusRule,Action_SendMail>>+0x24
// （PurchaseBonusRule 已按 verifyGiveBonusReward/sendPurchaseBonusReward
//   双报告建模为 3×int；解析标签见 importPurchaseCashItemBonusEventScript）。
class PurchaseCashItemBonusScript : public InGameEventScript
{
public:
    // 奖励档位规则（3×int）。布局由 verifyGiveBonusReward.md /
    // sendPurchaseBonusReward.md 汇编双证：+0x00 与购买数比较
    // （>count 则不发）、+0x04 上限（非 0 且 < count 不发）、
    // +0x08 步进（(count-start)%step!=0 不发）；
    struct PurchaseBonusRule
    {
        PurchaseBonusRule();          // ORIG W 0x08985ce6：调 reset()
        void reset();                 // ORIG T 0x08985cfa：三 int 全写 -1

        int m_purchaseStartCount;     // +0x00（[purchase start count]）
        int m_purchaseEndCount;       // +0x04（[purchase end count]）
        int m_purchaseMutipleNumber;  // +0x08（[purchase mutiple number]）
    };                                // sizeof = 0xc

    PurchaseCashItemBonusScript();
    ~PurchaseCashItemBonusScript();

    std::string m_dbTableName;    // +0x08
    std::set<unsigned int> m_set0c;  // +0x0c（目标商品 goods 集合，
                                     //   updateTargetGoodsCount find 键）
    std::vector<std::pair<PurchaseBonusRule, Action_SendMail> > m_ruleMails24;  // +0x24
};                                // 0x24+0xc = 0x30

// 0x19b（new 0x34）。ctor W 0x08985e0a：基类 → string+0x8 → 字节+c=0 →
// vec<int>+0x10 → vector<pair<vector<uint>,Action_SendMail>>+0x1c →
// vector<pair<ObjectBringUpRule,Action_SendMail>>+0x28。Rule 布局（3×int，
// C1=全 -1）由 ObjectBringUpRule C1/reset objdump 与
// verifyGiveBringUpReward.md（报告，已完整阅读）双证。
class ObjectBringUpScript : public InGameEventScript
{
public:
    struct ObjectBringUpRule
    {
        ObjectBringUpRule();          // ORIG W 0x08985dd4：调 reset()
        void reset();                 // ORIG T 0x08985de8：三 int 全写 -1

        int m_rewardStartCount;       // +0x00（[reward start count]）
        int m_rewardEndCount;         // +0x04（[reward end count]）
        int m_rewardMutipleNumber;    // +0x08（[reward mutiple number]）
    };                                // sizeof = 0xc

    ObjectBringUpScript();
    ~ObjectBringUpScript();

    std::string   m_dbTableName;  // +0x08（[db table]；C1 clear）
    unsigned char m_exchangeRuleFlag;  // +0x0c（[exchange rule]；C1 写 0）
    std::vector<int> m_materialItemIdxs;  // +0x10（[material item] 整数列）
    std::vector<std::pair<std::vector<unsigned int>, Action_SendMail> > m_ruleMails1c;  // +0x1c（[eventitem send]）
    std::vector<std::pair<ObjectBringUpRule, Action_SendMail> > m_sendMails28;  // +0x28（[send]）
};                                // 0x28+0xc = 0x34

// 0x193（new 0x24）。ctor W 0x08986134：基类 → vec<int>+0x8 → vec<int>+0x14 →
// string+0x20（get_db_table_name 返回处）。
class AddItemCountDBScript : public InGameEventScript
{
public:
    AddItemCountDBScript();
    ~AddItemCountDBScript();

    std::vector<int> m_itemIdxs;   // +0x08（is_event_item 查询）
    std::vector<int> m_questIds;   // +0x14（is_event_quest 查询）
    std::string m_dbTableName;     // +0x20
};                                 // 0x24

// ---- 按事件记录类型的脚本导入器（T 符号，定义归装载批次）----
// 签名/返回（bool 经 %al 传递）由 importInGameEventScript.md 报告实证。
bool importLevelUpRewardScript(LevelUpRewardScript* script, char const* path);                 // T 0x08980261
bool importUsedFatigueGiveItemEventScript(UsedFatigueGiveItemScript* script, char const* path);// T 0x0898491b
bool importAttendanceScript(AttendanceScript* script, char const* path);                       // T 0x08981539
bool importPcBangPlayTimeScript(PcBangPlayTimeScript* script, char const* path);               // T 0x0898512c
bool importAccountFirstLoginEventScript(AccountFirstLoginScript* script, char const* path);    // T 0x0897f7bf
bool importAddItemCountDBEventScript(AddItemCountDBScript* script, char const* path);          // T 0x0897fc99
bool importExchangeRandomItemRewardScript(ExchangeRandomItemRewardScript* script, char const* path); // T 0x08981e0f
bool importAvatarDisjointRandomRewardScript(AvatarDisjointRandomRewardScript* script, char const* path); // T 0x089824b4
bool importFatigueQuantityEventScript(FatigueQuantityScript* script, char const* path);        // T 0x08982fc6
bool importPurchaseCashItemBonusEventScript(PurchaseCashItemBonusScript* script, char const* path); // T 0x089834a6
bool importObjectBringUpEventScript(ObjectBringUpScript* script, char const* path);            // T 0x08983e48
bool importActionMailScript(Action_SendMail* mail);  // T 0x0897f147

#endif  // DNF_GAME_INGAME_EVENT_SCRIPTS_H
