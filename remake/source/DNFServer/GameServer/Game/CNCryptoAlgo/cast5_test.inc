int cast5_test(void)
{
 #ifndef LTC_TEST
    return CRYPT_NOP;
 #else
   static const struct {
       int keylen;
       unsigned char key[16];
       unsigned char pt[8];
       unsigned char ct[8];
   } tests[] = {
     { 16,
       {0x01, 0x23, 0x45, 0x67, 0x12, 0x34, 0x56, 0x78, 0x23, 0x45, 0x67, 0x89, 0x34, 0x56, 0x78, 0x9A},
       {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
       {0x23, 0x8B, 0x4F, 0xE5, 0x84, 0x7E, 0x44, 0xB2}
     },
     { 10,
       {0x01, 0x23, 0x45, 0x67, 0x12, 0x34, 0x56, 0x78, 0x23, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
       {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
       {0xEB, 0x6A, 0x71, 0x1A, 0x2C, 0x02, 0x27, 0x1B},
     },
     { 5,
       {0x01, 0x23, 0x45, 0x67, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
       {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
       {0x7A, 0xC8, 0x16, 0xD1, 0x6E, 0x9B, 0x30, 0x2E}
     }
   };
   int i, y, err;
   symmetric_key key;
   unsigned char tmp[2][8];

   for (i = 0; i < (int)(sizeof(tests) / sizeof(tests[0])); i++) {
       if ((err = cast5_setup(tests[i].key, tests[i].keylen, 0, &key)) != CRYPT_OK) {
          return err;
       }
       cast5_ecb_encrypt(tests[i].pt, tmp[0], &key);
       cast5_ecb_decrypt(tmp[0], tmp[1], &key);
       if ((ltc_compare_testvector(tmp[0], 8, tests[i].ct, 8, "CAST5 Encrypt", i) != 0) ||
             (ltc_compare_testvector(tmp[1], 8, tests[i].pt, 8, "CAST5 Decrypt", i) != 0)) {
          return CRYPT_FAIL_TESTVECTOR;
       }
      /* now see if we can encrypt all zero bytes 1000 times, decrypt and come back where we started */
      for (y = 0; y < 8; y++) tmp[0][y] = 0;
      for (y = 0; y < 1000; y++) cast5_ecb_encrypt(tmp[0], tmp[0], &key);
      for (y = 0; y < 1000; y++) cast5_ecb_decrypt(tmp[0], tmp[0], &key);
      for (y = 0; y < 8; y++) if (tmp[0][y] != 0) return CRYPT_FAIL_TESTVECTOR;

   }
   return CRYPT_OK;
 #endif
