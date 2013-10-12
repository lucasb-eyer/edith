// Prints out the pairwise distances between all heroes at every single tick.
// It optionally includes the ancients as heroes such that these can be used as
// known-to-be-fixed reference points.

#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <vector>
#include <array>

#include "property.h"
#include "hero_player_helper.h"
#include "visitor.h"
#include "edith.h"

using namespace std;

int ptdist(int x1, int y1, int x2, int y2) {
    return static_cast<int>(sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}

template<bool IncludeAncients>
class HeroDistancesVisitor : public Visitor {
public:
    HeroDistancesVisitor()
    {
        cout << "tick";

        for(auto& i : _hero_pos)
            i = make_tuple(false, 0, 0);
        for(int i = 0 ; i < _hero_pos.size()-1 ; ++i)
            for(int j = i+1 ; j < _hero_pos.size() ; ++j)
                cout << ",d" << i << (IncludeAncients ? "_" : "")<< j;
        cout << endl;

        if(IncludeAncients) {
            _hero_info[10] = make_tuple(2, "Radiant Ancient");
            _hero_info[11] = make_tuple(3, "Dire Ancient");
        }
    }

    virtual ~HeroDistancesVisitor()
    {
        cerr << "[" << endl;
        for(int i = 0 ; i < _hero_info.size() ; ++i) {
            cerr << "{\"team\": " << get<0>(_hero_info[i]) << ", \"name\": \"" << get<1>(_hero_info[i]) << "\"}";
            if(i < _hero_info.size()-1)
                cerr << ", ";
            cerr << endl;
        }
        cerr << "]" << endl;
    }

    virtual bool visit_entity_created(const Entity &entity) {
        return _hph.visit_entity_created(entity) || handle_entity(entity);
    }

    virtual bool visit_entity_updated(const Entity &entity) {
        return _hph.visit_entity_updated(entity) || handle_entity(entity);
    }

    virtual void visit_tick(uint32_t tick) {
        // if(_hero_pos.size() < 2)
        //     return;

        cout << tick;
        for(int i = 0 ; i < _hero_pos.size()-1 ; ++i) {
            for(int j = i+1 ; j < _hero_pos.size() ; ++j) {
                if(get<0>(_hero_pos[i]) && get<0>(_hero_pos[j])) {
                    int ix, iy, jx, jy;
                    tie(ignore, ix, iy) = _hero_pos[i];
                    tie(ignore, jx, jy) = _hero_pos[j];
                    cout << "," << ptdist(ix, iy, jx, jy);
                } else {
                    cout << ",0";
                }
            }
        }
        cout << endl;
    }

protected:
    bool handle_entity(const Entity &hero) {
        if(IncludeAncients && hero.clazz->name == "CDOTA_BaseNPC_Fort") {
            int cell_x = hero.properties.at("DT_DOTA_BaseNPC.m_cellX")->value_as<IntProperty>();
            int cell_y = hero.properties.at("DT_DOTA_BaseNPC.m_cellY")->value_as<IntProperty>();
            int cell_z = hero.properties.at("DT_DOTA_BaseNPC.m_cellZ")->value_as<IntProperty>();
            auto origin_prop = dynamic_pointer_cast<VectorXYProperty>(hero.properties.at("DT_DOTA_BaseNPC.m_vecOrigin"));
            int x = cell_x*128 + origin_prop->values[0];
            int y = cell_y*128 + origin_prop->values[1];
            int team = hero.properties.at("DT_BaseEntity.m_iTeamNum")->value_as<IntProperty>();
            _hero_pos[10 + team-2] = make_tuple(true, x, y);
            return true;
        }

        if(hero.clazz->name.find("CDOTA_Unit_Hero_") != 0)
            return false;

        // Most likely an illusion.
        string player = _hph.player_name_for_hero_id(hero.id);
        if(player.empty())
            return false;

        int cell_x = hero.properties.at("DT_DOTA_BaseNPC.m_cellX")->value_as<IntProperty>();
        int cell_y = hero.properties.at("DT_DOTA_BaseNPC.m_cellY")->value_as<IntProperty>();
        int cell_z = hero.properties.at("DT_DOTA_BaseNPC.m_cellZ")->value_as<IntProperty>();
        auto origin_prop = dynamic_pointer_cast<VectorXYProperty>(hero.properties.at("DT_DOTA_BaseNPC.m_vecOrigin"));
        int x = cell_x*128 + origin_prop->values[0];
        int y = cell_y*128 + origin_prop->values[1];
        int pid = hero.properties.at("DT_DOTA_BaseNPC_Hero.m_iPlayerID")->value_as<IntProperty>();

        // First time seen?
        if(!get<0>(_hero_pos[pid])) {
            _hero_info[pid] = make_tuple(hero.properties.at("DT_BaseEntity.m_iTeamNum")->value_as<IntProperty>(), _hph.heroname(hero));
        }
        _hero_pos[pid] = make_tuple(true, x, y);

        return true;
    }

private:
    HeroPlayerHelper _hph;
    array<tuple<bool, int, int>, IncludeAncients ? 12 : 10> _hero_pos;    // seen, x, y
    array<tuple<int, std::string>, IncludeAncients ? 12 : 10> _hero_info; // team, name
};

int main(int argc, char **argv) {
    if (argc <= 1) {
        cerr << "Usage: " << argv[0] << " something.dem" << endl;
        return 1;
    }

    HeroDistancesVisitor<true> visitor;
    dump(argv[1], visitor);
    return 0;
}

