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
    for (int i = 0; i < 64; ++i) {
        unsigned int t1 = h + SIG1(e) + CH(e, f, g) + sm_K256[i] + w[i];
        unsigned int t2 = SIG0(a) + MAJ(a, b, c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    m_state[0] += a; m_state[1] += b; m_state[2] += c; m_state[3] += d;
    m_state[4] += e; m_state[5] += f; m_state[6] += g; m_state[7] += h;
