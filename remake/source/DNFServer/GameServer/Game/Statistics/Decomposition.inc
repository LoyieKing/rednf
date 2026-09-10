void Statistics::Decomposition(unsigned int a, unsigned int b, int c)
{
    StatCompositionData data;

    data.a = a;
    data.b = b;
    data.type = 0x64;  // 'd'
    data.c = c;
    SendSignal(0x38, &data, 0xd);
