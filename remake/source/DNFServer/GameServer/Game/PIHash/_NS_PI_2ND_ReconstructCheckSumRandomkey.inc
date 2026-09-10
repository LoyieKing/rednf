void _NS_PI_2ND_ReconstructCheckSumRandomkey(unsigned int& checksum,
                                             unsigned int randomKey)
{
    checksum = (checksum & 0xff) | (randomKey << 8);
