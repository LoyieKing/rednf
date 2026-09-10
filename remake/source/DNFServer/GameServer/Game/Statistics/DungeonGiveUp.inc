void Statistics::DungeonGiveUp(unsigned int a, unsigned int b, int c, int d, int e,
                               int f)
{
    StatDungeonData data;

    data.a = a;
    data.b = b;
    data.c = c;
    data.d = d;
    data.type = 0x67;  // 'g'
    data.e = e;
    data.f = f;
    SendSignal(0x3c, &data, 0x19);
