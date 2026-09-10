void CSHA::Word2Bytes(const unsigned int& ruiWord, unsigned char* pcBytes)
{
    pcBytes = pcBytes + 3;
    *pcBytes = (unsigned char)ruiWord;
    pcBytes = pcBytes - 1;
    *pcBytes = (unsigned char)(ruiWord >> 8);
    pcBytes = pcBytes - 1;
    *pcBytes = (unsigned char)(ruiWord >> 0x10);
    pcBytes = pcBytes - 1;
    *pcBytes = (unsigned char)(ruiWord >> 0x18);
