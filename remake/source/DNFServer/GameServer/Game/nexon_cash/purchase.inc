int NexonCash::purchase(unsigned long long sn, int a, int b,
                        const char* c, const char* d, NXTransaction& tx)
{ return m_impl->purchase(sn, a, b, c, d, tx); }