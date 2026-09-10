int noekeon_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
   int err = s_noekeon_ecb_decrypt(ct, pt, skey);
   burn_stack(sizeof(ulong32) * 5 + sizeof(int));
   return err;
