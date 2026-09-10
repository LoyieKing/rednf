unsigned int CRijndael_fake::Pad(char* buffer, int pos, unsigned int* pLen)
{
    if (*pLen % GetBlockSize() != 0) {
        unsigned int pad = GetBlockSize() - (*pLen % GetBlockSize());
        std::memset(buffer + pos, 0, pad);      // ORIG：补零至块边界
        *pLen += pad;
    }
    return *pLen;
