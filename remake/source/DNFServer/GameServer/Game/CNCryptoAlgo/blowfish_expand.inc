int blowfish_expand(const unsigned char *key, int keylen,
                    const unsigned char *data, int datalen,
                    symmetric_key *skey)
{
   ulong32 x, y, A, B[2];
   int i;

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(skey != NULL);

   /* load in key bytes (Supplied by David Hopwood) */
   i = 0;
   for (x = 0; x < 18; x++) {
       A = s_blowfish_stream2word(key, keylen, &i);
       skey->blowfish.K[x] ^= A;
   }


   i = 0;
   B[0] = 0;
   B[1] = 0;
   for (x = 0; x < 18; x += 2) {
      if (data != NULL) {
         B[0] ^= s_blowfish_stream2word(data, datalen, &i);
         B[1] ^= s_blowfish_stream2word(data, datalen, &i);
      }
      /* encrypt it */
      s_blowfish_encipher(&B[0], &B[1], skey);
      /* copy it */
      skey->blowfish.K[x] = B[0];
      skey->blowfish.K[x+1] = B[1];
   }

   /* encrypt S array */
   for (x = 0; x < 4; x++) {
       for (y = 0; y < 256; y += 2) {
          if (data != NULL) {
             B[0] ^= s_blowfish_stream2word(data, datalen, &i);
             B[1] ^= s_blowfish_stream2word(data, datalen, &i);
          }
          /* encrypt it */
          s_blowfish_encipher(&B[0], &B[1], skey);
          /* copy it */
          skey->blowfish.S[x][y] = B[0];
          skey->blowfish.S[x][y+1] = B[1];
       }
   }

#ifdef LTC_CLEAN_STACK
   zeromem(B, sizeof(B));
#endif

   return CRYPT_OK;
}