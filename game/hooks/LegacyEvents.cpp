//
// Created by Twelvee on 11.10.2023.
//

#include "LegacyEvents.h"
#include "EventManager.h"

template<typename T>
RMEvent *CreateEventObject(IGameEvent *event) { return new T(event); }

void EventManager::FillLegacyEventsMap() {
    this->events["player_activate"] = &CreateEventObject<player_activate>;
    this->events["player_spawn"] = &CreateEventObject<player_spawn>;
    this->events["player_team"] = &CreateEventObject<player_team>;
    this->events["player_hurt"] = &CreateEventObject<player_hurt>;
    this->events["game_message"] = &CreateEventObject<game_message>;
    this->events["round_end"] = &CreateEventObject<round_end>;
    this->events["round_start"] = &CreateEventObject<round_start>;
    this->events["round_freeze_end"] = &CreateEventObject<round_freeze_end>;
    this->events["player_death"] = &CreateEventObject<player_death>;
    this->events["player_footstep"] = &CreateEventObject<player_footstep>;
    this->events["break_prop"] = &CreateEventObject<break_prop>;
    this->events["item_purchase"] = &CreateEventObject<item_purchase>;
    this->events["bomb_beginplant"] = &CreateEventObject<bomb_beginplant>;
    this->events["bomb_planted"] = &CreateEventObject<bomb_planted>;
    this->events["bomb_defused"] = &CreateEventObject<bomb_defused>;
    this->events["bomb_exploded"] = &CreateEventObject<bomb_exploded>;
    this->events["bomb_dropped"] = &CreateEventObject<bomb_dropped>;
    this->events["bomb_pickup"] = &CreateEventObject<bomb_pickup>;
    this->events["defuser_dropped"] = &CreateEventObject<defuser_dropped>;
    this->events["defuser_pickup"] = &CreateEventObject<defuser_pickup>;
    this->events["bomb_begindefuse"] = &CreateEventObject<bomb_begindefuse>;
    this->events["bomb_abortdefuse"] = &CreateEventObject<bomb_abortdefuse>;
    this->events["hostage_follows"] = &CreateEventObject<hostage_follows>;
    this->events["hostage_hurt"] = &CreateEventObject<hostage_hurt>;
    this->events["hostage_rescued"] = &CreateEventObject<hostage_rescued>;
    this->events["hostage_stops_following"] = &CreateEventObject<hostage_stops_following>;
    this->events["weapon_fire"] = &CreateEventObject<weapon_fire>;
    this->events["weapon_reload"] = &CreateEventObject<weapon_reload>;
    this->events["weapon_zoom"] = &CreateEventObject<weapon_zoom>;
    this->events["item_pickup"] = &CreateEventObject<item_pickup>;
    this->events["player_blind"] = &CreateEventObject<player_blind>;
}