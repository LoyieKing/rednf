// ORIG T 0x08650aa2 sz0x87f：SIG_SAVE_CHARAC(0x10d) 组包；四个脏块分支后投递
// 队列 2 并清 save 标志。个别字段写入为报告偏移直写（推断标注见注释）。
// ORIG T 0x088921c2 `_Z11setBit_ByteRhbi`（定义在 BitTools.cpp；本 TU 唯一调用点在下）。
void setBit_Byte(unsigned char& value, bool set, int bit);
