// ORIG 0x080afbf0/0x080b07ca/0x080ae158/0x080aed32：buffered=0，非 ECB 时 chain←vec(8)
void DES_EncInit(DES_ALG_INFO* alg)
{
    alg->buffered = 0;
    if (alg->mode == 1)
        return;
    std::memcpy(alg->chain, alg->vec, 8);
}