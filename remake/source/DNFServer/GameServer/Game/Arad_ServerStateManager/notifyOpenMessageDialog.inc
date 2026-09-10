bool notifyOpenMessageDialog(CUser* user, int a, int b,
                             const AradServerStateMessage& msg);

// ---------------------------------------------------------------------------
// SSendEffect —— sendState 内部 for_each 函子。
//   C1 0x081a9dac（存 user）；D1 0x081a9dba（空体）；
//   operator()(const pair&) 0x081a9dc0：
//     notifyOpenMessageDialog(m_user, 1, (int)pair.first, pair.second)。
// ---------------------------------------------------------------------------
