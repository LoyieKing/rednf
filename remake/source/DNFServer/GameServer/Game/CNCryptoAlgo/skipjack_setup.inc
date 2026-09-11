int skipjack_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int x;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);

   if (keylen != 10) {
      return CRYPT_INVALID_KEYSIZE;
   }

   if (num_rounds != 32 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   /* make sure the key is in range for platforms where CHAR_BIT != 8 */
   for (x = 0; x < 10; x++) {
       skey->skipjack.key[x] = key[x] & 255;
   }

   return CRYPT_OK;
}