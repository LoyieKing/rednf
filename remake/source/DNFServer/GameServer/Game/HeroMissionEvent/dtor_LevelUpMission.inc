LevelUpMission::~LevelUpMission() {}
// ============================================================================
// LevelUpMission::processMission —— ORIG T 0x08165c1e（processMission.md 全文）。
// 升级任务：m_extra[0] 为等级上限，field8 记录上次等级；达到新等级时遍历
// m_rewards 逐项构造 Inven_Item 并以系统邮件发放，随后 fieldA++/field6=1、
// field8 记当前等级，重复任务 reset。
// ============================================================================
