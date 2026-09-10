void CUser::AddMileage(int amount, eCoinAddReason reason)
{
    LogHistory("Mileage %s,%d,%d,%d,%d", "Add", amount, m_mileage,
               m_mileage + amount, reason);
    m_mileage += amount;
    SaveMileage();
