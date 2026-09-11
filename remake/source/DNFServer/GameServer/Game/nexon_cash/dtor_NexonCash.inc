// ORIG 0x081ae250：free(m_impl)，非 delete —— 忠实保留（ORIG free@plt）。
NexonCash::~NexonCash()
{
    free(m_impl);
}