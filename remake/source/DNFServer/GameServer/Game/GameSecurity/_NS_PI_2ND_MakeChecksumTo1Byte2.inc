void _NS_PI_2ND_MakeChecksumTo1Byte2(unsigned int& checksum1,
                                     unsigned int& checksum2)
{
    checksum1 = static_cast<unsigned int>(static_cast<unsigned char>(checksum1));
    checksum2 = static_cast<unsigned int>(static_cast<unsigned char>(checksum2));
