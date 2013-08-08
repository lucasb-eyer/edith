#ifndef _HERO_NAMES_PLAYERS_VISITOR_H
#define _HERO_NAMES_PLAYERS_VISITOR_H

#include <iostream>
#include <string>

#include "debug.h"
#include "property.h"
#include "hero_player_helper.h"
#include "visitor.h"

// Figures out which entity ID corresponds to which hero and by
// which player he is controlled.
class HeroNamesPlayersVisitor : public Visitor {
public:
    HeroNamesPlayersVisitor() {
        // std::cout << "tick,entity_id,class_name,player_name,x,y,cx,cy,cz" << std::endl;
    }

    virtual ~HeroNamesPlayersVisitor() {
        std::cout << "{"
                  << "  " << entid << ""
                  << "}"
    }

    virtual bool visit_entity_created(const Entity &entity) {
        return _hph.visit_entity_created(entity);
    }

    virtual bool visit_entity_updated(const Entity &entity) {
        return _hph.visit_entity_updated(entity);
    }

private:
    HeroPlayerHelper _hph;
};

#endif
