void DES3_DecInit(DES3_ALG_INFO* alg)
{
    alg->buffered = 0;
    if (alg->mode == 1)
        return;
    std::memcpy(alg->chain, alg->vec, 8);
