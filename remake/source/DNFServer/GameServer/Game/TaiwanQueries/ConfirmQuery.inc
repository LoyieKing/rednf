// ---- ConfirmQuery ---------------------------------------------------------
// ORIG T 0x08174ac6（C2 同址别名）：trans 前 12 字节逐 dword 拷贝进 m_trans；
// ORIG 尾部 movl $0x0,(%eax) 清零 m_resultCode（+0x00）。
ConfirmQuery::ConfirmQuery(const BillTransaction& trans)
{
    // [b27L8] ORIG 0x08174d08：trans 整体赋值在前、m_resultCode=0 在后
    // （同 CancelQuery 0x08174ce2 形）。
    m_trans = trans;
    m_resultCode = 0;
}