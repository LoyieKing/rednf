// ---- reseal_table_t（含 vector + 2×int[6]；ORIG 0x8370742）----
reseal_table_t::reseal_table_t()
{
    for (int i = 0; i <= 5; ++i)
        m_arr1[i] = 0;
    for (int i = 0; i <= 5; ++i)
        m_arr2[i] = 0;
}