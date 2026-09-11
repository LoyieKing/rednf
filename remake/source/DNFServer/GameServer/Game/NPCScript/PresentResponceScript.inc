// ----------------------------------------------------------------------------
// PresentResponceScript 族 —— ORIG [favor_dialog] 三结构生命周期：
//   Script    C1 0x8a00aba / clear 0x8a00a86（m_a/m_b/m_c=0 + m_str.clear()）
//   Exception C1 0x8a00b22 / clear 0x8a00b02（m_a=0 + m_str.clear()）
//   Event     C1 0x8a00b8a / clear 0x8a00b6a（m_a=0 + m_str.clear()）
// ----------------------------------------------------------------------------
PresentResponceScript::PresentResponceScript() : m_str()
{
    clear();
}