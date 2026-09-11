int skipjack_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
   int err = s_skipjack_ecb_decrypt(ct, pt, skey);
   burn_stack(sizeof(unsigned) * 7 + sizeof(int) * 2);
   return err;
}