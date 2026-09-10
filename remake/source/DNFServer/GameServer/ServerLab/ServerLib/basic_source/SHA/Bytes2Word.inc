void CSHA::Bytes2Word(const unsigned char* pcBytes, unsigned int& ruiWord)
{
    ruiWord = (unsigned int)pcBytes[3] | (unsigned int)(pcBytes[2] << 8) | (unsigned int)(pcBytes[1] << 0x10) | (unsigned int)(pcBytes[0] << 0x18);
