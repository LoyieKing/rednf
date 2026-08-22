#include "NPCScript.h"

// ---- CNPCScript（G2 桩，实现在后续批次还原）----
CNPCScript::CNPCScript() {}  // TODO(G2)
CNPCScript::~CNPCScript() {}  // TODO(G2)

void CNPCScript::load(char const*) {}  // TODO(G2)

bool CNPCScript::isExistRole(ENUM_NPC_ROLE) const { return false; }  // TODO(G2)

bool CNPCScript::isRewardLevel(unsigned short) const { return false; }  // TODO(G2)

bool CNPCScript::isKeyItem(unsigned long, unsigned int) const { return false; }  // TODO(G2)

int CNPCScript::getPeriodRewardType() const { return 0; }  // TODO(G2)