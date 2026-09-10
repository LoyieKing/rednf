int DES3_DecUpdate(DES3_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                   unsigned char* out, unsigned long* outLen)
{
    switch (alg->mode) {
    case 1:
        return ECB_DecUpdate(alg, in, inLen, out, outLen);
    case 2:
        return CBC_DecUpdate(alg, in, inLen, out, outLen);
    case 3:
        return OFB_DecUpdate(alg, in, inLen, out, outLen);
    case 4:
        return CFB_DecUpdate(alg, in, inLen, out, outLen);
    default:
        return AES_ERR_BAD_MODE;
    }
