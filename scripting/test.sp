/**
 * vim: set ai et ts=4 sw=4 :
 * File: test.sp
 * Description: Display values of various variables.
 * Author(s): -=|JFH|=-Naris
 */
 
#pragma semicolon 1

#include <sourcemod>
#include <sdktools>
#include <tf2_stocks>
#include <regex>

int m_OffsetCloakMeter;
int m_OffsetDisguiseTeam;
int m_OffsetDisguiseClass;
int m_OffsetDisguiseHealth;
int m_OffsetDisguiseTargetIndex;
int m_OffsetDesiredDisguiseTeam;
int m_OffsetDesiredDisguiseClass;
int m_OffsetInvisChangeCompleteTime;
int m_OffsetCritMult;
int m_OffsetStealthNoAttackExpire;
int m_OffsetStealthNextChangeTime;
int m_OffsetPlayerState;
int m_OffsetNumHealers;
int m_OffsetPlayerCond;
int m_OffsetClass;
int m_OffsetPoisoned;
int m_OffsetWearingSuit;
int m_OffsetBonusProgress;
int m_OffsetBonusChallenge;
int m_OffsetAirDash;
int m_OffsetMaxspeed;
int m_OffsetMyWepons;

Handle cvarTrack = null;

enum objects { dispenser, teleporter_entry, teleporter_exit, sentrygun, sapper, unknown };

public Plugin myinfo = 
{
    name = "Test Module",
    author = "-=|JFH|=-Naris",
    description = "A Testing Module to track various variables.",
    version = "1.0.0.0",
    url = "http://jigglysfunhouse.net/"
};

public OnPluginStart()
{
    cvarTrack=CreateConVar("sm_track_tf2","1");
    RegConsoleCmd("ent_remove",EntityRemoved);
    AddGameLogHook(InterceptLog);

    if(!HookEvent("player_builtobject", PlayerBuiltObject))
        SetFailState("Could not hook the player_builtobject event.");

    if (GetConVarBool(cvarTrack))
        CreateTimer(1.0,TrackVariables,null,TIMER_REPEAT);
}

// Events
public OnMapStart()
{
    m_OffsetCloakMeter=FindSendPropInfo("CTFPlayer","m_flCloakMeter");
    m_OffsetDisguiseTeam=FindSendPropInfo("CTFPlayer","m_nDisguiseTeam");
    m_OffsetDisguiseClass=FindSendPropInfo("CTFPlayer","m_nDisguiseClass");
    m_OffsetDisguiseHealth=FindSendPropInfo("CTFPlayer","m_iDisguiseHealth");
    m_OffsetDisguiseTargetIndex=FindSendPropInfo("CTFPlayer","m_iDisguiseTargetIndex");
    m_OffsetDesiredDisguiseTeam=FindSendPropInfo("CTFPlayer","m_nDesiredDisguiseTeam");
    m_OffsetDesiredDisguiseClass=FindSendPropInfo("CTFPlayer","m_nDesiredDisguiseClass");
    m_OffsetInvisChangeCompleteTime=FindSendPropInfo("CTFPlayer","m_flInvisChangeCompleteTime");
    m_OffsetCritMult=FindSendPropInfo("CTFPlayer","m_iCritMult");
    m_OffsetStealthNoAttackExpire=FindSendPropInfo("CTFPlayer","m_flStealthNoAttackExpire");
    m_OffsetStealthNextChangeTime=FindSendPropInfo("CTFPlayer","m_flStealthNextChangeTime");
    m_OffsetPlayerState=FindSendPropInfo("CTFPlayer","m_nPlayerState");
    m_OffsetNumHealers=FindSendPropInfo("CTFPlayer","m_nNumHealers");
    m_OffsetPlayerCond=FindSendPropInfo("CTFPlayer","m_nPlayerCond");
    m_OffsetClass=FindSendPropInfo("CTFPlayer","m_iClass");
    m_OffsetPoisoned=FindSendPropInfo("CTFPlayer","m_bPoisoned");
    m_OffsetWearingSuit=FindSendPropInfo("CTFPlayer","m_bWearingSuit");
    m_OffsetBonusProgress=FindSendPropInfo("CTFPlayer","m_iBonusProgress");
    m_OffsetBonusChallenge=FindSendPropInfo("CTFPlayer","m_iBonusChallenge");
    m_OffsetAirDash=FindSendPropInfo("CTFPlayer","m_bAirDash");
    m_OffsetMaxspeed=FindSendPropInfo("CTFPlayer","m_flMaxspeed");
    m_OffsetMyWepons=FindSendPropOffs("CTFPlayer", "m_hMyWeapons");
}

public Action TrackVariables(Handle timer)
{
    int maxplayers=GetMaxClients();
    for(int client=1;client<=maxplayers;client++)
    {
        if (IsClientInGame(client))
        {
            if (IsPlayerAlive(client))
            {
                new TFClassType:tfClass = TF2_GetPlayerClass(client);
                int class = m_OffsetClass>0 ? GetEntData(client,m_OffsetClass) : -99;
                float cloakMeter = m_OffsetCloakMeter>0 ? GetEntDataFloat(client,m_OffsetCloakMeter) : -99.9;
                int disguiseTeam = m_OffsetDisguiseTeam>0 ? GetEntData(client,m_OffsetDisguiseTeam) : -99;
                int disguiseClass = m_OffsetDisguiseClass>0 ? GetEntData(client,m_OffsetDisguiseClass) : -99;
                int disguiseTarget = m_OffsetDisguiseTargetIndex>0 ? GetEntData(client,m_OffsetDisguiseTargetIndex) : -99;
                int disguiseHealth = m_OffsetDisguiseHealth>0 ? GetEntData(client,m_OffsetDisguiseHealth) : -99;
                int desiredDisguiseTeam = m_OffsetDesiredDisguiseTeam>0 ? GetEntData(client,m_OffsetDesiredDisguiseTeam) : -99;
                int desiredDisguiseClass = m_OffsetDesiredDisguiseClass>0 ? GetEntData(client,m_OffsetDesiredDisguiseClass) : -99;
                float invisChangeCompleteTime = m_OffsetInvisChangeCompleteTime>0 ? GetEntDataFloat(client,m_OffsetInvisChangeCompleteTime) : -99.9;
                int critMult = m_OffsetCritMult>0 ? GetEntData(client,m_OffsetCritMult) : -99;
                float stealthNoAttackExpire = m_OffsetStealthNoAttackExpire>0 ? GetEntDataFloat(client,m_OffsetStealthNoAttackExpire) : -99.9;
                float stealthNextChangeTime = m_OffsetStealthNextChangeTime>0 ? GetEntDataFloat(client,m_OffsetStealthNextChangeTime) : -99.9;
                int playerState = m_OffsetPlayerState>0 ? GetEntData(client,m_OffsetPlayerState) : -99;
                int numHealers = m_OffsetNumHealers>0 ? GetEntData(client,m_OffsetNumHealers) : -99;
                int playerCond = m_OffsetPlayerCond>0 ? GetEntData(client,m_OffsetPlayerCond) : -99;
                int poisened = m_OffsetPoisoned>0 ? GetEntData(client,m_OffsetPoisoned) : -99;
                int wearingSuit = m_OffsetWearingSuit>0 ? GetEntData(client,m_OffsetWearingSuit) : -99;
                int bonusProgress = m_OffsetBonusProgress>0 ? GetEntData(client,m_OffsetBonusProgress) : -99;
                int bonusChallenge = m_OffsetBonusChallenge>0 ? GetEntData(client,m_OffsetBonusChallenge) : -99;
                int airDash = m_OffsetAirDash>0 ? GetEntData(client,m_OffsetAirDash) : -99;
                float maxSpeed= m_OffsetMaxspeed>0 ? GetEntDataFloat(client,m_OffsetMaxspeed) : -99.9;

                LogMessage("client=%d(%N),playerCond=%08x,tfClass=%d,class=%d,cloakMeter=%f,disguiseTeam=%d,disguiseClass=%d,disguiseTarget=%d,disguiseHealth=%d,desiredDisguiseTeam=%d,desiredDisguiseClass=%d,invisChangeCompleteTime=%f,critMult=%d,stealthNoAttackExpire=%f,stealthNextChangeTime=%f,playerState=%d,numHealers=%d,poisoned=%d,wearingSuit=%d,bonusProgress=%d,bonusChallenge=%d,airDash=%d,maxSpeed=%f",client,client,playerCond,tfClass,class,cloakMeter,disguiseTeam,disguiseClass,disguiseTarget,disguiseHealth,desiredDisguiseTeam,desiredDisguiseClass,invisChangeCompleteTime,critMult,stealthNoAttackExpire,stealthNextChangeTime,playerState,numHealers,poisened,wearingSuit,bonusProgress,bonusChallenge,airDash,maxSpeed);
                PrintToChat( client,"plrState=%d,plrCond=%08x,bP=%d,bC=%d,aD=%d",playerState,playerCond,bonusProgress,bonusChallenge,airDash);
            }
        }
    }
    return Plugin_Continue;
}

public OnUltimateCommand(client,player,race,bool pressed)
{
    if (pressed && IsPlayerAlive(client))
    {
        char wepName[128];
        int iterOffset=m_OffsetMyWepons;
        for(int y=0;y<48;y++)
        {
            int wepEnt=GetEntDataEnt2(client,iterOffset);
            if (wepEnt>0&&IsValidEdict(wepEnt))
            {
                GetEdictClassname(wepEnt,wepName,sizeof(wepName));
                PrintToChat(client, wepName);
            }
            iterOffset+=4;
        }
    }
}

public Action EntityRemoved(client,args)
{
    char arg[64];
    if (GetCmdArg(1,arg,sizeof(arg)) > 0)
    {
        if (IsPlayerAlive(client))
            PrintToChat(client, "ent_remove %s", arg);
    }
    return Plugin_Continue;
}

//19:26:28 L 04/18/2008 - 19:26:32: "-=|JFH|=-Naris<3><STEAM_0:1:5037159><Red>" triggered "killedobject" (object "OBJ_SENTRYGUN") (weapon "pda_engineer") (objectowner "-=|JFH|=-Naris<3><STEAM_0:1:5037159><Red>") (attacker_position "2100 2848 -847")

public Action InterceptLog(const char[] message)
{
    if (StrContains(message, "killedobject", true) >= 0)
    {
        int attacker = 0;
        int builder = 0;
        //char buffer[5];
        char obj[64];
        char a[64];
        char b[64];
        //Handle re = CompileRegex("\".+<([0-9]+)><.+><.+>.*\" triggered \"killedobject\" \\(object \"([A-Z_])\"\\) .*\\(objectowner \".+<([0-9]+)><.+>\"\\)");
        Handle re = CompileRegex("\".+<([0-9]+)><.+><.+>.*\".* triggered \"killedobject\" .object \"([[:word:]]+)\". .*objectowner \".+<([0-9]+)><.+>\"");
        if (re != null)
        {
            if (MatchRegex(re, message))
            {
                if (GetRegexSubString(re, 1, a, sizeof(a)))
                {
                    attacker = StringToInt(a);
                    if (GetRegexSubString(re, 2, obj, sizeof(obj)))
                    {
                        if (GetRegexSubString(re, 3, b, sizeof(b)))
                            builder = StringToInt(b);
                    }
                    else
                        obj[0] = 0;
                }
                LogMessage("===> %d(%s) destroyed %d(%s)'s %s!", attacker, a, builder, b, obj);
                //PrintToChat(attacker, "%d (%N) destroyed %d(%N)'s  %s!", attacker, attacker, builder, builder, obj);
                //PrintToChat(builder, "%d (%N) destroyed %d(%N)'s  %s!", attacker, attacker, builder, builder, obj);
            }
            else
                LogMessage("NO MATCH:%s", message);

            delete re;
        }
        else
            LogMessage("Invalid Regex!");
    }
}

public PlayerBuiltObject(Handle event,const char[] name,bool dontBroadcast)
{
    int userid = GetEventInt(event,"userid");
    if (userid > 0)
    {
        int index=GetClientOfUserId(userid);

        //new objects:type = unknown;
        int obj = GetEventInt(event,"obj");

        LogMessage("player_objectbuilt: userid=%d(%d), object=%d",
                   userid, index, obj);
    }
}

public OnObjectKilled(attacker, builder,const char[] obj)
{
    int objects:type = unknown;
    if (StrEqual(obj, "OBJ_SENTRYGUN", false))
        type = sentrygun;
    else if (StrEqual(obj, "OBJ_DISPENSER", false))
        type = dispenser;
    else if (StrEqual(obj, "OBJ_TELEPORTER_ENTRANCE", false))
        type = teleporter_entry;
    else if (StrEqual(obj, "OBJ_TELEPORTER_EXIT", false))
        type = teleporter_exit;

    LogMessage("objectkilled: builder=%d, type=%d, object=%s",
               builder, type, obj);
}

public Action TF2_CalcIsAttackCritical(client, weapon, char weaponname[], bool & result)
{
    PrintToChat(client, "weaponname=%s", weaponname);
    LogMessage("TF2_CalcIsAttackCritical: client=%d, weapon=%d, weaponname=%s",
               client, weapon, weaponname);
    return Plugin_Continue;
}  

