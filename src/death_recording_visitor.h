#ifndef _DEATH_RECORDING_VISITOR_H
#define _DEATH_RECORDING_VISITOR_H

// Maps player names to the entity of the hero they selected and outputs the position
// of that hero everytime they are updated and their health is set to zero.
//
// This shows an example usage of this API but is hilariously inefficient and terrible.

#include <iostream>
#include <map>
#include <string>

#include "debug.h"
#include "property.h"
#include "hero_player_helper.h"
#include "visitor.h"

class DeathRecordingVisitor : public Visitor {
public:
  DeathRecordingVisitor() {
    std::cout << "tick,entity_id,class_name,player_name,health,x,y,cx,cy,cz" << std::endl;
  }

  virtual void visit_tick(uint32_t t) {
    _tick = t;
  }

  virtual void visit_entity_created(const Entity &entity) {
    _hph.visit_entity_created(entity);
    update_hero(entity);
  }

  virtual void visit_entity_updated(const Entity &entity) {
    _hph.visit_entity_updated(entity);
    update_hero(entity);
  }

protected:
  // This handles CDOTA_Unit_Hero_* entities.
  // We first check if it's in our name map, and if it isn't then it must be an illusion
  // or something. After that we make sure the hero has 0 health and if it does we output
  // it.
  void update_hero(const Entity &hero) {
    using std::dynamic_pointer_cast;
    using std::shared_ptr;
    using std::cout;
    using std::endl;

    if (hero.clazz->name.find("CDOTA_Unit_Hero_") != 0)
      return;

    std::string playername = _hph.player_name_for_hero_id(hero.id);
    if (playername.empty()) {
      // An illusion.
      return;
    }

    try {
      int life = hero.properties.at("DT_DOTA_BaseNPC.m_iHealth")->value_as<IntProperty>();
      if (life > 0) {
        return;
      }

      int cell_x = hero.properties.at("DT_DOTA_BaseNPC.m_cellX")->value_as<IntProperty>();
      int cell_y = hero.properties.at("DT_DOTA_BaseNPC.m_cellY")->value_as<IntProperty>();
      int cell_z = hero.properties.at("DT_DOTA_BaseNPC.m_cellZ")->value_as<IntProperty>();
      auto origin_prop = dynamic_pointer_cast<VectorXYProperty>(hero.properties.at("DT_DOTA_BaseNPC.m_vecOrigin"));

      cout << _tick << "," << hero.id << "," << hero.clazz->name << ",";
      cout << "\"" << playername << "\",";
      cout << life << ",";
      cout << origin_prop->values[0] << ",";
      cout << origin_prop->values[1] << ",";
      cout << cell_x << ",";
      cout << cell_y << ",";
      cout << cell_z << endl;
    } catch(const std::out_of_range& e) {
      XASSERT(false, "%s", e.what());
    } catch(const std::bad_cast& e) {
      XASSERT(false, "%s", e.what());
    }
  }

private:
  uint32_t _tick;
  HeroPlayerHelper _hph;
};

#endif
