int DES_EncFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080b072f
    switch (alg->mode) {
    case 1:
        return ECB_EncFinal(alg, out, outLen);
    case 2:
        return CBC_EncFinal(alg, out, outLen);
    case 3:
        return OFB_EncFinal(alg, out, outLen);
    case 4:
        return CFB_EncFinal(alg, out, outLen);
    default:
        return AES_ERR_BAD_MODE;
    }
}