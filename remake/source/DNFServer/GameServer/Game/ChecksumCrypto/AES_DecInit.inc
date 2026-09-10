void AES_DecInit(AES_ALG_INFO* alg)
{
    alg->buffered = 0;
    if (alg->mode == 1)
        return;
    std::memcpy(alg->chain, alg->vec, 16);
