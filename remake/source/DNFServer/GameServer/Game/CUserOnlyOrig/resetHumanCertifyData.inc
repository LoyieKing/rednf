// ----------------------------------------------------------------------------
// ORIG 0x0867f720 CUser::resetHumanCertifyData() void
void CUser::resetHumanCertifyData()
{
    memset(m_capchaCertify, 0, 5);
}