int twofish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_key *skey)
{
   int err = s_twofish_ecb_encrypt(pt, ct, skey);
   burn_stack(sizeof(ulong32) * 10 + sizeof(int));
   return err;
}