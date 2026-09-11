void* random_option::GetRandomOption()
{
    // ORIG T 0x085f0eb0：返回 &m_randomOption（CDataManager+0x4e10，objdump 实证）。
    return &G_CDataManager()->m_randomOption;
}