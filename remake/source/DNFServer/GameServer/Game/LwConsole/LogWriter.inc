// [b28c-35] P210②：ORIG nm 实证 LogWriter::C1/C2 @0x08ad3990 与 LwConsole::C1
// @0x08ad39e2 同 TU 且定义在先 ⇒ 4.4 精确继承 outgoing 帧 `sub $0x4`；仅声明时
// 保守 `sub $0x18`。故 LogWriter::C1 的 out-of-line 空体定义从 LogManager.cpp
// 迁入本 TU（caller 之前）；符号全局仍唯一（探针 /tmp/l35_probe_frame.cpp）。
LogWriter::LogWriter()
{
}