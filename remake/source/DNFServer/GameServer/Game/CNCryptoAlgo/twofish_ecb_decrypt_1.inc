int twofish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
   int err = s_twofish_ecb_decrypt(ct, pt, skey);
   burn_stack(sizeof(ulong32) * 10 + sizeof(int));
   return err;
