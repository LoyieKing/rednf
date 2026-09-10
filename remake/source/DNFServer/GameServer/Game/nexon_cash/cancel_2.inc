int NexonCash::cancel(const NXTransaction& tx)
{ return m_impl->cancel(tx.m_type, tx.m_id); }
