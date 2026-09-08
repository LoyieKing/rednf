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

// [B24D判词 N] 语义正确：事件(0x5d)开启→IsEventing 双调用→seria is_possible_lv→
//   set_growth_power_exp_reward_right(true)+breakAway section 判定；breakAway||seria
//   →PacketGuard(0x178,3 byte)+Send；未开启→breakAway 判定→同包(byte 1,1)+
//   set_growth_power_exp_reward_right(true)。尾部两路径汇合均
//   set_growth_power_exp_reward(get_charac_level())。差异=O0 vptr 取表/寄存器分配/
//   jmp 布局 vs ORIG，纯 lowering 表示差异，语义等价。
void CUser::decide_growth_power_reward_system()
{
    bool breakAwayFlag = false;                     // -0xe（local_12）
    bool seriaFlag = false;                         // -0xd（local_11）

    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (ev->IsEventing(0))                          // vptr+0x34 @+0x20
    {
        if (ev->IsEventing(0))                      // vptr+0x34 @+0x4a（ORIG 双调用）
        {
            // +0x6e..+0xa1：stSeriaBlessingScript::is_possible_lv(level)，
            // m_seriaBlessing 位于 CDataManager +0x5094。
            if (G_CDataManager()->m_seriaBlessing.is_possible_lv(get_charac_level()))
            {
                seriaFlag = true;
                set_growth_power_exp_reward_right(true);   // +0xb1
            }
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
    }

    // 尾部（两分支汇合）：set_growth_power_exp_reward(get_charac_level())
    set_growth_power_exp_reward(get_charac_level());         // +0x202/+0x33f
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
    // [b28c-30] N93 封顶：ORIG pred@-0x18/end@-0x14/begin@-0x10（4.1 先声明深），
    // 4.4 恒先声明浅（pred@-0x10/begin@-0x18）。初始化式绑定声明序 ⇒ 槽序与
    // 构造序（pred→end→begin）不可兼得；「先声明后赋值」形引入迭代器 C1Ev
    // 默认构造 call + 临时槽拷贝（72 insn，P50/N86），已实测回退。
    FindOneByLv pred(lv);                     // ORIG：谓词在入口即构造
    std::map<int, Value>::iterator it = m_map0.begin();
    std::map<int, Value>::iterator itEnd = m_map0.end();
    std::map<int, Value>::iterator found = std::find_if(it, itEnd, pred);  // ORIG：返回值独立变量
    if (found == m_map0.end())   // ORIG：if 条件中再次调用 end()
        return false;
    out = found->second;         // ORIG：Value 整体赋值（五 int 逐字段展开）
    return true;
}

// ============================================================================
// STGrowthPowerData::get_idx_break_away_section(int) const
// ORIG W 0x822b136：遍历 m_vec0，首个满足 (unsigned char)m_field1 <= lv 且
// lv < (unsigned char)m_field2 的下标；未命中返回 -1。
// ============================================================================
int STGrowthPowerData::get_idx_break_away_section(int lv)
{
    int idx = 0;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it, ++idx)
    {
        if ((unsigned char)it->m_field1 <= lv &&
            lv < (unsigned char)it->m_field2)
        {
            return idx;
        }
    }
    return -1;
}

// ============================================================================
// CUser::set_growth_power_exp_reward(int)
// ORIG T 0x0868dace _ZN5CUser27set_growth_power_exp_rewardEi：
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
        if (G_CDataManager()->m_seriaBlessing.get_value(level, value) &&   // +0x36
            !is_growth_power_exp_reward_right())                            // +0x45
        {
            char* pBytes = reinterpret_cast<char*>(&m_reward);  // +0x8d24d
            pBytes[2] = static_cast<char>(value.m_fieldC);    // +0x69 → 0x8d24f
            pBytes[3] = static_cast<char>(value.m_field10);   // +0x77 → 0x8d250
            pBytes[1] = static_cast<char>(value.m_field8);    // +0x85 → 0x8d24e
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
        const std::size_t sz = G_CDataManager()->m_growthPower.m_vecC.size();  // +0xdf
        if (idx >= 0 && static_cast<std::size_t>(idx) < sz &&                  // +0xeb/+0xf3
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
