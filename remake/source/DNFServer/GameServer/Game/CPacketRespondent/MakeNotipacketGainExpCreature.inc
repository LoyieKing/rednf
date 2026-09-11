// ORIG MakeNotipacketGainExpCreature @ 0x08340760：clear → put_header(0,0x66)
// → put_byte(level)；
//   growFlag：put_byte(1)、put_int((int16)(exp>>16))、put_byte(10)、
//             put_byte(exp 低字节)；
//   else    ：put_byte(0)、put_int(exp)；
// 最后 finalize(true)。
void CPacketRespondent::MakeNotipacketGainExpCreature(int exp, int level,
