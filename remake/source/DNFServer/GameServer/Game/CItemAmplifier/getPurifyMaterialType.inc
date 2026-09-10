int CItemAmplifier::getPurifyMaterialType(int itemIdx) const
{
    // [b28d-34] AE（22=22）：字段 unsigned int + 表值在左翻正比较段（P274/P211）。
    for (int i = 0; i <= 1; ++i)
    {
        // [b28d-34] 字段 unsigned + 表值在左 ⇒ ORIG 两段物化（`mov 表项,%edx;
        // mov 形参,%eax; cmp %eax,%edx`）；探针 /tmp/p34e.cpp f4 逐条命中。
        if (m_script.m_purifyMatItem[i] == itemIdx)  // +0x9c 表查找
            return i;
    }
    return -1;
