#pragma once
#include <array>
#include "GameConfig.h"

// クリア時に確定した取得記録（シーンをまたいで残る）
class ItemRecord {
public:
	using StageFlags = std::array<bool, GameConfig::MAX_ITEMS_PER_STAGE>;

	static ItemRecord& GetInstance();

	void Commit(int _stage, const StageFlags& _picked);	// クリア時に呼ぶ
	bool IsObtained(int _stage, int _slot) const;
	bool IsAllObtained(int _stage) const;				// 3つ全部取得済みか

private:
	ItemRecord() = default;
	ItemRecord(const ItemRecord&) = delete;
	ItemRecord& operator=(const ItemRecord&) = delete;

	static bool IsValidStage(int _stage);
	static bool IsValidSlot(int _slot);

	std::array<StageFlags, GameConfig::STAGE_COUNT> mRecords{};
};