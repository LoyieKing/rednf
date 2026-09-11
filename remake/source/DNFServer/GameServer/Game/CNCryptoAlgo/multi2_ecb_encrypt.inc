int multi2_ecb_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_key *skey)
{
   ulong32 p[2];
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);
   LOAD32H(p[0], pt);
   LOAD32H(p[1], pt+4);
   s_encrypt(p, skey->multi2.N, skey->multi2.uk);
   STORE32H(p[0], ct);
   STORE32H(p[1], ct+4);
   return CRYPT_OK;
}