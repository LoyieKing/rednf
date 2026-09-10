WeaponInfo::WeaponInfo() { clear(); }

// ORIG STRDCharacterStatus::Clear W 0x08367642（objdump 全文转写，无独立报告）：
// +0x52 字节与 +0x53 dword 不在 [HP MAX]/[weight] 等导入写入区（仅清零）。
