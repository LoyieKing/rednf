// ============================================================================
// CraneMinigameScript C1/D1 —— ORIG 0x088e3736 / 0x088e37a8（外联定义，
// 产独立符号；头内不再内联）。
// C1：空体，仅按声明序默认构造 m_items(+0x04) → m_sendMail(+0x18) →
// m_needMaterials(+0x5c)；m_viewCnt/m_material/m_field14 故意不初始化（ORIG 无 store）。
// D1：begin/end 迭代器（声明序 it 先、end 后 = ORIG 栈槽 -0xc/-0x10），
// 循环内 O0 三次独立 operator*（判空 / delete / 置 0，P48 无 CSE），
// clear 后编译器按声明逆序析构 m_needMaterials → m_sendMail → m_items（ORIG 同序）。
// ============================================================================
CraneMinigameScript::CraneMinigameScript()
{
}