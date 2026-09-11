// ORIG 0x0822fc06 W _ZN5CUser29SetLoadRentalInfoFromExchangeEb：写 +0x6ef91 字节
void CUser::SetLoadRentalInfoFromExchange(bool flag)

{
    // ORIG 0x0822fc06：movzbl 参数后直接按字节存 +0x6ef91（无 0/1 归一化）。
    m_rentalFlag2 = flag;                      // +0x6ef91（m_rentalFlag2）
}
