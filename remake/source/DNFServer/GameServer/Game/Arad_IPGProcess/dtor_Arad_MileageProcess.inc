Arad_MileageProcess::~Arad_MileageProcess() {}

// ---------------------------------------------------------------------------
// BuySuccess —— ORIG T 0x0819e220（报告 BuySuccess.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] AE（200=200）：①kResultCode/kGiftFlag 改非 const 局部
// （ORIG 从槽 `movsbl -0x9` / `mov -0x10` 读，const 被前端折叠成立即数；同 BuyFail
// b28c-28 判据）；②迭代器 end 缓存（P13：end 临时 -0x14 + 转换构造 -0x2c）；
// ③getCurCharacInvenR() 两次独立调用（P48，原缓存 inven 少一次调用）。
