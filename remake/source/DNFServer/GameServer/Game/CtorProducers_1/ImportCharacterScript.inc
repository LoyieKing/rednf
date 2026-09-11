// ImportCharacterScript（ORIG T 0x08a0ce06，定义 ScriptDepsA.cpp）——CCharacList::init 真实调用。
// §4.5 正解：普通 C++ 自由函数前置声明（非 extern "C"），include STCharacterScript.h 取真实类型。
bool ImportCharacterScript(STCharacterScript* script, const char* path);
