void rc6_block_encrypt(const RC6_ALG_INFO* alg, const unsigned int* in, unsigned int* out)
{
    const unsigned char* K = alg->K;
    unsigned int A = in[0];                    // ORIG 80b5d82
    unsigned int B = in[1] + K[0];             // ORIG 80b5dab
    unsigned int C = in[2];                    // ORIG 80b5d98
    unsigned int D = in[3] + K[1];             // ORIG 80b5dba

    for (int r = 2; r <= 0x28; r += 2)         // ORIG 80b5dc3..eaa
    {
        unsigned int t = rc6_ror32((B * 2 + 1) * B, 0x1b);   // ORIG 80b5dcf..dd
        unsigned int u = rc6_ror32((D * 2 + 1) * D, 0x1b);   // ORIG 80b5de1..ed
        A = rc6_rol32(A ^ t, u & 0x1f) + K[r];         // ORIG 80b5df3..39
        C = rc6_rol32(C ^ u, t & 0x1f) + K[r + 1];     // ORIG 80b5e3c..85
        unsigned int tmp = A; A = B; B = C; C = D; D = tmp;  // ORIG 80b5e88..a3
    }
    out[0] = A + K[0x2a];                      // ORIG 80b5eb9..c5
    out[1] = B;                                // ORIG 80b5ee5
    out[2] = C + K[0x2b];                      // ORIG 80b5ecb..d4
    out[3] = D;                                // ORIG 80b5efb
}