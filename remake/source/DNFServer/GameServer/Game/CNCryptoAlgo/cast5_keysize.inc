int cast5_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize < 5) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (*keysize > 16) {
      *keysize = 16;
   }
   return CRYPT_OK;
