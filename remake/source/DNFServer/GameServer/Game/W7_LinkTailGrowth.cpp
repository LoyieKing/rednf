// ============================================================================
// W7_LinkTailGrowth.cpp —— W7G_LinkTail：CUser::decide_growth_power_reward_system
// ORIG T 0x0868d780 _ZN5CUser33decide_power_reward_system（完整 mangled：
//   _ZN5CUser33decide_growth_power_reward_systemEv），报告
//   docs/class_func_reports/CUser/decide_growth_power_reward_system.md 全文转写。
// 报告中"文本级别名"已用 nm 核出真实符号：
//   080da2b8 W CUserCharacInfo::get_charac_level() const
//   0822fb00 W CUserCharacInfo::get_growth_power_reward() const
//   0822fb50 W CUserCharacInfo::set_growth_power_exp_reward_right(bool)
//   080cc19b W G_CDataManager()
//   080cb8e6/8fc/920/958 W InterfacePacketBuf::{clear,put_header,put_byte,finalize}
//   vptr+0x34 槽 = CEventBase::IsEventing(CUser*) const（slot 13）。
// ============================================================================
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"      // CEventBase / CEventManager 权威声明
#include "CDataManager.h"           // G_CDataManager / m_seriaBlessing / m_growthPower
#include "STExpReward.h"
#include "PacketGuard.h"

// [b28d-solo-W7_LinkTailGrowth 判词·DIFF/N104+N7] 语义正确：事件(0x5d)开启→
//   IsEventing 双调用→seria is_possible_lv→set_growth_power_exp_reward_right(true)
//   +breakAway section 判定；breakAway||seria→PacketGuard(0x178,3 byte)+Send；
//   未开启→breakAway 判定→同包(byte 1,1)+set_growth_power_exp_reward_right(true)。
//   本轮修：① 第二 IsEventing+is_possible_lv 两条件改 P226 语句表达式（复现 ORIG
//   +0x8b..+0x9b 两 `je` 同址 0x816 的共享短路物化岛）；② 尾部
//   set_growth_power_exp_reward(get_charac_level()) 拆两份物理独立块（P165，ORIG
//   +0x1ed/+0x32a 各一份）。修后 231/231 指令数全等、帧 0x40 与全部槽号一致、
//   p167 调用点差集 42/42 CLEAR；唯一残差 = 第二分支 PacketGuard::~D1 的相对序
//   rotate（ORIG 正常路径 D1 在 `jmp +0x32a` 之前；4.4 恒把该 D1 挪到 unwind
//   landing 之后，块内容逐条全等）⇒ N104 封顶；另 N7 虚调用目标寄存器
//   %eax↔%edx（ORIG %edx；P230=4.4.6 后端可修，路由表不动）。
void CUser::decide_growth_power_reward_system()
{
    bool breakAwayFlag = false;                     // -0xe（local_12）
    bool seriaFlag = false;                         // -0xd（local_11）

    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (ev->IsEventing(0))                          // vptr+0x34 @+0x20
    {
        // ORIG +0x4a..+0x9d：第二 IsEventing 与 is_possible_lv 两条件**共享**同一
        // bool 物化岛（`je 0x816` 同址 + mov $1/jmp/mov $0/test;je，值无槽往返）
        // ⇒ P226 扩展：语句表达式包住整个 `&&` 条件（嵌套 if 被 4.4 jump-thread 折掉）。
        if (({ ev->IsEventing(0) &&                 // vptr+0x34 @+0x4a（ORIG 双调用）
               G_CDataManager()->m_seriaBlessing.is_possible_lv(get_charac_level()); }))
        {
            // +0x6e..+0xa1：stSeriaBlessingScript::is_possible_lv(level)，
            // m_seriaBlessing 位于 CDataManager +0x5094。
            seriaFlag = true;
            set_growth_power_exp_reward_right(true);   // +0xb1
        }

        // +0xbc..+0xf5：breakAwaySection = STGrowthPowerData(+0x8710)
        //   .get_break_away_section_data(get_charac_level())；
        //   get_growth_power_reward() < section.m_field1 → breakAwayFlag=1。
        STBreakAwaySection section =
            G_CDataManager()->m_growthPower.get_break_away_section_data(
                get_charac_level());
        if ((unsigned char)get_growth_power_reward() <
            (unsigned char)section.m_field1)
        {
            breakAwayFlag = true;
        }

        if (breakAwayFlag || seriaFlag)             // +0x100..+0x10a
        {
            PacketGuard packet;
            packet.clear();                                    // +0x121
            packet.put_header(0, 0x178);                       // +0x13c
            packet.put_byte((int)(signed char)get_charac_level());  // +0x159 movsbl
            packet.put_byte(breakAwayFlag ? 1 : 0);            // +0x17a
            packet.put_byte(seriaFlag ? 1 : 0);                // +0x19b
            packet.finalize(true);                             // +0x1ae
            Send(packet);                                      // +0x1c0
        }

        // ORIG +0x1ed 尾部第一份（独立物理块，P165 两份不共享；+0x202 jmp 到 epilogue）
        set_growth_power_exp_reward(get_charac_level());
    }
    else                                            // +0x20c 分支（事件未开启）
    {
        // +0x232..+0x245：同上取 break away section 并比较。
        STBreakAwaySection section =
            G_CDataManager()->m_growthPower.get_break_away_section_data(
                get_charac_level());
        if ((unsigned char)get_growth_power_reward() <
            (unsigned char)section.m_field1)                    // setb @+0x24b
        {
            PacketGuard packet;
            packet.clear();                                     // +0x267
            packet.put_header(0, 0x178);                        // +0x282
            packet.put_byte((int)(signed char)get_charac_level());  // +0x29f
            packet.put_byte(1);                                 // +0x2b2 固定 1
            packet.put_byte(1);                                 // +0x2c2 固定 1
            packet.finalize(true);                              // +0x2d8
            Send(packet);                                       // +0x2ea
            set_growth_power_exp_reward_right(true);            // +0x2fd
        }

        // ORIG +0x32a 尾部第二份（fall-through 到 epilogue）
        set_growth_power_exp_reward(get_charac_level());
    }
}

// ============================================================================
// set_growth_power_exp_reward 的依赖 producer（此前 repo 无 owner；
// 引用方 decide_growth_power_reward_system 强依赖，随本批一并落地）。
// ============================================================================

// stSeriaBlessingScript::get_value(int, Value&) const
// ORIG W 0x8693ad6：FindOneByLv(lv)（C1 0x822aebe / operator() 0x822aecc）
// 谓词 std::find_if 遍历 m_map0；谓词条件 = (m_minLv==m_maxLv && m_minLv==lv)
// 或 (m_minLv<=lv && lv<m_maxLv)。命中把 Value 五 int 拷入 out 返回 true。
// ============================================================================
bool stSeriaBlessingScript::get_value(int lv, Value& out)
{
    // [b28d-solo-W7_LinkTailGrowth 判词·NEAR] ORIG 0x08693ad6（62/62）：pred/end/begin
    // 三对象的槽号与构造序不可兼得——ORIG pred@-0x18、end@-0x14、begin@-0x10 且构造序
    // pred→end→begin（4.1 先声明深）；4.4 恒先声明浅，故按 ORIG 构造序声明
    // （pred、itEnd、it）后 pred@-0x10/end@-0x14/begin@-0x18，仅 pred↔begin 槽号互换。
    // N93/N98 封顶：「先声明后赋值」形引入迭代器 C1Ev 默认构造 call + 临时槽拷贝
    // （72 insn，P50/N86），已实测回退；声明序再换会翻构造序（call 块序）反劣。
    FindOneByLv pred(lv);                                   // ORIG 构造序第 1（+0x13）
    std::map<int, Value>::iterator itEnd = m_map0.end();    // ORIG 构造序第 2（+0x25）
    std::map<int, Value>::iterator it = m_map0.begin();     // ORIG 构造序第 3（+0x3a）
    std::map<int, Value>::iterator found = std::find_if(it, itEnd, pred);  // ORIG：返回值独立变量
    if (found == m_map0.end())   // ORIG：if 条件中再次调用 end()
        return false;
    // P167 新判据：ORIG 发 `_ZNK…deEv`（operator*）而非 `ptEv`（operator->）
    // ⇒ 源形必须写 `(*it).second`；`it->second` 会多一条 EXTRA ptEv 实例化。
    out = (*found).second;       // ORIG：Value 整体赋值（五 int 逐字段展开）
    return true;
}

// ============================================================================
// STGrowthPowerData::get_idx_break_away_section(int)（ORIG 非 const，mangled 无 K）
// ORIG W 0x822b136：遍历 m_vec0，首个满足 (unsigned char)m_field1 <= lv 且
// lv < (unsigned char)m_field2 的下标；未命中返回 -1。
// [b28d-solo-W7_LinkTailGrowth 判词·AE] ORIG 0x0822b136（52/52）：加具名
// `const STBreakAwaySection& sec = *it;`（ORIG de() 每轮一次落 -0xc 槽、两字段从槽重读，
// P0 槽往返 ⇒ 原码有具名局部）＋ 把 `&&` 拆成嵌套 if（ORIG 两守卫 `jg`/`jle` 同落
// 循环增量块，P133/P225 推广；合并形被 4.4 物化成 setcc 双物化）后与 ORIG 全等；
// p167 调用点差集 CLEAR。
// ============================================================================
int STGrowthPowerData::get_idx_break_away_section(int lv)
{
    int idx = 0;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it, ++idx)
    {
        // ORIG 0x0822b136：de() 每轮一次并把结果落 -0xc 具名槽，两字段从该槽重读
        // （P0 槽往返 ⇒ 原码确有具名引用局部）；两条件为**独立守卫**共享同一
        // continue 落点（`jg`/`jle` 同址 0x822b19d），合并 `&&` 会被 4.4 物化成
        // setcc 双物化（P133/P225 推广）⇒ 拆嵌套 if。
        const STBreakAwaySection& sec = *it;
        if ((unsigned char)sec.m_field1 <= lv)
        {
            if (lv < (unsigned char)sec.m_field2)
                return idx;
        }
    }
    return -1;
}

// ============================================================================
// CUser::set_growth_power_exp_reward(int)
// ORIG T 0x0868dace _ZN5CUser27set_growth_power_exp_rewardEi：
// [b28d-solo-W7_LinkTailGrowth 判词·NEAR] ORIG 0x0868dace（111/111 指令数全等，
// p167 调用点差集 14/14 CLEAR）：① `&&` 条件改 P226 语句表达式，复现 ORIG
// +0x6d..+0x8f 的短路物化岛（mov $1/jmp/mov $0/test;je，值无槽往返）；
// ② Value→m_reward 的字节写改 STExpReward union 具名视图（§4.3，删裸 char* 指针视图）；
// ③ 下标比较改有符号 `idx < (int)sz`（ORIG +0xf3 cmpl/jge，无符号版产 jae=P196 隐患）。
// 残差：N7 虚调用目标寄存器 %eax↔%edx（ORIG %edx；P230 判据=4.4.6 后端可翻，
// 路由表不改，故 4.4.7 封顶）＋ 尾部对齐 nop（§6.1 豁免）＋ `m_reward.m_reward=`
// 段 4.4 RHS-first vs ORIG LHS-first（P211 档③ packed 字段侧旋钮；STExpReward.h
// 换型会波及全消费点，按边界封顶）。callee is_growth_power_exp_reward_right
// 返回类型 unsigned char→bool 由 Main 代落（P248 档①，ORIG 调用点 xor $1 实证），
// 两处 xor 已归位。
//   ev = s_event_manager->GetRepeatEvent(0x5d);
//   if (ev->IsEventing(0))：                                  // +0x09..+0xb0
//     Value value;                                            // ctor 0x8693aa0
//     if (m_seriaBlessing.get_value(level,value) &&
//         !is_growth_power_exp_reward_right())
//       写 m_expReward 字节 [+2]=value.m_fieldC、[+3]=value.m_field10、
//                          [+1]=value.m_field8                // 0x8d24f/50/4e
//     else memset(&m_reward, 0, 4);                        // 0x8d24d
//   else：                                                    // +0xe7..+0x15f
//     idx = m_growthPower.get_idx_break_away_section(level);
//     sz = m_vecC.size();                                     // dm+0x871c
//     if (idx >= 0 && idx < sz && !is_growth_power_exp_reward_right())
//       m_reward = m_vecC[idx].m_reward;          // 0x8d24d int 写
//     else memset(&m_reward, 0, 4);
// ============================================================================
void CUser::set_growth_power_exp_reward(int level)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (ev->IsEventing(0))
    {
        stSeriaBlessingScript::Value value;               // +0x15 清零 ctor
        // ORIG +0x6d..+0x8f：`&&` 短路物化岛（mov $1/jmp/mov $0/test;je，值无槽往返）
        // ⇒ P226 扩展「语句表达式包住整个条件」（具名 bool 反产槽化+物化劣化）。
        if (({ G_CDataManager()->m_seriaBlessing.get_value(level, value) &&
               !is_growth_power_exp_reward_right(); }))
        {
            // ORIG +0x93..+0xbd：字节 store 序 +2(m_fieldC)→+3(m_field10)→+1(m_field8)，
            // 目标 char 与 RHS int 异型 ⇒ RHS-first+edx 中转（P211 档③）；用 STExpReward
            // 的 union 字节视图具名访问（§4.3，禁裸偏移指针视图）。
            m_reward.m_field5 = static_cast<char>(value.m_fieldC);    // +0x8d24f
            m_reward.m_powerUp = static_cast<char>(value.m_field10);  // +0x8d250
            m_reward.m_field4 = static_cast<char>(value.m_field8);    // +0x8d24e
        }
        else
        {
            memset(&m_reward, 0, 4);                       // +0xc2 分支
        }
    }
    else                                                      // +0xbb5 分支
    {
        // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（+0xca/+0xdf/+0x135
        // 共 3 处，加 if 分支 m_seriaBlessing 1 处 = 4 次调用）。
        const int idx = G_CDataManager()->m_growthPower.get_idx_break_away_section(level); // +0xca
        const int sz = G_CDataManager()->m_growthPower.m_vecC.size();  // +0xdf
        if (idx >= 0 && idx < sz &&                            // +0xeb cmpl/js、+0xf3 cmpl/jge（有符号）
            !is_growth_power_exp_reward_right())             // +0xfb
        {
            m_reward.m_reward = G_CDataManager()->m_growthPower.m_vecC[idx].m_reward;  // +0x135
        }
        else
        {
            memset(&m_reward, 0, 4);                      // +0x16f 分支
        }
    }
}
