// [b28d-solo-DB_Login] verdict=AE（P211 档③同型化：PktView m_accId→unsigned int、
// m_gcNo→int 使 RHS 直装 %edx 无 eax 中转，ndiff 4→0；P259 消 stream 具名局部）。
int DB_Login::UpdateLoginStat(int userDbIdx, SIG_LOGIN_DATA* sig)
