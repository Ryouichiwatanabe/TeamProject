#include "Game.h"
#include "ScreenConfig.h"
#include "InputManager.h"
#include "Master.h"
#include "EffectManager.h"


#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;

SceneManager* Master::mpSceneManager{ nullptr };
ResourceManager* Master::mpResource{ nullptr };
FontManager* Master::mpFontManager{ nullptr };
EffectManager* Master::mpEffectManager{ nullptr };

Game::Game()
{
    // ���������s(false)��Ԃ��ƁA�E�B���h�E���J�����ɃA�v�������I������
    if (!InitializeDxLib()) {
        return;
    }
   
    if (!m_effectManager.Initialize
    (static_cast<int>(ScreenConfig::SCREEN_WIDTH),
    (static_cast<int>(ScreenConfig::SCREEN_HEIGHT))))
    {
        return;
    }
    // 各マネージャーのポインタ受け渡し
    Master::mpResource = &m_resourceManager;
    Master::mpFontManager = &m_fontManager;
    Master::mpSceneManager = &m_sceneManager;
    Master::mpEffectManager = &m_effectManager;

    InputManager::GetInstance().InitializeButton();
    InputManager::GetInstance().InitializeAxis();
    m_cursor.Initialize();

    // �ŏ��ɓǂݍ��ރV�[���i�^�C�g���E�Q�[�����j��؂�ւ���Ȃ�A���� Initialize ����ҏW����
    m_sceneManager.Initialize();

    m_initSuccess = true;
    m_isRunning = true;
    m_lastFrameTime = std::chrono::steady_clock::now();
}

Game::~Game()
{
    if (m_initSuccess) {
        // �������Ɓu�t���v�Ŕj�����Ă���B
        // �Ⴆ�ΐ�� Resource ��j������ Scene ����񂵂ɂ���ƁA�摜������ɃN���b�V������
        m_cursor.Finalize();
        m_sceneManager.Finalize();
        m_fontManager.Clear();


        m_effectManager.Terminate();//Effecseerの終了
        Master::mpEffectManager = nullptr;


        Master::mpSceneManager = nullptr;
        Master::mpFontManager = nullptr;
        Master::mpResource = nullptr;
    }

    DxLib_End();
}

bool Game::InitializeDxLib()
{
    // TRUE  �� �E�B���h�E���[�h�i�f�o�b�O�����₷���j
    // FALSE �� �t���X�N���[�����[�h�i���i�ł�v�����d���̎��j
    ChangeWindowMode(TRUE);

    // �y�𑜓x�E�F�[�x�̕ύX�z
    // 32 �� 16 �ɂ���ƒ�X�y�b�N�����ɂȂ邪�A���F��O���f�[�V�������򉻂���
    // SCREEN_WIDTH / HEIGHT ��ς���ƃE�B���h�E�S�̂̕\���T�C�Y���ς��
    SetGraphMode(
        static_cast<int>(ScreenConfig::SCREEN_WIDTH),
        static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
        32
    );

    // �E�B���h�E�㕔�̃o�[�ɕ\�������^�C�g������
    SetMainWindowText("Game");

    // TRUE  �� 60Hz��144Hz�Ȃǂ̃��j�^�[�`�擯����҂i��ʂ̃`�����E�Y����h���j
    // FALSE �� V-Sync��҂����Ƀt���p���[�ŉ񂷁iFPS�������͒x�����Ɍ��܂Ō��炵�������j
    SetWaitVSyncFlag(TRUE);

    if (DxLib_Init() == -1) {
        return false;
    }

    // DX_SCREEN_BACK �� �_�u���o�b�t�@�����O�L���i��ʂ̃`�����h�~�j
    // DX_SCREEN_FRONT�ɂ���ƕ`��r���̉�ʂ����̂܂܉f���Č������_�ł���
    SetDrawScreen(DX_SCREEN_BACK);

    // 3D���f����`�悷��ۂ̉��s������iZ�o�b�t�@�j
    // �ǂ��炩�� FALSE �ɂ���ƁA���ɂ���L�����N�^�[����O�̕ǂ�˂������ĕ\������Ă��܂�
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);

    // FALSE �ɂ���ƉA�e�̌v�Z���Ȃ��Ȃ�A3D���f���S�̂����R�ŒP�F�̌����ڂɂȂ�
    SetUseLighting(TRUE);

    // �y�W�����C�g�̐F�w��F(��, ��, ��, �A���t�@)�z
    // RGB�̐��l�� (1.0f, 1.0f, 1.0f) �ɂ���Ɣ������R���ɂȂ�i���݂͂�≷���݂̂��鉩�F�n�j
    SetLightDifColor(GetColorF(1.0f, 0.8f, 0.4f, 0.0f));

    // �y�����i�e�ɂȂ�Â���̖��邳�j�z
    // ���l�������� (0.1f, 0.1f, 0.1f) ���ɂ���Ɩ��Í��������Ȃ�A�z���[�Q�[���̂悤�ȈÂ��G�ɂȂ�
    // ���݂� 3.2f �͂��Ȃ薾��߂̐ݒ�
    SetLightAmbColor(GetColorF(3.2f, 3.2f, 3.2f, 0.0f));

    return true;
}

void Game::Run()
{
    if (!m_initSuccess) {
        return;
    }

    while (m_isRunning && ProcessMessage() == 0) {

        // �y�I���L�[�̕ύX�z
        // KEY_INPUT_ESCAPE �� KEY_INPUT_Q �ɕς���΁uQ�L�[�ŏI���v�ɂȂ�
        // �J�����Ɍ딚�I����h�������ꍇ�́A���� if �����ƃR�����g�A�E�g����Ζ������ł���
        if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
            m_isRunning = false;
            break;
        }

        InputManager::GetInstance().Update();

        const auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> frameTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        // �y�X�p�C�N�΍�̏���l�z
        // 0.2s�i�ő�0.2�b���܂Œ~�ρj��ݒ蒆�B
        // ������ 0.05s �ɋ��߂�ƁA�d���������N�������ɃQ�[�������Ԃ��X�L�b�v����₷���Ȃ�
        // �t�� 1.0s �ȂǂɍL����ƁA�J�N������̕������ɃL�����N�^�[����u�҃X�s�[�h�œ���
        frameTime = std::clamp(frameTime, 0.0s, 0.2s);
        m_accumulator += frameTime;

        int stepCount{ 0 };

        // �y�X�V�p�x�̕ύX�z
        // FIXED_TIMESTEP �� (1.0s / 120.0) �ɕς����120FPS��̐����ȕ����v�Z�ɂȂ�
        // (1.0s / 30.0) �ɂ���ƌv�Z���ׂ͔������邪�A�����œ����e��L�������ǂ����蔲���₷���Ȃ�
        const std::chrono::duration<double> fixedStep = FIXED_TIMESTEP;

        // �y�������[�v�E�t���[�Y�h�~�K�[�h�z
        // MAX_SIMULATION_STEPS�i��: 5��Ȃǁj�̏����݂��Ă���B
        // ������ stepCount < 100 �ɑ��₷�ƁA�����������ɖ����ɒǂ������Ƃ��Ċ��S�ɃQ�[�����ł܂�iSpiral of Death�j
        while (m_accumulator >= fixedStep && stepCount < MAX_SIMULATION_STEPS) {
            UpdateLogic(fixedStep.count());
            m_accumulator -= fixedStep;
            ++stepCount;
        }

        Draw();
        ScreenFlip(); // ������Ă΂Ȃ��Ɨ���ʂɕ`�������e�����j�^�[�ɔ��f����Ȃ�
    }
}

void Game::UpdateLogic(double deltaTime)
{
    m_cursor.Update();

    // deltaTime �� 2.0f ���|����Ɓu�Q�[���S�̂�2�{���v�ɂȂ�i�{���f�o�b�O�Ȃǂɗ��p�\�j
    // 0.5f ���|����ƃX���[���[�V�����ɂȂ�
    m_sceneManager.Update(static_cast<float>(deltaTime));

    // 各種エフェクト更新
    m_clickEffect.update(static_cast<float>(deltaTime));

    m_effectManager.Update();//エフェクトを更新
}

void Game::Draw()
{
    // ����������ƁA�ߋ��t���[���̎c�������ׂēh��d�˂��ĉ�ʂ������Ⴎ����ɂȂ�
    ClearDrawScreen();

    // �y�`�揇���̕ύX�z
    // �`�揇���t�ɂ��� m_cursor.Draw() ���ɏ����ƁA
    // �w�i��3D���f���AUI�̌��ɃJ�[�\�����B��Č����Ȃ��Ȃ��Ă��܂�
    m_sceneManager.Draw();

    // 2. 2Dエフェクト描画（ライティング・ZテストをOFFにして最前面にクリア描画）
    SetUseZBufferFlag(FALSE);
    SetWriteZBufferFlag(FALSE);
    SetUseLighting(FALSE);

    m_clickEffect.draw();


    m_effectManager.Draw();

    // 3. 最前面カーソル
    m_cursor.Draw();

    // 描画ステートを3D用のデフォルト状態に復帰
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);
}