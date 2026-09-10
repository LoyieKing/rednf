void Dispatcher_RequestIngameAdvertisement::send(CUser* user, ParamBase& param)
{
    // 具名访问：stRequestIngameAdvertisement（PacketTypes.h 权威布局，总长 0x648）。
    // 字符串区落冻结头 pad 区（已 hub 报 Main 补具名串字段），此处以具名成员为基址。
    stRequestIngameAdvertisement& R =
        reinterpret_cast<stRequestIngameAdvertisement&>(param);
    const int err = R.m_errorCode;                                  // +0x640

    if (err == 0x7fffffff)
    {
        return;                                             // check_error 未跑（state 门早退）
    }

    if (err != 0)
    {
        // [b28d-solo-Dispatchers] ORIG 两个独立 PacketGuard：错误路径 -0x20
        //   （C1Ev 0x081e4701 / D1Ev 0x081e4a7d+0x081e4a94 EH），成功路径 -0x2c
        //   （C1Ev 0x081e4aa4）。p167 旧态 MISSING C2×2/D2×4/_Unwind_Resume×1、
        //   EXTRA C1×1/D1×2 ⇒ 原码是**两个分支各自作用域的 guard**，非函数级复用。
        // ---- 错误回包：byte 0 + err + type ----
        PacketGuard pg;
        pg.clear();                                         // InterfacePacketBuf::clear
        pg.put_header(1, 0x1e9);
        pg.put_byte(0);
        pg.put_byte(err & 0xff);
        pg.put_short(R.m_adType);                                   // +0x644
        pg.finalize(true);
        user->Send(pg);
        return;
    }

    // ---- 成功回包 ----
    PacketGuard pg;                                         // ORIG -0x2c
    pg.clear();
    pg.put_header(1, 0x1e9);
    pg.put_byte(1);
    pg.put_int(static_cast<int>(R.m_code));                         // +0x004
    pg.put_short(R.m_adType);                                       // +0x644
    // 以下 m_adShape/m_logFlag/m_prizeShape 与文本区字段同属 +0x04 内嵌通知信息块
    pg.put_byte(static_cast<signed char>(R.m_adShape));             // +0x33b

    {
        const int descLen =
            static_cast<int>(strlen(R.m_pad08));                    // +0x008
        pg.put_int(descLen);
        pg.put_str(R.m_pad08, descLen);
    }

    pg.put_byte(static_cast<signed char>(R.m_prizeShape));          // +0x33d

    if (R.m_prizeShape == 0)
    {
        const int textLen =
            static_cast<int>(strlen(R.m_pad33e));                   // +0x33e
        pg.put_int(textLen);
        pg.put_str(R.m_pad33e, textLen);

        const unsigned char kind =
            R.m_adShape;                                            // +0x33b
        if (kind == 0)
        {
            pg.put_int(static_cast<int>(R.m_itemID2));              // +0x53c
        }
        else if (kind == 1)
        {
            const int len =
                static_cast<int>(strlen(R.m_pad33e + 0xff));        // +0x43d
            pg.put_int(len);
            pg.put_str(R.m_pad33e + 0xff, len);
        }
        else if (kind == 3)
        {
            const int len =
                static_cast<int>(strlen(R.m_pad540));               // +0x540
            pg.put_int(len);
            pg.put_str(R.m_pad540, len);
        }
    }
    else
    {
        const int textLen =
            static_cast<int>(strlen(R.m_pad08 + 0x32));             // +0x03a
        pg.put_int(textLen);
        pg.put_str(R.m_pad08 + 0x32, textLen);

        const unsigned char kind =
            R.m_adShape;                                            // +0x33b
        if (kind == 0)
        {
            pg.put_int(static_cast<int>(R.m_itemID));               // +0x238
        }
        else if (kind == 1)
        {
            const int len =
                static_cast<int>(strlen(R.m_pad08 + 0x131));        // +0x139
            pg.put_int(len);
            pg.put_str(R.m_pad08 + 0x131, len);
        }
        else if (kind == 3)
        {
            const int len =
                static_cast<int>(strlen(R.m_pad23c));               // +0x23c
            pg.put_int(len);
            pg.put_str(R.m_pad23c, len);
        }
    }

    pg.put_byte(static_cast<signed char>(R.m_logFlag));             // +0x33c
    pg.finalize(true);
    user->Send(pg);
