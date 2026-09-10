int khazad_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int               r;
   const ulong64    *S;
   ulong64           K2, K1;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);
   if (keylen != 16) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (num_rounds != 8 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   /* use 7th table */
   S = T7;

    /*
    * map unsigned char array cipher key to initial key state (mu):
    */
   K2 =
      ((ulong64)key[ 0] << 56) ^
      ((ulong64)key[ 1] << 48) ^
      ((ulong64)key[ 2] << 40) ^
      ((ulong64)key[ 3] << 32) ^
      ((ulong64)key[ 4] << 24) ^
      ((ulong64)key[ 5] << 16) ^
      ((ulong64)key[ 6] <<  8) ^
      ((ulong64)key[ 7]      );
   K1 =
      ((ulong64)key[ 8] << 56) ^
      ((ulong64)key[ 9] << 48) ^
      ((ulong64)key[10] << 40) ^
      ((ulong64)key[11] << 32) ^
      ((ulong64)key[12] << 24) ^
      ((ulong64)key[13] << 16) ^
      ((ulong64)key[14] <<  8) ^
      ((ulong64)key[15]      );

   /*
    * compute the round keys:
    */
   for (r = 0; r <= R; r++) {
      /*
       * K[r] = rho(c[r], K1) ^ K2;
       */
      skey->khazad.roundKeyEnc[r] =
         T0[(int)(K1 >> 56)       ] ^
         T1[(int)(K1 >> 48) & 0xff] ^
         T2[(int)(K1 >> 40) & 0xff] ^
         T3[(int)(K1 >> 32) & 0xff] ^
         T4[(int)(K1 >> 24) & 0xff] ^
         T5[(int)(K1 >> 16) & 0xff] ^
         T6[(int)(K1 >>  8) & 0xff] ^
         T7[(int)(K1      ) & 0xff] ^
         c[r] ^ K2;
      K2 = K1; K1 = skey->khazad.roundKeyEnc[r];
   }
   /*
    * compute the inverse key schedule:
    * K'^0 = K^R, K'^R = K^0, K'^r = theta(K^{R-r})
    */
   skey->khazad.roundKeyDec[0] = skey->khazad.roundKeyEnc[R];
   for (r = 1; r < R; r++) {
      K1 = skey->khazad.roundKeyEnc[R - r];
      skey->khazad.roundKeyDec[r] =
         T0[(int)S[(int)(K1 >> 56)       ] & 0xff] ^
         T1[(int)S[(int)(K1 >> 48) & 0xff] & 0xff] ^
         T2[(int)S[(int)(K1 >> 40) & 0xff] & 0xff] ^
         T3[(int)S[(int)(K1 >> 32) & 0xff] & 0xff] ^
         T4[(int)S[(int)(K1 >> 24) & 0xff] & 0xff] ^
         T5[(int)S[(int)(K1 >> 16) & 0xff] & 0xff] ^
         T6[(int)S[(int)(K1 >>  8) & 0xff] & 0xff] ^
         T7[(int)S[(int)(K1      ) & 0xff] & 0xff];
   }
   skey->khazad.roundKeyDec[R] = skey->khazad.roundKeyEnc[0];

   return CRYPT_OK;
