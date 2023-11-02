#include <unordered_set> //in std library but wasn't working without including this
#include "util.h"

using namespace std;

pair<User, Vector3> find_closest_user(Vector3 v, 
                                    std::unordered_set<User> checked, 
                                    map<User, Vector3> users){
    Vector3 zero = Vector3();
    pair<User, Vector3> closest = {-1, zero};
    float min_degree = std::numeric_limits<float>::infinity();
    
    for(auto user:users){
        //user already is taken
        if(checked.find(user.first) != checked.end()){
            continue;
        }
        float angle_between = 180 - user.second.angle_between(zero, v);
        if(angle_between <= 45 && angle_between < min_degree){
            min_degree = angle_between;
            closest = {user.first, user.second};
        }
    }
    return closest;
}

pair<User, Vector3> find_user_fast(Vector3 v, 
                                    std::unordered_set<User> checked, 
                                    map<User, Vector3> users){
    Vector3 zero = Vector3();
    pair<User, Vector3> fast_user = {-1, zero};
    for(auto user:users){
        //user already is taken
        if(checked.find(user.first) != checked.end()){
            continue;
        }
        float angle_between = 180 - user.second.angle_between(zero, v);
        if(angle_between <= 45){
            fast_user = {user.first, user.second};
            break;
        }
    }
    return fast_user;
}


std::map<User, std::pair<Sat, Color>> solve(
    const std::map<User, Vector3>& users,
    const std::map<Sat, Vector3>& sats)
{
    std::map<User, std::pair<Sat, Color>> res;
    vector<Color> colors = {'A', 'B', 'C', 'D'};
    for(auto sat:sats){
        int beams_left = 32;
        int color_index = 0;
        std::map<Color, vector<Vector3>> beams_by_color;

        std::unordered_set<User> checked;
        for(auto r:res){
            checked.insert(r.first);
        }

        //begin greedy solution, adding beams of a certain color to the closest, valid users
        //once all valid users(up to 32) of all beam colors are added, move on to next sat
        while(color_index != 4){
            pair<User, Vector3> closest_user;            
            if(users.size() > 25000){
                //for larger tests, use a faster method to find the next user (case 5 does not pass without this)
                closest_user = find_user_fast(sat.second, checked, users);
            }
            else{
                closest_user = find_closest_user(sat.second, checked, users);
            }            
            checked.insert(closest_user.first);

            //if no valid users are found, try with the next beam color
            if(closest_user.first == -1){
                ++color_index;
                checked.clear();
                for(auto r:res){
                    checked.insert(r.first);
                }
                continue;
            }
           
            bool valid = true;
            for(auto beam:beams_by_color[colors[color_index]]){
                if(sat.second.angle_between(closest_user.second, beam) < 10){
                    valid = false;
                    break;
                }
            }
            if(!valid){
                continue;
            }
            if(beams_left > 0){
                --beams_left;
                res[closest_user.first] = {sat.first, colors[color_index]};
                beams_by_color[colors[color_index]].push_back(closest_user.second);
            }
        }
    }
    return res;
}



