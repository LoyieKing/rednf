// ============ Action_SendMail C1/D1【报告缺失·objdump 转写】 ============
// C1 0x088e40ce：_ZNSt3mapIiSt4pairIiiESt4lessIiESaIS0_IKiS1_EEEC1Ev(this) →
//   _ZNSsC1Ev(this+0x30) → _ZNSsC1Ev(this+0x34) → _ZNSt6vectorIiSaIiEEC1Ev(this+0x38)。
//   忠实保留：+0x18..+0x2f 标量区（m_field18/m_field1c/m_itemIdx/m_addInfo/
//   m_gold/m_coin/pad）不初始化——由 reset()（0x080ec026）或各 import* 解析器写入。
// D1 0x088e4160：逆序析构 vector(+0x38) → string(+0x34) → string(+0x30) → map(+0x00)。
Action_SendMail::Action_SendMail()
    : m_jobMap(),
      m_field30(),
      m_field34(),
      m_vec38()
{
}