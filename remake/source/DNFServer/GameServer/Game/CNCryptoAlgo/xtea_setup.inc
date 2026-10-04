int xtea_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 x, sum, K[4];

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(skey != NULL);

   /* check arguments */
   if (keylen != 16) {
      return CRYPT_INVALID_KEYSIZE;
   }

   if (num_rounds != 0 && num_rounds != 32) {
      return CRYPT_INVALID_ROUNDS;
   }

   /* [DNF-FIX-XTEA 2026-09-22] ORIG df_game_r.qf uses LOAD32L/STORE32L (LE); was LOAD32H/STORE32H (BE). */
   /* load key */
   LOAD32L(K[0], key+0);
   LOAD32L(K[1], key+4);
   LOAD32L(K[2], key+8);
   LOAD32L(K[3], key+12);

   for (x = sum = 0; x < 32; x++) {
       skey->xtea.A[x] = (sum + K[sum&3]) & 0xFFFFFFFFUL;
       sum = (sum + 0x9E3779B9UL) & 0xFFFFFFFFUL;
       skey->xtea.B[x] = (sum + K[(sum>>11)&3]) & 0xFFFFFFFFUL;
   }

#ifdef LTC_CLEAN_STACK
   zeromem(&K, sizeof(K));
#endif

   return CRYPT_OK;
}