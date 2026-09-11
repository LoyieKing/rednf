// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CtorProducers_2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CtorProducers_2.cpp — Ctor_Group2 批次：构造/装载 producer 真实实现
//
// 本 TU 提供以下 ORIG 符号（全部经 docs/class_func_reports 报告 + objdump 转写）：
//   DisjointScript         C1 0x08375d4c / clear 0x08375bca / D1 0x0837dff8
//   DisJoint               C1 0x0837e532 / LoadDisJoint T 0x0847300c
//   StrikerScript          getInsance/ctor/dtor/clear/create/destory（instance_
//                          B 0x09500978）/ importScript 系列声明见头，定义待续批
//   CharacManageScript     instance_ B 0x094f6b98 / getInsance/ctor/dtor/clear/importScript
//   SDCActiveStatusSyncCommon ctor W 0x08234870 / dtor W 0x08234876（空体）
//   SDCActiveStatusSyncServer ctor W 0x0823487c / dtor W 0x08234890 /
//                          loadActiveStatusScript T 0x0808e13c
//
// 不在本 TU 提供（避免链接重复，归属既有注册 TU）：
//   LotteryInfo C1/clear/D1 → CStackableItem.cpp 已定义（ORIG 0x083706d4/069c/4cfc）。
//
// 声明先行（定义待对应域批次，见交付报告）：
//   importDisjointScript / importCompoundAvatarTable / importActiveStatusTable /
//   ContentsType::getType / importSkillDataUp
// ==================== ANON-PAD (P203 `._381` 对齐) ====================
// [GiantB 2026-09-11] TU 匿名类型计数器只增不减；DisJoint.h 内 m_additional 的
// 自然编号实测 = 48（`_ZN14DisjointScript4._48C1Ev`），ORIG 目标 = `._381`
// ⇒ 在首个 include 前垫 333 个零符号哑匿名类型（每个占 1 编号，不发射符号）。
// 消费点 ScriptDepsA.cpp/CDataManager.cpp 各自独立计数（另测）。
typedef struct { char m_pad0; } AnonymousPad000;
typedef struct { char m_pad1; } AnonymousPad001;
typedef struct { char m_pad2; } AnonymousPad002;
typedef struct { char m_pad3; } AnonymousPad003;
typedef struct { char m_pad4; } AnonymousPad004;
typedef struct { char m_pad5; } AnonymousPad005;
typedef struct { char m_pad6; } AnonymousPad006;
typedef struct { char m_pad7; } AnonymousPad007;
typedef struct { char m_pad8; } AnonymousPad008;
typedef struct { char m_pad9; } AnonymousPad009;
typedef struct { char m_pad10; } AnonymousPad010;
typedef struct { char m_pad11; } AnonymousPad011;
typedef struct { char m_pad12; } AnonymousPad012;
typedef struct { char m_pad13; } AnonymousPad013;
typedef struct { char m_pad14; } AnonymousPad014;
typedef struct { char m_pad15; } AnonymousPad015;
typedef struct { char m_pad16; } AnonymousPad016;
typedef struct { char m_pad17; } AnonymousPad017;
typedef struct { char m_pad18; } AnonymousPad018;
typedef struct { char m_pad19; } AnonymousPad019;
typedef struct { char m_pad20; } AnonymousPad020;
typedef struct { char m_pad21; } AnonymousPad021;
typedef struct { char m_pad22; } AnonymousPad022;
typedef struct { char m_pad23; } AnonymousPad023;
typedef struct { char m_pad24; } AnonymousPad024;
typedef struct { char m_pad25; } AnonymousPad025;
typedef struct { char m_pad26; } AnonymousPad026;
typedef struct { char m_pad27; } AnonymousPad027;
typedef struct { char m_pad28; } AnonymousPad028;
typedef struct { char m_pad29; } AnonymousPad029;
typedef struct { char m_pad30; } AnonymousPad030;
typedef struct { char m_pad31; } AnonymousPad031;
typedef struct { char m_pad32; } AnonymousPad032;
typedef struct { char m_pad33; } AnonymousPad033;
typedef struct { char m_pad34; } AnonymousPad034;
typedef struct { char m_pad35; } AnonymousPad035;
typedef struct { char m_pad36; } AnonymousPad036;
typedef struct { char m_pad37; } AnonymousPad037;
typedef struct { char m_pad38; } AnonymousPad038;
typedef struct { char m_pad39; } AnonymousPad039;
typedef struct { char m_pad40; } AnonymousPad040;
typedef struct { char m_pad41; } AnonymousPad041;
typedef struct { char m_pad42; } AnonymousPad042;
typedef struct { char m_pad43; } AnonymousPad043;
typedef struct { char m_pad44; } AnonymousPad044;
typedef struct { char m_pad45; } AnonymousPad045;
typedef struct { char m_pad46; } AnonymousPad046;
typedef struct { char m_pad47; } AnonymousPad047;
typedef struct { char m_pad48; } AnonymousPad048;
typedef struct { char m_pad49; } AnonymousPad049;
typedef struct { char m_pad50; } AnonymousPad050;
typedef struct { char m_pad51; } AnonymousPad051;
typedef struct { char m_pad52; } AnonymousPad052;
typedef struct { char m_pad53; } AnonymousPad053;
typedef struct { char m_pad54; } AnonymousPad054;
typedef struct { char m_pad55; } AnonymousPad055;
typedef struct { char m_pad56; } AnonymousPad056;
typedef struct { char m_pad57; } AnonymousPad057;
typedef struct { char m_pad58; } AnonymousPad058;
typedef struct { char m_pad59; } AnonymousPad059;
typedef struct { char m_pad60; } AnonymousPad060;
typedef struct { char m_pad61; } AnonymousPad061;
typedef struct { char m_pad62; } AnonymousPad062;
typedef struct { char m_pad63; } AnonymousPad063;
typedef struct { char m_pad64; } AnonymousPad064;
typedef struct { char m_pad65; } AnonymousPad065;
typedef struct { char m_pad66; } AnonymousPad066;
typedef struct { char m_pad67; } AnonymousPad067;
typedef struct { char m_pad68; } AnonymousPad068;
typedef struct { char m_pad69; } AnonymousPad069;
typedef struct { char m_pad70; } AnonymousPad070;
typedef struct { char m_pad71; } AnonymousPad071;
typedef struct { char m_pad72; } AnonymousPad072;
typedef struct { char m_pad73; } AnonymousPad073;
typedef struct { char m_pad74; } AnonymousPad074;
typedef struct { char m_pad75; } AnonymousPad075;
typedef struct { char m_pad76; } AnonymousPad076;
typedef struct { char m_pad77; } AnonymousPad077;
typedef struct { char m_pad78; } AnonymousPad078;
typedef struct { char m_pad79; } AnonymousPad079;
typedef struct { char m_pad80; } AnonymousPad080;
typedef struct { char m_pad81; } AnonymousPad081;
typedef struct { char m_pad82; } AnonymousPad082;
typedef struct { char m_pad83; } AnonymousPad083;
typedef struct { char m_pad84; } AnonymousPad084;
typedef struct { char m_pad85; } AnonymousPad085;
typedef struct { char m_pad86; } AnonymousPad086;
typedef struct { char m_pad87; } AnonymousPad087;
typedef struct { char m_pad88; } AnonymousPad088;
typedef struct { char m_pad89; } AnonymousPad089;
typedef struct { char m_pad90; } AnonymousPad090;
typedef struct { char m_pad91; } AnonymousPad091;
typedef struct { char m_pad92; } AnonymousPad092;
typedef struct { char m_pad93; } AnonymousPad093;
typedef struct { char m_pad94; } AnonymousPad094;
typedef struct { char m_pad95; } AnonymousPad095;
typedef struct { char m_pad96; } AnonymousPad096;
typedef struct { char m_pad97; } AnonymousPad097;
typedef struct { char m_pad98; } AnonymousPad098;
typedef struct { char m_pad99; } AnonymousPad099;
typedef struct { char m_pad100; } AnonymousPad100;
typedef struct { char m_pad101; } AnonymousPad101;
typedef struct { char m_pad102; } AnonymousPad102;
typedef struct { char m_pad103; } AnonymousPad103;
typedef struct { char m_pad104; } AnonymousPad104;
typedef struct { char m_pad105; } AnonymousPad105;
typedef struct { char m_pad106; } AnonymousPad106;
typedef struct { char m_pad107; } AnonymousPad107;
typedef struct { char m_pad108; } AnonymousPad108;
typedef struct { char m_pad109; } AnonymousPad109;
typedef struct { char m_pad110; } AnonymousPad110;
typedef struct { char m_pad111; } AnonymousPad111;
typedef struct { char m_pad112; } AnonymousPad112;
typedef struct { char m_pad113; } AnonymousPad113;
typedef struct { char m_pad114; } AnonymousPad114;
typedef struct { char m_pad115; } AnonymousPad115;
typedef struct { char m_pad116; } AnonymousPad116;
typedef struct { char m_pad117; } AnonymousPad117;
typedef struct { char m_pad118; } AnonymousPad118;
typedef struct { char m_pad119; } AnonymousPad119;
typedef struct { char m_pad120; } AnonymousPad120;
typedef struct { char m_pad121; } AnonymousPad121;
typedef struct { char m_pad122; } AnonymousPad122;
typedef struct { char m_pad123; } AnonymousPad123;
typedef struct { char m_pad124; } AnonymousPad124;
typedef struct { char m_pad125; } AnonymousPad125;
typedef struct { char m_pad126; } AnonymousPad126;
typedef struct { char m_pad127; } AnonymousPad127;
typedef struct { char m_pad128; } AnonymousPad128;
typedef struct { char m_pad129; } AnonymousPad129;
typedef struct { char m_pad130; } AnonymousPad130;
typedef struct { char m_pad131; } AnonymousPad131;
typedef struct { char m_pad132; } AnonymousPad132;
typedef struct { char m_pad133; } AnonymousPad133;
typedef struct { char m_pad134; } AnonymousPad134;
typedef struct { char m_pad135; } AnonymousPad135;
typedef struct { char m_pad136; } AnonymousPad136;
typedef struct { char m_pad137; } AnonymousPad137;
typedef struct { char m_pad138; } AnonymousPad138;
typedef struct { char m_pad139; } AnonymousPad139;
typedef struct { char m_pad140; } AnonymousPad140;
typedef struct { char m_pad141; } AnonymousPad141;
typedef struct { char m_pad142; } AnonymousPad142;
typedef struct { char m_pad143; } AnonymousPad143;
typedef struct { char m_pad144; } AnonymousPad144;
typedef struct { char m_pad145; } AnonymousPad145;
typedef struct { char m_pad146; } AnonymousPad146;
typedef struct { char m_pad147; } AnonymousPad147;
typedef struct { char m_pad148; } AnonymousPad148;
typedef struct { char m_pad149; } AnonymousPad149;
typedef struct { char m_pad150; } AnonymousPad150;
typedef struct { char m_pad151; } AnonymousPad151;
typedef struct { char m_pad152; } AnonymousPad152;
typedef struct { char m_pad153; } AnonymousPad153;
typedef struct { char m_pad154; } AnonymousPad154;
typedef struct { char m_pad155; } AnonymousPad155;
typedef struct { char m_pad156; } AnonymousPad156;
typedef struct { char m_pad157; } AnonymousPad157;
typedef struct { char m_pad158; } AnonymousPad158;
typedef struct { char m_pad159; } AnonymousPad159;
typedef struct { char m_pad160; } AnonymousPad160;
typedef struct { char m_pad161; } AnonymousPad161;
typedef struct { char m_pad162; } AnonymousPad162;
typedef struct { char m_pad163; } AnonymousPad163;
typedef struct { char m_pad164; } AnonymousPad164;
typedef struct { char m_pad165; } AnonymousPad165;
typedef struct { char m_pad166; } AnonymousPad166;
typedef struct { char m_pad167; } AnonymousPad167;
typedef struct { char m_pad168; } AnonymousPad168;
typedef struct { char m_pad169; } AnonymousPad169;
typedef struct { char m_pad170; } AnonymousPad170;
typedef struct { char m_pad171; } AnonymousPad171;
typedef struct { char m_pad172; } AnonymousPad172;
typedef struct { char m_pad173; } AnonymousPad173;
typedef struct { char m_pad174; } AnonymousPad174;
typedef struct { char m_pad175; } AnonymousPad175;
typedef struct { char m_pad176; } AnonymousPad176;
typedef struct { char m_pad177; } AnonymousPad177;
typedef struct { char m_pad178; } AnonymousPad178;
typedef struct { char m_pad179; } AnonymousPad179;
typedef struct { char m_pad180; } AnonymousPad180;
typedef struct { char m_pad181; } AnonymousPad181;
typedef struct { char m_pad182; } AnonymousPad182;
typedef struct { char m_pad183; } AnonymousPad183;
typedef struct { char m_pad184; } AnonymousPad184;
typedef struct { char m_pad185; } AnonymousPad185;
typedef struct { char m_pad186; } AnonymousPad186;
typedef struct { char m_pad187; } AnonymousPad187;
typedef struct { char m_pad188; } AnonymousPad188;
typedef struct { char m_pad189; } AnonymousPad189;
typedef struct { char m_pad190; } AnonymousPad190;
typedef struct { char m_pad191; } AnonymousPad191;
typedef struct { char m_pad192; } AnonymousPad192;
typedef struct { char m_pad193; } AnonymousPad193;
typedef struct { char m_pad194; } AnonymousPad194;
typedef struct { char m_pad195; } AnonymousPad195;
typedef struct { char m_pad196; } AnonymousPad196;
typedef struct { char m_pad197; } AnonymousPad197;
typedef struct { char m_pad198; } AnonymousPad198;
typedef struct { char m_pad199; } AnonymousPad199;
typedef struct { char m_pad200; } AnonymousPad200;
typedef struct { char m_pad201; } AnonymousPad201;
typedef struct { char m_pad202; } AnonymousPad202;
typedef struct { char m_pad203; } AnonymousPad203;
typedef struct { char m_pad204; } AnonymousPad204;
typedef struct { char m_pad205; } AnonymousPad205;
typedef struct { char m_pad206; } AnonymousPad206;
typedef struct { char m_pad207; } AnonymousPad207;
typedef struct { char m_pad208; } AnonymousPad208;
typedef struct { char m_pad209; } AnonymousPad209;
typedef struct { char m_pad210; } AnonymousPad210;
typedef struct { char m_pad211; } AnonymousPad211;
typedef struct { char m_pad212; } AnonymousPad212;
typedef struct { char m_pad213; } AnonymousPad213;
typedef struct { char m_pad214; } AnonymousPad214;
typedef struct { char m_pad215; } AnonymousPad215;
typedef struct { char m_pad216; } AnonymousPad216;
typedef struct { char m_pad217; } AnonymousPad217;
typedef struct { char m_pad218; } AnonymousPad218;
typedef struct { char m_pad219; } AnonymousPad219;
typedef struct { char m_pad220; } AnonymousPad220;
typedef struct { char m_pad221; } AnonymousPad221;
typedef struct { char m_pad222; } AnonymousPad222;
typedef struct { char m_pad223; } AnonymousPad223;
typedef struct { char m_pad224; } AnonymousPad224;
typedef struct { char m_pad225; } AnonymousPad225;
typedef struct { char m_pad226; } AnonymousPad226;
typedef struct { char m_pad227; } AnonymousPad227;
typedef struct { char m_pad228; } AnonymousPad228;
typedef struct { char m_pad229; } AnonymousPad229;
typedef struct { char m_pad230; } AnonymousPad230;
typedef struct { char m_pad231; } AnonymousPad231;
typedef struct { char m_pad232; } AnonymousPad232;
typedef struct { char m_pad233; } AnonymousPad233;
typedef struct { char m_pad234; } AnonymousPad234;
typedef struct { char m_pad235; } AnonymousPad235;
typedef struct { char m_pad236; } AnonymousPad236;
typedef struct { char m_pad237; } AnonymousPad237;
typedef struct { char m_pad238; } AnonymousPad238;
typedef struct { char m_pad239; } AnonymousPad239;
typedef struct { char m_pad240; } AnonymousPad240;
typedef struct { char m_pad241; } AnonymousPad241;
typedef struct { char m_pad242; } AnonymousPad242;
typedef struct { char m_pad243; } AnonymousPad243;
typedef struct { char m_pad244; } AnonymousPad244;
typedef struct { char m_pad245; } AnonymousPad245;
typedef struct { char m_pad246; } AnonymousPad246;
typedef struct { char m_pad247; } AnonymousPad247;
typedef struct { char m_pad248; } AnonymousPad248;
typedef struct { char m_pad249; } AnonymousPad249;
typedef struct { char m_pad250; } AnonymousPad250;
typedef struct { char m_pad251; } AnonymousPad251;
typedef struct { char m_pad252; } AnonymousPad252;
typedef struct { char m_pad253; } AnonymousPad253;
typedef struct { char m_pad254; } AnonymousPad254;
typedef struct { char m_pad255; } AnonymousPad255;
typedef struct { char m_pad256; } AnonymousPad256;
typedef struct { char m_pad257; } AnonymousPad257;
typedef struct { char m_pad258; } AnonymousPad258;
typedef struct { char m_pad259; } AnonymousPad259;
typedef struct { char m_pad260; } AnonymousPad260;
typedef struct { char m_pad261; } AnonymousPad261;
typedef struct { char m_pad262; } AnonymousPad262;
typedef struct { char m_pad263; } AnonymousPad263;
typedef struct { char m_pad264; } AnonymousPad264;
typedef struct { char m_pad265; } AnonymousPad265;
typedef struct { char m_pad266; } AnonymousPad266;
typedef struct { char m_pad267; } AnonymousPad267;
typedef struct { char m_pad268; } AnonymousPad268;
typedef struct { char m_pad269; } AnonymousPad269;
typedef struct { char m_pad270; } AnonymousPad270;
typedef struct { char m_pad271; } AnonymousPad271;
typedef struct { char m_pad272; } AnonymousPad272;
typedef struct { char m_pad273; } AnonymousPad273;
typedef struct { char m_pad274; } AnonymousPad274;
typedef struct { char m_pad275; } AnonymousPad275;
typedef struct { char m_pad276; } AnonymousPad276;
typedef struct { char m_pad277; } AnonymousPad277;
typedef struct { char m_pad278; } AnonymousPad278;
typedef struct { char m_pad279; } AnonymousPad279;
typedef struct { char m_pad280; } AnonymousPad280;
typedef struct { char m_pad281; } AnonymousPad281;
typedef struct { char m_pad282; } AnonymousPad282;
typedef struct { char m_pad283; } AnonymousPad283;
typedef struct { char m_pad284; } AnonymousPad284;
typedef struct { char m_pad285; } AnonymousPad285;
typedef struct { char m_pad286; } AnonymousPad286;
typedef struct { char m_pad287; } AnonymousPad287;
typedef struct { char m_pad288; } AnonymousPad288;
typedef struct { char m_pad289; } AnonymousPad289;
typedef struct { char m_pad290; } AnonymousPad290;
typedef struct { char m_pad291; } AnonymousPad291;
typedef struct { char m_pad292; } AnonymousPad292;
typedef struct { char m_pad293; } AnonymousPad293;
typedef struct { char m_pad294; } AnonymousPad294;
typedef struct { char m_pad295; } AnonymousPad295;
typedef struct { char m_pad296; } AnonymousPad296;
typedef struct { char m_pad297; } AnonymousPad297;
typedef struct { char m_pad298; } AnonymousPad298;
typedef struct { char m_pad299; } AnonymousPad299;
typedef struct { char m_pad300; } AnonymousPad300;
typedef struct { char m_pad301; } AnonymousPad301;
typedef struct { char m_pad302; } AnonymousPad302;
typedef struct { char m_pad303; } AnonymousPad303;
typedef struct { char m_pad304; } AnonymousPad304;
typedef struct { char m_pad305; } AnonymousPad305;
typedef struct { char m_pad306; } AnonymousPad306;
typedef struct { char m_pad307; } AnonymousPad307;
typedef struct { char m_pad308; } AnonymousPad308;
typedef struct { char m_pad309; } AnonymousPad309;
typedef struct { char m_pad310; } AnonymousPad310;
typedef struct { char m_pad311; } AnonymousPad311;
typedef struct { char m_pad312; } AnonymousPad312;
typedef struct { char m_pad313; } AnonymousPad313;
typedef struct { char m_pad314; } AnonymousPad314;
typedef struct { char m_pad315; } AnonymousPad315;
typedef struct { char m_pad316; } AnonymousPad316;
typedef struct { char m_pad317; } AnonymousPad317;
typedef struct { char m_pad318; } AnonymousPad318;
typedef struct { char m_pad319; } AnonymousPad319;
typedef struct { char m_pad320; } AnonymousPad320;
typedef struct { char m_pad321; } AnonymousPad321;
typedef struct { char m_pad322; } AnonymousPad322;
typedef struct { char m_pad323; } AnonymousPad323;
typedef struct { char m_pad324; } AnonymousPad324;
typedef struct { char m_pad325; } AnonymousPad325;
typedef struct { char m_pad326; } AnonymousPad326;
typedef struct { char m_pad327; } AnonymousPad327;
typedef struct { char m_pad328; } AnonymousPad328;
typedef struct { char m_pad329; } AnonymousPad329;
typedef struct { char m_pad330; } AnonymousPad330;
typedef struct { char m_pad331; } AnonymousPad331;
typedef struct { char m_pad332; } AnonymousPad332;
// ==================== /ANON-PAD ====================
#include "DisJoint.h"
#include "StrikerScript.h"
#include "NpcScript.h"          // STScriptFileList 完整定义（create 用）
#include "CharacManageScript.h"
#include "CItemUpgrade_Separate.h"
#include "STStackableScript.h"        // ENUM_MODULE_TYPE（DataManagerInitDepends 前置要求）
#include "DataManagerInitDepends.h"   // SDCActiveStatusSyncServer / WongWork::CCompoundAvatar
#include "PvPTypes.h"                 // CMTRand
#include "DNFLexWrapper.h"            // ScanType/ScanInt/ScanFloat/ScanStr/loadRDARScriptFile

#include <cstdio>
#include <ctime>
#include <new>
#include <string>
#include <vector>

// ============================ DisjointScript ============================


#include "CtorProducers_2/LoadDisJoint.inc"


#include "CtorProducers_2/DisjointSkillScript.inc"


#include "CtorProducers_2/clear.inc"


// ---- DisjointScript::._381（真匿名 struct，DisJoint.h 内 m_additional[6]）----
// ORIG ctor _ZN14DisjointScript5._381C1Ev t 0x08348088 (0x16)：仅构造 +4 vector<int>；
// dtor _ZN14DisjointScript5._381D1Ev t 0x0834809e：逆序析构 vector。
// [P203 2026-09-11 GiantB] 原「命名类 t_381 + asm(".set")」改为编译器自然发射
// （`.set` 改不了重定位名，AnonLane 实证）⇒ 本文件不再显式定义 ctor/dtor，
// 由 DisJoint.h 的匿名 struct 自动合成；编号由顶部 ANON-PAD 垫到 381。

#include "CtorProducers_2/DisjointScript.inc"


#include "CtorProducers_2/clear_1.inc"


#include "CtorProducers_2/dtor_DisjointScript.inc"


// ============================ DisJoint ============================

#include "CtorProducers_2/DisJoint.inc"


#include "CtorProducers_2/dtor_DisJoint.inc"


// ============================ StrikerScript ============================


// ORIG 静态成员单例指针（nm B 数据；getInsance 懒加载落点）。
StrikerScript* StrikerScript::instance_ = 0;              // _ZN13StrikerScript9instance_E B 0x09500978
// [SplitC batch5] CharacManageScript::instance_ 定义已迁出至 RDARScriptCharacManage.cpp（类内声明见 CharacManageScript.h）


#include "CtorProducers_2/getInsance.inc"


#include "CtorProducers_2/clear_2.inc"


#include "CtorProducers_2/create.inc"


#include "CtorProducers_2/destory.inc"




#include "CtorProducers_2/StrikerScript.inc"


#include "CtorProducers_2/dtor_StrikerScript.inc"


// ============================ CharacManageScript ============================

#include "CtorProducers_2/getInsance_1.inc"


#include "CtorProducers_2/CharacManageScript.inc"


#include "CtorProducers_2/clear_3.inc"


#include "CtorProducers_2/dtor_CharacManageScript.inc"


#include "CtorProducers_2/importScript.inc"


// ============================ SDCActiveStatusSync Common/Server ============================

#include "CtorProducers_2/SDCActiveStatusSyncCommon.inc"


#include "CtorProducers_2/dtor_SDCActiveStatusSyncCommon.inc"


#include "CtorProducers_2/loadActiveStatusScript.inc"


// [P203 2026-09-11 GiantB] 原「命名类 t_381 + `.globl`+`.set` 别名」块已**删除**：
// `.set` 只改 nm 轴/反汇编标注名，改不了**重定位目标名**（我方 call 写 `5t_381C1Ev`，
// ORIG 是 `5._381C1Ev` ⇒ p167 恒 MISSING/EXTRA，AnonLane 实证）。现由 DisJoint.h 的
// 真匿名 struct 自然发射 `_ZN14DisjointScript5._381{C1,C2,D1,D2}Ev`（编号由顶部
// ANON-PAD 垫到 381：声明点自然编号 48 + 333）。orphan：CtorProducers_2/t_381.inc、
// dtor_t_381.inc 已不再被任何 TU include（可删）。
