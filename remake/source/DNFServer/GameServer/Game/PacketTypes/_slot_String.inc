// ORIG _slot_String 成员函数（0x8239e66/0x8239f16 实证）：word@+0、string@+2。
//   copy ctor / op= 首 2 字节均走一次 16 位字拷贝（movzwl (%eax),%edx; mov %dx,(%eax)），
//   m_slot 现为单一 unsigned short（PacketTypes.h 代落），两连并行字拷贝即 byte-identical。
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String::_slot_String() {}
