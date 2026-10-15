#include "InputManager.h"

#include <DxLib.h>
#include <cmath>

namespace {
	constexpr float AXIS_DEAD_ZONE = 0.2f;
}

InputManager& InputManager::GetInstance() {
	static InputManager instance;
	return instance;
}

void InputManager::InitializeButton() {
	mButtonBindings[(int)Button::Confirm] =
	{
		InputMode::Normal,

		KEY_INPUT_RETURN,
		MOUSE_INPUT_LEFT,
		PAD_INPUT_1
	};

	mButtonBindings[(int)Button::Cancel] =
	{
		InputMode::Normal,

		KEY_INPUT_ESCAPE,
		-1,
		PAD_INPUT_2
	};

	mButtonBindings[(int)Button::Jump] =
	{
		InputMode::Normal,

		KEY_INPUT_SPACE,
		-1,
		PAD_INPUT_3
	};

	mButtonBindings[(int)Button::Attack] =
	{
		InputMode::Normal,

		KEY_INPUT_Z,
		-1,
		PAD_INPUT_4
	};

	mButtonBindings[(int)Button::Dash] =
	{
		InputMode::Normal,

		KEY_INPUT_LSHIFT,
		-1,
		PAD_INPUT_5
	};

	mButtonBindings[(int)Button::Shape] =
	{
		InputMode::Normal,

		KEY_INPUT_E,
		-1,
		-1
	};

	mButtonBindings[(int)Button::OperationChange_toFakePlayer] =
	{
		InputMode::Normal,

		KEY_INPUT_C,
		-1,
		-1
	};

	mButtonBindings[(int)Button::OperationChange_toPlayer] =
	{
		InputMode::Fake,

		KEY_INPUT_C,
		-1,
		-1
	};
}

void InputManager::InitializeAxis() {
	mAxisBindings[(int)Axis::MoveX] =
	{
		InputMode::Normal,

		KEY_INPUT_D,
		KEY_INPUT_A,
		PadAxis::LeftX
	};

	mAxisBindings[(int)Axis::MoveY] =
	{
		InputMode::Normal,

		KEY_INPUT_S,
		KEY_INPUT_W,
		PadAxis::LeftY
	};

	// デバック用
	mAxisBindings[(int)Axis::MoveX_FAKE] =
	{
		InputMode::Fake,

		KEY_INPUT_D,
		KEY_INPUT_A,
		PadAxis::LeftX
	};

	// デバック用
	mAxisBindings[(int)Axis::MoveY_FAKE] =
	{
		InputMode::Fake,

		KEY_INPUT_S,
		KEY_INPUT_W,
		PadAxis::LeftY
	};

	mAxisBindings[(int)Axis::Rotation] =
	{
		InputMode::Normal,

		KEY_INPUT_L,
		KEY_INPUT_J,
		PadAxis::RightX
	};

	mAxisBindings[(int)Axis::Rotation_FAKE] =
	{
		InputMode::Fake,

		KEY_INPUT_L,
		KEY_INPUT_J,
		PadAxis::RightX
	};

	mAxisBindings[(int)Axis::LookX] =
	{
		InputMode::Normal,

		KEY_INPUT_RIGHT,
		KEY_INPUT_LEFT,
		PadAxis::RightX
	};

	mAxisBindings[(int)Axis::LookY] =
	{
		InputMode::Normal,

		KEY_INPUT_UP,
		KEY_INPUT_DOWN,
		PadAxis::RightY
	};
}

void InputManager::Update() {
	// 各入力デバイスを更新
	mKeyboard.Update();
	mMouse.Update();
	mGamePad.Update();

	// 論理入力を更新
	UpdateButtons();
	UpdateAxes();
}

void InputManager::UpdateButtons() {
	// Todo : 全Bindingを繰り返して、Press,Down,Upの入力状態を得る
	for (size_t i = 0;
		i < static_cast<size_t>(Button::Max);
		++i) {
		const ButtonBinding& binding = mButtonBindings[i];

		if (binding.mode != mMode) {
			mButtonStates[i] = {};
			continue;
		}

		mButtonStates[i].Press =
			IsButtonPressed(binding);

		mButtonStates[i].Down =
			IsButtonDown(binding);

		mButtonStates[i].Up =
			IsButtonUp(binding);
	}
}

void InputManager::UpdateAxes() {
	// Todo : 全Bindingを繰り返して、Valueを得る
	for (size_t i = 0;
		i < static_cast<size_t>(Axis::Max);
		++i) {
		const AxisBinding& binding = mAxisBindings[i];

		if (binding.mode != mMode) {
			mAxisStates[i] = 0.0f;
			continue;
		}

		mAxisStates[i] =
			GetAxisValue(binding);
	}
}

bool InputManager::IsButtonPressed(
	const ButtonBinding& _binding) const {
	if (_binding.mnKeyboardKey != -1 &&
		mKeyboard.IsPress(_binding.mnKeyboardKey)) {
		return true;
	}

	if (_binding.mnMouseButton != -1 &&
		mMouse.IsPress(_binding.mnMouseButton)) {
		return true;
	}

	if (_binding.mnPadButton != -1 &&
		mGamePad.IsPress(_binding.mnPadButton)) {
		return true;
	}

	return false;
}

bool InputManager::IsButtonDown(
	const ButtonBinding& _binding) const {
	if (_binding.mnKeyboardKey != -1 &&
		mKeyboard.IsDown(_binding.mnKeyboardKey)) {
		return true;
	}

	if (_binding.mnMouseButton != -1 &&
		mMouse.IsDown(_binding.mnMouseButton)) {
		return true;
	}

	if (_binding.mnPadButton != -1 &&
		mGamePad.IsDown(_binding.mnPadButton)) {
		return true;
	}

	return false;
}

bool InputManager::IsButtonUp(
	const ButtonBinding& _binding) const {
	if (_binding.mnKeyboardKey != -1 &&
		mKeyboard.IsUp(_binding.mnKeyboardKey)) {
		return true;
	}

	if (_binding.mnMouseButton != -1 &&
		mMouse.IsUp(_binding.mnMouseButton)) {
		return true;
	}

	if (_binding.mnPadButton != -1 &&
		mGamePad.IsUp(_binding.mnPadButton)) {
		return true;
	}

	return false;
}

float InputManager::GetAxisValue(
	const AxisBinding& _binding) const {
	float fKeyboardValue = 0.0f;

	if (_binding.mnPositiveKey != -1 &&
		mKeyboard.IsPress(_binding.mnPositiveKey)) {
		fKeyboardValue += 1.0f;
	}

	if (_binding.mnNegativeKey != -1 &&
		mKeyboard.IsPress(_binding.mnNegativeKey)) {
		fKeyboardValue -= 1.0f;
	}

	float fPadValue = 0.0f;

	switch (_binding.padAxis) {
	case PadAxis::LeftX:
		fPadValue = mGamePad.GetLeftStick().x;
		break;

	case PadAxis::LeftY:
		fPadValue = mGamePad.GetLeftStick().y;
		break;

	case PadAxis::RightX:
		fPadValue = mGamePad.GetRightStick().x;
		break;

	case PadAxis::RightY:
		fPadValue = mGamePad.GetRightStick().y;
		break;
	}

	if (std::fabs(fPadValue) < AXIS_DEAD_ZONE) {
		fPadValue = 0.0f;
	}

	if (fPadValue != 0.0f) {
		return fPadValue;
	}

	return fKeyboardValue;
}

bool InputManager::GetButton(
	Button _button
) const {
	return mButtonStates[(int)_button].Press;
}

bool InputManager::GetButtonDown(
	Button _button
) const {
	return mButtonStates[(int)_button].Down;
}

bool InputManager::GetButtonUp(
	Button _button
) const {
	return mButtonStates[(int)_button].Up;
}

float InputManager::GetAxis(
	Axis _axis) const {
	return mAxisStates[
		static_cast<size_t>(_axis)
	];
}

