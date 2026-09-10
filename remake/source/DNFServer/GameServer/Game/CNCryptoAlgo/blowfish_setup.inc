int blowfish_setup(const unsigned char *key, int keylen, int num_rounds,
                  symmetric_key *skey)
{
   /* check key length */
   if (keylen < 8 || keylen > 72) {
      return CRYPT_INVALID_KEYSIZE;
   }
   /* check rounds */
   if (num_rounds != 0 && num_rounds != 16) {
      return CRYPT_INVALID_ROUNDS;
   }

   return blowfish_setup_with_data(key, keylen, NULL, 0, skey);
