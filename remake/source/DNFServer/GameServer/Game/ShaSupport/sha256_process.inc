void sha256_process(sha256_context* ctx, unsigned char* data)
{
    unsigned long w[64];
    unsigned long t0;
    unsigned long t1;
    unsigned long s[8];
    w[0] = ((unsigned long)(data[0] & 255) << 24)
           | ((unsigned long)(data[1] & 255) << 16)
           | ((unsigned long)(data[2] & 255) << 8)
           | (unsigned long)(data[3] & 255);
    w[1] = ((unsigned long)(data[4] & 255) << 24)
           | ((unsigned long)(data[5] & 255) << 16)
           | ((unsigned long)(data[6] & 255) << 8)
           | (unsigned long)(data[7] & 255);
    w[2] = ((unsigned long)(data[8] & 255) << 24)
           | ((unsigned long)(data[9] & 255) << 16)
           | ((unsigned long)(data[10] & 255) << 8)
           | (unsigned long)(data[11] & 255);
    w[3] = ((unsigned long)(data[12] & 255) << 24)
           | ((unsigned long)(data[13] & 255) << 16)
           | ((unsigned long)(data[14] & 255) << 8)
           | (unsigned long)(data[15] & 255);
    w[4] = ((unsigned long)(data[16] & 255) << 24)
           | ((unsigned long)(data[17] & 255) << 16)
           | ((unsigned long)(data[18] & 255) << 8)
           | (unsigned long)(data[19] & 255);
    w[5] = ((unsigned long)(data[20] & 255) << 24)
           | ((unsigned long)(data[21] & 255) << 16)
           | ((unsigned long)(data[22] & 255) << 8)
           | (unsigned long)(data[23] & 255);
    w[6] = ((unsigned long)(data[24] & 255) << 24)
           | ((unsigned long)(data[25] & 255) << 16)
           | ((unsigned long)(data[26] & 255) << 8)
           | (unsigned long)(data[27] & 255);
    w[7] = ((unsigned long)(data[28] & 255) << 24)
           | ((unsigned long)(data[29] & 255) << 16)
           | ((unsigned long)(data[30] & 255) << 8)
           | (unsigned long)(data[31] & 255);
    w[8] = ((unsigned long)(data[32] & 255) << 24)
           | ((unsigned long)(data[33] & 255) << 16)
           | ((unsigned long)(data[34] & 255) << 8)
           | (unsigned long)(data[35] & 255);
    w[9] = ((unsigned long)(data[36] & 255) << 24)
           | ((unsigned long)(data[37] & 255) << 16)
           | ((unsigned long)(data[38] & 255) << 8)
           | (unsigned long)(data[39] & 255);
    w[10] = ((unsigned long)(data[40] & 255) << 24)
           | ((unsigned long)(data[41] & 255) << 16)
           | ((unsigned long)(data[42] & 255) << 8)
           | (unsigned long)(data[43] & 255);
    w[11] = ((unsigned long)(data[44] & 255) << 24)
           | ((unsigned long)(data[45] & 255) << 16)
           | ((unsigned long)(data[46] & 255) << 8)
           | (unsigned long)(data[47] & 255);
    w[12] = ((unsigned long)(data[48] & 255) << 24)
           | ((unsigned long)(data[49] & 255) << 16)
           | ((unsigned long)(data[50] & 255) << 8)
           | (unsigned long)(data[51] & 255);
    w[13] = ((unsigned long)(data[52] & 255) << 24)
           | ((unsigned long)(data[53] & 255) << 16)
           | ((unsigned long)(data[54] & 255) << 8)
           | (unsigned long)(data[55] & 255);
    w[14] = ((unsigned long)(data[56] & 255) << 24)
           | ((unsigned long)(data[57] & 255) << 16)
           | ((unsigned long)(data[58] & 255) << 8)
           | (unsigned long)(data[59] & 255);
    w[15] = ((unsigned long)(data[60] & 255) << 24)
           | ((unsigned long)(data[61] & 255) << 16)
           | ((unsigned long)(data[62] & 255) << 8)
           | (unsigned long)(data[63] & 255);

    s[0] = ctx->m_state[0];
    s[1] = ctx->m_state[1];
    s[2] = ctx->m_state[2];
    s[3] = ctx->m_state[3];
    s[4] = ctx->m_state[4];
    s[5] = ctx->m_state[5];
    s[6] = ctx->m_state[6];
    s[7] = ctx->m_state[7];

    SHA256_RND (s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 0, 0x428a2f98UL);
    SHA256_RND (s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 1, 0x71374491UL);
    SHA256_RND (s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 2, 0xb5c0fbcfUL);
    SHA256_RND (s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 3, 0xe9b5dba5UL);
    SHA256_RND (s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 4, 0x3956c25bUL);
    SHA256_RND (s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 5, 0x59f111f1UL);
    SHA256_RND (s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 6, 0x923f82a4UL);
    SHA256_RND (s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 7, 0xab1c5ed5UL);
    SHA256_RND (s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 8, 0xd807aa98UL);
    SHA256_RND (s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 9, 0x12835b01UL);
    SHA256_RND (s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 10, 0x243185beUL);
    SHA256_RND (s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 11, 0x550c7dc3UL);
    SHA256_RND (s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 12, 0x72be5d74UL);
    SHA256_RND (s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 13, 0x80deb1feUL);
    SHA256_RND (s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 14, 0x9bdc06a7UL);
    SHA256_RND (s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 15, 0xc19bf174UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 16, 0xe49b69c1UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 17, 0xefbe4786UL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 18, 0x0fc19dc6UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 19, 0x240ca1ccUL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 20, 0x2de92c6fUL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 21, 0x4a7484aaUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 22, 0x5cb0a9dcUL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 23, 0x76f988daUL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 24, 0x983e5152UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 25, 0xa831c66dUL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 26, 0xb00327c8UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 27, 0xbf597fc7UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 28, 0xc6e00bf3UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 29, 0xd5a79147UL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 30, 0x06ca6351UL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 31, 0x14292967UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 32, 0x27b70a85UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 33, 0x2e1b2138UL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 34, 0x4d2c6dfcUL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 35, 0x53380d13UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 36, 0x650a7354UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 37, 0x766a0abbUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 38, 0x81c2c92eUL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 39, 0x92722c85UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 40, 0xa2bfe8a1UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 41, 0xa81a664bUL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 42, 0xc24b8b70UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 43, 0xc76c51a3UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 44, 0xd192e819UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 45, 0xd6990624UL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 46, 0xf40e3585UL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 47, 0x106aa070UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 48, 0x19a4c116UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 49, 0x1e376c08UL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 50, 0x2748774cUL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 51, 0x34b0bcb5UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 52, 0x391c0cb3UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 53, 0x4ed8aa4aUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 54, 0x5b9cca4fUL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 55, 0x682e6ff3UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 56, 0x748f82eeUL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 57, 0x78a5636fUL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 58, 0x84c87814UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 59, 0x8cc70208UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 60, 0x90befffaUL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 61, 0xa4506cebUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 62, 0xbef9a3f7UL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 63, 0xc67178f2UL);

    ctx->m_state[0] += s[0];
    ctx->m_state[1] += s[1];
    ctx->m_state[2] += s[2];
    ctx->m_state[3] += s[3];
    ctx->m_state[4] += s[4];
    ctx->m_state[5] += s[5];
    ctx->m_state[6] += s[6];
    ctx->m_state[7] += s[7];
}