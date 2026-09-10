int blowfish_setup_with_data(const unsigned char *key, int keylen,
                             const unsigned char *data, int datalen,
                             symmetric_key *skey)
{
   XMEMCPY(skey->blowfish.K, ORIG_P, sizeof(ORIG_P));
   XMEMCPY(skey->blowfish.S, ORIG_S, sizeof(ORIG_S));
   return blowfish_expand(key, keylen, data, datalen, skey);
