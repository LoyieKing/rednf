int blowfish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
    int err = s_blowfish_ecb_decrypt(ct, pt, skey);
    burn_stack(sizeof(ulong32) * 2 + sizeof(int));
    return err;
}