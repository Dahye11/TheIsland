#pragma once
#include <Engine/Ptr.h>
#include <Engine/Texture.h>

enum ITEM_TYPE {
	ITEM_NOTHING = 0,

	ITEM_TOOL,					//======= ���� =======
	ITEM_PICKAXE,
	ITEM_AXE,
	ITEM_HAMMER,
	ITEM_MACHETTE,
	ITEM_BOW,
	ITEM_CAMPFIRE,
	ITEM_MAP,
	ITEM_TOOL_END,				//======= =======

	ITEM_STUFF = ITEM_TOOL_END, //======= ��� =======
	ITEM_WOOD,
	ITEM_WOODSTICK,
	ITEM_STONE,
	ITEM_LEATHER,
	ITEM_CLOTH,
	ITEM_BONE,
	ITEM_ARROW,
	ITEM_STUFF_END,				//======= =======

	ITEM_USABLE = ITEM_STUFF_END,	//======= �Ҹ�ǰ =======
	ITEM_FOOD = ITEM_USABLE,		//===== �ķ� =====
	ITEM_MEAT,
	ITEM_COOKMEAT,
	ITEM_CHOCOLATEBAR,
	ITEM_CAN,
	ITEM_APPLE,
	ITEM_FOOD_END,					//===== =====
	ITEM_DRINK = ITEM_FOOD_END,		//===== ���ǰ� =====
	ITEM_EMPTY_BOTTLE,
	ITEM_WATER_BOTTLE,
	ITEM_SODA,
	ITEM_DRINK_END,					//===== =====
	ITEM_HEAL = ITEM_DRINK_END,		//===== ȸ�� =====
	ITEM_MEDICKIT,
	ITEM_BANDAGE,
	ITEM_HEAL_END,					//===== =====
	ITEM_USABLE_END,				//======= =======

	ITEM_ARMOR = ITEM_USABLE_END,	//======= ��(��) =======
	ITEM_TSHIRT,
	ITEM_SHIRT,
	ITEM_JACKET,
	ITEM_ARMOR_END,					//======= =======

	ITEM_HOUSING = ITEM_ARMOR_END,	//======= �Ͽ�¡ ========
	ITEM_BLUEPRINT,
	ITEM_HOUSING_END,				//======= ========

	ITEM_END
};

typedef struct tItemRecipe {
	ITEM_TYPE eItem;
	UINT iCount;
}tItemRecipe;

Ptr<CTexture> GetItemIconByType(ITEM_TYPE eType);

vector<tItemRecipe> GetItemRecipe(ITEM_TYPE eType);

string GetItemNameByType(ITEM_TYPE eType);
