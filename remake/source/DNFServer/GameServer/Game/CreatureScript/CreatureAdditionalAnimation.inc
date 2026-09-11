// ---- OnlyOrigBatch3：CreatureAdditionalAnimation 3参构造（ORIG W 0x08a3dcc2）----
// ORIG：m_name 默认构造（_ZNSsC1Ev）**先于**标量 store（+0x0/+0x4），再赋值
// （_ZNSsaSEPKc）。4.1 发射序=meminit（按声明序）→ ctor 体；把 string 写进初始化
// 列表、标量移入 body 即复现（P239 ctor 标量段拆入 body）。
CreatureAdditionalAnimation::CreatureAdditionalAnimation(int a, int b, const char* name)
    : m_name()
{
    m_field0 = a;
    m_field4 = b;
    m_name = name;
}