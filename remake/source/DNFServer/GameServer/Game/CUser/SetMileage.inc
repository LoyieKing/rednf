// ORIG _ZN5CUser10SetMileageEi T 0x086509c6：LogHistory 记录新旧值后写 m_mileage。
void CUser::SetMileage(int mileage)
{
    LogHistory("Mileage %s,%d,%d,%d", "Set", mileage, m_mileage, mileage);
    m_mileage = mileage;
}