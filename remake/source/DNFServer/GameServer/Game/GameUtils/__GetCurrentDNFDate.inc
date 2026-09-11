// ---- __GetCurrentDNFDate（ORIG _Z19__GetCurrentDNFDateR2tm，0x86b1fc0，0x5f 字节）----
// ORIG：cur = getCurSec()；若 localtime 后 tm_hour < CEnvironment+0x37c
// （m_serverEnvir.m_fatigueTime，即每日调度小时）则回退一天再 localtime。
void __GetCurrentDNFDate(tm& out)
