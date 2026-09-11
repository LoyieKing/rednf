// ORIG 0x080afaa4 / 0x080ae00c：mode/padding 直写，vec 拷 8 字节（DES 块大小）
void DES_SetAlgInfo(unsigned long mode, unsigned long padding,
                    unsigned char* vec, DES_ALG_INFO* alg)
{
    alg->mode = mode;
    alg->padding = padding;
    if (vec != 0)
        std::memcpy(alg->vec, vec, 8);
    else
        std::memset(alg->vec, 0, 8);
}