unsigned int XorRand4::getStaticUInt32()
{
    static __thread bool s_isCounterInited;
    static __thread unsigned int s_counter;
    unsigned int v;
    unsigned char* c;
    if (!s_isCounterInited)
    {
        unsigned int seed = (unsigned int)(clock() + (unsigned int)pthread_self());
        s_counter = getStaticUInt32(seed);
        s_isCounterInited = true;
    }
    v = s_counter++;
    c = (unsigned char*)&v;
    return ms_table0[c[0]] ^ ms_table1[c[1]] ^ ms_table2[c[2]] ^ ms_table3[c[3]];
}