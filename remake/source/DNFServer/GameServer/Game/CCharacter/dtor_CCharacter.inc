CCharacter::~CCharacter()
{
    // ORIG ~CCharacter @ 0x834842a：先调 destroy()（→clear()）再析构成员。
    destroy();
