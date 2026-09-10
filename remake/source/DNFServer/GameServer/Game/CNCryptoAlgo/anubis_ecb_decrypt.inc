int anubis_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (skey->anubis.R < 12 || skey->anubis.R > 18) {
       return CRYPT_INVALID_ROUNDS;
   }

   anubis_crypt(ct, pt, skey->anubis.roundKeyDec, skey->anubis.R);
   return CRYPT_OK;
