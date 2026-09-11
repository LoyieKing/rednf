// ORIG 0x889037c：构造 string/set/向量 后调 clear()。
// ORIG 0x0889037c：只构造成员容器（string/set/vector<int>[3]/vector<CatchObjectInfo>）
// 后调 clear()，标量值全部由 clear() 设定（ctor 不预置 0）。
AttackInfo::AttackInfo()
    : m_hitWav(), m_elements(), m_activeStatusVectors(), m_catchObjects()
{
    clear();
}