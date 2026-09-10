int AuthGarenaToken::decrypt(const char* token, int len, GarenaAuthData& out)
{
    if (token == 0)
    {
        // [b28d-solo-D02] P221：具名 cMyTrace → 全表达式临时链（ORIG 0x08173222
        // 该 trace 占 -0x48，4.4 具名形占更深槽 ⇒ 反向）。
        cMyTrace(__FUNCTION__, 0x7a, 5)("[Taiwan, Auth] Token is null!");
        return 0x7b;
    }

    // [b28d-solo-D02] ORIG 0x08173244 把 token 复制到具名槽 -0x20（P0：有槽
    // 往返 ⇒ 原码该值有具名局部），后续 string 构造从该槽读取。
    const char* pToken = token;
    std::vector<unsigned char> encrypted;
    base64_decode(std::string(pToken), encrypted);
    if (encrypted.size() == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanAuth.cpp", __FUNCTION__, 0x83,
            "[Taiwan, Auth] Invalid token format!");
        return 0x84;
    }
    std::vector<unsigned char> decrypted;
    // [b28d-solo-D02] ORIG 0x08173334..0x0817335d 直取 &encrypted[0]（无三目
    // 判空 —— P171 照抄原码无防御形）。
    bool ok = openSSLRSA_.Decrypt(
        reinterpret_cast<const char*>(&encrypted[0]),
        static_cast<int>(encrypted.size()), decrypted);
    if (!ok)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanAuth.cpp", __FUNCTION__, 0x8b,
            "[Taiwan, Auth] Decrypt token failed!");
        return 0x8c;
    }

    if (decrypted.size() >= 0x69)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanAuth.cpp", __FUNCTION__, 0x92,
            "[Taiwan, Auth] Invalid decrypted token length! "
            "(buffer_size:%u, token_size:%u)",
            0x68, static_cast<unsigned int>(decrypted.size()));
        return 0x93;
    }

    memset(&out, 0, 0x68);
    // [b28d-solo-D02] ORIG 0x08173423..0x08173452 同形：&decrypted[0] 直取。
    memcpy(&out, &decrypted[0], decrypted.size() - 1);

    bool mobile = out.getMobileAuth();
    unsigned int gold = out.getGoldMemberType();
    unsigned short gca = out.getGcaType();
    unsigned int stamp = out.getTimeStamp();

    // [b28d-solo-D02] P221：同上（ORIG 0x081734da 该 trace 占 -0x30 浅槽）。
    // 另：ORIG 0x081734a8/0x081734b5 的 getUname/getUid 无槽（留 %edi/%esi 跨 ctor
    // 保活）⇒ 原码这两值是实参内联表达式（P0）。裸 register 具名形实测同形
    // （227=227，无改善，已回退）。
    cMyTrace(__FUNCTION__, 0x99, 0)(
        "[Taiwan, Auth] Success!! Uid:%u, UserName:%s, Timestamp:%u, "
        "GcaType:%u, GoldMemberType:%u, Mobile:%u",
        out.getUid(), out.getUname(), stamp, gca, gold,
        static_cast<unsigned int>(mobile));
    return 0;
