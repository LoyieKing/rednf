int finishItemChecksum(int* sum, int money, int coin, int a, int b)
{
    if (sum == 0)
    {
        return 0;
    }
    *sum = *sum + (money + coin + a);
    *sum = *sum ^ b;
    return *sum;
