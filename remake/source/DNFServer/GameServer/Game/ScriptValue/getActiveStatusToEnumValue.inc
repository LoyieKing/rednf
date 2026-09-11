int getActiveStatusToEnumValue(std::string& s);        // 0x88c29a2

// [b28d-ScriptValue 判据㉓ 符号绑定] ORIG 五个符号全为 **W**（弱绑定）：
//   C1/C2 0x088c4200 W 0x38、C1ERKS_/C2ERKS_ 0x083661e2 W 0x3e、aSERKS_ 0x08366220 W 0x41；
// 我方原为 T（强绑定，唯一 .cpp 外联定义）。ORIG 的两处地址分布说明其定义在头内、
// 由各使用点发射；按 Main 批准的「属主对自己 TU 内定义加 __attribute__((weak))」
// 单点修法（SweepCipher 广播 判据㉓ 清单口径），此处三处定义加 weak ⇒ 绑定对齐 W，
// 尺寸/函数体/mangled 零变化（本类全仓仅本 TU 定义，无覆盖风险）。
// ORIG 0x088c4200：type=3（空标记），value[0..3]=0（循环 0..3）。
__attribute__((weak)) ScriptValue::ScriptValue()
{
    type = 3;
    for (int i = 0; i < 4; ++i)
        value[i] = 0.0f;
}