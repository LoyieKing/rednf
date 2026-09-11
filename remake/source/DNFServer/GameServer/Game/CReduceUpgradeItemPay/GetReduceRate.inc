// ---------------------------------------------------------------------------
// ORIG W 0x0854b56e：movzwl 0x4(%eax),%eax; cwtl; ret —— +0x04 ushort 经
// 符号扩展返回 int（P11：movzwl 读 + cwtl 符号扩展 ⇔ (short) cast）。
int CReduceUpgradeItemPay::GetReduceRate()
{
    return (short)m_addInfo.m_info0;
}