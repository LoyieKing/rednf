unsigned int XorRand4::getUInt32()
{
    unsigned int v = m_counter++;
    unsigned char* c = (unsigned char*)&v;
    return ms_table0[c[0]] ^ ms_table1[c[1]] ^ ms_table2[c[2]] ^ ms_table3[c[3]];
}