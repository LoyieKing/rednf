int khazad_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);
   khazad_crypt(ct, pt, skey->khazad.roundKeyDec);
   return CRYPT_OK;
