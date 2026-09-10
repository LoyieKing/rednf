bool ImportCharacterScript(STCharacterScript* script, const char* path);

// ---- STAICharacterCommonScript 权威布局视图（sizeof 0x1010 —— init 栈帧
//      -0x104c..-0x3c 实证；CAICharacter::set 访问最远字段 +0x100c。
//      sync_script.h 现存 8 字节占位与本视图冲突，故本 TU 不含该头。
//      CAICharacter::set 符号由 sub_CAISet asm 别名提供（见下））----
