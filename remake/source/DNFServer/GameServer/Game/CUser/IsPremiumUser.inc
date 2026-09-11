// ORIG T 0x0864fc3e CheckLimitMiniumAge：唯一 owner 为 CUserOnlyOrig.cpp
// （W7_BuildHarmonize 去重：该版含 outUnique=unique_id 低16位写出，更忠实）。
bool CUser::IsPremiumUser() const
{
    return m_premium.GetTotalPremiumCount() != 0;
}