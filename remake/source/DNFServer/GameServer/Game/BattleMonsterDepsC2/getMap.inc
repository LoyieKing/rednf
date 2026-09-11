// [W13E 2026-09-11] ORIG 0x08300ed4 返回 const CMap*（prett 串直证）⇒ 定义侧
// 签名同步；函数体内部仍用非 const 局部再隐式转 const 返回（机器码不变）。
const CMap* CBattle_Field::getMap(int mapIndex)
