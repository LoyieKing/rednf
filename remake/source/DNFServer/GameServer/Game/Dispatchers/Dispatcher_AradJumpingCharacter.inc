// ==================== Dispatcher_AradJumpingCharacter（ORIG 0x819f1ec）====================
// dispatch_sig：user==0→0x59b；getCurCharacR==0→0；读 byte+byte+short+int
//   （错误码 0x5a9/0x5ac/0x5af/0x5b1/0x5b4/0x5b6）；state∉[3,4]→0；
//   通过后发内部流 0x2d（0x11 字节，含 acc_id/charac_no）。
Dispatcher_AradJumpingCharacter::Dispatcher_AradJumpingCharacter() throw() {}
