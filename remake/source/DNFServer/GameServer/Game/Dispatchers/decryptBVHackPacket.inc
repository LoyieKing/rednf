// 2026-09-11 K1DispG1（P248 档①）：ORIG 调用点 0x0820766c `call; xor $0x1,%eax;
// test %al,%al; je`（无 setne）⇒ callee 返回类型是 bool（返回类型不入 mangled，
// 定义侧 SDCLink.cpp 语义不变）。旧 int 声明在调用点产 `test %eax; sete %al`。
bool decryptBVHackPacket(char* payload, stBVHackInfo_t& info);
