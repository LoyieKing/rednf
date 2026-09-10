CCreatureScript::CCreatureScript()
    : m_vec8(), m_field24(0), m_field28(0), m_field2c(0), m_set34(), m_vec68()
{
    // [b28d-34] 按 ORIG 0x833c77e 逐条核减：ORIG 只初始化 +0x24/+0x28/+0x2c
    // （列表）与 +0x20（体），其余标量（+0/+4/+0x14/+0x18/+0x1c/+0x4c..+0x64）
    // 不写；旧列表多 33 条 store（100 vs 67）。
    m_vec8.clear();
    m_artifactSlotFlags = 0;
    m_vec68.clear();
