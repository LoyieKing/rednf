// [b28d-Main·检查点14] ORIG 0x08696ca4：13 条，`movzbl 0x127b(%eax),%eax; jmp`
// 直返无 setne ⇒ 字段/返回同型 bool；调用点 0x0868dbfb `xor $1` 联判。
// 字段已由 Main 在 CUserCharacInfo.h 正式改 bool，TU 局部视图已删除（直读）。
bool CUserCharacInfo::is_growth_power_exp_reward_right() const
{
    return m_selected ? CUR->m_growthPowerExpRewardRight : 0;
}