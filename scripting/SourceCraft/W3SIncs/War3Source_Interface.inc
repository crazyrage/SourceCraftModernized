/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: War3Source_Interface.inc
 * Description: The big one that includes everything else
 * Author(s): War3Source Team  
 */
#pragma semicolon 1

#if defined _War3Source_Interface_included
 #endinput
#endif
#define _War3Source_Interface_included

#undef REQUIRE_EXTENSIONS
#include <sdktools>
#include <tf2>
#include <tf2_stocks>
#include <cstrike>

#tryinclude <tf2_player>
#tryinclude <gametype>
#tryinclude <weapons>
#tryinclude <colors>
#define REQUIRE_EXTENSIONS

#tryinclude "sc/SourceCraft"

#if defined SOURCECRAFT
#include "sc/RateOfFire"
#include "sc/maxhealth"
#include "sc/freeze"
#endif

#include "W3SIncs/War3Source_Constants"
#include "W3SIncs/War3Source_Bots"
#include "W3SIncs/colors"
#include "W3SIncs/War3Source_Aura"
#include "W3SIncs/War3Source_Wards"
#include "W3SIncs/War3Source_Races"
#include "W3SIncs/War3Source_Logging"
#include "W3SIncs/War3Source_Gamecheck"

//Interface version, change this when the API requirement changes to force a recompile
//There will be a system check to make sure all plugins are compiled to the same int erface.
char int erfaceVersion[]="1.2.4.1";
#if defined SOURCECRAFT // Not for SourceCraft!
#pragma unused int erfaceVersion
#endif

stock int dummy; //variable for dummies! Use this where ever u want for a dummy return
stock char dummystr[32]; //just a dummy string! 

/**********************
 * CAUTION, THE War3 INTERFACE NOW HANDLES AskPluginLoad2Custom BECAUSE IT IS REQUIRED TO HANDLE CERTAIN TASKS
 * It acually simplifies things for you:
 * Determines game mode
 * Mark Natives optional
 * Calls your own functions (hackish way) if you have them:
 * InitNativesForwards()   
 * AskPluginLoad2Custom(Handle myself,bool:late,char[] error,err_max);
 * So if you want to do something in AskPluginLoad2, implement public AskPluginLoad2Custom(...) instead. 
 */
public APLRes AskPluginLoad2(Handle plugin, bool late, char[] error, int err_max)
{
  DetermineGameMode();
  Function func;
  func=GetFunctionByName(plugin, "InitNativesForwards");
  if(func!=INVALID_FUNCTION) { //non war3 plugins dont have this function
    Call_StartFunction(plugin, func);
    Call_Finish(dummy);
    if(!dummy) {
      LogError("InitNativesForwards did not return true, possible failure");
    }
  }
  func=GetFunctionByName(plugin, "AskPluginLoad2Custom");
  if(func!=INVALID_FUNCTION) { //non war3 plugins dont have this function
    Call_StartFunction(plugin, func);
    Call_PushCell(plugin);
    Call_PushCell(late);
    Call_PushString(error);
    Call_PushCell(err_max);
    Call_Finish(dummy);
    if(APLRes:dummy==APLRes_SilentFailure) {
      return APLRes_SilentFailure;
    }
    if(APLRes:dummy!=APLRes_Success) {
      LogError("AskPluginLoad2Custom did not return true, possible failure");
    }
  }
  func=GetFunctionByName(plugin, "LoadCheck");
  if(func!=INVALID_FUNCTION) { //non war3 plugins dont have this function
    Call_StartFunction(plugin, func);
    Call_Finish(dummy);
    if(dummy==0) {
      return APLRes_SilentFailure;
    }
  }

  return APLRes_Success;
}
/**********************
 * Implement this if you have natives/forwards
 */
forward bool InitNativesForwards();

/**********************
 * Implement this if you need AskPluginLoad2
 */
forward APLRes AskPluginLoad2Custom(Handle plugin, bool late, char[] error, int err_max);

/**********************
 * Checks plugin to allow for silent failure
 * Called from APLRes:AskPluginLoad / War3Source main 
 *  
 * Return true to allow loading
 * Return false to silently disable loading 
 */
forward LoadCheck();


#if !defined SOURCECRAFT // Not Implemented!
//Called for each category if the user opens the catted cr menu
forward Action OnW3DrawCategory(int client, int categoryindex, char[] categoryname);
#endif

//Warcraft required executions, does common tasks, such as loading translations.
forward War3InterfaceExec();
public War3InterfaceExec() {
  LoadTranslations("w3s._common.phrases");
}

/*
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 * Natives and misc stocks
 */

#if !defined SOURCECRAFT // Not Implemented!
stock SetSafeMenuTitle(Handle menu, const char[] fmt, any ...)
{
  char menuTitle[4096];
  VFormat(menuTitle, sizeof(menuTitle), fmt, 3);

  // There's a silly bug with the Left4Dead engine where menus won't
  // show up if the title starts with [ :(
  if(GAMEL4DANY && menuTitle[0] == '[')
  {
    Format(menuTitle, sizeof(menuTitle), " %s", menuTitle);
  }
  
  SetMenuTitle(menu, menuTitle);
}

native W3GetW3Version(char[] retstr, int maxlen);//str
native W3GetW3Revision();//int
native W3GetStatsVersion();//int

//Socket functions
native W3Socket(char[] url, Function callback);
native W3Socket2(char[] url, char[] postdata, Function callback);
#endif

//forces weapon drop, given a weapon entity
native W3DropWeapon(client, weaponent);

/**
 * Checks if we are in the freeze time. Certain games support this, like CS.
 * @return True if so, false if not.
 *  
 */
#pragma deprecated
#if !defined SOURCECRAFT
native bool War3_InFreezeTime();
#else
stock bool War3_InFreezeTime()
{
    return (GetRoundState() == RoundFreeze);
}
#endif

/* BEGIN GLOBAL VARS */
//retrieves specified values from the main war3 plugin, see W3Var enum in constants.inc, only those values are allowed
//this mostly used for accessing a variable without creating a special native just for it
//this returns any:, you should tag it if return value is not suppose to be normal int eger. like float W3GetVar( or Handle W3GetVar(
//most are temporary variables and this function should be called immidiately in the right functions
//examples W3GetVar(OldRace)   W3GetVar(DeathRace)
//See W3Var enum in constants to get a list
//Do not get/set vars arbitrarily unless you know what you are doing
native any W3GetVar(W3Var:variabletoretrieve);
native any W3SetVar(W3Var:variabletoretrieve,any value);
/* END GLOBAL VARS */

/**
 * Registers a help command with the War3Source plugin for war3help
 * @param name: The name of the command. (max 64)
 * @param desc: The description of the command. (max 256) 
 * @noreturn
 */
#if !defined SOURCECRAFT
native War3_CreateHelpCommand(char[] name, char[] desc);
#else
#define War3_CreateHelpCommand CreateHelpCommand
#endif

#if !defined SOURCECRAFT // Not Implemented!
native W3GetLevelsSpent(client, race);
#endif

#if !defined SOURCECRAFT
native W3ClearSkillLevels(client, race);
#else
#define W3ClearSkillLevels ResetUpgradeLevels
#endif


/**
 * Finds the best target within a player's view cone
 * Can be limited by max distance
 * function perfers closer target over a target that has less degrees 
 * @param client: Client index
 * @param max_distance: Optional, set to 0 to allow unlimited distance
 * @param include_friendlys: Optional, should we look for friendlys too?
 * @param cone_angle: Optional, view cone angle, SDK default is 23.0 degrees.
 * @param FilterFunction: Callback filter function, return false in this function to ignore entity.
 */
native War3_GetTargetInViewCone(int client, float max_distance=0.0, bool include_friendlys=false, float cone_angle=23.0, Function FilterFunction=INVALID_FUNCTION);

//generic immunity filter for ultimate
public bool UltFilter(int client)
{
  return (!W3HasImmunity(client,Immunity_Ultimates));
}
//generic immunity filter for skills
public bool SkillFilter(int client)
{
  return (!W3HasImmunity(client,Immunity_Skills));
}

///traces from client to target and see if they are in view of each other (Line of Signt - LOS)
native W3LOS(int client, int target);

stock bool ValidPlayer(int client, bool check_alive=false, bool alivecheckbyhealth=false) {
  if(client>0 && client<=MaxClients && IsClientConnected(client) && IsClientInGame(client))
  {
    if(check_alive && !IsPlayerAlive(client))
    {
      return false;
    }
    if(alivecheckbyhealth&&GetClientHealth(client)<1) {
      return false;
    }
    return true;
  }
  return false;
}

stock bool ValidRace(int raceid_) {
  return bool:(raceid_>0&&raceid_<=War3_GetRacesLoaded());
}
/**
 * Respawns a dead player.
 * @param client: Client's index.
 * @param ignore_dead_check: Optional, ignore dead checking. 
 * @noreturn
 */
#if !defined SOURCECRAFT
native War3_SpawnPlayer(int client, bool ignore_dead_check=false);
#else
stock War3_SpawnPlayer(int client, bool ignore_dead_check=false)
{
    if (ignore_dead_check || !IsPlayerAlive(client))
    {
        RespawnPlayer(client);
    }
}
#endif

/**
 * Stock, checks if a player is near any entity in a datapack of entities, again by name.
 * @param client: Client's index.
 * @param hEnts: Datapack of entity names.
 * @param pack_size: Size of datapack.
 * @param distance: Optional, how far is "near."
 * @return ADT array of results, otherwise INVALID_HANDLE.
 * DO NOT FORGET TO CloseHandle() the results if they don't equal null!!!
 */
stock Handle War3_NearEntsByName(int client, Handle hEnts, int pack_size, float distance=150.0)
{
  Handle hResults=CreateArray();
  ResetPack(hEnts);
  for(int x=0;x<pack_size;x++)
  {
    char ent_name[128];
    ReadPackString(hEnts,ent_name,sizeof(ent_name));
    Handle result_found=War3_NearEntByName(client,ent_name,distance);
    if(result_found!=null)
    {
      int size=GetArraySize(result_found);
      for(int y=0;y<size;y++)
      {
        PushArrayCell(hResults,GetArrayCell(result_found,y));
      }
      CloseHandle(result_found);
    }
  }
  if(GetArraySize(hResults)>0)
    return hResults;
  else
  {
    CloseHandle(hResults);
    return null;
  }
}

stock Handle War3_NearEntByName(int client, char[] ent_name, float distance)
{
  if(distance<0.0)
  return null; // wtf? lol
  if(client>0 && client<=MaxClients && IsClientConnected(client) && IsClientInGame(client) && IsPlayerAlive(client))
  {
    Handle hResults=CreateArray();
    float curPos[3];
    GetClientAbsOrigin(client,curPos);
    int ent=0;
    while((ent=FindEntityByClassname(ent,ent_name))>0)
    {
      if(!IsValidEdict(ent)) continue;
      float entPos[3];
      GetEntPropVector(ent,Prop_Send,"m_vecOrigin",entPos);
      float dist=GetVectorDistance(curPos,entPos);
      if(dist<=distance)
      {
        PushArrayCell(hResults,ent);
      }
    }
    if(GetArraySize(hResults)>0)
    {
      return hResults;
    }
    else
    {
      CloseHandle(hResults);
    }
  }
  return null;
}

// THE FOLLOWING NATIVES RETRIEVE DATA CACHED ONGAMEFRAME!
// THE FUNCTIONS NAMED War3_CachedDead* ARE VALUES SET WHEN THE PLAYER DIES
// THESE ARE ADVANCED FUNCTIONS, CHANCES ARE IF U DON'T KNOW WHAT THEY MEAN
// YOU PROBABLY DON'T HAVE TO USE THEM

/**
 * Get last cached player eye angle.
 * @param client: Client index
 * @param angle[3]: Output array
 * @noreturn
 */
native War3_CachedAngle(int client, float angle[3]);

/**
 * Get last cached player position.
 * @param client: Client index
 * @param position[3]: Output array
 * @noreturn
 */
native War3_CachedPosition(int client, float position[3]);

/**
 * Get last cached player ducking state.
 * @param client: Client index
 * @return True or false.
 */
native bool War3_CachedDucking(int client);

/**
 * Get cached weapon entity based on iterator.
 * This function is usually called from a loop. 
 * @param client: Client index
 * @param weapon_iterator: Number from 0-9.
 * @return Weapon entity, 0 if none.
 */
native War3_CachedWeapon(int client, int weapon_iterator);

/**
 * Get cached weapon entity "clip1" based on iterator.
 * This function is usually called from a loop.
 * @param client: Client index
 * @param weapon_iterator: Number from 0-9.
 * @return Value of clip1.
 */
native War3_CachedClip1(int client, int weapon_iterator);

/**
 * Get cached ammo of a player with a specific weapon ID.
 * This function is usually called from a loop.
 * @param client: Client index
 * @param game_weapon_id: Game specific weapon identifier, this is constant for each weapon.
 * @return Value of iAmmo.
 */
native War3_CachedAmmo(int client, int game_weapon_id);

/**
 * Get cached weapon entity "clip1" of iterator, only changes when a player dies.
 * This function is usually called from a loop.
 * @param client: Client index
 * @param weapon_iterator: Number from 0-9.
 * @return Value of clip1 from last death.
 */
native War3_CachedDeadClip1(int client, int weapon_iterator);

/**
 * Get cached ammo of a player with a specific weapon ID, only changes when a player dies.
 * This function is usually called from a loop.
 * @param client: Client index
 * @param game_weapon_id: Game specific weapon identifier, this is constant for each weapon.
 * @return Value of iAmmo from last death.
 */
native War3_CachedDeadAmmo(int client, int game_weapon_id);

/**
 * Get cached weapon name of iterator, only changes when a player dies.
 * This function is usually called from a loop, sets output_buffer to "" if no weapon at iterator.
 * @param client: Client index
 * @param weapon_iterator: Number from 0-9.
 * @param output_buffer: Output buffer
 * @param output_size: Size of output buffer.
 * @noreturn
 */
native War3_CachedDeadWeaponName(int client, int weapon_iterator, char[] output_buffer, int output_size);

//LO AND BEHOLD THE COOLDOWN MANAGER, YOUR LIFE SIMPLIFIED!!!
//we essentially move the cooldown system out of our races int o our main plugin
/**
 * cooldown manager
 * basically self explainatory parameters, creates a cooldown for a skill (term skill used here is generic for skill/ability/ultimate)
 * printMsgOnExpireByTime prints message to client (if alive and same race) when this skill expires by time (will not print when expired by spawn or death or force reset)
 * The skill name of the skill will be in ready and not ready messages.
 * when a cooldown expires (by time, by death, by spawn, by force reset), it will forward to OnCooldownExpired(....) forward, use if u need to
 * @noreturn
 * 
 * Usually the first 4 parameters are enough
 */
#if !defined SOURCECRAFT
native War3_CooldownMGR(int client, float cooldownTime, int raceid, int skillNum, bool resetOnSpawn=true, bool printMsgOnExpireByTime=true);
#else
stock War3_CooldownMGR(int client, float cooldownTime, int raceid, int skillNum, bool resetOnSpawn=true, bool printMsgOnExpireByTime=true)
{
    static const char abilityReadySound[]="war3source/ability_refresh.mp3";
    static const char ultimateReadySound[]="war3source/ult_ready.wav";

    CooldownType type = Cooldown_ResetOnChange | Cooldown_ResetOnDeath;

    if (resetOnSpawn)
        type |= Cooldown_ResetOnSpawn;
    if (printMsgOnExpireByTime)
        type |= Cooldown_ExpireNotify;

    CreateCooldown(client,raceid,skillNum,cooldownTime,"",type,
                   (skillNum==3) ? ultimateReadySound : abilityReadySound);
}
#endif

//tells the cooldown manager to create a predefined cooldown time when player spawns with that race. 
//cooldown is created regardless of resetOnSpawn in War3_CooldownMGR(...) calls
//only prints expired if client remains that race
//set this ONCE when u register your skills
#if !defined SOURCECRAFT
native W3SkillCooldownOnSpawn(int raceid, int skillnum, float cooldownTime, bool printmsgonexpire=true);
#else
stock W3SkillCooldownOnSpawn(int raceid, int skillnum, float cooldownTime, bool printmsgonexpire=true)
{
    CooldownType type = Cooldown_CreateOnSpawn;
    if (printmsgonexpire)
        type |= Cooldown_ExpireNotify;
    SetUpgradeCooldown(raceid, skillnum, cooldownTime, type);
}
#endif

/**
 * how much time is left on this particular cooldown?
 * returns an int (rounted up from the float)
 */
#if !defined SOURCECRAFT
native War3_CooldownRemaining(client, raceid, skillNum);
#else
#define War3_CooldownRemaining(%1) RoundToCeil(GetCooldownRemaining(%1))
#endif

/**
 * basically make this cooldown expire, this expiration is not considered "by time"
 */
#if !defined SOURCECRAFT
native War3_CooldownReset(client, raceid, skillNum);
#else
#define War3_CooldownReset ResetCooldown
#endif

/**
 * is this skill NOT in COOLDOWN? YOU NEED TO CHECK IF CLIENT HAS LEVELED THIS SKILL FIRST, THIS IS ONLY COOLDOWN RELATED
 * you would only do this if this skill has a cooldown and u called War3_CooldownMGR
 * printTextIfNotReady=true will print a "not ready" message
 */
#if !defined SOURCECRAFT
native bool War3_SkillNotInCooldown(int client, int raceid, int skillNum, bool printTextIfNotReady=false);
#else
stock bool War3_SkillNotInCooldown(int client, int raceid, int skillNum, bool printTextIfNotReady=false)
{
    //return HasCooldownExpired(client,raceid,skillNum,printTextIfNotReady);
    return CanInvokeUpgrade(client,raceid,skillNum,true,printTextIfNotReady);
}
#endif

/**
 * prints Skill Is Not Ready
 */
#if !defined SOURCECRAFT
native War3_PrintSkillIsNotReady(int client, int raceid, int skillNum);
#else
stock War3_PrintSkillIsNotReady(int client, int raceid, int skillNum)
{
    HasCooldownExpired(client, raceid, skillNum, true);
}
#endif

//DELAY TRACKER
//DELAY TRACKER
//DELAY TRACKER
//DELAY TRACKER
//DELAY TRACKER
//DELAY TRACKER

///registers a tracker and returns an index that you must keep. usually do this for each each player or once if global
// Register once on plugin start please!
//returns -1 if tracker is full. LogErrors on Full
/*
 usage:

 register tracker
 create delay

 repeat:{
 expired?
 create delay
 }

 */
native War3_RegisterDelayTracker();

//create a delay
native War3_TrackDelay(trackerIndex,float delay);

//did the delay expire?
native War3_TrackDelayExpired(trackerIndex);

///PLAYER  TRACE
///PLAYER  TRACE
///PLAYER  TRACE
///PLAYER  TRACE
///PLAYER  TRACE

//get location of where the player is aiming (trace to the end of your crosshair)
native War3_GetAimEndPoint(int client, float endpos[3]);

//get location of where the player is aiming (trace to the end of your crosshair) limited by distance
native War3_GetAimTraceMaxLen(int client, float endpos[3], float maxdistance);

//simple weapon restriction:  pass "weapon_knife,weapon_hegrenade" to only allow these weapons, separated by comma.
//pass "" to allow all weapons again
//c4 etc is automatically allowed
//to override other restrictions, use a higher priority
//use 1 priorty for ur base race restrictions, USE AT OWN RISK!
native War3_WeaponRestrictTo(int client, int raceid, char[] onlyallowedweaponsnames, int priority=1);

#if !defined SOURCECRAFT // Not Implemented!
//get weaponlist
native War3_GetWeaponRestriction(int client, int raceid, char[] buffer, int maxlength);
#endif

native float W3GetPhysicalArmorMulti(int client); //damage multipler for client by accounting for physical armor (warcraft physical armor, not cs armor)
native float W3GetMagicArmorMulti(int client); //damage multipler for client by accounting for magic armor

#if !defined SOURCECRAFT
native W3GetCurrentWeaponEnt(client); //get the entity of the current (active) weapon
#else
#define W3GetCurrentWeaponEnt GetActiveWeapon
#endif


#if !defined SOURCECRAFT // Not Implemented!
native W3GetMinUltLevel();

//force all supported war3 plugins int o failed mode (pause plugin)
native War3Failed(char[] reason);
#endif

/**
 * Use this for getting a chance modifier in OnWar3TakeDamage!!
 * This is a cvar based value used for lowering chance of skill activation when a weapon such as flamethrower inflicts damage many times over a period of time.
 * If you are making a skill that has a high chance, example 1 in 10, it is advised to change it to something like 10 in 100 if using GetRandomInt, however if you are using GetRandomFloat with the chance modifier*original chance then it shouldn't be an issue. 
 * @param attacker: Attacker's client index.
 * @param inflictor: Inflictor entity index. (ie sentry gun)
 * @param damagetype: Type of damage.
 * @return float modifier, 1.0 for 100% chance otherwise percentage between 0.0 and 1.0
 */

//helper, directly uses last inflictor and damagetype
native float W3ChanceModifier(int attacker);

/**
 * Prints a message to the right-hand display
 *
 * @param   client    Client index.
 * @param   szFormat  Formatting rules.
 * @param   ...    Variable number of format parameters.
 */
native War3_KeyHintText(int client, const char[] szFormat, any ...);

//print via hint engine. format accepts translations
//maximum duration is 20.0
native W3Hint(int client, W3HintPriority type=HINT_LOWEST, float duration=5.0, char[] format, any ...);

//for activatable skills only
stock bool SkillAvailable(int client, int yourRaceID, int skillnumber, bool printCooldown=true, bool checksilenced=true, bool printSilenced=true) {
  return War3_SkillNotInCooldown(client,yourRaceID,skillnumber,printCooldown)&& (!checksilenced||!Silenced(client,printSilenced));
}

#if !defined SOURCECRAFT
native War3_GetRace(client);
native War3_SetRace(client, race);

native War3_SetLevel(client, race, level);
native War3_GetLevel(client, race);

native W3GetTotalLevels(client); //sum of  levels in each race
#else
#define War3_GetRace GetRace
#define War3_SetRace ChangeRace

#define War3_SetLevel SetLevel
#define War3_GetLevel GetLevel

#define W3GetTotalLevels GetOverallLevel
#endif

#if !defined SOURCECRAFT // Not Implemented!
native W3GetLevelBank(client);
native W3SetLevelBank(client, newlevelbank);
#endif

#if !defined SOURCECRAFT
native War3_SetXP(client, race, newxp);
native War3_GetXP(client, race);
#else
#define War3_SetXP SetXP
#define War3_GetXP GetXP
#endif

#if !defined SOURCECRAFT
//native War3_SetSkillLevel(client,race,skill,newlevel);
native War3_GetSkillLevel(client, race, skill);
#else
#define War3_GetSkillLevel(%1,%2,%3) GetUpgradeLevel(%1,%2,%3,true)
#endif

#if !defined SOURCECRAFT // Not Implemented!
native War3_SetSkillLevelINTERNAL(client, race, skill, newlevel);
native War3_GetSkillLevelINTERNAL(client, race, skill);
#endif

/*
Gets the skill level of a generic skill
@client
@g_skill_id = generic skill id
@genericSkillData = array that the customer race passed to you (if any) when attaching itself to this generic skill
@customerRaceID = the matching race that is trying to use this generic skill
@customerSkillID = the matching skill id for the race trying to use this as generic skill

for example, if race 4 skill 3 is redirected as generic skill 2,
you should have passed 2 as g_skill_id
customerRaceID will be returned as 4
customerSkillID returns 3

*/
native W3_GenericSkillLevel(int client, int g_skill_id, Handle &genericSkillData, int &customerRaceID=0, int &customerSkillID=0);

#if !defined SOURCECRAFT // Not Implemented, since it's not needed!
native W3SetPlayerProp(client,W3PlayerProp:property,any value);
native any W3GetPlayerProp(client,W3PlayerProp:property);
#endif

//TKV = trie key value , a abstract data storage, emulates cvars for war3
///Creates int ernal war3 cvars. returns a id to your cvar, store it!
#if !defined SOURCECRAFT // Not Implemented, since it's not needed!
//cvarid of 0 is null null (filled auto)
//make sure native have binded (on plugin start is fine)
native W3CreateCvar(char[] cvarstr,char[] cvarvalue,char[] cvardesc);
native W3GetCvar(cvarid,char[] returnstr,maxlen);
native W3SetCvar(cvarid,char[] cvarvalue);

//returns -1 if not found, otherwise returns cvar id
native W3FindCvar(char[] cvar);
native Handle W3CvarList(); //returns a handle arraylist of cvar list (no values, names only. CLOSE THIS!!!

//get cvar value when you have the key
native W3GetCvarByString(char[] cvarstr,char[] returnstr,maxlen);

//get actual cvar name: undead_blah (the KEY)
native W3GetCvarActualString(cvarid,char[] returnstr,maxlen);

stock W3CreateCvarInt(char[] cvarstr,int val,char[] cvardesc) {
  char str[1024];
  Format(str,sizeof(str),"%d",int val);
  return W3CreateCvar(cvarstr,str,cvardesc);
}
stock W3GetCvarInt(cvarid) {
  char ret[1024];
  W3GetCvar(cvarid, ret, sizeof(ret));
  //PrintToChatAll("%s %d",ret,StringToInt(ret));
  return StringToInt(ret);
}
stock W3SetCvarInt(cvarid, int val) {
  char str[1024];
  Format(str, sizeof(str), "%d", int val);
  return W3SetCvar(cvarid, str);
}
stock W3CreateCvarFloat(char[] cvarstr,int val,char[] cvardesc) {
  char str[1024];
  Format(str,sizeof(str),"%f",int val);
  W3CreateCvar(cvarstr,str,cvardesc);
}
stock W3GetCvarFloat(cvarid) {
  char ret[1024];
  W3GetCvar(cvarid, ret, sizeof(ret));
  return StringToFloat(ret);
}
stock W3SetCvarFloat(cvarid, int val) {
  char str[1024];
  Format(str, sizeof(str), "%f", int val);
  W3SetCvar(cvarid, str);
}
#endif


#if !defined SOURCECRAFT
stock War3_SetMaxHP_INTERNAL(int client, int maxhp) {
  //  DP("set to %d",maxhp);
  W3SetPlayerProp(client, iMaxHP, maxhp);
}
stock War3_GetMaxHP(int client) {
  //DP("add %d",W3GetBuffSumInt(client,iAdditionalMaxHealth));
  return W3GetPlayerProp(client, iMaxHP);//+ W3GetBuffSumInt(client,iAdditionalMaxHealth);
}
#else
native War3_SetMaxHP_INTERNAL(int client, int max_hp);
native War3_GetMaxHP(int client);
#endif

#if !defined SOURCECRAFT // Not Implemented!
stock bool W3IsPlayerXPLoaded(int client) {
  return W3GetPlayerProp(client,xpLoaded);
}
stock W3GetPendingRace(int client) {
  return W3GetPlayerProp(client, PendingRace);
}
stock W3SetPendingRace(int client, int pendrace) {
  W3SetPlayerProp(client, PendingRace, pendrace);
}
#else
#define War3_SetGold SetCrystals
#define War3_GetGold GetCrystals
#endif

stock W3IsDeveloper(int client)
{
  if(ValidPlayer(client)){
    return W3GetPlayerProp(client,isDeveloper);
  }
  return false;
}

#if !defined SOURCECRAFT // Not Implemented!
///returns the handle to the database which your xp is saved, so you dont have to connect again for your addon scipts
///use W3GetVar(hDatabase) to get the handle to the database which your xp is saved, so you dont have to connect again for your addon scipts

native bool W3SaveEnabled();
native W3SaveXP(int client, int race);
#endif

//Skills
#if !defined SOURCECRAFT // Not Implemented!
native War3_SuicideBomber(client, float location[], float damage, race_skillid, float radius, bool:effect=true);

//disabled due to human race requirements..for now
//native bool:War3_Teleport(client, float distance);
#endif

#if !defined SOURCECRAFT // Not Implemented!
native War3_StringMath(char[] equation);

native W3HasDiedThisFrame(client);
#endif

#if !defined SOURCECRAFT // Not Implemented!
/**
 * Adds a dependency on a skill
 *
 * @param iRaceID    Race ID
 * @param iSkill     Skill on which the dependency should be added onto
 * @param iRequiredSkill Required Skill ID
 * @param iRequiredLevel Required Level Num
 * @return         true on success, otherwhise false!
 * @error        Thrown on invalid param num/race
 */
native bool War3_SetDependency(int iRaceID, int iSkill, int iRequiredSkill, int iRequiredLevel);

/**
 * Removes any known dependency from a skill(if any)
 * NOTE: you should be able to call this safetly without having to bother about anything,
 * since no error will be thrown if there currently aren't any active dependencys on this
 *
 * @param iRaceID    Race ID
 * @param iSkill     Skill on which the dependency should be removed from
 * @noreturn
 * @error        Thrown on invalid param num/race
 */
native War3_RemoveDependency(int iRaceID, int iSkill);

/**
 * Retrieves various informations about a depending skill
 * NOTE: This can be called to check if there is any dependency or not
 *
 * @param iRaceID    Race ID
 * @param iSkill     Target skill to get informations from
 * @param eInfo      Type of information to retrieve
 * @return         Retrieved information about the dependency
 * @error        Thrown on invalid param num/race
 */
native War3_GetDependency(int iRaceID, int iSkill, SkillDependency eInfo=ID);
#endif

/**
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS 
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS 
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS 
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS 
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS
 * FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS FORWARDS 
 */

#if !defined SOURCECRAFT // Not Implemented!
/*create an error displayed in various places at runtime*/
native CreateWar3GlobalError(char[] str);

//forwarded when someone calls CreateWar3GlobalError, store that string for yourself
forward OnWar3GlobalError(char[] str);

forward War3FailedSignal(char[] str);

/* int erface side implementation, do not use */
public War3FailedSignal(char[] str) {
  LogError(str); //must be a non dependent native log error
  SetFailState(str);
}

forward CheckWar3Compatability(char[] w3mainInterfVersion);

/* int erface side implementation, do not use */
public CheckWar3Compatability(char[] w3mainInterfVersion) {
  if(!StrEqual(int erfaceVersion,w3mainInterfVersion)) {
    //War3_LogCritical("FATAL ERROR, PLUGIN COMPILED INTERFACE VERSION (%s) IS DIFFERENT FROM WAR3SOURCE BASE VERSION (%s). Please Compile With Latest Version!!!",int erfaceVersion,w3mainInterfVersion);
    SetFailState("FATAL ERROR, PLUGIN COMPILED INTERFACE VERSION (%s) IS DIFFERENT FROM WAR3SOURCE BASE VERSION (%s). Please Compile With Latest Version!!!",int erfaceVersion,w3mainInterfVersion);
  }
}
#endif

/**
 * Called when the plugin is ready.
 * this is called many times, increasing num each time 0.0-float(MAXRACES)*10 in 1 increments
 * USED FOR OFFICIAL PLUGINS ONLY
 */
forward OnWar3LoadRaceOrItemOrdered(num);

/**
 * Called when the plugin is ready.
 * this is called many times, increasing num each time 0.0-float(MAXRACES)*10 in 1 increments
 * this is called after all default races have been loaded (OnWar3LoadRaceOrItemOrdered finished)
 */
forward OnWar3LoadRaceOrItemOrdered2(num);
 
/**
 * Called when the plugin is ready. (after ordered)
 */
forward OnWar3PluginReady();

/**
 * Gets called when after PutInServer, xp retrievcal has started, initial variables may have been set
 * @param client: The client's index.
 */
#if !defined SOURCECRAFT
forward OnWar3PlayerAuthed(client);
#else
#define OnWar3PlayerAuthed OnPlayerAuthed
#endif

/**
 * Gets called when someone changes their race.
 * @param client: The client's index.
 * @param newrace: The player's new race.
 */
#pragma deprecated Use OnRaceChanged(client,oldrace,newrace);
#if !defined SOURCECRAFT
forward OnRaceSelected(int client, int newrace); //to be removed
#else
forward OnWar3RaceSelected(int client, int newrace);
#define OnRaceSelected OnWar3RaceSelected
#endif

//client is not check if valid or not, still forwarded if client is not present, so you can disable buffs etc
forward OnRaceChanged(int client, int oldrace, int newrace);

/**
 * Gets called when the +ultimate or -ultimate command is called, IT ISNT ALWAYS FOR YOUR RACE, YOU NEED TO CHECK!!!
 * @param client: The client's index.
 * @param race: The race for which it was called.
 * @param pressed: If true, +ultimate, false, -ultimate.
 */
#if !defined SOURCECRAFT
forward OnUltimateCommand(int client, int race, bool pressed);
#else
forward OnWar3UltimateCommand(int client, int race, bool pressed);
#define OnUltimateCommand OnWar3UltimateCommand
#endif

/**
 * Gets called when the +ability# or -ability# command is called.
 * If its +ability or -ability (without a number), 0 is passed to ability.
 * Isn't always for you, check War3_GetRace(client)==yourRace
 * @param client: The client's index.
 * @param ability: The ability number.
 * @param pressed: If true, +ability#, false, -ability#.
 */
forward OnAbilityCommand(int client, int ability, bool pressed);

/**
 * Gets called when a skill level is changed.
 */
forward OnSkillLevelChanged(int client, int race, int skill, int newskilllevel);
/**
 * Gets called when a GENERIC skill level is changed.
 */
forward OnGenericSkillLevelChanged(int client, int generic_skill_id, int newlevel, Handle generic_Skill_Options, int customer_race, int customer_skill);

//when a weapon fires, via weapon_fire or TF2 Calc critical (which crits must be on)
#if !defined SOURCECRAFT
forward OnWeaponFired(int victim, int attacker, float damage);
#else
// defined in RateOfFire.inc
#endif

/**
 * Called when a cooldown expires (timed, death, or spawn)
 */
#if !defined SOURCECRAFT
forward OnCooldownExpired(int client, int raceID, int skillNum, bool expiredByTime);
#else
// defined in SourceCraft.inc
#endif

///general events, see W3EVENT enum in constants
///client may not always matter
forward OnWar3Event(W3EVENT event, int client);

native W3CreateEvent(W3EVENT event, int client);

//simple system for stopping actions, send a specific event with optinal player, those hooking can use W3Deny() to deny
forward OnW3Denyable(W3DENY event, int client);
native bool W3Denied(W3DENY event, int client); //returns FALSE if NOT DENIED
stock bool W3Denyable(W3DENY event, int client) {
  return !W3Denied(event,client);
}
native W3Deny();

#if !defined SOURCECRAFT // Not Implemented, since it's not needed!
stock bool CanSelectRace(int client, int race) {
  W3SetVar(EventArg1,race);
  bool value=W3Denyable(DN_CanSelectRace,client);
  //DP("dp %d",value);
  if(value==false && W3IsDeveloper(client)) {
    //DP("dp2 %d",value);
    War3_ChatMessage(client,"You are normally not allowed to select this race, but since you are developer we will allow you to select this race");

    return true;
  }
  return value;
}

stock ShowChangeRaceMenu(int client) {
  W3CreateEvent(DoShowChangeRaceMenu, client);
}

stock W3DoLevelCheck(int client) {
  W3CreateEvent(DoLevelCheck, client);
}
#endif

//when player spawns, fires only in war3 mode
forward OnWar3EventSpawn(int client);

//when player dies, fires only in war3 mode
forward OnWar3EventDeath(int victim, int attacker, int deathrace);

//Fired after a player dodges, useful primarily for attaching effects
forward OnW3DodgePost(int victim, int attacker);

//Fired before a player dodges, you can edit the dodge chance here
forward OnW3DodgePre(int victim, int attacker, float chance);

//Fired every time a ward is created
forward OnWardCreated(int wardindex, int behaviorID);

//Fired every time a ward "pulses"
forward OnWardPulse(int wardindex, int behaviorID);

//Fired every time a ward "pulses" when a valid target is in range
forward OnWardTrigger(int wardindex, int victim, int owner, int behaviorID);

//Fired every time a ward is removed
forward OnWardExpire(int wardindex, int owner, int behaviorID);

/*
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 * STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS STOCKS
 */

#define MAX_MESSAGE_LENGTH 250

/** 
 * Prints a message in the chat area with [War3Source] pre-appended.
 * Support color tags: {default}, {green}, {lightgreen}, {red}, {blue}, {olive}.
 * 
 * @param client    Client index, pass 0 for message to all.
 * @param szMessage   Message (formatting rules).
 * @return      No return
 */
stock War3_ChatMessage(int client, const char[] szMessage, any ...)
{
  if (client == 0)
  {
    char szBuffer[MAX_MESSAGE_LENGTH];
    for (int i = 1; i <= MaxClients; i++)
    {
      if (IsClientInGame(i) && !IsFakeClient(i))
      {
        SetGlobalTransTarget(i);
        VFormat(szBuffer, sizeof(szBuffer), szMessage, 3);
        Format(szBuffer, sizeof(szBuffer), "%T%s", "[War3Source]", i, szBuffer);
        CPrintToChat(i, szBuffer);
      }
    }
  }
  else
  {
    char szBuffer[MAX_MESSAGE_LENGTH];
    SetGlobalTransTarget(client);
    VFormat(szBuffer, sizeof(szBuffer), szMessage, 3);
    Format(szBuffer, sizeof(szBuffer), "%T%s", "[War3Source]", client, szBuffer);
    CPrintToChat(client, szBuffer);
  }
}

/** 
 * Prints a message in the chat area with [War3Source] pre-appended.
 * Support color tags: {default}, {green}, {olive}, {teamcolor}.
 * 
 * @param client    Client index, pass 0 for message to all.
 * @param author    Author index whose color will be used for teamcolor tag.
 * @param szMessage   Message (formatting rules).
 * @return      No return
 */
stock War3_ChatMessageEx(int client, int author, const char[] szMessage, any ...)
{
  if (client == 0)
  {
    if (author < 0 || author > MaxClients)
    ThrowError("Invalid client index %d", author);

    if (!IsClientInGame(author))
    ThrowError("Client %d is not in game", author);

    char szBuffer[MAX_MESSAGE_LENGTH];
    for (int i = 1; i <= MaxClients; i++)
    {
      if (IsClientInGame(i) && !IsFakeClient(i))
      {
        SetGlobalTransTarget(i);
        VFormat(szBuffer, sizeof(szBuffer), szMessage, 4);
        Format(szBuffer, sizeof(szBuffer), "%T%s", "[War3Source]", i, szBuffer);
        CPrintToChatEx(i, author, szBuffer);
      }
    }
  }
  else
  {
    char szBuffer[MAX_MESSAGE_LENGTH];
    SetGlobalTransTarget(client);
    VFormat(szBuffer, sizeof(szBuffer), szMessage, 4);
    Format(szBuffer, sizeof(szBuffer), "%T%s", "[War3Source]", client, szBuffer);
    CPrintToChatEx(client, author, szBuffer);
  }
}

//who should the tanslated phrase be translated to, 0 = server default language
stock SetTrans(int client) {
  W3SetVar(TransClient, client);
}
stock GetTrans() {
  return W3GetVar(TransClient);
}

stock bool IS_PLAYER(int x)
{
  if(x>0&&x<=MaxClients)
  return true;
  return false;
}

/*
 There are two ways to get admin
 //need 
 //good if you need the string to admin flag like "o" -> number
 id= native bool:GetAdminFlag(AdminId:id, AdminFlag:flag, AdmAccessMode:mode=Access_Effective);
 Admin_RCON,      
 Admin_Root,
 
 VS
 
 //good if u know the access level
 GetUserFlagBits(client)  
 
 ADMFLAG_RCON
 ADMFLAG_ROOT
 */

stock HasSMAccess(int client, int flag) {
  int flags = GetUserFlagBits(client);
  //DP("flags %d",flags);
  if (flags & (flag | ADMFLAG_ROOT)) //ADMFLAG_ROOT is "z"
  {
    return true;
  }

  return W3IsDeveloper(client);
}

/* is a normal engine cvar empty (zero length, "") ? */
stock bool CvarEmpty(Handle sourcemod_convar) {
  static char zzstr[32];
  return (GetConVarString(sourcemod_convar, zzstr, sizeof(zzstr))&&strlen(zzstr)<1);

}

//returns true/false on a chance cast. 1.0=100%, 0.1 = 10% chance
stock bool War3_Chance(float chancepercent=1.0) {
  return (GetRandomFloat(0.0,1.0)<=chancepercent);
}
stock bool W3Chance(float chancepercent=1.0) {
  return (GetRandomFloat(0.0,1.0)<=chancepercent);
}

/**
 * Returns the amount of money you have in CS/CS:GO
 * When used in TF2 it returns the amount of money the player has in MVM
 */
#if !defined SOURCECRAFT
stock GetCSMoney(int client) {
  ValveGameEnum war3Game = War3_GetGame();
  if (war3Game == Game_CS || war3Game == Game_CSGO) {
    return GetEntProp(client, Prop_Send, "m_iAccount");
  } else if (war3Game == Game_TF) {
    return GetEntProp(client, Prop_Send, "m_nCurrency");
  }

  return 0;
}
#else
#define GetCSMoney GetMoney
#endif

/**
 * Sets the amount of money the player has in CS/CS:GO
 * When used in TF2 it sets the amount of money the player has in MVM
 */
#if !defined SOURCECRAFT
stock SetCSMoney(int client, int newamount) {
  ValveGameEnum war3Game = War3_GetGame();
  if (war3Game == Game_CS || war3Game == Game_CSGO) {
    SetEntProp(client, Prop_Send, "m_iAccount", newamount);
  } else if (war3Game == Game_TF) {
    if (newamount < 0)
      newamount = 0;
    if (newamount > 32767)
      newamount = 32767;
    SetEntProp(client, Prop_Send, "m_nCurrency", newamount);
  }
}
#else
#define SetCSMoney SetMoney
#endif

stock UTIL_Remove(int entity) {
  if (IsValidEdict(entity))
    AcceptEntityInput(entity, "Kill");
}

stock W3ShowSkillsInfo(int client) {
  if (War3_GetRace(client) > 0) {
    W3SetVar(RaceinfoRaceToShow, War3_GetRace(client));
    W3CreateEvent(DoShowParticularRaceInfo, client);
  } else {
    War3_ChatMessage(client, "%T", "Select a race first!", client);
    W3CreateEvent(DoShowChangeRaceMenu, client);
  }
}

//get numver of players on the specified team (int eger team)
stock PlayersOnTeam(int team) {
  int num;
  for (int x = 1; x <= MaxClients; x++) {
    if (IsClientInGame(x) && GetClientTeam(x) == team) {
      num++;
    }
  }
  return num;
}

stock GetShortTeamName(int team, char[] retstr, int maxlen) {
  if(War3_GetGame()==CS || War3_GetGame()==CSGO) {
    if(team==1) {
      Format(retstr,maxlen,"%t","CS Spec");
      return;
    }
    if(team==TEAM_T) {
      Format(retstr,maxlen,"%t","CS T");
      return;
    }
    if(team==TEAM_CT) {
      Format(retstr,maxlen,"%t","CS CT");
      return;
    }
  }
  else if(War3_GetGame()==TF) {
    if(team==1) {
      Format(retstr,maxlen,"%t","TF Spec");
      return;
    }
    if(team==TEAM_RED) {
      Format(retstr,maxlen,"%t","TF RED");
      return;
    }
    if(team==TEAM_BLUE) {
      Format(retstr,maxlen,"%t","TF BLU");
      return;
    }
  }

  Format(retstr,maxlen,"%t","Unknown Team");
  return;
}

stock GetRacesOnTeam(int raceid, int team, int ignoreIfAdminSetRace = 0) {
  int num;
  for (int y = 1; y <= MaxClients; y++) {

    if (ValidPlayer(y, false)) {

      if (War3_GetRace(y) == raceid) {
        if (GetClientTeam(y) == team) {
          if (ignoreIfAdminSetRace&&W3GetPlayerProp(y,RaceSetByAdmin)) {
          } else {
            num++;
          }
        }
      }
    }
  }
  return num;
}

stock bool TF2_HasTheFlag(int client)
{
  if (War3_GetGame() == Game_TF)
  {
    int ent = -1;
    while ((ent = FindEntityByClassname(ent, "item_teamflag")) != -1)
    {
      if (GetEntPropEnt(ent, Prop_Data, "m_hOwnerEntity")==client)
      return true;
    }
  }
  return false;
}
stock TE_SetupKillPlayerAttachments(int client) {
  TE_Start("KillPlayerAttachments");
  TE_WriteNum("m_nPlayer", client);
}



#if !defined SOURCECRAFT // Not Implemented!
/// General callback for threaded queries.  No Action s
public SQLWar3GeneralCallback(Handle owner, Handle hndl, const char[] error, any data)
{
  SQLCheckForErrors(hndl,error,"SQLWar3GeneralCallback");
}

//pass me a trie with key "query" = your query int o originalqueryTrie
stock SQLCheckForErrors(Handle hndl, const char[] originalerror, const char[] prependstr="", Handle originalqueryTrie=null) {
  char orignalquerystr[512];
  if(originalqueryTrie) {
    if(!GetTrieString(originalqueryTrie,"query",orignalquerystr,sizeof(orignalquerystr))) {
      LogError("SQLCheckForErrors: originalqueryTrie is not null but key 'query' not set from trie");
    }
    CloseHandle(originalqueryTrie);
    //DP("closed");
  }

  if(!StrEqual("", originalerror))
  LogError("SQL error: [%s] %s QUERY:%s", prependstr, originalerror,orignalquerystr);
  else if(hndl == null)
  {
    char err[512];
    SQL_GetError(hndl, err, sizeof(err));
    LogError("SQLCheckForErrors: [%s] %s QUERY:%s", prependstr, err,orignalquerystr);
  }
}

public bool SQL_FastQueryLogOnError(Handle DB, const char[] query) {
  if(!SQL_FastQuery(DB,query)) {
    char error[256];
    SQL_GetError(DB, error, sizeof(error));
    LogError("SQLFastQuery %s failed, Error: %s",query,error);
    return false;
  }
  return true;
}

//normal query like SQL_QUERY, database must be locked
stock bool SQL_War3_NormalQuery(Handle DB, char[] querystr) {
  Handle result= SQL_Query(DB, querystr);
  if(result==null) {
    char error[256];
    SQL_GetError(DB, error, sizeof(error));
    LogError("SQL_War3_NormalQuery %s failed, Error: %s",querystr,error);
    return false;
  }
  else {
    CloseHandle(result);
  }
  return true;
}
//fetch using column string
stock W3SQLPlayerInt(Handle query, const char[] columnname) //fech from query
{
  int column;
  SQL_FieldNameToNum(query,columnname,column);
  char result[16];
  SQL_FetchString(query,column,result,sizeof(result));
  return StringToInt(result);
}
//fetch using column string
stock W3SQLPlayerFloat(Handle query, const char[] columnname) //fech from query
{
  int column;
  SQL_FieldNameToNum(query,columnname,column);
  char result[16];
  SQL_FetchString(query,column,result,sizeof(result));
  return StringToFloat(result);
}
//fetch using column string
stock W3SQLPlayerString(Handle query, const char[] columnname, char[] out_buffer, int size_out) //fech from query
{
  int column;
  if(SQL_FieldNameToNum(query,columnname,column))
  {
    SQL_FetchString(query,column,out_buffer,size_out);
    return true;
  }
  return false;
}

///add a column to table, be careful of parameters
///table should be locked!!!
stock AddColumn(Handle DB, const char[] columnname, const char[] datatype, const char[] table_name)
{
  char query[256];
  Format(query,256,"ALTER TABLE %s ADD COLUMN %s %s DEFAULT '0'",table_name,columnname,datatype);
  PrintToServer("[War3Source] Tried to ADD column in TABLE %s: %s",table_name,columnname);
  SQL_FastQueryLogOnError(DB,query);
}
#endif

///string?


// Stocks
stock StrToken(const char[] inputstr, int tokennum, char[] outputstr, int maxlen)
{
  char buf[maxlen+1];
  int cur_idx;
  int idx;
  int curind;
  idx=BreakString(inputstr,buf,maxlen);
  if(tokennum==1)
  {
    strcopy(outputstr,maxlen,buf);
    return;
  }
  curind=1;
  while(idx!=-1)
  {
    cur_idx+=idx;
    idx=BreakString(inputstr[cur_idx],buf,maxlen);
    curind++;
    if(tokennum==curind)
    {
      strcopy(outputstr,maxlen,buf);
      break;
    }
  }
}

stock StrTokenCount(const char[] input)
{
  char buf[32];
  int cur_idx;
  int idx;
  int curind;
  while(idx!=-1)
  {
    cur_idx+=idx;
    idx=BreakString(input[cur_idx],buf,sizeof(buf));
    curind++;
  }
  return curind;
}

stock float GetPlayerDistance(int client1, int client2) {
  static float vec1[3];
  static float vec2[3];
  GetClientAbsOrigin(client1,vec1);
  GetClientAbsOrigin(client2,vec2);
  return GetVectorDistance(vec1,vec2);
}

//////MESSAGES


//prints [W3S] You did %d damage to %name with SKILLNAME
//prints [W3S] %name did %d damage to you with SKILLNAME
char zzname[32];
stock W3PrintSkillDmgConsole(int victim = 0, int attacker, int damage, int skillnum) {
  GetClientName(victim, zzname, sizeof(zzname));
  int race = War3_GetRace(attacker);
  char skillname[32];
  SetTrans( attacker);
  W3GetRaceSkillName(race, skillnum, skillname, sizeof(skillname));
  PrintToConsole(attacker, "%T",
      "[W3S] You did +{amount} damage to {player} with {skill}",
      attacker, damage, zzname, skillname);

  if (victim > 0) {
    SetTrans( victim);
    W3GetRaceSkillName(race, skillnum, skillname, sizeof(skillname)); //get trans again
    GetClientName(attacker, zzname, sizeof(zzname));
    PrintToConsole(victim, "%T",
        "[W3S] {player} did {amount} damage to you with {skill}",
        victim, zzname, damage, skillname);
  }
}
//+%d damage with skill name
stock W3PrintSkillDmgHint(int victim = 0, int attacker, int damage, int SKILLNUM) {

  int race = War3_GetRace(attacker);
  char skillname[32];
  SetTrans( attacker);
  W3GetRaceSkillName(race, SKILLNUM, skillname, sizeof(skillname));

  PrintHintText(attacker, "%T", "+{amount} damage with {skill}", attacker,
      damage, skillname);

  if (victim > 0) {
    SetTrans( victim);
    W3GetRaceSkillName(race, SKILLNUM, skillname, sizeof(skillname));
    PrintHintText(victim, "%T", "Received {amount} damage from {skill}",
        victim, damage, skillname);
  }
}
stock W3PrintSkillDmgHintConsole(int victim = 0, int attacker, int damage, int SKILLNUM) {
  W3PrintSkillDmgHint(victim, attacker, damage, SKILLNUM);
  W3PrintSkillDmgConsole(victim, attacker, damage, SKILLNUM);
}

//colored
//prints [war3 tag] You did %d damage to %name with SKILLNAME
//prints [war3 tag] %name did %d damage to you with SKILLNAME
///automatically goes int o console
//removed in favor of console and hint
/*
 stock W3PrintSkillDmgChat(victim,attacker,damage,char[] skillname){
 GetClientName(victim,zzname,sizeof(zzname));
 War3_ChatMessage(attacker,"%T","You did +{amount} damage to {player} with {skill}",attacker,damage,zzname,skillname);
 
 if(victim>0){
 GetClientName(attacker,zzname,sizeof(zzname));
 War3_ChatMessage(victim,"%T","{player} did {amount} damage to you with {skill}",victim,zzname,damage,skillname);
 }
 }
 */

stock W3MsgUltNotLeveled(int client) {
  char buffer[100];
  Format(buffer, sizeof(buffer), "%T", "Your Ultimate is not leveled", client);
  PrintHintText(client, buffer);
}

stock W3MsgEvaded(int victim, int attacker){
  PrintHintText(victim,"%T","You Evaded a Shot",victim);
  PrintHintText(attacker,"%T","Enemy Evaded",attacker);
}

stock W3MsgNoTargetFound(int client, float distancegameunits=0.0) {
  char print[500];
  Format(print,sizeof(print),"%T","No target found",client);
  if(distancegameunits>0.1) {
    if(GetConVarInt(W3GetVar(hUseMetricCvar))==0)
    {
      Format(print,sizeof(print),"%T","No target found within {amount} feet",client,distancegameunits/10.0);
    }
    else
    {
      Format(print,sizeof(print),"%T","No target found within {amount} meters",client,distancegameunits/30.0);
    }
  }
  PrintHintText(client,"%s",print);
}
stock W3MsgCreatedWard(int client, int currentwardcount = 0, int totalwardcount = 0) {
  char print[500];
  Format(print, sizeof(print), "%T", "You created a Ward", client);
  if (totalwardcount > 0) {
    Format(print, sizeof(print), "%T",
        "You created a Ward {amount}/{amount}", client,
        currentwardcount, totalwardcount);
  }
  PrintHintText(client, "%s", print);
}
stock W3MsgWardLocationDeny(int client) {
  PrintHintText(client, "%T", "You can not build a Ward Here", client);
}
stock W3MsgNoWardsLeft(int client) {
  PrintHintText(client, "%T", "You have used up all your Wards!", client);
}
stock W3MsgNoWardWhenInvis(int client) {
  PrintHintText(client, "%T", "No Ward Placement While Invisible", client);
}
stock W3MsgEntangle(int victim, int attacker) {
  PrintHintText(victim, "%T", "You are Entangled!", victim);
  PrintHintText(attacker, "%T", "Entangled!", attacker);
}

stock W3MsgNoCastDuringFreezetime(int client) {
  PrintHintText(client, "%T", "Cannot cast during freezetime", client);
}
stock W3MsgSkillBlocked(int victim=0, int attacker=0, char[] skillname) {
  if(victim>0) {
    PrintHintText(victim,"%T","Blocked enemy {skill}",victim,skillname);
  }
  if(attacker>0) {
    PrintHintText(victim,"%T","Enemy Blocked {skill}",victim,skillname);
  }
}
stock W3MsgBanished(int victim, int attacker) {
  PrintHintText(victim, "%T", "You have been Banished", victim);
  PrintHintText(attacker, "%T", "You Banished", attacker);
}
stock W3MsgStoleMoney(int victim, int attacker, int dollars) {
  PrintHintText(victim, "%T", "Enemy stole {dollars} dollars from you",
      victim, dollars);
  PrintHintText(attacker, "%T", "You Stole {dollars} dollars", attacker,
      dollars);
}
stock W3MsgStoleGold(int victim, int attacker, int gold) {
  PrintHintText(victim, "%T", "Enemy stole {gold} gold from you", victim,
      gold);
  PrintHintText(attacker, "%T", "You Stole {gold} gold", attacker, gold);
}
stock W3MsgRevivedBM(int playerrevived, int savior) {
  char clientName[64];
  GetClientName(playerrevived, clientName, sizeof(clientName));
  char saviorName[64];
  GetClientName(savior, saviorName, sizeof(saviorName));

  PrintHintText(savior, "%T", "You revived {player}", savior, clientName);
  War3_ChatMessage(playerrevived, "%T", "{player} revived you",
      playerrevived, saviorName);
}
stock W3MsgUsingVoodoo(int client) {
  PrintHintText(client, "%T", "Activated Voodoo!", client);
}
stock W3MsgVoodooEnded(int client) {
  PrintHintText(client, "%T", "Voodoo has ended", client);
}
stock W3MsgEnemyHasImmunity(int client, bool console=true) {
  PrintHintText(client,"%T","Enemy has immunity!",client);
  PrintToConsole(client,"%T","[W3S] Enemy has immunity!",client);
}
stock W3MsgUltimateNotActivatable(int client) {
  PrintHintText(client, "%T", "This ultimate is not activatable", client); //prints
}
stock W3MsgUltimateBlocked(int client) {
  PrintHintText(client, "%T", "Ultimate Blocked", client);
}
stock W3MsgThrewKnives(int client) {
  PrintHintText(client, "%T", "You threw knives around you", client);
}
stock W3MsgHitByKnives(int client) {
  PrintHintText(client, "%T", "Attacked by Fan of Knives", client);
}
stock W3MsgActivated(int client, char[] postpendstr) {
  PrintHintText(client,"%T","Activated {skillname}",client,postpendstr);
}
stock W3MsgAttackedBy(int client, char[] postpendstr) {
  PrintHintText(client,"%T","Attacked By {skillname}",client,postpendstr);
}
stock W3MsgMoleIn(int client, float timetillactivation) {
  PrintHintText(client,"%T","WARNING! MOLE IN {amount} SECONDS!",client,timetillactivation);
  War3_ChatMessage(client,"%T","WARNING! MOLE IN {amount} SECONDS!",client,timetillactivation);
}
stock W3MsgMoled(int client) {
  War3_ChatMessage(client, "%T", "You have moled!", client);
  PrintHintText(client, "%T", "You have moled!", client);
}
stock W3MsgNoLongerDisguised(int client) {
  War3_ChatMessage(client, "%T", "You are no longer disguised", client);
  PrintHintText(client, "%T", "You are no longer disguised", client);
}
stock W3MsgVengenceWasBlocked(int client, char[] reason) {
  War3_ChatMessage(client,"%T","Vengence blocked ({reason})",client,reason);
}


#if !defined SOURCECRAFT
native W3IsBuffInvised(int client); //returns true if client alpha is <50
#else
stock W3IsBuffInvised(int client) //returns true if client alpha is <50
{
	int m_clrRender;
	FindSendPropInfo("CBaseAnimating", "m_clrRender", .local_offset=m_clrRender);
	return GetEntData(client,m_clrRender+3,1)<50;
}
#endif

//invisible or cloaked?
stock IsInvis(int client) {
  return (GameTF() && (TF2_IsPlayerInCondition(client, TFCond_Cloaked)
      || W3IsBuffInvised(client)));
}

/**
 * Prints Message to server and all chat
 * For debugging prints
 */
stock DP(const char[] szMessage, any ...)
{

  char szBuffer[1000];

  VFormat(szBuffer, sizeof(szBuffer), szMessage, 2);
  PrintToServer("[DebugP %d] %s",RoundFloat(GetGameTime())%100, szBuffer);
  PrintToChatAll("[DebugP %d] %s",RoundFloat(GetGameTime())%100, szBuffer);

}
//prring plugin
stock DPP(Plugin plugin)
{
  char szBuffer[1000];
  GetPluginFilename(plugin, szBuffer,sizeof(szBuffer));
  DP("[Debug PrintP] %s", szBuffer);
}

//First match bases, insensitive
stock Handle FindPluginByFileCustom(const char[] filename)
{
    char buffer[256];
    
    Handle iter = GetPluginIterator();
    Handle pl;
    
    while (MorePlugins(iter))
    {
        pl = ReadPlugin(iter);
        
        GetPluginFilename(pl, buffer, sizeof(buffer));
        //DP("%s",buffer);
        if (StrContains(buffer,filename,false)>-1) //not case sensitive
        {
            CloseHandle(iter);
            return pl;
        }
    }
    
    CloseHandle(iter);
    
    return null;
}

stock PrintPlugin(Handle plugin) {
  char str[100];
  GetPluginFilename(plugin, str, sizeof(str));
  War3_LogInfo("%s",str);
}
stock PrintPluginError(Handle plugin) {
  char str[100];
  GetPluginFilename(plugin, str, sizeof(str));
  LogError("%s",str);
  War3_LogError("%s",str);
}

///find a "string" in a war3cvar that is delimited by commas "rubbish,string" = true
stock bool W3FindStringInCvar(int cvarid, char[] findme, int max=9999) {
  char cvarstr[100];
  char exploded[32][32];

  W3GetCvar(cvarid,cvarstr,sizeof(cvarstr));

  int num;
  if(strlen(cvarstr)>0) {
    num=ExplodeString(cvarstr,",",exploded,256,256);
    for(int i=0;i<num && i<max;i++) {
      //PrintToServer("'%s' compared to: '%s' num%d",exploded[i],itemShort,num);
      if(StrEqual(exploded[i],findme,false)) {
        //PrintToServer("TRUE");
        return true;
      }
    }
  }
  return false;
}

// Picks a random player currently ingame.
stock W3GetRandomPlayer(int team, bool check_alive=false, War3Immunity check_immunity=Immunity_None) {

  int clients[MaxClients+1], clientCount;
  for (int i = 1; i <= MaxClients; i++)
    if (IsClientInGame(i) && (GetClientTeam(i) == team) && (!check_alive || IsPlayerAlive(i)) && (!W3HasImmunity(i,check_immunity)))
      clients[clientCount++] = i;
  return (clientCount == 0) ? -1 : clients[GetRandomInt(0, clientCount-1)];
}

// Returns true if a and b are the same
stock bool VectorEquals(const float a[3], const float b[3])
{
  if(a[0]==b[0] && a[1]==b[1] && a[2]==b[2]) {
    return true;
  }
  return false;
}

// Revan: This was originally a hotfix for sounds in csgo
// it just ClientCommand("playgamesound")..
// Its not required anymore because a more clean fix
// for sounds has been implemented!
// @deprecated This is here for backwards compatibility only.
stock W3EmitSoundToAll(const char[] sample,
    entity = SOUND_FROM_PLAYER,
    channel = SNDCHAN_AUTO,
    level = SNDLEVEL_NORMAL,
    flags = SND_NOFLAGS,
    float volume = SNDVOL_NORMAL,
    pitch = SNDPITCH_NORMAL,
    speakerentity = -1,
    float origin[3] = NULL_VECTOR,
    const float dir[3] = NULL_VECTOR,
    bool updatePos = true,
    float soundtime = 0.0)
{
  EmitSoundToAll(sample,entity,channel,level,flags,volume,pitch,speakerentity,origin,dir,updatePos,soundtime);
}

/**
 * Returns UNIX time (int eger)
 */
stock NOW() {
  return GetTime();
}
/**
 * returns the larger int eger of the two
 */
stock IntMax(int one, int two) {
  return one > two ? one : two;
}

//what we want is something that always increments even past map changes
//not int ended to matter of game pauses or not (tick)
stock float AbsoluteTime() {
  return GetEngineTime();
}

stock CreateTimer_DEBUG(any f=0,any ff=0,any fff=0,any ffff=0) {}


// PUT THIS WHERE IT MAKES SENSE SOMETIME

//check race limit and force re-choose race
stock CheckRaceTeamLimit(int raceid, int team){

  float youngest=-1.0;
  int target=0;
  for(int i=1;i<=MaxClients;i++){
    if(ValidPlayer(i))
    {
      if(War3_GetRace(i)==raceid&&!W3IsDeveloper(i))
      {
        if(GetClientTeam(i)==team)
        {
          if(W3GetPlayerProp(i,LastChangeTeamTime)>youngest){
            target=i;
            youngest=W3GetPlayerProp(i,LastChangeTeamTime);
          }
        }
      }
    }
  }
  if(target>0){
    War3_SetRace(target,0);
    char racename[64];
    War3_GetRaceName(raceid,racename,sizeof(racename));
    War3_ChatMessage(target,"%T","{race} race team limit has reached (MAX {amount}), please select a different race",target,racename,W3GetRaceMaxLimitTeam(raceid,team));
    PrintToConsole(target,"Your race has been set via raceutils.inc");
    int cvar=W3GetRaceMaxLimitTeamCvar(raceid,team);
    char cvarstr[64];
    if(cvar>-1){
      W3GetCvarActualString(cvar,cvarstr,sizeof(cvarstr));
    }
    cvar=W3FindCvar(cvarstr);
    char cvarvalue[64];
    if(cvar>-1){
      W3GetCvar(cvar,cvarvalue,sizeof(cvarvalue));
    }
    
    War3_LogInfo("race %s blocked on client %d due to restrictions limit %d (CheckRaceTeamLimit)  %s %s",racename,target,W3GetRaceMaxLimitTeam(raceid,team),cvarstr,cvarvalue);
    
    W3CreateEvent(DoShowChangeRaceMenu,target);
  }
}

/**
 * Extends the path to a sound clip with the correct prefix
 */
stock War3_AddSoundFolder(char[] sBaseString, int maxlength, char[] sSoundFile)
{
  Format(sBaseString, maxlength, GAMECSGO ? "music/war3source/%s" : "war3source/%s", sSoundFile);
  
  War3_LogInfo("Constructed sound path: \"%s\"", sBaseString);
}

/**
 * Get the hint sound effect for the current game
 */
stock War3_GetHintSound(char[] sSoundFile, int maxlength)
{
  if(GAMECSGO)
  {
    strcopy(sSoundFile, maxlength, "music/war3source/csgo/ui/hint.mp3");
  }
  else
  {
    strcopy(sSoundFile, maxlength, "UI/hint.wav");
  }
}

#include "W3SIncs/War3Source_L4D_Interface"
#include "W3SIncs/War3Source_CS_Interface"
#include "W3SIncs/War3Source_TF2_Interface"
#include "W3SIncs/War3Source_Notifications"
#include "W3SIncs/War3Source_Health"
#include "W3SIncs/War3Source_SkillEffects"
#include "W3SIncs/War3Source_Shopitems"
#include "W3SIncs/War3Source_Buffs"
#include "W3SIncs/War3Source_XP_Gold"
#include "W3SIncs/War3Source_Config"
#include "W3SIncs/War3Source_Attributes"
#include "W3SIncs/War3Source_AttributeBuffs"
#include "W3SIncs/War3Source_Events"
#include "W3SIncs/War3Source_Currency"
#include "W3SIncs/War3Source_Effects"
#include "W3SIncs/War3Source_PrecacheDownload"
#include "W3SIncs/War3Source_Shopitems2"


/*
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*   - BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-- BACKWARDS COMPATIBILITY-
*/

/**
 * Backwards compatibility for the precaches of a given sound.
 *
 * @param sound     Name of the sound to download and precache.
 * @param precache    If precache is true the file will be precached.
 * @param preload   If preload is true the file will be precached before level startup.
 * 
 * @return True if successfully precached, false otherwise.
 */

stock bool War3_PrecacheSound(char[] sound)
{
  char longsound[512];
  Format(longsound,sizeof(longsound), "sound/%s", sound);

  if(PrecacheSound(sound, true)){
    //PrintToServer("TWar3_PrecacheSound %s",longsound);
    return true;
  }
  else{
    PrintToServer("Fail War3_PrecacheSound %s",longsound);  
  }
  return false;
}

// For Shopmenu 2
stock War3_GetDiamonds(int client) {
  return W3GetPlayerProp(client, PlayerDiamonds);
}
// For Shopmenu 2
stock War3_SetDiamonds(int client, int newd) {
  W3SetPlayerProp(client, PlayerDiamonds, newd);
}
