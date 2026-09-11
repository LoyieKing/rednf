// ORIG _ZN5CUser15reqHumanCertifyEv T 0x0867f26e（0x252 字节）：未认证时发起
// CleanPad 人工认证——记日志/暴露计数、从验证队列随机取一条填 0x7d 包
// （byte 5 + int + binary + 认证种子三段 + 错误计数 + 0），发送后记 tick。
// [b28d-GrpD 判词·等价 N] ORIG 0x0867f26e：唯一实差异是 put_binary 实参照搬对中
//   的寄存器选择（ORIG `mov 0xa(%eax),%eax; mov -0xc(%ebp),%edx; add $0xe,%edx` vs
//   我方 `mov 0xa(%eax),%edx; mov -0xc(%ebp),%eax; add $0xe,%eax`）—— 同址同值等价；
//   其余为未解析重定位。字段偏移（msg+0x2/+0xa、m_capchaCertify 0xa 字节）与
//   GenerateCleanPadVeriData 三出参序全一致 ⇒ 无语义缺口。
bool CUser::reqHumanCertify()
{
    if (!isHumanCertified())
    {
        return false;
    }
    m_historyLog.RequestCleanPad(m_mcap.getPoint(), 0);
    m_mcap.incExposedCount();
    if (GlobalData::s_capchaDataManager.isEmpty())
    {
        return false;
    }
    MSG_UDP_CAPCHA_CERTIFY* msg = GlobalData::s_capchaDataManager.peekData();
    memcpy(m_capchaCertify, msg->m_text, 0xa);

    PacketGuard pg;
    pg.put_header(0, 0x7d);
    pg.put_byte(5);
    pg.put_int(msg->m_size);
    // ORIG 0x867f366-0x867f376：len→%eax、ptr→%edx —— 无具名槽（P0：msg->m_size
    // 表达式直传；历史 register asm 钉法已按 P0/N96 清绑，改内联实参）。
    pg.put_binary((const char*)msg->m_image, msg->m_size);
    pg.put_byte(m_mcap.getPoint() > 0x251b ? 1 : 0);

    unsigned short veriA = 0;
    unsigned char veriB = 0;
    unsigned short veriC = 0;
    GenerateCleanPadVeriData(veriA, veriB, veriC);
    pg.put_short(veriA);
    pg.put_byte(veriB);
    pg.put_short(veriC);
    pg.put_byte((unsigned char)m_humanCertifyErrorCnt);
    pg.put_byte(0);
    pg.finalize(true);
    Send(pg);
    m_mcap.m_capchaReqTick = GlobalData::s_systemTime_.getCurTickCount();
    return true;
}