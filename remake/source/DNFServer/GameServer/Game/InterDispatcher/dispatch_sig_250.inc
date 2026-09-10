int Inter_LoadMileage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadMileagePktView* pPkt = reinterpret_cast<Inter_LoadMileagePktView*>(data);
    // ORIG +0x0b：`mov -0x10(%ebp),%eax`+`mov 0x8(%eax),%eax` 单级带偏移读 ⇒ 无中间 int* 槽。
    int mileage = pPkt->m_8;
    user->SetMileage(mileage);
    user->SendCashData(false);
    return 0;
