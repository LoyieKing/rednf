void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user)
{
    unsigned int bodyLen = len - 0xf;
    unsigned char* body = reinterpret_cast<unsigned char*>(data + 0xf);
    char first = data[0];
    short index = _NS_PI_2ND_GetPacketIndex(data);
    bool bNonClient;
    if (first == '\0' && index == 1)
        bNonClient = true;
    else
        bNonClient = false;

    unsigned int hash =
        reinterpret_cast<PISenderManager*>(user->GetPISenderManager())
            ->MakeHashForSending(body, bodyLen, bNonClient);
    unsigned int* pCksum = reinterpret_cast<unsigned int*>(data + 0xb);
    _NS_PI_2ND_ReconstructCheckSumRandomkey(*pCksum, hash);
