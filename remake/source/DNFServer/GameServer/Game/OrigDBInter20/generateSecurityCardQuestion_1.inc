void generateSecurityCardQuestion(char* out)
{
    int v1, a1, v2, a2;   // P28 声明序 → 槽 -0x18/-0x14/-0x10/-0xc
    a1 = get_rand_int(4);
    if (a1 == 3)
    {
        v1 = get_rand_int(8);
    }
    else
    {
        v1 = get_rand_int(10);
    }
    do
    {
        a2 = get_rand_int(4);
        if (a2 == 3)
        {
            v2 = get_rand_int(8);
        }
        else
        {
            v2 = get_rand_int(10);
        }
    } while ((v1 / 2 == v2 / 2) && (a1 == a2));

    out[0] = static_cast<char>(v1 + 0x30);
    out[1] = static_cast<char>(a1 + 0x30);
    out[2] = static_cast<char>(v2 + 0x30);
    out[3] = static_cast<char>(a2 + 0x30);
