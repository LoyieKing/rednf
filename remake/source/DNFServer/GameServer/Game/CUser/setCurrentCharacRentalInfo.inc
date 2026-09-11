// ORIG T 0x08677516：`m_rentalInfo[param][charac_no] = in`（vector 拷贝）。
void CUser::setCurrentCharacRentalInfo(
    std::vector<InstanceRentalSystem::RentalInfo>& in, int param)
{
    // ORIG 0x8677516：cur 落 -0xc 槽判空（mov %eax,-0xc; cmpl $0,-0xc; je）；
    // characNo = 直读 cur->m_characNo（`mov (%eax),%eax` @0x867752x，非
    // getCurCharacNo 调用——我们旧形多一次 call 且判空被折 test;setne）。
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0)
        return;
    unsigned int characNo = (unsigned int)cur->m_characNo;
    if (characNo == 0)
        return;
    m_rentalInfo[(unsigned int)param][characNo] = in;
}