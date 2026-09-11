int DES_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{
    switch (alg->mode) {
    case 1:
        return ECB_DecFinal(alg, out, outLen);
    case 2:
        return CBC_DecFinal(alg, out, outLen);
    case 3:
        return OFB_DecFinal(alg, out, outLen);
    case 4:
        return CFB_DecFinal(alg, out, outLen);
    default:
        return AES_ERR_BAD_MODE;
    }
}