// ----------------------------------------------------------------------------
// ORIG 0x08647ffc CUser::Recv() uint  (报告 CUser/Recv.md)
bool CUser::Recv()
{
    int n = m_network.recv_packet();
    if (n >= 1) {
        if (!Parsing(n)) {
            DisConnSig(DISCONN_SIG_4, false, 0);
            return 0;
        }
        return 1;
    }
    if (n < 0) {
        DisConnSig((DISCONN_SIG)5, false, 0);
        return 0;
    }
    return 1;
}