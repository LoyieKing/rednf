STAIInfoScript::~STAIInfoScript()
{
    // ORIG D1 W 0x083671a4：写 vtable 后先 destroy() 释放 pattern 桶，
    // 随后编译器按声明逆序析构成员。
    destroy();
}