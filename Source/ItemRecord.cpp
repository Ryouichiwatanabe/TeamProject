#include "ItemRecord.h"

ItemRecord& ItemRecord::GetInstance() {
	static ItemRecord instance;
	return instance;
}

void ItemRecord::Commit(int _stage, const StageFlags& _picked) {
	if (!IsValidStage(_stage))
		return;

	auto& record = mRecords[_stage - 1];
	for (int i = 0; i < GameConfig::MAX_ITEMS_PER_STAGE; ++i)
		record[i] = record[i] || _picked[i];	// 前に取ったものは消さない
}

bool ItemRecord::IsObtained(int _stage, int _slot) const {
	if (!IsValidStage(_stage) || !IsValidSlot(_slot))
		return false;
	return mRecords[_stage - 1][_slot];
}

bool ItemRecord::IsAllObtained(int _stage) const {
	for (int i = 0; i < GameConfig::MAX_ITEMS_PER_STAGE; ++i) {
		if (!IsObtained(_stage, i))
			return false;
	}
	return true;
}

bool ItemRecord::IsValidStage(int _stage) {
	return 1 <= _stage && _stage <= GameConfig::STAGE_COUNT;
}

bool ItemRecord::IsValidSlot(int _slot) {
	return 0 <= _slot && _slot < GameConfig::MAX_ITEMS_PER_STAGE;
}