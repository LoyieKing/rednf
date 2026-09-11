void Statistics::BreakJar(unsigned int a, unsigned int b, int c)
{
    unsigned int localA;
    unsigned int localB;
    int localC;

    localA = a;
    localB = b;
    localC = c;
    SendSignal(0x37, &localA, 0xc);
}