void IMethod::WordToBytes(unsigned int uiWord, unsigned char* pucBytes)
{
    *pucBytes = (unsigned char)(uiWord >> 0x18);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)(uiWord >> 0x10);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)(uiWord >> 8);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)uiWord;
