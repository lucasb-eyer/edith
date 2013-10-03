#ifndef _OUTPUT_ALL_THE_THINGS_H
#define _OUTPUT_ALL_THE_THINGS_H

#include <iostream>
#include <string>

#include "debug.h"
#include "property.h"
#include "visitor.h"

// Prints out the heroe's positions at any given time.
class OutputALLTheThingsVisitor : public Visitor {
public:
    virtual void visit_tick(uint32_t t) {
        _tick = t;
        std::cout << "{\"type\": \"tick\", \"tick\": " << t << "}" << std::endl;
    }

    virtual bool visit_entity_created(const Entity &entity) {
        return dump_entity("new_entity", entity);
    }

    virtual bool visit_entity_updated(const Entity &entity) {
        return dump_entity("update_entity", entity);
    }

    virtual bool visit_entity_deleted(const Entity &entity) {
        return dump_entity("delete_entity", entity);
    }

protected:
    bool dump_entity(const std::string& type, const Entity &hero) {
        std::cout << "{\"type\": \"" << type << "\", ";
        std::cout << "\"class\": \"" << hero.clazz->name << "\", ";
        for(const auto& prop : hero.properties) {
            // TODO: not always stringify!
            std::cout << "\"" << prop.first << "\": \"" << *prop.second << "\", ";
        }
        std::cout << "}" << std::endl;
        return true;
    }

private:
    uint32_t _tick;
};

#endif
