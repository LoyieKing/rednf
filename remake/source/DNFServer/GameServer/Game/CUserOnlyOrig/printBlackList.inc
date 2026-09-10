void CUser::printBlackList()
{
    if (m_vec8cef4.empty()) return;
    char buf[1024] = {0};                                  // -0x518（P214③ 复现 rep stos）
    // [b28d-solo-CUserOnlyOrig P167/P50] ORIG 命中迭代器族
    // `begin/end/ne/de/ppEv` + `normal_iterator::C1Ev`（默认构造）⇒ 原码是
    // 「先声明后赋值」的迭代器遍历（旧索引形产 EXTRA operator[]/size）。
    std::vector<unsigned int>::iterator it;
    for (it = m_vec8cef4.begin(); it != m_vec8cef4.end(); ++it)
        sprintf(buf, "%s,%d", buf, *it);
    sprintf(buf, "%s\n", buf);
    char path[256] = {0};                                  // -0x118（P214③）
    sprintf(path, "./log/%s/BlackList",
            G_CEnvironment()->get_file_name());
    // [b28d-solo-CUserOnlyOrig P221] 具名 logFile 令 4.4 占最深槽 -0x18（it 被顶浅 -0x10，
    // ORIG 反之：it=-0x18/log=-0x10）⇒ 改全表达式临时链翻槽方向。
    CMyFileLog(__PRETTY_FUNCTION__, 0x7d57)(path, "%s", buf);
