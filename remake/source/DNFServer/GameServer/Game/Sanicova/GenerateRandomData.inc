void CPad::GenerateRandomData()
{
    if (isValidKeyGenTime())
    {
        GenerateRandomKeyIdx();
        GenerateRandomShakeData();
    }
