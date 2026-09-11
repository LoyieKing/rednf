// ORIG T 0x08599918：+0x2e = get_rand_int(5000)。
void CPad::GenerateRandomKeyIdx()
{
    m_keyIdx2e = (unsigned short)get_rand_int(5000);
}