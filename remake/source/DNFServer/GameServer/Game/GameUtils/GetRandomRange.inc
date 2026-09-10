int GetRandomRange(int a, int b)
{
    if (b < a)
        return (int)((rand() % (a - b + 1)) + b);
    return (int)((rand() % (b - a + 1)) + a);
