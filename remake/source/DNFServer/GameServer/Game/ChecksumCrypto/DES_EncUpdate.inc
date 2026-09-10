int DES_EncUpdate(DES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                  unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080b0393：按 alg->mode 分派
    switch (alg->mode) {
    case 1:
        return ECB_EncUpdate(alg, in, inLen, out, outLen);
    case 2:
        return CBC_EncUpdate(alg, in, inLen, out, outLen);
    case 3:
        return OFB_EncUpdate(alg, in, inLen, out, outLen);
    case 4:
        return CFB_EncUpdate(alg, in, inLen, out, outLen);
    default:
        return AES_ERR_BAD_MODE;               // ORIG 返 0x1001
    }
