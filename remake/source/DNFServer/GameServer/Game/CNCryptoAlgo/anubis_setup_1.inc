int  anubis_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
  int err;
  err = s_anubis_setup(key, keylen, num_rounds, skey);
  burn_stack(sizeof(int) * 5 + sizeof(ulong32) * (MAX_N + MAX_N + 5));
  return err;
