void EncodeDressStringWide(const wchar_t* in, wchar_t* out,
                           unsigned long param);                            // _ZN10Encryption21EncodeDressStringWideEPKwPwm
// [b28d-Main·检查点22] 删除原先的 `__attribute__((weak))` 空 stub：
//   ① 二者在 ORIG 里是 **U 引用**（`nm $O` = U，由 NEEDED `libnxencryption.so`
//      在运行期提供；0x0807e190/0x0807e8e0 为 PLT 调用点），我方在 Game/ 源码内
//      发射 W 定义 ⇒ 属「我方 .cpp 发明导出符号」（only_new 2 项）。
//   ② 空 stub 运行期返回 0/不写 out，与厂商实现不一致（行为差异）。
//   现由 `toolchains/nxencryption/libnxencryption.so`（厂商真实二进制，已入仓）
//   参与链接：本 TU 只留声明 ⇒ 我方 df_game_r 变成 U + NEEDED libnxencryption.so，
//   与 ORIG readelf/nm 完全同型。
