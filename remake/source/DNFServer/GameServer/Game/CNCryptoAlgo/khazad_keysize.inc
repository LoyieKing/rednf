int khazad_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize >= 16) {
      *keysize = 16;
      return CRYPT_OK;
   }
   return CRYPT_INVALID_KEYSIZE;
