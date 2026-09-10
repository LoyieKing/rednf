int skipjack_ecb_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_key *skey)
{
   int err = s_skipjack_ecb_encrypt(pt, ct, skey);
   burn_stack(sizeof(unsigned) * 8 + sizeof(int) * 2);
   return err;
