void CSHA::Transform()
{
    unsigned int w[64];
    for (int i = 0; i < 16; ++i) {
        Bytes2Word(m_buffer + i * 4, w[i]);
    }
    for (int i = 16; i < 64; ++i) {
        w[i] = sig1(w[i - 2]) + w[i - 7] + sig0(w[i - 15]) + w[i - 16];
    }
    unsigned int a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3];
    unsigned int e = m_state[4], f = m_state[5], g = m_state[6], h = m_state[7];
    // [P165 we-smaller 还原 2026-09-11] ORIG 主循环为**源码级 64 轮完全展开**：
    // 2281 条指令 / helper 各 64 个 call 点（0x80bbb0b..0x80bd761 分散全段）、
    // t1/t2 复用槽（-0xc/-0x10）、sm_K256[i]/w[i] 为常量索引直址（`mov 0x8b22fe0,%eax`）。
    // 重建原为 for 循环（198 条、每 helper 0~1 call）⇒ 按 ORIG 结构展开为 64 份独立块。
    unsigned int t1, t2;                     // 复用槽（宏内赋值，勿每轮新声明）
    #define SHA256_ROUND(va, vb, vc, vd, ve, vf, vg, vh, idx)                 \
        t1 = vh + SIG1(ve) + CH(ve, vf, vg) + sm_K256[idx] + w[idx];           \
        t2 = SIG0(va) + MAJ(va, vb, vc);                                       \
        vd += t1;                                                              \
        vh = t1 + t2
    SHA256_ROUND(a, b, c, d, e, f, g, h, 0);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 1);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 2);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 3);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 4);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 5);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 6);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 7);
    SHA256_ROUND(a, b, c, d, e, f, g, h, 8);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 9);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 10);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 11);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 12);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 13);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 14);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 15);
    SHA256_ROUND(a, b, c, d, e, f, g, h, 16);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 17);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 18);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 19);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 20);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 21);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 22);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 23);
    SHA256_ROUND(a, b, c, d, e, f, g, h, 24);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 25);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 26);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 27);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 28);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 29);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 30);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 31);
    SHA256_ROUND(a, b, c, d, e, f, g, h, 32);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 33);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 34);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 35);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 36);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 37);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 38);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 39);
    SHA256_ROUND(a, b, c, d, e, f, g, h, 40);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 41);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 42);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 43);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 44);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 45);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 46);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 47);
    SHA256_ROUND(a, b, c, d, e, f, g, h, 48);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 49);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 50);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 51);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 52);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 53);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 54);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 55);
    SHA256_ROUND(a, b, c, d, e, f, g, h, 56);
    SHA256_ROUND(h, a, b, c, d, e, f, g, 57);
    SHA256_ROUND(g, h, a, b, c, d, e, f, 58);
    SHA256_ROUND(f, g, h, a, b, c, d, e, 59);
    SHA256_ROUND(e, f, g, h, a, b, c, d, 60);
    SHA256_ROUND(d, e, f, g, h, a, b, c, 61);
    SHA256_ROUND(c, d, e, f, g, h, a, b, 62);
    SHA256_ROUND(b, c, d, e, f, g, h, a, 63);
    #undef SHA256_ROUND
    m_state[0] += a; m_state[1] += b; m_state[2] += c; m_state[3] += d;
    m_state[4] += e; m_state[5] += f; m_state[6] += g; m_state[7] += h;
}