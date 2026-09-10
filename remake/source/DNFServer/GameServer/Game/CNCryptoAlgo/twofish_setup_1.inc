int twofish_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int x;
   x = s_twofish_setup(key, keylen, num_rounds, skey);
   burn_stack(sizeof(int) * 7 + sizeof(unsigned char) * 56 + sizeof(ulong32) * 2);
   return x;
