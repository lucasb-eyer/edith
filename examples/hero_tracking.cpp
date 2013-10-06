#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>

#include "debug.h"
#include "property.h"
#include "hero_player_helper.h"
#include "visitor.h"
#include "edith.h"

// Prints out the heroe's positions at any given time.
class HeroTrackingVisitor : public Visitor {
public:
    HeroTrackingVisitor(bool json, std::string basepath = "./")
        : _json(json)
        , _basepath(basepath)
        , _all((basepath + "all." + (_json ? "json" : "csv")).c_str())
    { }

    virtual ~HeroTrackingVisitor()
    {
        std::ofstream heroes(_basepath + "heroes.txt");

        for(auto& kv : _files) {
            // Write out the hero names
            heroes << kv.first << std::endl;

            if(_json) {
                *kv.second << "]" << std::endl;
            }

            // And close all the files.
            kv.second->flush();
            delete kv.second;
        }

        if(_json) {
            _all << "]" << std::endl;
        }
    }

    virtual bool visit_entity_created(const Entity &entity) {
        return _hph.visit_entity_created(entity) || handle_entity(entity);
    }

    virtual bool visit_entity_updated(const Entity &entity) {
        return _hph.visit_entity_updated(entity) || handle_entity(entity);
    }

protected:
    bool handle_entity(const Entity &hero) {
        if(hero.clazz->name.find("CDOTA_Unit_Hero_") != 0)
            return false;

        // Most likely an illusion.
        std::string player = _hph.player_name_for_hero_id(hero.id);
        if(player.empty())
            return false;

        try {
            int cell_x = hero.properties.at("DT_DOTA_BaseNPC.m_cellX")->value_as<IntProperty>();
            int cell_y = hero.properties.at("DT_DOTA_BaseNPC.m_cellY")->value_as<IntProperty>();
            int cell_z = hero.properties.at("DT_DOTA_BaseNPC.m_cellZ")->value_as<IntProperty>();
            auto origin_prop = std::dynamic_pointer_cast<VectorXYProperty>(hero.properties.at("DT_DOTA_BaseNPC.m_vecOrigin"));
            int x = cell_x*128 + origin_prop->values[0];
            int y = cell_y*128 + origin_prop->values[1];

            // Write the coords to the "coords of all heroes" file.
            if(_json) {
                if(_files.empty()) {
                    _all << "[ ";
                } else {
                    _all << ", ";
                }
            }

            // Write to the hero-specific files.
            // TODO: A "hero created" callback would be nice, wouldn't it?
            std::string heroname(hero.clazz->name, 16);
            auto iFile = _files.find(heroname);
            if(iFile == _files.end()) {
                _files[heroname] = new std::ofstream((_basepath + heroname + (_json ? ".json" : ".csv")).c_str());
                iFile = _files.find(heroname);

                if(_json) {
                    *(iFile->second) << "[ ";
                }
            } else if(_json) {
                *(iFile->second) << ", ";
            }

            if(_json) {
                *(iFile->second) << '[' << x << ',' << y << ']';
                _all             << '[' << x << ',' << y << ']';
            } else {
                *(iFile->second) << x << ',' << y << '\n';
                _all             << x << ',' << y << '\n';
            }

        } catch(const std::out_of_range& e) {
            XASSERT(false, "%s", e.what());
        } catch(const std::bad_cast& e) {
            XASSERT(false, "%s", e.what());
        }
        return true;
    }

private:
    HeroPlayerHelper _hph;
    bool _json;
    std::string _basepath;
    std::map<std::string, std::ofstream*> _files;
    std::ofstream _all;
};

int main(int argc, char* argv[])
{
    if (argc <= 2) {
        std::cerr << "Usage: " << argv[0] << " [csv|json] something.dem" << std::endl;
        return 1;
    }

    HeroTrackingVisitor visitor(std::string(argv[1]) == "csv");
    dump(argv[2], visitor);
    return 0;
}
