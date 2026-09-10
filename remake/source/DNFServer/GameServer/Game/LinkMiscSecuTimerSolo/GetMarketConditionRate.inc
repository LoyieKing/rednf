void GetMarketConditionRate(double rate, double amount, double* arr,
                            int nCount, int& outIdx, double& outRate)
{
    outIdx = 0;
    outRate = 1.0;
    for (outIdx = 0; outIdx <= 4; outIdx = outIdx + 1)
    {
        if (arr[outIdx] * rate > amount)
            break;
    }
    if (nCount + 1 < outIdx)
    {
        outIdx = nCount + 1;
    }
    if (outIdx != 0)
    {
        int idx = outIdx + 4;
        outRate = arr[idx];
    }
