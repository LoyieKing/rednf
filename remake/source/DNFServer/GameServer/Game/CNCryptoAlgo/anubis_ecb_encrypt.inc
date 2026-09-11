int anubis_ecb_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (skey->anubis.R < 12 || skey->anubis.R > 18) {
       return CRYPT_INVALID_ROUNDS;
   }

   anubis_crypt(pt, ct, skey->anubis.roundKeyEnc, skey->anubis.R);
   return CRYPT_OK;
}