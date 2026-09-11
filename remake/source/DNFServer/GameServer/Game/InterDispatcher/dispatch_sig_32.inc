// [b28d-lane2] verdict=NEAR ndiff=20（ORIG 0x084cefd2）：槽序/帧差(N82/P212/P221)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=42（ORIG 0x84cefd2）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorEventItemUpdate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [P339·2026-09-11] 全头部声明 + 声明序 = ORIG 槽深→浅：ORIG pPkt@-0x1c（浅）、
    //   v1a@-0x40 / v16@-0x3c（深）⇒ 4.4 头部声明恒「先声明→更深槽」，故 v1a/v16 先声明、
    //   pPkt 后声明（赋值语句序保持 ORIG：pPkt → v1a → v16）。
    unsigned int v1a;
    unsigned int v16;
    Inter_MonitorEventItemUpdatePktView* pPkt = reinterpret_cast<Inter_MonitorEventItemUpdatePktView*>(data);
    // ORIG 0x84cefd2：m_1a/m_16 落槽（-0x40/-0x3c），m_e/m_12/NumberToString
    // 经 block-scope register 直落 %edi/%esi/%ebx 跨 cMyTrace ctor 缓存（P36）。
    v1a = pPkt->m_1a;
    v16 = pPkt->m_16;
    register unsigned int v0e = pPkt->m_e;
    register unsigned int v12 = pPkt->m_12;
    register char* mid = NumberToString(pPkt->m_a, 0);
    cMyTrace(__PRETTY_FUNCTION__,
        // [H2·2026-09-11] §6.5 语义轴：ORIG 实字节为 EUC-KR
        //   （`\xc0\xcc\xba\xa5\xc6\xae \xc0\xaf\xc0\xfa DB ID : ...`），旧写 UTF-8 ⇒ 发射字节不同。
        0x2bff, 0)("[%s][%d],\xc0\xcc\xba\xa5\xc6\xae \xc0\xaf\xc0\xfa DB ID : %s [EventType: %d][CharacNo: %d][ItemID: %d][Stack: %d]\n",
          __PRETTY_FUNCTION__,
          0x2bff,
          mid,
          v12,
          v0e,
          v16,
          v1a);
    WongWork::CWebEvent::RecvInfoFromWeb(
        pPkt->m_a,
        pPkt->m_e,
        pPkt->m_16,
        pPkt->m_1a,
        pPkt->m_12,
        pPkt->m_1e);
    return 0;

// [G2B-Monitor] 残余留 N：本件 diff 仅 2 项（`sub $0x6c` vs `sub $0x5c` / 对应 epilogue），
//   ndiff=4、ratio 0.971，指令序列其余逐条相同 ⇒ 是 ORIG 多 0x10 字节帧空间。
//   方向与我方「多具名局部」相反（本例我方**少** 16B），本件亦无我方独有槽
//   （v1a/v16 正是 ORIG 的 -0x40/-0x3c），16B 归属未定（疑 cMyTrace 临时对象/局部
//   register 物化档），无源码旋钮可判 ⇒ 留 N，不改（避免无据试错）。
}