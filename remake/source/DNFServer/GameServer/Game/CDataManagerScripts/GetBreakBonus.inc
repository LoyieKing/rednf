int PowerParameterScript::GetBreakBonus(int winCount)
{
    // ORIG 0x8a59d5e：m_vecC8 不物化引用（每处 call 现场 `add $0xc8,%edx`）；
    // size→-0xc；0x8a59d77 `cmp -0xc,%eax; jl 0x8a59d98` ⇒ winCount>=size 为
    // fall-through 首分支（ix(size-1) @0x8a59d8f），jl 目标=ix(winCount)。
    int size = m_vecC8.size();
    if (winCount >= size)
        return m_vecC8[size - 1];
    return m_vecC8[winCount];
