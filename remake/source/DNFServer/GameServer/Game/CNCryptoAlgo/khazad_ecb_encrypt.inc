int khazad_ecb_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);
   khazad_crypt(pt, ct, skey->khazad.roundKeyEnc);
   return CRYPT_OK;
}