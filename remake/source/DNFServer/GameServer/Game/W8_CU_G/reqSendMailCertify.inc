// ORIG 0x0868a51a：向客户端下发人机认证图（capcha 队列随机一条 + 新鲜种子三段
// + 错误计数），并重置请求态/失败标志与 60 秒认证超时定时器（TIMER_MESSAGE 9）。
// [b22判词·N] ORIG 155 vs 重建 154：唯一真差异在 memcpy 首参（dst = this+0x8e0ec，
// m_capchaCertify 数组）地址物化——ORIG 4.1.2 `mov 0x8(%ebp),%edx; add $0x8e0ec,%edx`
// （mov+add 两条），4.4.7 折叠为 `lea 0x8e0ec(%eax),%edx`（单条），差 1 条属跨版本
// 寻址伪影（范式库 memcpy/memset 首参 lea-vs-add 族，禁 register asm 强凑）。其余指令/
// 控制流/栈槽/EH landing pad 全对齐；call 目标占位、数据地址 `movl $0x0,(%esp)`
// （&s_capchaDataManager）、字符串立即数为 --no-compile .o 重定位假阳性（§6.3），
// relink 后转 AE。语义等价。
// b28d-W8(2026-09-09): DIFF 155/154（gap 1）— 本波修**真 bug**（p167 重载选错）：
// `put_binary(reinterpret_cast<char*>(m_image), size)` 选中 `EPci` 非 const 重载；
// ORIG 调 `_ZN18InterfacePacketBuf10put_binaryEPKci`（const char*）⇒ 改
// `reinterpret_cast<const char*>`，p167 转 CLEAR。余 1 条 N 族：
// ①memcpy 首参 dst(this+0x8e0ec) ORIG `mov;add` 两步 vs 4.4 `lea` 单条（N107 负例）；
//    b28d-W8b 复测 `&m_capchaCertify[0]` / `reinterpret_cast<char*>(&数组)` 两形
//    仍产 lea（154 不变）⇒ 封 N；
// ②+0x8e0f8 成员读 ORIG `mov mem,%eax; mov %eax,%esi` 两段 vs 4.4 单段（N18a）；
//    b28d-W8b 复测实参 `static_cast<unsigned int>(m_humanCertifyTimerKey)` 形
//    无效（154 不变）⇒ 封 N。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N107，gap=+1/blocks=4）→ 试=①prett/q3_gap2 缺口 0、
//   strall CLEAR ②逐 hunk 定性：(a) `this+0x8e0ec` 取址 ORIG `mov 0x8(%ebp),%edx;
//   add $0x8e0ec,%edx`（2 条、this 直落目标寄存器后原地 add）vs 4.4 `mov 0x8(%ebp),%eax;
//   lea 0x8e0ec(%eax),%edx`（目标寄存器不同 ⇒ lea 单条）——对照同函数 (b) 段
//   `this+0xe`（ORIG/我方**两侧皆 add**）⇒ add↔lea 由**目标寄存器是否等于基址寄存器**决定，
//   属 4.1/4.4 寄存器选择差（N107 负例方向）；(c) `mov 0x8e0f8(%eax),%eax; mov %eax,%esi`
//   = 4.1 冗余 regmove（同 GiveEquipItemsReturnUser 惯例）⇒ 维持 N（155/154，gap +1）。
bool CUser::reqSendMailCertify()   // [b28d-Main·P292] unsigned int→bool
{
    if (GlobalData::s_capchaDataManager.isEmpty())
    {
        return 0;
    }

    MSG_UDP_CAPCHA_CERTIFY* data = GlobalData::s_capchaDataManager.peekData();
    // b28d-W8b 复测（均无效，保持原形）：ORIG 0x0868a557 `mov 0x8(%ebp),%edx;
    // add $0x8e0ec,%edx` 两步取址 vs 4.4 `lea 0x8e0ec(%eax),%edx` 单条
    //（N107 负例方向）；`&m_capchaCertify[0]` / `reinterpret_cast<char*>(&数组)`
    // 两形实测均仍产 lea（154 不变）⇒ 封 N。
    memcpy(m_capchaCertify, data, 10);

    PacketGuard packet;
    packet.put_header(0, 0x7d);
    packet.put_byte(5);
    packet.put_int(data->m_size);
    // b28d-W8 真 bug（p167 重载选错）：ORIG 调 `put_binary(char const*, int)`
    //（_ZN18InterfacePacketBuf10put_binaryEPKci）；我方 `reinterpret_cast<char*>`
    // 选中非 const 重载 `put_binary(char*, int)`（EPci）⇒ 显式 const 版本。
    packet.put_binary(reinterpret_cast<const char*>(data->m_image), data->m_size);
    packet.put_byte(0);

    unsigned short veriA = 0;
    unsigned char veriB = 0;
    unsigned short veriC = 0;
    GenerateCleanPadVeriData(veriA, veriB, veriC);
    packet.put_short(static_cast<int>(veriA));
    packet.put_byte(static_cast<int>(veriB));
    packet.put_short(static_cast<int>(veriC));

    // ORIG 0x086530xx：`mov 0x8e0fc(%eax),%eax; movzbl %al,%eax` = int 字段
    // **uchar→int 双段截断**形（probe p18.z1：`(int)(unsigned char)` 直落产
    // movzbl %al；`& 0xff` 形产 and——两 lowering 指令名不同）。
    packet.put_byte(static_cast<int>(static_cast<unsigned char>(m_humanCertifyErrorCnt)));
    packet.put_byte(1);
    packet.finalize(true);
    Send(packet);

    setHumanCertified(false);
    m_sendMailCertified = false;

    if (static_cast<unsigned int>(m_humanCertifyTimerKey) > 0x7ffffffeu)
    {
        m_humanCertifyTimerKey = 0;
    }
    ++m_humanCertifyTimerKey;

    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                static_cast<int>(GetUID()),
                                static_cast<TIMER_MESSAGE>(9), 0x3c,
                                m_humanCertifyTimerKey, 0xffffffffu);
    return 1;
}