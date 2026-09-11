int NexonCash::gift(unsigned long long sn, int a, int b, const char* c,
                    const char* d, const char* e, const char* f,
                    NXTransaction& tx)
{ return m_impl->gift(sn, a, b, c, d, e, f, tx); }