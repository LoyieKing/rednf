void CItemGeneratorMgr::setDropRatio(float ratio)
{
    // ORIG 0x8534278：遍历 i=0..7，虚槽 +0x00（setDropRatio，权威头已落地）。
    // ORIG 0x8534287-0x85342b3：m_generators[i] 被读两遍（0x8534293 取 vtable、
    // 0x85342a6 再取作 this），无具名 gen 局部 ⇒ i 独占 -0xc。
    for (int i = 0; i < 8; ++i)
        m_generators[i]->setDropRatio(ratio);
