int cast5_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int z;
   z = s_cast5_setup(key, keylen, num_rounds, skey);
   burn_stack(sizeof(ulong32)*8 + 16 + sizeof(int)*2);
   return z;
}