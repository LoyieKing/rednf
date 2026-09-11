void DES3_SetAlgInfo(unsigned long mode, unsigned long padding,
                     unsigned char* vec, DES3_ALG_INFO* alg)
{
    alg->mode = mode;
    alg->padding = padding;
    if (vec != 0)
        std::memcpy(alg->vec, vec, 8);
    else
        std::memset(alg->vec, 0, 8);
}