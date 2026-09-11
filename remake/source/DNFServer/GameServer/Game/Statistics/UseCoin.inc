void Statistics::UseCoin(unsigned int a, unsigned int b, int c)
{
    unsigned int localA;
    unsigned int localB;
    int localC;

    localA = a;
    localB = b;
    localC = c;
    SendSignal(0x39, &localA, 0xc);
}