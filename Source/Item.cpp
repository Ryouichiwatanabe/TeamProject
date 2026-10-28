#include "Item.h"

// -- Module -- //
#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

#include "GameConfig.h"	// CELL_SIZE

void Item::Init() {
	SetTag(Tag::ITEM);
}

void Item::InitComponent() {
	GetModule<Transform>();
	GetModule<Graphic>();
	auto collider = GetModule<Collider>();

	if (collider)
		collider->SetHalfSize(
			VGet(
				GameConfig::CELL_SIZE / 4.0f,
				25.0f,
				0.0f
			)
		);
}