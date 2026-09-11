// ORIG T 0x08599946：+0x34[i] = get_rand_int(200)+20，i∈[0,4)。
void CPad::GenerateRandomShakeData()
{
    int i = 0;
    for (i = 0; i < 4; ++i)
        m_shake34[i] = (char)(get_rand_int(200) + 0x14);
}