void AES_EncInit(AES_ALG_INFO* alg)
{
    alg->buffered = 0;                    // ORIG 0x080a27ce
    if (alg->mode == 1)
        return;                           // ECB 无链初始化
    std::memcpy(alg->chain, alg->vec, 16); // ORIG 0x080a27fd：vec→chain
