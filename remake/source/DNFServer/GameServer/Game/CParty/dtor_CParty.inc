CParty::~CParty()
{
    // [b28d-solo-CParty2] 7 个 pad 区已字段化 ⇒ 手写析构 cast 全删（重复析构 = 真 bug）；
    // 各子对象由编译器在成员析构阶段按声明逆序自动调用 D1（与 ORIG D2Ev 同址别名）。
    // 残余 = N104（ORIG 每个析构带异常中继块，4.1/4.4 EH 布局差）。@0x082a57fe
