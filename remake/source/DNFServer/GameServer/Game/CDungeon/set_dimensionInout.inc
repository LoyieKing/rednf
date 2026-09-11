// ORIG _ZN12CDataManager18set_dimensionInoutEic / _ZN12CDataManager21set_limit_inout_countEc15ENUM_BLOOD_TYPE
// 真实 C++ 成员实现（权威声明在 CDataManager.h），语义按 ORIG 反汇编：
//   set_dimensionInout（080eed82）：idx>5 或 idx<0 早退，再写数组
//   set_limit_inout_count（08374c82）：写 m_limitInoutCount[type]
void CDataManager::set_dimensionInout(int idx, char value)
{
    if (idx > 5)
        return;
    if (idx < 0)
        return;
    m_dimensionInout[idx] = value;
}