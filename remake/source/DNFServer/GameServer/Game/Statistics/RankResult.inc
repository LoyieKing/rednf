void Statistics::RankResult(unsigned int a, unsigned int b, int c, int d, int e,
                            int f, int g, GameResultType& result)
{
    MSG_STAT_RANKRES msg;

    msg.m_a = a;
    msg.m_b = b;
    msg.m_c = c;
    msg.m_d = d;
    msg.m_e = e;
    msg.m_f = g;   // param_7 -> +0x14
    msg.m_g = f;   // param_6 -> +0x18
    msg.m_result = result;
    SendSignal(0x3b, &msg, 0x6c);
