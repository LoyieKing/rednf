int get_regular_rand(int n)
{
    int rounds = 0xc;
    int sum = 0;
    int centered = 0;
    float factor = 0.95f;
    int i = 0;
    for (; i < rounds; ++i)
        sum += get_rand_int(100);
    sum /= 100;
    centered = sum - (rounds / 2);
    centered = static_cast<int>(static_cast<float>(centered) * factor);
    return centered + n;
