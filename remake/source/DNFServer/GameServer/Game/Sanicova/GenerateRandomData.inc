// ORIG T 0x085998ea：key 生成时间有效时刷新 keyIdx 与扰动数据。
void CPad::GenerateRandomData()
{
    if (isValidKeyGenTime())
    {
        GenerateRandomKeyIdx();
        GenerateRandomShakeData();
    }
}