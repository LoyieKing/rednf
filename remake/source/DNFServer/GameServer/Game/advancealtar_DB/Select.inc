// [P8S3 2026-09-11] P139 prett size 不等（ORIG 0x76 / 我方 0x77，证据：
//   python3 /tmp/r5/p8/prett6.py advancealtar_DB；pstr 逐字节核对）：
//   ORIG "bool advancealtar::DB_Load::Select(advancealtar::CharacAdvanceAltarDbData&)"
//   我方 "…(advancealtar::_CharacAdvanceAltarDbData&)" —— 修法需在 advancealtar.h
//   加 `typedef _CharacAdvanceAltarDbData CharacAdvanceAltarDbData;`（头改动 = 越界）
//   ⇒ 只报告 Main/P7HeaderAxis，本片未改。
bool DB_Load::Select(_CharacAdvanceAltarDbData& data)
