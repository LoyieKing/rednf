// ORIG T 0x081731f2
// [b28d-solo-D02] verdict=DIFF（227 vs 229，N95/N104 族封顶）。本轮已修：
// ①两处 `size()?&v[0]:0` 三目删（P171 直取，ORIG 0x08173334/0x08173423）；
// ②日志参数去 `&0xffff`/`&0xff` 冗余掩码（ORIG movzbl/movzwl 已零扩展）；
// ③两处 cMyTrace 改全表达式临时链（P221：失败 trace 槽 -0x64→-0x4c）；
// ④getUname/getUid 改实参内联（P0：ORIG 无槽、值留 %edi/%esi 跨 ctor）；
// ⑤token 具名局部照抄（ORIG 0x08173244 `mov %eax,-0x20`）。
// 残差 = ①标量槽区分配序（ORIG ok -0x19/token -0x20/gold -0x74/gca -0x70/
// stamp -0x6c 与我方相反 ⇒ 4.1 先声明深 vs 4.4 先声明浅，N95/N126）；
// ②帧 0x8c vs 0x80 + %edi push 缺失（4.4 不把 uname 升格 callee-saved；
// 全表达式形已是 P226/P227 上限）；③EH 清理链块序（N104）。
// p167 仅 C1/C2、D1/D2 构造/析构变体差（clean 归一口径 CLEAR）⇒ 非缺段。
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
}