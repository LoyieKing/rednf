int Inter_LoadMileage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_MILEAGE_STRUCT* mil = reinterpret_cast<SIG_MILEAGE_STRUCT*>(data);
    // ORIG +0x0b：`mov -0x10(%ebp),%eax`+`mov 0x8(%eax),%eax` 单级带偏移读 ⇒ 无中间 int* 槽。
    int mileage = mil->m_mileage;
    user->SetMileage(mileage);
    user->SendCashData(false);
    return 0;
}