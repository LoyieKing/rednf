void blowfish_enc(ulong32 *data, unsigned long blocks, symmetric_key *skey)
{
   unsigned long i;
   ulong32 *d = data;

   for (i = 0; i < blocks; ++i) {
      s_blowfish_encipher(d, d + 1, skey);
      d += 2;
   }
}