int Dispatcher_ChangeHost::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// read() 直接依赖（tsv 清单外）：G_CDataManager()+0x14 指向的基址 +0x48fc
// 为 vector<pair<int,int>>（ComboSkill.cpp 同源布局视图：6 组组合技表首列），
// 线性查 first==idx 命中返回 1，否则 0。
