// ORIG 0x084ebe2e（_ZN14CPowerManager16SetPowerWarPointEii，W）：
//   *(int*)(this+0x04) = arg1; *(int*)(this+0x08) = arg2; ret。
void CPowerManager::SetPowerWarPoint(int sideIdx, int point)
{
    m_side1Point = sideIdx;
    m_side2Point = point;
}