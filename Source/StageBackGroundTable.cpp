#include "StageBackGroundTable.h"

namespace {
	using namespace BgBit;

	constexpr const char* BG_DIR = "Resource/UI/BackGround/";

	// ---- Stage1 冷奴（S0:ネギ / S1:生姜 / S2:醤油） ---- //
	constexpr BgLayerDef CHILLED_TOFU[] = {
		{ "Tofu.png",                         0,  S2, 456.0f, 255.0f, 0.980f, 0.00f },	// 醤油なし
		{ "Tofu drizzled with soy sauce.png", S2, 0,  457.0f, 258.0f, 1.045f, 0.00f },	// 醤油あり
		{ "Green Onion.png",                  S0, 0,  446.0f, 185.0f, 0.180f, 0.00f },
		{ "Ginger.png",                       S1, 0,  526.0f, 215.0f, 0.129f, 0.00f },
	};

	// ---- Stage2 湯豆腐（S0:昆布 / S1:ネギ / S2:ポン酢） ---- //
	constexpr BgLayerDef HOT_TOFU[] = {
		{ "Kelp.png",                S0, 0,   378.0f, 277.0f, 0.850f, -0.66f },	// 鍋の中
		{ "Sliced Green Onions.png", S1, 0,   440.0f, 210.0f, 0.994f,  0.00f },	// 鍋の中
		{ "Ponzu.png",               S2, 0,    15.0f, -25.0f, 0.609f,  0.00f },
		{ "Ponzu on a plate.png",    S2, 0,   -50.0f, 228.0f, 0.355f,  0.00f },
		{ "Green Onion.png",         S1, 0,   -23.0f, 400.0f, 0.750f,  0.00f },
		{ "Packaged kelp.png",       S0, 0,  1115.0f, 355.0f, 1.000f,  0.40f },
	};

	// ---- Stage3 厚揚げ（S0:油 / S1:ネギ / S2:醤油）※ItemTableと不一致・要確認 ---- //
	constexpr BgLayerDef THICK_FRIED_TOFU[] = {
		{ "Pot.png",          0,  S0, 461.0f, 268.0f, 1.212f, 0.00f },	// 油なし
		{ "Tofu and Oil.png", S0, 0,  461.0f, 268.0f, 1.212f, 0.00f },	// 油あり
		{ "Oil.png",          S0, 0,  832.0f,   3.0f, 0.242f, 0.00f },
		{ "Green Onion.png",  S1, 0,  207.0f,  16.0f, 0.371f, 0.00f },
		{ "Soy Sauce.png",    S2, 0,  -16.0f, 141.0f, 0.305f, 0.00f },
	};

	// ---- Stage4 味噌汁（S0:味噌 / S1:ネギ / S2:わかめ） ---- //
	constexpr BgLayerDef MISO_SOUP[] = {
		{ "Pot.png",                    0,   S0, 525.0f, 488.0f, 1.451f,  0.00f },	// お湯
		{ "Hot Pot with Miso Soup.png", S0,  0,  525.0f, 488.0f, 1.451f,  0.00f },	// 味噌入り
		{ "Wakame.png",                 S2,  0,  527.0f, 303.0f, 0.850f,  0.00f },
		{ "Tofu.png",                   0,   0,  523.0f, 339.0f, 0.990f,  0.00f },	// 常に表示
		{ "Chopped Green Onions.png",   S1,  0,  535.0f, 343.0f, 1.050f,  0.00f },	// ※要透過
		{ "Ladle with Miso Soup.png",   ALL, 0,  774.0f, -39.0f, 0.942f,  0.00f },	// 完成時のみ
		{ "Miso.png",                   S0,  0,  -74.0f,  47.0f, 0.282f,  0.00f },
		{ "Green Onion.png",            S1,  0,  -63.0f, 372.0f, 0.885f,  0.00f },
		{ "Packaged Wakame.png",        S2,  0, 1063.0f, 405.0f, 0.531f, -0.37f },
	};

	// ---- Stage5 麻婆豆腐（S0:ニンニク / S1:ひき肉 / S2:豆板醤） ---- //
	constexpr BgLayerDef MAPO_TOFU[] = {
		// フライパンの中身（組み合わせで1枚だけ出る）
		{ "Garlic Only.png",            S0,      S1 | S2, 490.0f, 305.0f, 0.900f, 0.00f },
		{ "Meat.png",                   S1,      S0 | S2, 496.0f, 324.0f, 0.344f, 0.00f },
		// 豆板醤のみ → 画像なし
		{ "Garlic and Meat .png",       S0 | S1, S2,      496.0f, 330.0f, 1.000f, 0.00f },
		{ "Garlic and Doubanjiang.png", S0 | S2, S1,      509.0f, 304.0f, 1.200f, 0.00f },
		{ "Meat and Doubanjiang .png",  S1 | S2, S0,      489.0f, 333.0f, 1.002f, 0.00f },
		{ "Skillet with Mapo Tofu.png", ALL,     0,       493.0f, 485.0f, 1.666f, 0.00f },
		// 周りの素材
		{ "Garlic.png",                 S0,      0,       -13.0f, 213.0f, 0.247f, 0.00f },
		{ "Meat Pack.png",              S1,      0,       257.0f,  52.0f, 0.225f, 0.00f },
		{ "Doubanjiang.png",            S2,      0,       902.0f, 190.0f, 0.503f, 0.00f },
	};

	// ---- Stage6 肉豆腐（S0:牛肉 / S1:ネギ / S2:タレ） ---- //
	constexpr BgLayerDef MEAT_AND_TOFU[] = {
		{ "Pot_in_Water.png",        0,  S2, 500.0f, 280.0f, 1.280f, 0.00f },	// 縮尺違い注意
		{ "Pot in Sauce.png",        S2, 0,  500.0f, 280.0f, 0.765f, 0.00f },
		{ "Tofu.png",                0,  0,  556.0f, 197.0f, 0.461f, 0.00f },	// 常に表示
		{ "Meat.png",                S0, 0,  458.0f, 313.0f, 1.179f, 0.00f },
		{ "Sliced Green Onions.png", S1, 0,  374.0f, 159.0f, 0.922f, 0.00f },
		{ "Meat Pack.png",           S0, 0,  194.0f,  26.0f, 0.800f, 0.00f },
		{ "Green Onion.png",         S1, 0,  -55.0f, 355.0f, 0.750f, 0.00f },
		{ "Measuring Cup.png",       S2, 0,  919.0f, 170.0f, 0.634f, 0.00f },
	};

	// ---- Stage7 揚げ出し豆腐（S0:めんつゆ / S1:ネギ / S2:油） ---- //
	constexpr BgLayerDef DEEP_FRIED_TOFU[] = {
		{ "Pot.png",                             0,  S2, 459.0f, 265.0f, 1.169f, 0.00f },
		{ "Tofu and Oil.png",                    S2, 0,  459.0f, 265.0f, 1.169f, 0.00f },
		{ "Oil.png",                             S2, 0,  844.0f,   8.0f, 0.257f, 0.00f },
		{ "Green Onion.png",                     S1, 0,  240.0f,  14.0f, 0.445f, 0.00f },
		{ "Noodle Dipping Sauce on a plate.png", S0, 0,  -19.0f, 191.0f, 0.328f, 0.00f },
		{ "Noodle Dipping Sauce..png",           S0, 0,   32.0f, -29.0f, 0.529f, 0.00f },
	};

	// ---- Stage8 杏仁豆腐（S0:クコの実 / S1:杏仁霜 / S2:牛乳） ---- //
	constexpr BgLayerDef ALMOND_TOFU[] = {
		{ "A bowl containing almond paste.png",          S1,      S2, 487.0f, 295.0f, 1.252f, 0.00f },
		{ "A bowl of milk.png",                          S2,      S1, 487.0f, 295.0f, 1.252f, 0.00f },
		{ "A bowl containing almond paste and milk.png", S1 | S2, 0,  487.0f, 295.0f, 1.252f, 0.00f },
		{ "Goji Berries.png",                            S0,      0,   50.0f,  37.0f, 0.844f, 0.00f },
		{ "Almond Paste.png",                            S1,      0,  366.0f, -15.0f, 0.450f, 0.00f },
		{ "Milk.png",                                    S2,      0,  779.0f, -27.0f, 0.500f, 0.00f },
	};

	constexpr BgDishDef DISH_DEFS[] = {
		{ 1, "Chilled Tofu",             "Foundation.png",            CHILLED_TOFU },
		{ 2, "Hot Tofu",                 "Foundation.png",            HOT_TOFU },
		{ 3, "Thick Fried Tofu",         "Foundation.png",            THICK_FRIED_TOFU },
		{ 4, "Miso Soup",                "Foundation.png",            MISO_SOUP },
		{ 5, "Mapo Tofu",                "Foundation Frying pan.png", MAPO_TOFU },
		{ 6, "Meat and Tofu",            "Foundation.png",            MEAT_AND_TOFU },
		{ 7, "Deep-Fried Tofu in Broth", "Foundation.png",            DEEP_FRIED_TOFU },
		{ 8, "Almond Tofu",              "Foundation.png",            ALMOND_TOFU },
	};
}

const BgDishDef* StageBackGroundTable::Find(int _stage) {
	for (const auto& def : DISH_DEFS) {
		if (def.stage == _stage)
			return &def;
	}
	return nullptr;
}

std::string StageBackGroundTable::GetFilePath(const BgDishDef& _dish, const char* _file) {
	return std::string(BG_DIR) + _dish.folder + "/" + _file;
}