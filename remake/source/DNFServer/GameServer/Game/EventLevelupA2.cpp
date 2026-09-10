// ============================================================================
// EventLevelupA2.cpp —— 事件/升级剩余依赖恢复（独占 TU；EventLevelupA2 批次）。
//
// 指派符号（全部逐份完整阅读 ## 汇编 + ## 反编译 C 后转写，报告路径见各函数）：
//   1. LevelupSupportEventManger::LevelupSupportEventManger   C1 0x08146d30
//      docs/class_func_reports/LevelupSupportEventManger/LevelupSupportEventManger.md
//   2. LevelupSupportEventManger::~LevelupSupportEventManger  D1 0x08146dc0
//      docs/class_func_reports/LevelupSupportEventManger/~LevelupSupportEventManger.md
//   3. LevelupSupport2ndEventManger::LevelupSupport2ndEventManger C1 0x0814778e
//      docs/class_func_reports/LevelupSupport2ndEventManger/LevelupSupport2ndEventManger.md
//   4. LevelupSupport2ndEventManger::~LevelupSupport2ndEventManger D1 0x081477c4
//      docs/class_func_reports/LevelupSupport2ndEventManger/~LevelupSupport2ndEventManger.md
//   5. EventClassify::CLevelUpReward::check_recv_level(int,int,int,bool) 0x0810e192
// b28d-45 判词汇总（本 TU 2 件）：
//   LevelupSupportEventManger::C1 0x08146d30 DIFF42 N109：4.4 多基类 ctor 在首个
//     基类 ctor 后多写一次 vptr（`movl $0x0,(%eax)`），P242 提首基类法无解。
//   importInGameEventScript 0x08982a4a DIFF320 N82/N89：352:351 大解析函数
//     决策链/槽序（仅 1 条差）。
//      docs/class_func_reports/EventClassify__CLevelUpReward/check_recv_level.md
//   6. EventClassify::CLevelUpReward::set_reward_item(int,int,int) 0x0810e302
//      docs/class_func_reports/EventClassify__CLevelUpReward/set_reward_item.md
//   7. InGameEventScriptLoader::importInGameEventScript(int,std::string const&) 0x08982a4a
//      docs/class_func_reports/InGameEventScriptLoader/importInGameEventScript.md
//
// 直接依赖的小生命周期（无独立报告——【报告缺失】情形，已先完整阅读其调用者/
// 同类报告后按 ORIG 地址 objdump 逐指令转写；符号均经 ORIG nm 核实）：
//   - InGameEventScript::InGameEventScript   W 0x08985864
//   - InGameEventScript::~InGameEventScript  W 0x08110e96
//   - Action_SendMail::reset                 W 0x080ec026
//   - Action_SendMail::set_item(int,int)     W 0x080ec08c
//
// 本 TU 引用、待对应批次提供定义的 ORIG 符号（声明见 InGameEventScripts.h /
// LevelupSupportEventManger.h / Arad_DataManager.h）：
//   - LevelupSupportEventManger::Load/Destroy、LevelupSupport2ndEventManger::
//     Load/Destroy（ARAD 脚本服务 #6/#9 批次；本 TU 的 ctor/dtor 写 vptr 引用
//     两类 vtable，注册顺序需与该批次协调）
//   - LevelupSupportSciprt/LevelupSupport2ndScript C1/D1（报告已存在）
//   - 11 个事件脚本类 ctor/dtor + import* 解析器 + Action_SendMail/Condition_
//     LevelUp/Reward_Item C1/D1（InGameEventScript 装载批次）
// ============================================================================

#include "CEventScriptMng.h"           // InGameEventScriptLoader/CLevelUpReward 声明
#include "InGameEventScripts.h"        // Action_SendMail/Condition_LevelUp/各脚本类
#include "LevelupSupportEventManger.h" // LevelupSupport(2nd)EventManger 及脚本解析器

// [T2-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit（ORIG 实有符号），
// 使本 TU 发射 _GLOBAL__I_（ORIG CU global_LevelupSupport.cpp 的键名
// `_ZN25LevelupSupportEventMangerC2Ev` = 本 TU 首个强外部符号）。零语义/布局改动。
#include <iostream>

// ============ LevelupSupportEventManger C1/D1（报告转写） ============
// 报告：Singleton 空 base → ScriptInterface base → 写 vptr →
// LevelupSupportSciprt(+0x04) → map<int,int>(+0x1c) → vector<EventInfo>(+0x34)。
// D1：写 vptr → 析构 vector → map → LevelupSupportSciprt。
// 注：ORIG C1 另有一个空基构造调用 _ZN4ARAD9SingletonI25…EC1Ev（W 0x08147b56，
// 体=push ebp/mov/pop ebp/ret）。本仓 Singleton<T> 主模板无用户构造（隐式平凡，
// g++ 不产生该调用），且 g++4.4 禁止特化隐式声明的特殊成员函数——为不污染
// 共享模板（Arad_DataManager 等 ORIG 无对应实例符号），此处接受该单条
// 基构造调用的指令形态差异，语义完全一致（空操作）。

// [round3-L30 修] ORIG 0x08146d30：基类构造后**无** `mov 0x8(%ebp),%eax;
//   movl $0x0,(%eax)` 清零（我方显式列 Singleton/ScriptInterface 会插入该 2 条）
//   ⇒ 按本文件 LevelupSupport2ndEventManger 先例，省略基类（默认构造），
//   只留成员初始化（同 gcc 4.4 对「显式基类构造」的插入行为）。
LevelupSupportEventManger::LevelupSupportEventManger()
    : m_script(),
      m_stepIndexMap(),
      m_events()
{
}

LevelupSupportEventManger::~LevelupSupportEventManger()
{
}

// ============ LevelupSupport2ndEventManger C1/D1（报告转写） ============
// 报告：Singleton base → ScriptInterface base → 写 vptr →
// LevelupSupport2ndScript(+0x04)。D1：写 vptr → 析构脚本对象。
LevelupSupport2ndEventManger::LevelupSupport2ndEventManger()
    : m_script()
{
    // 显式列出基类构造会让 gcc 4.4 在 Singleton 构造后插入 movl $0,(%eax)
    // （ORIG 无此清零）；省略基类（默认构造）以匹配 ORIG 字节。
}

LevelupSupport2ndEventManger::~LevelupSupport2ndEventManger()
{
}

// ============ InGameEventScript C1/D1【报告缺失·objdump 转写】 ============
// C1 0x08985864：_ZNSsC1Ev(this)、_ZNSsC1Ev(this+4)，随后对两者 _ZNSs5clearEv。
// D1 0x08110e96：析构 string(this+4)、string(this)；无虚表写入（非多态）。
InGameEventScript::InGameEventScript()
    : m_field00(), m_field04()
{
    m_field00.clear();
    m_field04.clear();
}

InGameEventScript::~InGameEventScript()
{
}

// ============ Action_SendMail::reset【报告缺失·objdump 转写】 ============
// 0x080ec026：map(+0x00)::clear → (+0x18)=0 → byte(+0x1c)=0 → (+0x24)=0 →
// (+0x20)=0 → (+0x28)=0 → byte(+0x2c)=0 → string(+0x30).clear → string(+0x34).clear。
// 忠实保留：不触碰 m_vec38(+0x38)。
void Action_SendMail::reset()
{
    m_jobMap.clear();
    m_field18 = 0;
    m_field1c = 0;
    m_addInfo = 0;
    m_itemIdx = 0;
    m_gold = 0;
    m_coin = 0;          // ORIG 仅清 +0x2c 低字节（movb），字段即按字节建模
    m_field30.clear();
    m_field34.clear();
}

// ============ Action_SendMail::set_item【报告缺失·objdump 转写】 ============
// 0x080ec08c：(+0x20)=arg1、(+0x24)=arg2。仅此两条存储。
void Action_SendMail::set_item(int itemIndex, int addInfo)
{
    m_itemIdx = itemIndex;
    m_addInfo = addInfo;
}

namespace EventClassify
{

// 共同查找路径（两报告一致）：this->m_script1c(+0x1c) 为 LevelUpRewardScript*
// （事件记录类型 0x194 实体专用）；脚本 +0x10 为 map<int,Condition_LevelUp>，
// find 键=job；命中项 value 基址 +5 字节（即 m_filter1）为 growType 过滤。

// ============ check_recv_level（报告转写） ============
// 参数映射（调用点 CEventScriptMng::process_level_up_reward）：
//   level=角色等级、job=职业(map 键)、growType=FirstGrowType、flag=比较模式。
// flag 真：item.m_level <= level 即中；假：== 才中。未命中返回 0。
// 形态说明：ORIG 每次使用都重新经 this->+0x1c 取脚本指针（无局部缓存），
// 本转写按同样语句形状书写以保持 O0 指令序列一致。
bool CLevelUpReward::check_recv_level(int level, int job, int growType, bool flag)   // [b28d-Main 代落·S366 P292] int→bool
{
    std::map<int, Condition_LevelUp>::iterator it =
        static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.find(job);
    if (it != static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.end())
    {
        // value 基址 +5 字节过滤：先零扩展比 0xff（通配），再有符号扩展比 growType。
        if (it->second.m_filter1 == static_cast<unsigned char>(0xff)
            || static_cast<int>(static_cast<signed char>(it->second.m_filter1))
               == growType)
        {
            for (int i = 0; static_cast<int>(it->second.m_items.size()) > i; ++i)
            {
                if (flag)
                {
                    if (it->second.m_items[i].m_level <= level)   // setle 分支
                        return 1;
                }
                else if (it->second.m_items[i].m_level == level)  // sete 分支
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// ============ set_reward_item（报告转写） ============
// 查找/过滤与 check_recv_level 同形；命中 item.m_level == level 时对脚本
// +0x28 的 mail 执行 reset → set_item(itemIndex,itemCount) → 标题/正文 key
// 赋值（mail+0x30/+0x34 ← item+0xc/+0x10），返回 1；否则继续循环，耗尽返回 0。
bool CLevelUpReward::set_reward_item(int level, int job, int growType)   // [b28d-Main 代落·S366 P292] int→bool
{
    std::map<int, Condition_LevelUp>::iterator it =
        static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.find(job);
    if (it != static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.end())
    {
        if (it->second.m_filter1 == static_cast<unsigned char>(0xff)
            || static_cast<int>(static_cast<signed char>(it->second.m_filter1))
               == growType)
        {
            for (int i = 0; static_cast<int>(it->second.m_items.size()) > i; ++i)
            {
                if (it->second.m_items[i].m_level == level)       // sete 分支
                {
                    // ORIG 对 mail 的每步操作都重读 this->+0x1c 并加 +0x28。
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.reset();
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.set_item(
                        it->second.m_items[i].m_itemIndex,
                        it->second.m_items[i].m_itemCount);
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field30 =
                        it->second.m_items[i].m_title;
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field34 =
                        it->second.m_items[i].m_content;
                    return 1;
                }
            }
        }
    }
    return 0;
}

}  // namespace EventClassify

// 以 path.c_str() 调对应 import* 解析器。解析器返回非 1 视为失败：
// 显式调基类 ~InGameEventScript（非虚直调）后 ::operator delete，置空返回 0；
// 未建模类型直接返回 0（ORIG local_24=0 且不清算）。
//
// 形态说明（对照 ORIG 比较树）：本函数语义与 ORIG 完全一致；残余指令形态差为
// GCC 对相邻 case 的合并策略——ORIG 树中 {0x191,0x192}→AFL 与 {0x193}→AddItem
// 编码为「jl 默认 / jle AFL / jmp AddItem」三向节点，本仓 g++4.4 将相邻同类项
// 合并为「sub $0x191 / cmp $1 / ja」无符号区间节点。两编码判定结果等价。
InGameEventScript* InGameEventScriptLoader::importInGameEventScript(
    int recordType, const std::string& path)
{
    InGameEventScript* script = 0;
    bool loaded = true;

    switch (recordType)
    {
    case 0x194:  // 升级奖励（new 0x6c）
        script = new LevelUpRewardScript();
        loaded = importLevelUpRewardScript(
            static_cast<LevelUpRewardScript*>(script), path.c_str());
        break;
    case 0x65:   // 疲劳消耗赠物（new 100）
        script = new UsedFatigueGiveItemScript();
        loaded = importUsedFatigueGiveItemEventScript(
            static_cast<UsedFatigueGiveItemScript*>(script), path.c_str());
        break;
    case 0x56:   // 出勤签到（new 0x78）
        script = new AttendanceScript();
        loaded = importAttendanceScript(
            static_cast<AttendanceScript*>(script), path.c_str());
        break;
    case 0x5c:   // 网吧游戏时长（new 0x18）
        script = new PcBangPlayTimeScript();
        loaded = importPcBangPlayTimeScript(
            static_cast<PcBangPlayTimeScript*>(script), path.c_str());
        break;
    case 0x191:
    case 0x192:
    case 0x19c:  // 账号首次登录 / 每日变体共用 AccountFirstLoginScript（new 0x50）
        script = new AccountFirstLoginScript();
        loaded = importAccountFirstLoginEventScript(
            static_cast<AccountFirstLoginScript*>(script), path.c_str());
        break;
    case 0x193:  // 按条目计数 DB（new 0x24）
        script = new AddItemCountDBScript();
        loaded = importAddItemCountDBEventScript(
            static_cast<AddItemCountDBScript*>(script), path.c_str());
        break;
    case 0x196:  // 交换随机奖励（new 0x68）
        script = new ExchangeRandomItemRewardScript();
        loaded = importExchangeRandomItemRewardScript(
            static_cast<ExchangeRandomItemRewardScript*>(script), path.c_str());
        break;
    case 0x197:  // 头像分解随机奖励（new 100）
        script = new AvatarDisjointRandomRewardScript();
        loaded = importAvatarDisjointRandomRewardScript(
            static_cast<AvatarDisjointRandomRewardScript*>(script), path.c_str());
        break;
    case 0x199:  // 疲劳量（new 0x10）
        script = new FatigueQuantityScript();
        loaded = importFatigueQuantityEventScript(
            static_cast<FatigueQuantityScript*>(script), path.c_str());
        break;
    case 0x19a:  // 购买现金道具奖励（new 0x30）
        script = new PurchaseCashItemBonusScript();
        loaded = importPurchaseCashItemBonusEventScript(
            static_cast<PurchaseCashItemBonusScript*>(script), path.c_str());
        break;
    case 0x19b:  // 养成本体（new 0x34）
        script = new ObjectBringUpScript();
        loaded = importObjectBringUpEventScript(
            static_cast<ObjectBringUpScript*>(script), path.c_str());
        break;
    default:     // 未建模类型：ORIG 保持 ptr=0、ok=1，直接返回 0
        return 0;
    }

    if (!loaded)
    {
        // ORIG 失败路径：显式基类析构（D1 非虚直调）后 operator delete，两次判空。
        if (script != 0)
        {
            script->~InGameEventScript();
            ::operator delete(script);
        }
        script = 0;
    }
    return script;
}
