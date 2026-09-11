// ---- 0x0869729a incPlayExpAdd：m_gameOption.m_playExpAdd += v（ORIG W 内联）----
void CUser::incPlayExpAdd(int v)
{
    // ORIG 0x86972a0-0x86972a9：mov field,%edx; mov 0xc(%ebp),%eax; add %eax,%edx（不折叠 add MEM）。
    // 4.4 字段载入恒经 %eax 中转，单段直落 %edx 不可造（N18a/N96；register 绑定全域禁用）。
    m_gameOption.m_playExpAdd = m_gameOption.m_playExpAdd + v;
}