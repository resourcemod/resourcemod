//
// Created by Twelvee on 10.10.2023.
//

#ifndef RESOURCEMOD_LEGACYEVENTS_H
#define RESOURCEMOD_LEGACYEVENTS_H

#include <entityhandle.h>
#include <igameevents.h>
#include "../cs2/cbaseentity.h"
#include "../../logger/logger.h"
#include "../cs2/cbaseplayercontroller.h"
#include <v8.h>
#include "../../engine/Engine.h"
#include "../js/Player.h"

extern Engine* g_Engine;

class RMEvent {
public:
    const char *event_name;
    virtual v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) = 0;
};

class server_spawn : public RMEvent {
public:
    server_spawn(IGameEvent *event) {
        this->hostname = event->GetString("hostname");
        this->address = event->GetString("address");
        this->port = event->GetInt("port");
        this->game = event->GetString("game");
        this->mapname = event->GetString("mapname");
        this->addonname = event->GetString("addonname");
        this->maxplayers = event->GetInt("maxplayers");
        this->os = event->GetString("os");
        this->dedicated = event->GetBool("dedicated");
        this->password = event->GetBool("password");
    };
    const char *event_name = "server_spawn";
    const char *hostname;
    const char *address;
    int port;
    const char *game;
    const char *mapname;
    const char *addonname;
    int maxplayers;
    const char *os;
    bool dedicated;
    bool password;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        // todo: implement
        return v8::Null(g_Engine->isolate).As<v8::Object>();
    };
};

/*
class server_pre_shutdown : public RMEvent {
public:
    server_pre_shutdown(IGameEvent *event) {
        this->reason = event->GetString("reason");
    };
    const char *event_name = "server_pre_shutdown";
    const char *reason;
};

class server_shutdown : public RMEvent {
public:
    server_shutdown(IGameEvent *event) {
        this->reason = event->GetString("reason");
    };
    const char *event_name = "server_shutdown";
    const char *reason;
};

class server_message : public RMEvent {
public:
    server_message(IGameEvent *event) {
        this->text = event->GetString("text");
    };
    const char *event_name = "server_message";
    const char *text;
};

class server_cvar : public RMEvent {
public:
    server_cvar(IGameEvent *event) {
        this->cvarname = event->GetString("cvarname");
        this->cvarvalue = event->GetString("cvarvalue");
    };
    const char *event_name = "server_cvar";
    const char *cvarname;
    const char *cvarvalue;
};

class player_activate : public RMEvent {
public:
    player_activate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "player_activate";
    IHandleEntity *userid; //playercontroller
};

class player_connect_full : public RMEvent {
public:
    player_connect_full(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "player_connect_full";
    IHandleEntity *userid; //playercontroller
};

class player_full_update : public RMEvent {
public:
    player_full_update(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "player_full_update";
    IHandleEntity *userid; //playercontroller
};

class player_connect : public RMEvent {
public:
    player_connect(IGameEvent *event) {
        this->name = event->GetString("name");
        this->userid = event->GetPlayerController("userid");
        this->networkid = event->GetString("networkid");
        this->xuid = event->GetInt("xuid");
        this->address = event->GetString("address");
        this->bot = event->GetBool("bot");
    };
    const char *event_name = "player_connect";
    const char *name;
    IHandleEntity *userid; //playercontroller
    const char *networkid; // i.e. steam id
    int xuid; // steamid
    const char *address; //ip:port
    bool bot;
};

class player_disconnect : public RMEvent {
public:
    player_disconnect(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->reason = event->GetInt("reason");
        this->name = event->GetString("name");
        this->networkid = event->GetString("networkid");
        this->xuid = event->GetInt("xuid");
        this->PlayerID = event->GetInt("PlayerID");
    };
    const char *event_name = "player_disconnect";
    IHandleEntity *userid; //playercontroller
    int reason;
    const char *name;
    const char *networkid; // i.e. steam id
    int xuid; // steamid
    int PlayerID; //?
};

class player_info : public RMEvent {
public:
    player_info(IGameEvent *event) {
        this->name = event->GetString("name");
        this->userid = event->GetPlayerController("userid");
        this->steamid = event->GetInt("steamid");
        this->bot = event->GetBool("bot");
    };
    const char *event_name = "player_info";
    const char *name;
    IHandleEntity *userid;//playercontroller
    int steamid;
    bool bot;
};

class player_spawn : public RMEvent {
public:
    player_spawn(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "player_spawn";
    IHandleEntity *userid; //playercontroller
    IHandleEntity *userid_pawn; //strict_ehandle
};

class player_team : public RMEvent {
public:
    player_team(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->team = event->GetInt("team");
        this->oldteam = event->GetInt("oldteam");
        this->disconnect = event->GetBool("disconnect");
        this->silent = event->GetBool("silent");
        this->isbot = event->GetBool("isbot");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "player_team";
    IHandleEntity *userid;//playercontroller
    int team;
    int oldteam;
    bool disconnect;
    bool silent;
    bool isbot;
    IHandleEntity *userid_pawn; //strict_ehandle
};

class player_changename : public RMEvent {
public:
    player_changename(IGameEvent *event) {
        this->oldname = event->GetString("oldname");
        this->newname = event->GetString("newname");
    };
    const char *event_name = "player_changename";
    const char *oldname;
    const char *newname;
};

class player_hurt : public RMEvent {
public:
    player_hurt(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->attacker = event->GetPlayerController("attacker");
        this->health = event->GetInt("health");
        this->armor = event->GetInt("armor");
        this->weapon = event->GetString("weapon");
        this->dmg_health = event->GetInt("dmg_health");
        this->dmg_armor = event->GetInt("dmg_armor");
        this->hitgroup = event->GetInt("hitgroup");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
        this->attacker_pawn = event->GetPlayerPawn("attacker_pawn");
    };
    const char *event_name = "player_hurt";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *attacker; //playercontroller
    int health;
    int armor;
    const char *weapon;
    int dmg_health;
    int dmg_armor;
    int hitgroup;
    IHandleEntity *userid_pawn; //strict_ehandle
    IHandleEntity *attacker_pawn; //strict_ehandle
};

class player_chat : public RMEvent {
public:
    player_chat(IGameEvent *event) {
        this->teamonly = event->GetBool("teamonly");
        this->userid = event->GetInt("userid");
        this->text = event->GetString("text");
    };
    const char *event_name = "player_chat";
    bool teamonly;
    int userid;
    const char *text;
};

class teamplay_broadcast_audio : public RMEvent {
public:
    teamplay_broadcast_audio(IGameEvent *event) {
        this->team = event->GetInt("team");
        this->sound = event->GetString("sound");
    };
    const char *event_name = "teamplay_broadcast_audio";
    int team;
    const char *sound;
};

class player_stats_updated : public RMEvent {
public:
    player_stats_updated(IGameEvent *event) {
        this->forceupload = event->GetBool("forceupload");
    };
    const char *event_name = "player_stats_updated";
    bool forceupload;
};

// Note: Fired when achievements/stats are downloaded from Steam or XBox Live
class user_data_downloaded : public RMEvent {
public:
    user_data_downloaded(IGameEvent *event) {
    };
    const char *event_name = "user_data_downloaded";
};

class team_info : public RMEvent {
public:
    team_info(IGameEvent *event) {
        this->teamid = event->GetInt("teamid");
        this->teamname = event->GetString("teamname");
    };
    const char *event_name = "team_info";
    int teamid;
    const char *teamname;
};

// Team score changed
class team_score : public RMEvent {
public:
    team_score(IGameEvent *event) {
        this->teamid = event->GetInt("teamid");
        this->score = event->GetInt("score");
    };
    const char *event_name = "team_score";
    int teamid;
    int score;
};

class map_shutdown : public RMEvent {
public:
    map_shutdown(IGameEvent *event) {
    };
    const char *event_name = "map_shutdown";
};

class map_transition : public RMEvent {
public:
    map_transition(IGameEvent *event) {
    };
    const char *event_name = "map_transition";
};

class hostname_changed : public RMEvent {
public:
    hostname_changed(IGameEvent *event) {
        this->hostname = event->GetString("hostname");
    };
    const char *event_name = "hostname_changed";
    const char *hostname;
};

class difficulty_changed : public RMEvent {
public:
    difficulty_changed(IGameEvent *event) {
        this->newDifficulty = event->GetInt("newDifficulty");
        this->oldDifficulty = event->GetInt("oldDifficulty");
        this->strDifficulty = event->GetString("strDifficulty");
    };
    const char *event_name = "difficulty_changed";
    int newDifficulty;
    int oldDifficulty;
    const char *strDifficulty;
};

class game_message : public RMEvent {
public:
    game_message(IGameEvent *event) {
        this->target = event->GetInt("target");
        this->text = event->GetString("text");
    };
    const char *event_name = "game_message";
    int target; //0- console, 1 - hud
    const char *text;
};

class game_newmap : public RMEvent {
public:
    game_newmap(IGameEvent *event) {
        this->mapname = event->GetString("mapname");
    };
    const char *event_name = "game_newmap";
    const char *mapname;
};

class round_start : public RMEvent {
public:
    round_start(IGameEvent *event) {
        this->timelimit = event->GetInt("timelimit");
        this->fraglimit = event->GetInt("fraglimit");
        this->objective = event->GetString("objective");
    };
    const char *event_name = "round_start";
    int timelimit;
    int fraglimit;
    const char *objective;
};

class round_end : public RMEvent {
public:
    round_end(IGameEvent *event) {
        this->winner = event->GetInt("winner");
        this->reason = event->GetInt("reason");
        this->message = event->GetString("message");
        this->legacy = event->GetInt("legacy");
        this->player_count = event->GetInt("player_count");
        this->nomusic = event->GetInt("nomusic");
    };
    const char *event_name = "round_end";
    int winner;
    int reason;
    const char *message;
    int legacy;
    int player_count;
    int nomusic;
};

class round_freeze_end : public RMEvent {
public:
    round_freeze_end(IGameEvent *event) {
    };
    const char *event_name = "round_freeze_end";
};

class player_death : public RMEvent {
public:
    player_death(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->attacker = event->GetPlayerController("attacker");
        this->assister = event->GetPlayerController("assister");
        this->assistedflash = event->GetBool("assistedflash");
        this->weapon = event->GetString("weapon");
        this->weapon_itemid = event->GetString("weapon_itemid");
        this->weapon_fauxitemid = event->GetString("weapon_fauxitemid");
        this->weapon_originalowner_xuid = event->GetString("weapon_originalowner_xuid");
        this->headshot = event->GetBool("headshot");
        this->dominated = event->GetInt("dominated");
        this->revenge = event->GetInt("revenge");
        this->wipe = event->GetInt("wipe");
        this->penetrated = event->GetInt("penetrated");
        this->noreplay = event->GetBool("noreplay");
        this->noscope = event->GetBool("noscope");
        this->thrusmoke = event->GetBool("thrusmoke");
        this->attackerblind = event->GetBool("attackerblind");
        this->distance = event->GetFloat("distance");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
        this->attacker_pawn = event->GetPlayerPawn("attacker_pawn");
        this->assister_pawn = event->GetPlayerPawn("assister_pawn");
    };
    const char *event_name = "player_death";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *attacker;//playercontroller
    IHandleEntity *assister;//playercontroller
    bool assistedflash;
    const char *weapon;
    const char *weapon_itemid;
    const char *weapon_fauxitemid;
    const char *weapon_originalowner_xuid;
    bool headshot;
    int dominated;
    int revenge;
    int wipe;
    int penetrated;
    bool noreplay;
    bool noscope;
    bool thrusmoke;
    bool attackerblind;
    float distance;
    IHandleEntity *userid_pawn;//strict_ehandle
    IHandleEntity *attacker_pawn;//strict_ehandle
    IHandleEntity *assister_pawn;//strict_ehandle
};

class player_footstep : public RMEvent {
public:
    player_footstep(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "player_footstep";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle
};

class player_hintmessage : public RMEvent {
public:
    player_hintmessage(IGameEvent *event) {
        this->hintmessage = event->GetString("hintmessage");
    };
    const char *event_name = "player_hintmessage";
    const char *hintmessage;
};

class break_breakable : public RMEvent {
public:
    break_breakable(IGameEvent *event) {
        this->entindex = event->GetInt("entindex");
        this->userid = event->GetPlayerController("userid");
        this->material = event->GetInt("material");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "break_breakable";
    int entindex;
    IHandleEntity *userid; //player_pawn
    int material; // BREAK_GLASS, BREAK_WOOD etc TODO: check what acutually happened
    IHandleEntity *userid_pawn; //strict_ehandle
};

class break_prop : public RMEvent {
public:
    break_prop(IGameEvent *event) {
        this->entindex = event->GetInt("entindex");
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "break_prop";
    int entindex;
    IHandleEntity *userid;// player_pawn
    IHandleEntity *userid_pawn; //strict_ehandle
};

class entity_killed : public RMEvent {
public:
    entity_killed(IGameEvent *event) {
        this->entindex_killed = event->GetInt("entindex_killed");
        this->entindex_attacker = event->GetInt("entindex_attacker");
        this->entindex_inflictor = event->GetInt("entindex_inflictor");
        this->damagebits = event->GetInt("damagebits");
    };
    const char *event_name = "entity_killed";
    int entindex_killed;
    int entindex_attacker;
    int entindex_inflictor;
    int damagebits;
};

class door_close : public RMEvent {
public:
    door_close(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->checkpoint = event->GetBool("checkpoint");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "door_close";
    IHandleEntity *userid; //player_pawn
    bool checkpoint;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class vote_started : public RMEvent {
public:
    vote_started(IGameEvent *event) {
        this->issue = event->GetString("issue");
        this->param1 = event->GetString("param1");
        this->team = event->GetInt("team");
        this->initiator = event->GetInt("initiator");
    };
    const char *event_name = "vote_started";
    const char *issue;
    const char *param1;
    int team;
    int initiator;
};

class vote_failed : public RMEvent {
public:
    vote_failed(IGameEvent *event) {
        this->team = event->GetInt("team");
    };
    const char *event_name = "vote_failed";
    int team;
};

class vote_passed : public RMEvent {
public:
    vote_passed(IGameEvent *event) {
        this->details = event->GetString("details");
        this->param1 = event->GetString("param1");
        this->team = event->GetInt("team");
    };
    const char *event_name = "vote_passed";
    const char *details;
    const char *param1;
    int team;
};

class vote_changed : public RMEvent {
public:
    vote_changed(IGameEvent *event) {
        this->vote_option1 = event->GetInt("vote_option1");
        this->vote_option2 = event->GetInt("vote_option2");
        this->vote_option3 = event->GetInt("vote_option3");
        this->vote_option4 = event->GetInt("vote_option4");
        this->vote_option5 = event->GetInt("vote_option5");
        this->potentialVotes = event->GetInt("potentialVotes");
    };
    const char *event_name = "vote_changed";
    int vote_option1;
    int vote_option2;
    int vote_option3;
    int vote_option4;
    int vote_option5;
    int potentialVotes;
};

class vote_cast_yes : public RMEvent {
public:
    vote_cast_yes(IGameEvent *event) {
        this->team = event->GetInt("team");
        this->entityid = event->GetInt("entityid");
    };
    const char *event_name = "vote_cast_yes";
    int team;
    int entityid;
};

class vote_cast_no : public RMEvent {
public:
    vote_cast_no(IGameEvent *event) {
        this->team = event->GetInt("team");
        this->entityid = event->GetInt("entityid");
    };
    const char *event_name = "vote_cast_no";
    int team;
    int entityid;
};

class achievement_event : public RMEvent {
public:
    achievement_event(IGameEvent *event) {
        this->achievement_name = event->GetString("achievement_name");
        this->cur_val = event->GetInt("cur_val");
        this->max_val = event->GetInt("max_val");
    };
    const char *event_name = "achievement_event";
    const char *achievement_name;
    int cur_val;
    int max_val;
};

class achievement_earned : public RMEvent {
public:
    achievement_earned(IGameEvent *event) {
        this->player = event->GetPlayerController("player");
        this->achievement = event->GetInt("achievement");
    };
    const char *event_name = "achievement_earned";
    IHandleEntity *player; //playercontroller
    int achievement;
};

class spec_target_updated : public RMEvent {
public:
    spec_target_updated(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->target = event->GetInt("target");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "spec_target_updated";
    IHandleEntity *userid; //playercontroller
    int target;
    IHandleEntity *userid_pawn; //strict_ehandle
};

class spec_mode_updated : public RMEvent {
public:
    spec_mode_updated(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "spec_mode_updated";
    IHandleEntity *userid; //playercontroller
};

class entity_visible : public RMEvent {
public:
    entity_visible(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->subject = event->GetInt("subject");
        this->classname = event->GetString("classname");
        this->entityname = event->GetString("entityname");
    };
    const char *event_name = "entity_visible";
    IHandleEntity *userid;//playercontroller
    int subject;
    const char *classname;
    const char *entityname;
};

class items_gifted : public RMEvent {
public:
    items_gifted(IGameEvent *event) {
        this->player = event->GetPlayerController("player");
        this->itemdef = event->GetInt("itemdef");
        this->numgifts = event->GetInt("numgifts");
        this->giftidx = event->GetInt("giftidx");
        this->accountid = event->GetInt("accountid");
    };
    const char *event_name = "items_gifted";
    IHandleEntity *player;//playercontroller
    int itemdef;
    int numgifts;
    int giftidx;
    int accountid;
};

class player_score : public RMEvent {
public:
    player_score(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->kills = event->GetInt("kills");
        this->deaths = event->GetInt("deaths");
        this->score = event->GetInt("score");
    };
    const char *event_name = "player_score";
    IHandleEntity *userid; //playercontroller
    int kills;
    int deaths;
    int score;
};

class player_shoot : public RMEvent {
public:
    player_shoot(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetInt("weapon");
        this->mode = event->GetInt("mode");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "player_shoot";
    IHandleEntity *userid; //playercontroller
    int weapon;
    int mode;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class game_init : public RMEvent {
public:
    game_init(IGameEvent *event) {
    };
    const char *event_name = "game_init";
};

class game_start : public RMEvent {
public:
    game_start(IGameEvent *event) {
        this->roundslimit = event->GetInt("roundslimit");
        this->timelimit = event->GetInt("timelimit");
        this->fraglimit = event->GetInt("fraglimit");
        this->objective = event->GetString("objective");
    };
    const char *event_name = "game_start";
    int roundslimit;
    int timelimit;
    int fraglimit;
    const char *objective;
};

class game_end : public RMEvent {
public:
    game_end(IGameEvent *event) {
        this->winner = event->GetInt("winner");
    };
    const char *event_name = "game_end";
    int winner;
};

class round_announce_match_point : public RMEvent {
public:
    round_announce_match_point(IGameEvent *event) {
    };
    const char *event_name = "round_announce_match_point";
};

class round_announce_final : public RMEvent {
public:
    round_announce_final(IGameEvent *event) {
    };
    const char *event_name = "round_announce_final";
};

class round_announce_last_round_half : public RMEvent {
public:
    round_announce_last_round_half(IGameEvent *event) {
    };
    const char *event_name = "round_announce_last_round_half";
};

class round_announce_match_start : public RMEvent {
public:
    round_announce_match_start(IGameEvent *event) {
    };
    const char *event_name = "round_announce_match_start";
};

class round_announce_warmup : public RMEvent {
public:
    round_announce_warmup(IGameEvent *event) {
    };
    const char *event_name = "round_announce_warmup";
};

class round_end_upload_stats : public RMEvent {
public:
    round_end_upload_stats(IGameEvent *event) {
    };
    const char *event_name = "round_end_upload_stats";
};

class round_officially_ended : public RMEvent {
public:
    round_officially_ended(IGameEvent *event) {
    };
    const char *event_name = "round_officially_ended";
};

class round_time_warning : public RMEvent {
public:
    round_time_warning(IGameEvent *event) {
    };
    const char *event_name = "round_time_warning";
};

class begin_new_match : public RMEvent {
public:
    begin_new_match(IGameEvent *event) {
    };
    const char *event_name = "begin_new_match";
};

class other_death : public RMEvent {
public:
    other_death(IGameEvent *event) {
        this->otherid = event->GetInt("otherid");
        this->othertype = event->GetString("othertype");
        this->attacker = event->GetInt("attacker");
        this->weapon = event->GetString("weapon");
        this->weapon_itemid = event->GetString("weapon_itemid");
        this->weapon_fauxitemid = event->GetString("weapon_fauxitemid");
        this->weapon_originalowner_xuid = event->GetString("weapon_originalowner_xuid");
        this->headshot = event->GetBool("headshot");
        this->penetrated = event->GetInt("penetrated");
        this->noscope = event->GetBool("noscope");
        this->thrusmoke = event->GetBool("thrusmoke");
        this->attackerblind = event->GetBool("attackerblind");
    };
    const char *event_name = "other_death";
    int otherid;
    const char *othertype;
    int attacker;
    const char *weapon;
    const char *weapon_itemid;
    const char *weapon_fauxitemid;
    const char *weapon_originalowner_xuid;
    bool headshot;
    int penetrated;
    bool noscope;
    bool thrusmoke;
    bool attackerblind;
};

class item_purchase : public RMEvent {
public:
    item_purchase(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->team = event->GetInt("team");
        this->loadout = event->GetInt("loadout");
        this->weapon = event->GetString("weapon");
    };
    const char *event_name = "item_purchase";
    IHandleEntity *userid; //playercontroller
    int team;
    int loadout;
    const char *weapon;
};

class bomb_beginplant : public RMEvent {
public:
    bomb_beginplant(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_beginplant";
    IHandleEntity *userid;//playercontroller
    int site;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_abortplant : public RMEvent {
public:
    bomb_abortplant(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_abortplant";
    IHandleEntity *userid;//playercontroller
    int site;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_planted : public RMEvent {
public:
    bomb_planted(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_planted";
    IHandleEntity *userid;//playercontroller
    int site;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_defused : public RMEvent {
public:
    bomb_defused(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_defused";
    IHandleEntity *userid;//playercontroller
    int site;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_exploded : public RMEvent {
public:
    bomb_exploded(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_exploded";
    IHandleEntity *userid;//playercontroller
    int site;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_dropped : public RMEvent {
public:
    bomb_dropped(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entindex = event->GetInt("entindex");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_dropped";
    IHandleEntity *userid;//playercontroller
    int entindex;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_pickup : public RMEvent {
public:
    bomb_pickup(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_pickup";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle
};

class defuser_dropped : public RMEvent {
public:
    defuser_dropped(IGameEvent *event) {
        this->entityid = event->GetInt("entityid");
    };
    const char *event_name = "defuser_dropped";
    int entityid;
};

class defuser_pickup : public RMEvent {
public:
    defuser_pickup(IGameEvent *event) {
        this->entityid = event->GetInt("entityid");
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "defuser_pickup";
    int entityid;
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_begindefuse : public RMEvent {
public:
    bomb_begindefuse(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->haskit = event->GetBool("haskit");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_begindefuse";
    IHandleEntity *userid; //playercontroller
    bool haskit;
    IHandleEntity *userid_pawn; //strict_ehandle
};

class bomb_abortdefuse : public RMEvent {
public:
    bomb_abortdefuse(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bomb_abortdefuse";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle
};

class hostage_follows : public RMEvent {
public:
    hostage_follows(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "hostage_follows";
    IHandleEntity *userid; //playercontroller
    int hostage;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class hostage_hurt : public RMEvent {
public:
    hostage_hurt(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "hostage_hurt";
    IHandleEntity *userid;//playercontroller
    int hostage;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class hostage_killed : public RMEvent {
public:
    hostage_killed(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "hostage_killed";
    IHandleEntity *userid;//playercontroller
    int hostage;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class hostage_rescued : public RMEvent {
public:
    hostage_rescued(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "hostage_rescued";
    IHandleEntity *userid;//playercontroller
    int hostage;
    int site;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class hostage_stops_following : public RMEvent {
public:
    hostage_stops_following(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "hostage_stops_following";
    IHandleEntity *userid;//playercontroller
    int hostage;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class hostage_rescued_all : public RMEvent {
public:
    hostage_rescued_all(IGameEvent *event) {
    };
    const char *event_name = "hostage_rescued_all";
};

class hostage_call_for_help : public RMEvent {
public:
    hostage_call_for_help(IGameEvent *event) {
        this->hostage = event->GetInt("hostage");
    };
    const char *event_name = "hostage_call_for_help";
    int hostage;
};

class player_radio : public RMEvent {
public:
    player_radio(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->slot = event->GetInt("slot");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "player_radio";
    IHandleEntity *userid; //playercontroller
    int slot;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bomb_beep : public RMEvent {
public:
    bomb_beep(IGameEvent *event) {
        this->entindex = event->GetInt("entindex");
    };
    const char *event_name = "bomb_beep";
    int entindex;
};

class weapon_fire : public RMEvent {
public:
    weapon_fire(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetString("weapon");
        this->silenced = event->GetBool("silenced");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "weapon_fire";
    IHandleEntity *userid;//playercontroller
    const char *weapon;
    bool silenced;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class weapon_fire_on_empty : public RMEvent {
public:
    weapon_fire_on_empty(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetString("weapon");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "weapon_fire_on_empty";
    IHandleEntity *userid; //playercontroller
    const char *weapon;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class grenade_thrown : public RMEvent {
public:
    grenade_thrown(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetString("weapon");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "grenade_thrown";
    IHandleEntity *userid;//playercontroller
    const char *weapon;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class weapon_outofammo : public RMEvent {
public:
    weapon_outofammo(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "weapon_outofammo";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle;
};

class weapon_reload : public RMEvent {
public:
    weapon_reload(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "weapon_reload";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle;
};

class weapon_zoom : public RMEvent {
public:
    weapon_zoom(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "weapon_zoom";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle;
};

class silencer_detach : public RMEvent {
public:
    silencer_detach(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "silencer_detach";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle;
};

class inspect_weapon : public RMEvent {
public:
    inspect_weapon(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "inspect_weapon";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle;
};

class player_spawned : public RMEvent {
public:
    player_spawned(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->inrestart = event->GetBool("inrestart");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "player_spawned";
    IHandleEntity *userid; //playercontroller
    bool inrestart;
    IHandleEntity *userid_pawn;//strict_ehandle
};*/

class item_pickup : public RMEvent {
public:
    item_pickup(IGameEvent *event) {
        this->event_name = event->GetName();
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->silent = event->GetBool("silent");
        this->defindex = event->GetInt("defindex");
        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };

    static void GetItem(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        const char * value = static_cast<item_pickup*>(ptr)->item;
        v8::Local<v8::String> item = v8::String::NewFromUtf8(info.GetIsolate(), value).ToLocalChecked();
        info.GetReturnValue().Set(item);
    }

    const char *event_name = "item_pickup";
    CEntityInstance *userid;//playercontroller
    Player * player;
    const char *item;
    bool silent;
    int defindex;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        t->SetAccessor(v8::String::NewFromUtf8(g_Engine->isolate, "name", v8::NewStringType::kInternalized).ToLocalChecked(), Player::GetName);
        t->SetAccessor(v8::String::NewFromUtf8(g_Engine->isolate, "steam_id", v8::NewStringType::kInternalized).ToLocalChecked(), Player::GetSteamID);
        t->SetAccessor(v8::String::NewFromUtf8(g_Engine->isolate, "item", v8::NewStringType::kInternalized).ToLocalChecked(), item_pickup::GetItem);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));
        return obj;
    };
};
/*
class item_pickup_slerp : public RMEvent {
public:
    item_pickup_slerp(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->index = event->GetInt("index");
        this->behavior = event->GetInt("behavior");
    };
    const char *event_name = "item_pickup_slerp";
    IHandleEntity *userid;//playercontroller
    int index;
    int behavior;
};

class item_pickup_failed : public RMEvent {
public:
    item_pickup_failed(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->reason = event->GetInt("reason");
        this->limit = event->GetInt("limit");
    };
    const char *event_name = "item_pickup_failed";
    IHandleEntity *userid;//playercontroller
    const char *item;
    int reason;
    int limit;
};

class item_remove : public RMEvent {
public:
    item_remove(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->defindex = event->GetInt("defindex");
    };
    const char *event_name = "item_remove";
    IHandleEntity *userid;//playercontroller
    const char *item;
    int defindex;
};

class ammo_pickup : public RMEvent {
public:
    ammo_pickup(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->index = event->GetInt("index");
    };
    const char *event_name = "ammo_pickup";
    IHandleEntity *userid; //playercontroller
    const char *item;
    int index;
};

class item_equip : public RMEvent {
public:
    item_equip(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->defindex = event->GetInt("defindex");
        this->canzoom = event->GetBool("canzoom");
        this->hassilencer = event->GetBool("hassilencer");
        this->issilenced = event->GetBool("issilenced");
        this->hastracers = event->GetBool("hastracers");
        this->weptype = event->GetInt("weptype");
        this->ispainted = event->GetBool("ispainted");
    };
    const char *event_name = "item_equip";
    IHandleEntity *userid;//playercontroller
    const char *item;
    int defindex;
    bool canzoom;
    bool hassilencer;
    bool issilenced;
    bool hastracers;
    int weptype;
    bool ispainted;
};

class enter_buyzone : public RMEvent {
public:
    enter_buyzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->canbuy = event->GetBool("canbuy");
    };
    const char *event_name = "enter_buyzone";
    IHandleEntity *userid;//playercontroller
    bool canbuy;
};

class exit_buyzone : public RMEvent {
public:
    exit_buyzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->canbuy = event->GetBool("canbuy");
    };
    const char *event_name = "exit_buyzone";
    IHandleEntity *userid; //playercontroller
    bool canbuy;
};

class buytime_ended : public RMEvent {
public:
    buytime_ended(IGameEvent *event) {

    };
    const char *event_name = "buytime_ended";
};

class enter_bombzone : public RMEvent {
public:
    enter_bombzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hasbomb = event->GetBool("hasbomb");
        this->isplanted = event->GetBool("isplanted");
    };
    const char *event_name = "enter_bombzone";
    IHandleEntity *userid;//playercontroller
    bool hasbomb;
    bool isplanted;
};

class exit_bombzone : public RMEvent {
public:
    exit_bombzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hasbomb = event->GetBool("hasbomb");
        this->isplanted = event->GetBool("isplanted");
    };
    const char *event_name = "exit_bombzone";
    IHandleEntity *userid;//playercontroller
    bool hasbomb;
    bool isplanted;
};

class enter_rescue_zone : public RMEvent {
public:
    enter_rescue_zone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "enter_rescue_zone";
    IHandleEntity *userid;//playercontroller
};

class exit_rescue_zone : public RMEvent {
public:
    exit_rescue_zone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "exit_rescue_zone";
    IHandleEntity *userid;//playercontroller
};

class silencer_off : public RMEvent {
public:
    silencer_off(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "silencer_off";
    IHandleEntity *userid;//playercontroller
};

class silencer_on : public RMEvent {
public:
    silencer_on(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "silencer_on";
    IHandleEntity *userid;//playercontroller
};

class buymenu_open : public RMEvent {
public:
    buymenu_open(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "buymenu_open";
    IHandleEntity *userid;//playercontroller
};

class buymenu_close : public RMEvent {
public:
    buymenu_close(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "buymenu_close";
    IHandleEntity *userid;//playercontroller
};

class round_prestart : public RMEvent {
public:
    round_prestart(IGameEvent *event) {

    };

    const char *event_name = "round_prestart";
};

class round_poststart : public RMEvent {
public:
    round_poststart(IGameEvent *event) {

    };

    const char *event_name = "round_poststart";
};

class grenade_bounce : public RMEvent {
public:
    grenade_bounce(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "grenade_bounce";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle
};

class hegrenade_detonate : public RMEvent {
public:
    hegrenade_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "hegrenade_detonate";
    IHandleEntity *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class flashbang_detonate : public RMEvent {
public:
    flashbang_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    }

    const char *event_name = "flashbang_detonate";
    IHandleEntity *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class smokegrenade_detonate : public RMEvent {
public:
    smokegrenade_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "smokegrenade_detonate";
    IHandleEntity *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class smokegrenade_expired : public RMEvent {
public:
    smokegrenade_expired(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "smokegrenade_expired";
    IHandleEntity *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class molotov_detonate : public RMEvent {
public:
    molotov_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "molotov_detonate";
    IHandleEntity *userid;//playercontroller
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class decoy_detonate : public RMEvent {
public:
    decoy_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "decoy_detonate";
    IHandleEntity *userid;//playercontroller
    short entityid;
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class decoy_started : public RMEvent {
public:
    decoy_started(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "decoy_started";
    IHandleEntity *userid;//player_pawn
    short entityid;
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class tagrenade_detonate : public RMEvent {
public:
    tagrenade_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
    };
    const char *event_name = "tagrenade_detonate";
    IHandleEntity *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
};

class decoy_firing : public RMEvent {
public:
    decoy_firing(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "decoy_firing";
    IHandleEntity *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class bullet_impact : public RMEvent {
public:
    bullet_impact(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bullet_impact";
    IHandleEntity *userid;//playercontroller
    float x;
    float y;
    float z;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class player_jump : public RMEvent {
public:
    player_jump(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "player_jump";
    IHandleEntity *userid;//playercontroller
};

class player_blind : public RMEvent {
public:
    player_blind(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid = event->GetPlayerController("attacker");
        this->entityid = event->GetInt("entityid");
        this->blind_duration = event->GetFloat("blind_duration");
    };
    const char *event_name = "player_blind";
    IHandleEntity *userid; //playercontroller
    IHandleEntity *attacker; //playercontroller
    int entityid;
    float blind_duration;
};

class player_falldamage : public RMEvent {
public:
    player_falldamage(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->damage = event->GetFloat("damage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "player_falldamage";
    IHandleEntity *userid;//playercontroller
    float damage;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class door_moving : public RMEvent {
public:
    door_moving(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entindex = event->GetInt("entindex");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "door_moving";
    IHandleEntity *userid;//playercontroller
    int entindex;
    IHandleEntity *userid_pawn;//strict_ehandle
};

class cs_win_panel_round : public RMEvent {
public:
    cs_win_panel_round(IGameEvent *event) {
        this->show_timer_defend = event->GetBool("show_timer_defend");
        this->show_timer_attack = event->GetBool("show_timer_attack");
        this->timer_time = event->GetInt("timer_time");
        this->final_event = event->GetInt("final_event");
        this->funfact_token = event->GetString("funfact_token");
        this->funfact_player = event->GetPlayerController("funfact_player");
        this->funfact_data1 = event->GetInt("funfact_data1");
        this->funfact_data2 = event->GetInt("funfact_data2");
        this->funfact_data3 = event->GetInt("funfact_data3");
    };
    const char *event_name = "cs_win_panel_round";

    bool show_timer_defend;
    bool show_timer_attack;
    int timer_time;
    int final_event;
    const char *funfact_token;
    IHandleEntity *funfact_player;//playercontroller
    int funfact_data1;
    int funfact_data2;
    int funfact_data3;
};

class cs_win_panel_match : public RMEvent {
public:
    cs_win_panel_match(IGameEvent *event) {};

    const char *event_name = "cs_win_panel_match";
};

class cs_match_end_restart : public RMEvent {
public:
    cs_match_end_restart(IGameEvent *event) {};

    const char *event_name = "cs_match_end_restart";
};

class cs_pre_restart : public RMEvent {
public:
    cs_pre_restart(IGameEvent *event) {};

    const char *event_name = "cs_pre_restart";
};

class show_deathpanel : public RMEvent {
public:
    show_deathpanel(IGameEvent *event) {
        this->victim = event->GetPlayerController("victim");
        this->killer = event->GetInt("killer");
        this->killer_controller = event->GetPlayerController("killer_controller");
        this->hits_taken = event->GetInt("hits_taken");
        this->damage_taken = event->GetInt("damage_taken");
        this->hits_given = event->GetInt("hits_given");
        this->damage_given = event->GetInt("damage_given");
        this->victim_pawn = event->GetPlayerPawn("victim");
    };
    const char *event_name = "show_deathpanel";
    IHandleEntity *victim;//playercontroller
    int killer;
    IHandleEntity *killer_controller;//playercontroller
    int hits_taken;
    int damage_taken;
    int hits_given;
    int damage_given;
    IHandleEntity *victim_pawn;//strict_ehandle
};

class hide_deathpanel : public RMEvent {
public:
    hide_deathpanel(IGameEvent *event) {};

    const char *event_name = "hide_deathpanel";
};

class player_avenged_teammate : public RMEvent {
public:
    player_avenged_teammate(IGameEvent *event) {
        this->avenger_id = event->GetPlayerController("avenger_id");
        this->avenged_player_id = event->GetPlayerController("avenged_player_id");
    };
    const char *event_name = "player_avenged_teammate";
    IHandleEntity *avenger_id;//playercontroller
    IHandleEntity *avenged_player_id; //playercontroller
};

class match_end_conditions : public RMEvent {
public:
    match_end_conditions(IGameEvent *event) {
        this->frags = event->GetInt("frags");
        this->max_rounds = event->GetInt("max_rounds");
        this->win_round = event->GetInt("win_round");
        this->time = event->GetInt("time");
    };
    const char *event_name = "match_end_conditions";
    int frags;
    int max_rounds;
    int win_round;
    int time;
};

class round_mvp : public RMEvent {
public:
    round_mvp(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->reason = event->GetInt("reason");
        this->value = event->GetInt("value");
        this->musickitmvps = event->GetInt("musickitmvps");
        this->nomusic = event->GetInt("nomusic");
        this->musickitid = event->GetInt("musickitid");
    };
    const char *event_name = "round_mvp";
    IHandleEntity *userid;//playercontroller
    int reason;
    int value;
    int musickitmvps;
    int nomusic;
    int musickitid;
};

class switch_team : public RMEvent {
public:
    switch_team(IGameEvent *event) {
        this->numPlayers = event->GetInt("numPlayers");
        this->numSpectators = event->GetInt("numSpectators");
        this->avg_rank = event->GetInt("avg_rank");
        this->numTSlotsFree = event->GetInt("numTSlotsFree");
        this->numCTSlotsFree = event->GetInt("numCTSlotsFree");
    };
    const char *event_name = "switch_team";
    int numPlayers;
    int numSpectators;
    int avg_rank;
    int numTSlotsFree;
    int numCTSlotsFree;
};

class player_given_c4 : public RMEvent {
public:
    player_given_c4(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
    };
    const char *event_name = "player_given_c4";
    IHandleEntity *userid;//playercontroller
};

class bot_takeover : public RMEvent {
public:
    bot_takeover(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->botid = event->GetPlayerController("botid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
    };
    const char *event_name = "bot_takeover";
    IHandleEntity *userid;//playercontroller
    IHandleEntity *botid;//playercontroller
    IHandleEntity *userid_pawn;//strict_ehandle
};

class jointeam_failed : public RMEvent {
public:
    jointeam_failed(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->reason = event->GetInt("reason");
    };
    const char *event_name = "jointeam_failed";
    IHandleEntity *userid;//playercontroller
    int reason; // 0 - full team
};

class nextlevel_changed : public RMEvent {
public:
    nextlevel_changed(IGameEvent *event) {
        this->nextlevel = event->GetString("nextlevel");
        this->mapgroup = event->GetString("mapgroup");
        this->skirmishmode = event->GetString("skirmishmode");
    };
    const char *event_name = "nextlevel_changed";
    const char *nextlevel;
    const char *mapgroup;
    const char *skirmishmode;
};

class team_intro_start : public RMEvent {
public:
    team_intro_start(IGameEvent *event) {};
    const char *event_name = "team_intro_start";
};

class team_intro_end : public RMEvent {
public:
    team_intro_end(IGameEvent *event) {
    };
    const char *event_name = "team_intro_end";
};*/

#endif //RESOURCEMOD_LEGACYEVENTS_H
