int skipjack_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize < 10) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (*keysize > 10) {
      *keysize = 10;
   }
   return CRYPT_OK;
}