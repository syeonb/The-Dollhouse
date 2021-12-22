#include <../ext/json/single_include/nlohmann/json.hpp>
using json = nlohmann::json;

#include "world_init.hpp"
#include "world_system.hpp"
#include "job_system.h"
#include <iostream>

using namespace std;
class JsonReader {
public:
    json readFile(const string& fileName) {
        std::string dataPath = data_path() + fileName;
        json obj;
        std::ifstream file(dataPath);
        if (!file.fail()) {
            if(!is_empty(file)) {
                file >> obj;
            }

        }
        return obj;
    }

    static bool is_empty(std::ifstream& file)
    {
        return file.peek() == std::ifstream::traits_type::eof();
    }

    json readGameProgress() {
        string fileName = "/gameData/gameProgress.json";
        return readFile(fileName);
    }

    json readLevel(int level) {
        std::string levelName = "/levels/level_" +  to_string(level) + ".json";
        return readFile(levelName);
    }
};

class Factory {
    public:
         virtual void create(RenderSystem* renderer, json components) = 0;
};

class BedFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createBed(renderer,
                  vec2(components["pos"]["x"].get<float>(),
                          components["pos"]["y"].get<float>()),
                          components["dir"].get<Direction>(),
                          components["angle"].get<float>());
    }

};

//class HouseFactory : public Factory {
//public:
//    void create(RenderSystem* renderer, json components) override {
//        createHouse(renderer,
//                  vec2(components["pos"]["x"].get<float>(),
//                       components["pos"]["y"].get<float>()),
//                  components["dir"].get<Direction>(),
//                  components["angle"].get<float>());
//    }
//
//};

class LampFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createLamp(renderer,
                  vec2(components["pos"]["x"].get<float>(),
                       components["pos"]["y"].get<float>()),
                  components["dir"].get<Direction>(),
                  components["angle"].get<float>());
    }
};

class TableFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createTable(renderer,
                   vec2(components["pos"]["x"].get<float>(),
                        components["pos"]["y"].get<float>()),
                   components["dir"].get<Direction>(),
                   components["angle"].get<float>());
    }

};

class DoorFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createDoor(renderer,
            vec2(components["pos"]["x"].get<float>(), components["pos"]["y"].get<float>()),
            vec2(components["scale"]["x"].get<float>(), components["scale"]["y"].get<float>()),
            (TEXTURE_ASSET_ID)components["texture_id"].get<int>());
    }
};

class HealthPotionFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createHealthPotion(renderer,
            components["world_id"].get<int>(),
            vec2(components["pos"]["x"].get<float>(),
                components["pos"]["y"].get<float>()),
            (GameItemName)components["game_item_name"].get<int>());
    }
};

class MementoFactory : public Factory {
public:
    void create(RenderSystem* renderer, json components) override {
        createMemento(renderer,
            (MementoName)components["id"].get<int>(),
            vec2(components["pos"]["x"].get<float>(),
                components["pos"]["y"].get<float>()));
    }
};

class EnemyFactory {
public:
    void create(RenderSystem* render, WorldSystem* world, json components) {
        world->clearEnemy();
        for(auto& enemy: components) {
            world->addEnemy(enemy["type"].get<std::string>(), enemy["frequency"].get<int>());
        }
    }

};

class attackMagicGameItemLists {
public:
    static void getMagic(json components, vector<Magic> * magicList) {
        for(auto& magic: components) {
            magicList->push_back(createMagic(magic["name"].get<MagicName>(),
                    magic["magicType"].get<MagicType>(),
                            magic["attackType"].get<AttackType>(),
                                    magic["amount"].get<float>(),
                                            magic["physical_defense_boost"].get<float>(),
                                                    magic["magic_defense_boost"].get<float>(),
                                                            magic["timer"].get<int>(),
                                                                    magic["countdown"].get<int>(),
                                                                            magic["isTemporary"].get<bool>()));
        }
    }

    static void getAttacks(json components, vector<Attack> * attacks) {
        for(auto& attack: components) {
            attacks->push_back(createAttack(attack["name"].get<AttackName>(), attack["type"].get<AttackType>(), attack["damage"].get<int>()));
        }
    }

    static void getGameItems(json components, vector<GameItem>* items) {
        for (auto& item : components) {
            items->push_back(
                createGameItem(
                    item["item_name"].get<GameItemName>(),
                    item["enemyRepel"].get<bool>(),
                    item["health"].get<int>(),
                    item["mana"].get<int>(),
                    item["speed"].get<int>(),
                    item["timed"].get<bool>(),
                    item["timer"].get<float>(),
                    item["world_id"].get<int>()));
        }
    }

    static void getAllCollectedItemIds(json components, vector<int>* all_collected_item_ids) {
        for (auto item_id : components) {
            all_collected_item_ids->push_back(item_id.get<int>());
        }
    }
};

class PlayerFactory {
public:
    Entity createDollFactory(WorldSystem * world, RenderSystem* renderer, json components) {
        std::vector<Attack> attacks;
        json attackList = components["attacks"];
        attackMagicGameItemLists::getAttacks(attackList, &attacks);

        std::vector<Magic> magic;
        json magicList = components["magic"];
        attackMagicGameItemLists::getMagic(magicList, &magic);
        
        std::vector<GameItem> items;
        json itemList = components["game_item_list"];
        attackMagicGameItemLists::getGameItems(itemList, &items);

        return createDoll(
            renderer,
            vec2(components["pos"]["x"].get<float>(),components["pos"]["y"].get<float>()),
            {world->fbHeight,world->fbWidth},
            attacks, magic, items,
            components["hp"].get<int>(), components["dp"].get<int>(), components["maxHP"].get<int>(),
            components["mana"].get<int>(), components["maxMana"].get<int>());
    }

};

class WallFactory : public Factory {
public:
    void create(RenderSystem * renderer, json components) override {
        createWallBlock(
            vec2(components["pos"]["x"].get<float>(),
                components["pos"]["y"].get<float>()),
            vec2(components["scale"]["x"].get<float>(),
                components["scale"]["y"].get<float>())
        );
    }
};

class BackgroundFactory : public Factory {
public:
    void create(RenderSystem* render, json components) override {
        createBackground(
            render,
            vec2(components["pos"]["x"].get<float>(),
                components["pos"]["y"].get<float>()),
            (TEXTURE_ASSET_ID)components["asset_id"].get<int>()
        );
    }
};

class GameProgressFactory {
public:
    GameProgress create(RenderSystem* renderer, json components) {
        Health h = createHealth(
            components["health"]["health"].get<int>(), 
            components["health"]["healthDecrement"].get<int>(), 
            components["health"]["maxHP"].get<int>());

        Mana m = createMana(
            components["mana"]["mana"].get<int>(),
            components["mana"]["maxMana"].get<int>());

        std::vector<Attack> attacks;
        uint64_t j1 = multithreading.addJob([&] {
            attackMagicGameItemLists::getAttacks(components["attacks"], &attacks);
        });

        std::vector<Magic> magic;
        uint64_t j2 = multithreading.addJob([&] {
            attackMagicGameItemLists::getMagic(components["magic"], &magic);
        });

        std::vector<GameItem> items;
        uint64_t j3 = multithreading.addJob([&] {
            attackMagicGameItemLists::getGameItems(components["inventory_items"], &items);
        });

        std::vector<int> all_collected_item_ids;
        uint64_t j4 = multithreading.addJob([&] {
            attackMagicGameItemLists::getAllCollectedItemIds(components["all_collected_item_ids"], &all_collected_item_ids);
        });

        std::vector<int> collected_memento_ids;
        uint64_t j5 = multithreading.addJob([&] {
            attackMagicGameItemLists::getAllCollectedItemIds(components["collected_memento_ids"], &collected_memento_ids);
        });

        multithreading.join(j1);
        multithreading.join(j2);
        multithreading.join(j3);
        multithreading.join(j4);
        multithreading.join(j5);


        json motion = components["motion"];
        Motion motionObj = createMotion(vec2(motion["scale"]["x"].get<float>(), motion["scale"]["y"].get<float>()),
                                vec2(motion["position"]["x"].get<float>(), motion["position"]["y"].get<float>()),
                                vec2(motion["velocity"]["x"].get<float>(), motion["velocity"]["y"].get<float>()),
                                motion["angle"].get<float>(),
                                motion["dir"].get<Direction>(),
                                motion["collision_proof"].get<bool>());

        return createGameProgress(components["level"].get<int>(), h, m, motionObj, attacks, magic, items, all_collected_item_ids, collected_memento_ids);
    }
};

class MotionFactory {
public:
    Motion create(json motion) {
        Motion m = createMotion(vec2(motion["scale"]["x"].get<float>(), motion["scale"]["y"].get<float>()),
                                vec2(motion["position"]["x"].get<float>(), motion["position"]["y"].get<float>()),
                                vec2(motion["velocity"]["x"].get<float>(), motion["velocity"]["y"].get<float>()),
                                motion["angle"].get<float>(),
                                motion["dir"].get<Direction>(),
                                motion["collision_proof"].get<bool>());
        return m;
    }
};