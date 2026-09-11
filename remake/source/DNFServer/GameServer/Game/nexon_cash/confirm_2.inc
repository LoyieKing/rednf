int NexonCash::confirm(const NXTransaction& tx)
{ return m_impl->confirm(tx.m_type, tx.m_id); }