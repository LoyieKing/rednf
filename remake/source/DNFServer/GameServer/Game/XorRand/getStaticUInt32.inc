unsigned int XorRand4::getStaticUInt32(unsigned int seed)
{
    unsigned char* c = (unsigned char*)&seed;
    return ms_table0[c[0]] ^ ms_table1[c[1]] ^ ms_table2[c[2]] ^ ms_table3[c[3]];
}