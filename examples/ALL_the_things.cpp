// Prints out _everything_ entity-related edith can parse as a list of json
// objects, one json object per line.
#include <iostream>
#include <string>

#include "property.h"
#include "visitor.h"
#include "edith.h"

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
        std::cout << "\"tick\": " << _tick << ", ";
        for(const auto& prop : hero.properties) {
            // TODO: not always stringify!
            std::cout << "\"" << prop.first << "\": " << *prop.second << ", ";
        }
        std::cout << "}" << std::endl;
        return true;
    }

private:
    uint32_t _tick;
};

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " something.dem" << std::endl;
        return 1;
    }

    OutputALLTheThingsVisitor visitor;
    dump(argv[1], visitor);
    return 0;
}
