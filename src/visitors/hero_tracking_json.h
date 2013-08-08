#ifndef _HERO_TRACKING_VISITOR_JSONS_H
#define _HERO_TRACKING_VISITOR_JSONS_H

#include <iostream>
#include <string>

#include "debug.h"
#include "property.h"
#include "hero_player_helper.h"
#include "visitor.h"

// Prints out the heroe's positions at any given time.
// Difference between this and the other one, is that this one's output is
// exactly as needed by my python heatmap scripts
class HeroTrackingVisitorJsons : public Visitor {
public:
    HeroTrackingVisitorJsons(std::string basepath = "./")
        : _basepath(basepath)
        , _all((basepath + "all.json").c_str())
    { }

    virtual ~HeroTrackingVisitorJsons()
    {
        std::ofstream heroes(_basepath + "heroes.txt");

        for(auto& kv : _files) {
            // Write out the hero names
            heroes << kv.first << std::endl;
            // And close all the files.
            *kv.second << "]" << std::endl;
            // kv.second->flush();
            delete kv.second;
        }

        _all << "]" << std::endl;
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

            if(_files.empty()) {
                _all << "[ [" << x << ',' << y << ']';
            } else {
                _all << ", [" << x << ',' << y << ']';
            }

            std::string heroname(hero.clazz->name, 16);
            auto iFile = _files.find(heroname);
            if(iFile == _files.end()) {
                _files[heroname] = new std::ofstream((_basepath + heroname + ".json").c_str());
                iFile = _files.find(heroname);
                *(iFile->second) << "[ [" << x << ',' << y << ']';
            } else {
                *(iFile->second) << ", [" << x << ',' << y << ']';
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
    std::string _basepath;
    std::map<std::string, std::ofstream*> _files;
    std::ofstream _all;
};

#endif
