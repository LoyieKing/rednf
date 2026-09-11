// ---------------------------------------------------------------------------
// AttachSessionInExchangeServer            ORIG 0x8491296
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：P221/P227 临时链复现 serverId 跨 ctor 保活 + trace 槽 -0x18。
bool CExchangeServer::AttachSessionInExchangeServer(int serverId,
                                                    CSession* session)
{
    // [b28d-48 复核] 维持 N93附+P75（NDIFF 53，53/55 条）：iterator 对槽深
    // 方向 + session 形参跨 cMyTrace ctor 保活差。
    // [b28c-6] fresh 复验维持 N93附+P75（ndiff 22，55/53 条）：
    // [b28-L11 判 N·N93附+P75] 残差两类：①函数级 iterator 对(-0x20/-0x1c)与
    // if 尾块内 cMyTrace(-0x18) 的槽深方向跨版本相反（N93 附：兄弟/块内对象
    // 分配方向，P212 头部声明法对带参 ctor 类不可用）；②ORIG push %ebx 将
    // session 形参跨 cMyTrace ctor 调用保活（`mov %ebx,0x8(%esp)`），4.4 恒在
    // 组参点重读形参槽（P75/N82附 多实参首值寄存器对调）——ndiff 22 封顶。
    // [K2 2026-09-11 复核] fresh 165/166（NEAR）：唯一残差 = ①3 处 `lea -0x3f8a5(%ebp)`
    // vs 我方 `-0x3f8a8`（CACHE_CHARACTER_TYPE 局部落 3 字节深：ORIG 槽基准为奇数 ⇒ 该
    // 类型/局部对齐为 1，我方为 4/8 ⇒ **属 CacheCharacterType.h 侧（他人写域，已上报）**）；
    // ②两处 SendResponseCharacInfo 实参装载角色（ORIG `mov -0x44,%ecx;…mov -0x1c,%ecx` vs
    // 我方 `mov -0x44,%ebx; mov -0x1c,%ecx`，P200/P231 register 复本对「已物化 callee-saved
    // 链」无效）⇒ N18a/N7 封顶。
    session->SetChannelNo(serverId);
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it != m_serverMap.end())
    {
        it->second.m_pSession = session;
        return true;
    }
    // P221/P227 临时链：复现 ORIG 0x8491314 `mov 0xc(%ebp),%ebx` + trace 槽 -0x18。
    cMyTrace(__PRETTY_FUNCTION__, 0x11b, 0)
        ("AttachSessionInExchangeServer() channel_no(%d) failed", serverId);
    return false;
}