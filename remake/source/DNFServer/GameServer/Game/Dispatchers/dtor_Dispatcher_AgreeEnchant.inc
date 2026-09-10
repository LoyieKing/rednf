Dispatcher_AgreeEnchant::~Dispatcher_AgreeEnchant() {}
// ORIG 0x0820adc6（报告 Dispatcher_AgreeEnchant/dispatch_sig.md）：
// get_packet(0) → MSG_STATIC_COMMAND*；curCharac==0 → LineFunc 0xa6f1；
// expertJob==0 / GetType()!=1 → 静默 return 0；否则 CEnchanter::OnAgreeEnchant。
// [b28c-31] AE：P0 内联表达式（删 job/enchanter 具名局部）+ cmd 槽 -0xc 回归。
