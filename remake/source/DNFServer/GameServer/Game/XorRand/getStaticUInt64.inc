unsigned long long XorRand8::getStaticUInt64()
{
    static __thread bool s_isCounterInited;
    static __thread unsigned long long s_counter;
    unsigned long long v;
    unsigned char* c;
    if (!s_isCounterInited)
    {
        unsigned long long seed = (unsigned long long)((long long)clock() + (unsigned long long)pthread_self());
        s_counter = getStaticUInt64(seed);
        s_isCounterInited = true;
    }
    v = s_counter++;
    c = (unsigned char*)&v;
    return ms_table0[c[0]] ^ ms_table1[c[1]] ^ ms_table2[c[2]] ^ ms_table3[c[3]] ^
           ms_table4[c[4]] ^ ms_table5[c[5]] ^ ms_table6[c[6]] ^ ms_table7[c[7]];
