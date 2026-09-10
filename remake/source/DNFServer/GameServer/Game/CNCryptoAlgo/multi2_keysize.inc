int multi2_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize >= 40) {
      *keysize = 40;
   } else {
      return CRYPT_INVALID_KEYSIZE;
   }
   return CRYPT_OK;
