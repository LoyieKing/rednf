int twofish_keysize(int *keysize)
{
   LTC_ARGCHK(keysize);
   if (*keysize < 16) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (*keysize < 24) {
      *keysize = 16;
      return CRYPT_OK;
   }
   if (*keysize < 32) {
      *keysize = 24;
      return CRYPT_OK;
   }
   *keysize = 32;
   return CRYPT_OK;
