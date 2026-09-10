void Dispatcher_ObjectBringUp::send(CUser* user, ParamBase& param)
{
    const stObjectBringUp& out = static_cast<const stObjectBringUp&>(param);

    PacketGuard pg;                                         // ORIG 无 clear（同汇编）
    pg.put_header(1, 0x1d5);

    if (out.m_4 == 0)
    {
        pg.put_byte(1);
        pg.put_short(static_cast<unsigned short>(out.m_8));
        pg.put_byte(static_cast<unsigned int>(out.m_a));
        pg.put_byte(static_cast<unsigned int>(out.m_b));

        if (out.m_b != 0)
        {
            // +0x0c 处 vector<int>：每轮消费 4 个 int（short,int,int,short）
            const std::vector<int>& rewards =
                *reinterpret_cast<const std::vector<int>*>(
                    reinterpret_cast<const char*>(&out) + 0xc);
            std::vector<int>::const_iterator it = rewards.begin();
            while (it != rewards.end())
            {
                const short v0 = static_cast<short>(*it); ++it;
                const int v1 = *it; ++it;
                const int v2 = *it; ++it;
                const short v3 = static_cast<short>(*it); ++it;
                pg.put_short(v0);
                pg.put_int(v1);
                pg.put_int(v2);
                pg.put_short(v3);
            }
        }
    }
    else
    {
        pg.put_byte(0);
        pg.put_byte(out.m_4 & 0xff);
    }

    pg.finalize(true);
    user->Send(pg);
