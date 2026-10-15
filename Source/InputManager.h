#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "GamePad.h"

#include <array>

enum class Button {
	Confirm,
	Cancel,

	Jump,
	Attack,
	Dash,

	Shape,

	OperationChange_toPlayer,	// (デバック用)
	OperationChange_toFakePlayer,	// (デバック用)

	Max
};

enum class Axis {
	MoveX,
	MoveY,

	MoveX_FAKE,	// デバック用
	MoveY_FAKE,	// デバック用

	Rotation,
	Rotation_FAKE,

	LookX,
	LookY,

	Max,
};

enum class InputMode
{
	Normal,
	Pause,

	Fake	// 偽Player操作
};

/* memo : 
* Stateの指定で入力状態を取得できるクラス。
* 従来の入力処理は、
*	INPUT_KEY_SPACEや
* 以下使用例
* 
* Player.cpp
* VECTOR input;
* input.x = InputManager::GetInstance().GetButton(Axis::MoveX);
* 
* Player.cpp
* bool bJumpFlag = InputManager::GetInstance().IsButtonDown(Button::Jump);
* 
* Ex.理解できるものへ
* これだけSingletonパターンでキモイので、
* いつか余力あるとき静的メンバ関数にします
*/

class InputManager
{
public:
	struct ButtonBinding {
		InputMode mode = InputMode::Normal;

		int mnKeyboardKey = -1;
		int mnMouseButton = -1;
		int mnPadButton = -1;
	};

	struct AxisBinding {
		InputMode mode = InputMode::Normal;

		int mnPositiveKey = -1;
		int mnNegativeKey = -1;

		PadAxis padAxis = PadAxis::None;
	};

	struct ButtonState {
		bool Press = false;
		bool Down = false;
		bool Up = false;
	};

public:
	InputManager() = default;
	~InputManager() = default;

	static InputManager& GetInstance();

	void InitializeButton();
	void InitializeAxis();

	void Update();

	bool GetButton(Button _button) const;
	bool GetButtonDown(Button _button) const;
	bool GetButtonUp(Button _button) const;
 
	float GetAxis(Axis _axis) const;

	Mouse& GetMouse() { return mMouse; }

	void SetInputMode(InputMode _mode) { mMode = _mode; }
	InputMode GetInputMode() { return mMode; }
private:
	void UpdateButtons();
	void UpdateAxes();

	bool IsButtonPressed(const ButtonBinding& _binding) const;
	bool IsButtonDown(const ButtonBinding& _binding) const;
	bool IsButtonUp(const ButtonBinding& _binding) const;

	float GetAxisValue(const AxisBinding& _binding) const;

private:
	Keyboard mKeyboard;
	Mouse mMouse;
	GamePad mGamePad;

	InputMode mMode = InputMode::Normal;

	std::array<ButtonBinding, static_cast<size_t>(Button::Max)> mButtonBindings;
	std::array<AxisBinding, static_cast<size_t>(Axis::Max)> mAxisBindings;

	std::array<ButtonState, static_cast<size_t>(Button::Max)> mButtonStates;
	std::array<float, static_cast<size_t>(Axis::Max)> mAxisStates;
};