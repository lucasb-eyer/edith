#ifndef _HERO_TRACKING_VISITOR_H
#define _HERO_TRACKING_VISITOR_H

// Maps player names to the entity of the hero they selected and outputs the position
// of that hero everytime they are updated and their health is set to zero.
//
// This shows an example usage of this API but is hilariously inefficient and terrible.

#include <iostream>
#include <string>

#include "debug.h"
#include "property.h"
#include "hero_player_helper.h"
#include "visitor.h"

// Prints out the heroe's positions at any given time.
class HeroTrackingVisitor : public Visitor {
public:
    HeroTrackingVisitor() {
        std::cout << "tick,entity_id,class_name,player_name,x,y,cx,cy,cz" << std::endl;
    }

    virtual void visit_tick(uint32_t t) {
        _tick = t;
    }

    virtual bool visit_entity_created(const Entity &entity) {
        // Shouldn't do it this way, it's just too clever:
        // _hph... returns true if it used the entity, in which case it is not a hero -> short-circuit.
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

            std::cout << _tick << ","
                      << hero.id << ","
                      << hero.clazz->name << ","
                      << "\"" << player << "\","
                      << origin_prop->values[0] << ","
                      << origin_prop->values[1] << ","
                      << cell_x << ","
                      << cell_y << ","
                      << cell_z << std::endl;
        } catch(const std::out_of_range& e) {
            XASSERT(false, "%s", e.what());
        } catch(const std::bad_cast& e) {
            XASSERT(false, "%s", e.what());
        }
        return true;
    }

private:
    uint32_t _tick;
    HeroPlayerHelper _hph;
};

#endif
