// Prints out the heroe's positions at any given time.
// Difference between this and the other one, is that this one's output is
// exactly as needed by my python heatmap scripts

#include <iostream>
#include <fstream>
#include <string>

#include "property.h"
#include "hero_player_helper.h"
#include "visitor.h"
#include "edith.h"

class OutputAllHeroThingsVisitor : public Visitor {
public:
    OutputAllHeroThingsVisitor()
        : _tick(0)
    { }

    virtual void visit_tick(uint32_t t) {
        _tick = t;
    }

    virtual ~OutputAllHeroThingsVisitor()
    { }

    virtual bool visit_entity_created(const Entity &entity) {
        return _hph.visit_entity_created(entity) || handle_entity("new", entity);
    }

    virtual bool visit_entity_updated(const Entity &entity) {
        return _hph.visit_entity_updated(entity) || handle_entity("updated", entity);
    }

protected:
    bool handle_entity(const std::string& type, const Entity &hero) {
        if(hero.clazz->name.find("CDOTA_Unit_Hero_") != 0)
            return false;

        // Most likely an illusion.
        std::string player = _hph.player_name_for_hero_id(hero.id);
        if(player.empty())
            return false;

        std::cout << "{\"type\": \"" << type << "\", ";
        std::cout << "\"class\": \"" << hero.clazz->name << "\", ";
        std::cout << "\"tick\": " << _tick << ", ";
        for(const auto& prop : hero.properties) {
            // TODO: not always stringify!
            std::cout << "\"" << prop.first << "\": " << *prop.second << ", ";
        }
        std::cout << "}" << std::endl;

        return true;
    }

private:
    HeroPlayerHelper _hph;
    uint32_t _tick;
};

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " something.dem" << std::endl;
        return 1;
    }

    OutputAllHeroThingsVisitor visitor;
    dump(argv[1], visitor);
    return 0;
}

