void sha256_finish(sha256_context* ctx, unsigned char output[0x20])
{
    unsigned char bits[8];
    unsigned long last;
    unsigned long padn;
    unsigned long hi;
    unsigned long lo;

    // bitlen 高/低 32 位（ORIG：hi=total0>>29|total1<<3; lo=total0<<3）
    hi = (ctx->m_total[0] >> 29) | (ctx->m_total[1] << 3);
    lo = ctx->m_total[0] << 3;

    bits[0] = static_cast<unsigned char>(hi >> 24);
    bits[1] = static_cast<unsigned char>(hi >> 16);
    bits[2] = static_cast<unsigned char>(hi >> 8);
    bits[3] = static_cast<unsigned char>(hi);
    bits[4] = static_cast<unsigned char>(lo >> 24);
    bits[5] = static_cast<unsigned char>(lo >> 16);
    bits[6] = static_cast<unsigned char>(lo >> 8);
    bits[7] = static_cast<unsigned char>(lo);

    last = ctx->m_total[0] & 0x3f;
    padn = (last < 0x38) ? (0x38 - last) : (0x78 - last);

    sha256_update(ctx, sha256_padding, padn);
    sha256_update(ctx, bits, 8);

    output[0] = static_cast<unsigned char>(ctx->m_state[0] >> 24);
    output[1] = static_cast<unsigned char>(ctx->m_state[0] >> 16);
    output[2] = static_cast<unsigned char>(ctx->m_state[0] >> 8);
    output[3] = static_cast<unsigned char>(ctx->m_state[0]);
    output[4] = static_cast<unsigned char>(ctx->m_state[1] >> 24);
    output[5] = static_cast<unsigned char>(ctx->m_state[1] >> 16);
    output[6] = static_cast<unsigned char>(ctx->m_state[1] >> 8);
    output[7] = static_cast<unsigned char>(ctx->m_state[1]);
    output[8] = static_cast<unsigned char>(ctx->m_state[2] >> 24);
    output[9] = static_cast<unsigned char>(ctx->m_state[2] >> 16);
    output[10] = static_cast<unsigned char>(ctx->m_state[2] >> 8);
    output[11] = static_cast<unsigned char>(ctx->m_state[2]);
    output[12] = static_cast<unsigned char>(ctx->m_state[3] >> 24);
    output[13] = static_cast<unsigned char>(ctx->m_state[3] >> 16);
    output[14] = static_cast<unsigned char>(ctx->m_state[3] >> 8);
    output[15] = static_cast<unsigned char>(ctx->m_state[3]);
    output[16] = static_cast<unsigned char>(ctx->m_state[4] >> 24);
    output[17] = static_cast<unsigned char>(ctx->m_state[4] >> 16);
    output[18] = static_cast<unsigned char>(ctx->m_state[4] >> 8);
    output[19] = static_cast<unsigned char>(ctx->m_state[4]);
    output[20] = static_cast<unsigned char>(ctx->m_state[5] >> 24);
    output[21] = static_cast<unsigned char>(ctx->m_state[5] >> 16);
    output[22] = static_cast<unsigned char>(ctx->m_state[5] >> 8);
    output[23] = static_cast<unsigned char>(ctx->m_state[5]);
    output[24] = static_cast<unsigned char>(ctx->m_state[6] >> 24);
    output[25] = static_cast<unsigned char>(ctx->m_state[6] >> 16);
    output[26] = static_cast<unsigned char>(ctx->m_state[6] >> 8);
    output[27] = static_cast<unsigned char>(ctx->m_state[6]);
    output[28] = static_cast<unsigned char>(ctx->m_state[7] >> 24);
    output[29] = static_cast<unsigned char>(ctx->m_state[7] >> 16);
    output[30] = static_cast<unsigned char>(ctx->m_state[7] >> 8);
    output[31] = static_cast<unsigned char>(ctx->m_state[7]);
}