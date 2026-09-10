int anubis_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize >= 40) {
      *keysize = 40;
   } else if (*keysize >= 36) {
      *keysize = 36;
   } else if (*keysize >= 32) {
      *keysize = 32;
   } else if (*keysize >= 28) {
      *keysize = 28;
   } else if (*keysize >= 24) {
      *keysize = 24;
   } else if (*keysize >= 20) {
      *keysize = 20;
   } else if (*keysize >= 16) {
      *keysize = 16;
   } else {
      return CRYPT_INVALID_KEYSIZE;
   }
   return CRYPT_OK;
