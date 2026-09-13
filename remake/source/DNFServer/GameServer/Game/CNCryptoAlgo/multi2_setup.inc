int  multi2_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 sk[8], dk[2];
   int      x;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);

   if (keylen != 40) return CRYPT_INVALID_KEYSIZE;
   if (num_rounds == 0) num_rounds = 128;

   skey->multi2.N = num_rounds;
   for (x = 0; x < 8; x++) {
       LOAD32H(sk[x], key + x*4);
   }
   LOAD32H(dk[0], key + 32);
   LOAD32H(dk[1], key + 36);
   setup(dk, sk, skey->multi2.uk);   /* b24FIX: 宏 s_setup → static setup()（ORIG 0x080b482e） */

   zeromem(sk, sizeof(sk));
   zeromem(dk, sizeof(dk));
   return CRYPT_OK;
}