_Quest_Authen_Data::_Quest_Authen_Data() throw() { reset(); }  // throw()：ORIG UserQuest C1 0x8697b8a 无 EH pad
// ORIG reset 0x0814aa16 逐字段清：movw +0/+2、movl +8、movb +0xc、movl +0x10/+0x14
// （不动 +0x04/+0x18 起字段）；旧 memset 0x48 语义与形态均不符。
