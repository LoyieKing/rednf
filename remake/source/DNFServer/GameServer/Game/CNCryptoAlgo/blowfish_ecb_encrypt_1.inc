int blowfish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_key *skey)
{
    int err = s_blowfish_ecb_encrypt(pt, ct, skey);
    burn_stack(sizeof(ulong32) * 2 + sizeof(int));
    return err;
