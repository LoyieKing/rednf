void AES_SetAlgInfo(unsigned long mode, unsigned long padding,
                    unsigned char* vec, AES_ALG_INFO* alg)
{
    alg->mode = mode;
    alg->padding = padding;
    if (vec != 0)
        std::memcpy(alg->vec, vec, 16);
    else
        std::memset(alg->vec, 0, 16);
}