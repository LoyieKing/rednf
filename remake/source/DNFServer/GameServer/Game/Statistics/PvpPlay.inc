void Statistics::PvpPlay(unsigned int a, unsigned int b, int c, int d)
{
    unsigned int localA;
    unsigned int localB;
    int localC;
    int localD;

    localA = a;
    localB = b;
    localC = c;
    localD = d;
    SendSignal(0x3a, &localA, 0x10);
