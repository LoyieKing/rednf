// ============ Action_SendMail::reset【报告缺失·objdump 转写】 ============
// 0x080ec026：map(+0x00)::clear → (+0x18)=0 → byte(+0x1c)=0 → (+0x24)=0 →
// (+0x20)=0 → (+0x28)=0 → byte(+0x2c)=0 → string(+0x30).clear → string(+0x34).clear。
// 忠实保留：不触碰 m_vec38(+0x38)。
void Action_SendMail::reset()
