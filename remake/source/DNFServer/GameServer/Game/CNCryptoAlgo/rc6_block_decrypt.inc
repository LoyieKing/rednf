void rc6_block_decrypt(const RC6_ALG_INFO* alg, const unsigned int* in, unsigned int* out)
{
    const unsigned char* K = alg->K;
    unsigned int A = in[0] - K[0x2a];          // ORIG 80b5f53..5c
    unsigned int B = in[1];                    // ORIG 80b5f28
    unsigned int C = in[2] - K[0x2b];          // ORIG 80b5f44..4d
    unsigned int D = in[3];                    // ORIG 80b5f3c

    for (int r = 0x28; r >= 2; r -= 2)         // ORIG 80b5f5f..6051
    {
        unsigned int tmp = D; D = C; C = B; B = A; A = tmp;  // ORIG 80b5f6b..86
        // ORIG 80b5f89 取 -0x1c（轮换后 D = 加密前 D）、80b5f9b 取 -0x24
        // （轮换后 B = 加密前 B）——u/t 与加密同源。
        unsigned int u = rc6_ror32(D * 2 + 1, 0x1b);
        unsigned int t = rc6_ror32(B * 2 + 1, 0x1b);
        C = rc6_ror32(C - K[r + 1], t & 0x1f) ^ u;     // ORIG 80b5fad..ffa
        A = rc6_ror32(A - K[r], u & 0x1f) ^ t;         // ORIG 80b6000..4a
    }
    B -= K[0]; D -= K[1];                              // ORIG 80b6060..78
    out[0] = A; out[1] = B; out[2] = C; out[3] = D;
}