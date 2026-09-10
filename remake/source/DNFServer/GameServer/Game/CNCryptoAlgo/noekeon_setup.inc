int noekeon_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 temp;

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(skey != NULL);

   if (keylen != 16) {
      return CRYPT_INVALID_KEYSIZE;
   }

   if (num_rounds != 16 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   LOAD32H(skey->noekeon.K[0],&key[0]);
   LOAD32H(skey->noekeon.K[1],&key[4]);
   LOAD32H(skey->noekeon.K[2],&key[8]);
   LOAD32H(skey->noekeon.K[3],&key[12]);

   LOAD32H(skey->noekeon.dK[0],&key[0]);
   LOAD32H(skey->noekeon.dK[1],&key[4]);
   LOAD32H(skey->noekeon.dK[2],&key[8]);
   LOAD32H(skey->noekeon.dK[3],&key[12]);

   kTHETA(skey->noekeon.dK[0], skey->noekeon.dK[1], skey->noekeon.dK[2], skey->noekeon.dK[3]);

   return CRYPT_OK;
