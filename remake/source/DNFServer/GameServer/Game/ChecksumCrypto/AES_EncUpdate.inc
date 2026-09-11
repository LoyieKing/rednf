int AES_EncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                  unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a31bc：按 alg->mode 分派到对应模式 Update
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
}