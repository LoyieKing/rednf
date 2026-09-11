// ORIG T _ZN5CUser10AddMileageEi14eCoinAddReason @0x08650954（报告 AddMileage.md）：
// LogHistory("Mileage %s,%d,%d,%d,%d","Add",amount,m_mileage,m_mileage+amount,reason)
// → m_mileage(+0x796d4) += amount → SaveMileage()。
void CUser::AddMileage(int amount, eCoinAddReason reason)
{
    LogHistory("Mileage %s,%d,%d,%d,%d", "Add", amount, m_mileage,
               m_mileage + amount, reason);
    m_mileage += amount;
    SaveMileage();
}