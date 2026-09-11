// ORIG ~CCharacter @ 0x834842a：destroy → 逆序析构 STSecondGrowTypeInfo[7]、
// vector[7]（成员析构由编译器按声明逆序生成，语义一致）。
CCharacter::~CCharacter()
{
    // ORIG ~CCharacter @ 0x834842a：先调 destroy()（→clear()）再析构成员。
    destroy();
}