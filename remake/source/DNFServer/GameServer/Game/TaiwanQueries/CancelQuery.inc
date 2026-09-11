// ORIG T 0x08174ce2（C2 同址别名）；ORIG 尾部同样清零 m_resultCode（+0x00）。
CancelQuery::CancelQuery(const BillTransaction& trans)
{
    // [b27L8] ORIG 0x08174ce2：trans 12B 整体赋值（dest 缓存三连 mov）在前、
    // m_resultCode=0 在后；memcpy 形出 call 且 m_resultCode(0) 列表形提前。
    m_trans = trans;
    m_resultCode = 0;
}