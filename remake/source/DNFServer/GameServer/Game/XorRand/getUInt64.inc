unsigned long long XorRand8::getUInt64()
{
    unsigned long long v = m_counter++;
    unsigned char* c = (unsigned char*)&v;
    return ms_table0[c[0]] ^ ms_table1[c[1]] ^ ms_table2[c[2]] ^ ms_table3[c[3]] ^
           ms_table4[c[4]] ^ ms_table5[c[5]] ^ ms_table6[c[6]] ^ ms_table7[c[7]];
}