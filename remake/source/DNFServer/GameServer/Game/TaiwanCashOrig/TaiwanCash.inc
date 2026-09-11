// ---- TaiwanCash::TaiwanCash —— ORIG 0x08173608：MySQL@+8 自动构造、Mutex@+0x42094
// 自动构造，随后 +4=0、+0=type、+2=field2。----
TaiwanCash::TaiwanCash(unsigned char type, short field2)
    : m_mysql(), m_mutex()
{
    m_requestCount = 0;
    m_type = type;
    m_field2 = field2;
}