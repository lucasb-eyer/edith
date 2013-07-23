#ifndef _HERO_PLAYER_HELPER_H
#define _HERO_PLAYER_HELPER_H

#include <map>
#include <string>

#include "debug.h"
#include "entity.h"
#include "property.h"
#include "visitor.h"

// Can be used (by aggregation, not inheritance) to keep track of
// the mapping heroid -> playername.
class HeroPlayerHelper : public Visitor {
public:
    virtual void visit_entity_created(const Entity &entity) {
        update_name_map(entity);
    }

    virtual void visit_entity_updated(const Entity &entity) {
        update_name_map(entity);
    }

    bool is_hero(uint32_t entityid) const {
        return _hero_to_playername.find(entityid) != _hero_to_playername.end();
    }

    std::string player_name_for_hero_id(uint32_t heroid) const {
        auto name = _hero_to_playername.find(heroid);

        // Mistake, illusion, or some other weird thing.
        return name != _hero_to_playername.end() ? name->second : "";
    }

protected:
    // This handles the CDOTA_PlayerResource entitiy.
    // We want the 24 send props from the m_iszPlayerNames table named 0000-0023
    // that contain the names of the (up to) 24 people connected and we want the 24
    // send props from m_hSelectedHero that contain the entity ID of the heroes they
    // selected.
    void update_name_map(const Entity &player_resource) {
        if(player_resource.clazz->name != "CDOTA_PlayerResource") {
            return;
        }

        // TODO: go up to 23 (inclusive) if interested in others, like casters.
        // Valve packs some additional data in the upper bits, we only care about the lower ones.
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0000")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0000")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0001")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0001")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0002")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0002")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0003")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0003")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0004")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0004")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0005")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0005")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0006")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0006")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0007")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0007")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0008")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0008")->value_as<StringProperty>();
        _hero_to_playername[player_resource.properties.at("m_hSelectedHero.0009")->value_as<IntProperty>() & 0x7FF] = player_resource.properties.at("m_iszPlayerNames.0009")->value_as<StringProperty>();

        // XASSERT(_hero_to_playername.size() == 24, "Player names not complete");
    }

private:
    std::map<uint32_t, std::string> _hero_to_playername;
};

#endif
