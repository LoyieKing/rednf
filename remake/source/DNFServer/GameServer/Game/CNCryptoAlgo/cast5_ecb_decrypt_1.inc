int cast5_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
   int err = s_cast5_ecb_decrypt(ct,pt,skey);
   burn_stack(sizeof(ulong32)*3);
   return err;
