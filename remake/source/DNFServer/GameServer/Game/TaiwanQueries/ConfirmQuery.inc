ConfirmQuery::ConfirmQuery(const BillTransaction& trans)
{
    // [b27L8] ORIG 0x08174d08：trans 整体赋值在前、m_resultCode=0 在后
    // （同 CancelQuery 0x08174ce2 形）。
    m_trans = trans;
    m_resultCode = 0;
