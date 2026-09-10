void IMethod::BytesToWord(const unsigned char* pucBytes, unsigned int& ruiWord)
{
    ruiWord = 0;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 0x18);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 0x10);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 8);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | (unsigned int)*pucBytes;
