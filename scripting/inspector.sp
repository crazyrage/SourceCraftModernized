/**
 * vim: set ai et ts=4 sw=4 :
 * File: inspector.sp
 * Description: Display values of various entities.
 * Author(s): -=|JFH|=-Naris
 */
 
#pragma semicolon 1

#include <sourcemod>
#include <sdktools>
#include <tf2_stocks>
#include <tf2_objects>

#define PL_VERSION "1.3"

Handle g_InspectionEnabled = null;

public Plugin myinfo = 
{
    name = "The Inspector",
    author = "-=|JFH|=-Naris",
    description = "A Testing Module to inspect various entities.",
    version = PL_VERSION,
    url = "http://jigglysfunhouse.net/"
};

public OnPluginStart()
{
    CreateConVar("sm_obj_inspector", PL_VERSION, "Object Inspector", FCVAR_SPONLY|FCVAR_REPLICATED|FCVAR_NOTIFY);
    g_InspectionEnabled = CreateConVar("sm_obj_inspection","0","Enable inspecting object on events (0=disabled|1=enabled)", _, true, 0.0, true, 1.0);

    if (!HookEvent("player_builtobject", PlayerBuiltObject))
        SetFailState("Could not hook the player_builtobject event.");

    if (!HookEvent("player_upgradedobject", PlayerBuiltObject))
        SetFailState("Could not hook the player_upgradedobject event.");

    if (!HookEvent("object_destroyed", ObjectRemoved))
        SetFailState("Could not hook the object_destroyed event.");

    if (!HookEvent("object_removed", ObjectRemoved))
        SetFailState("Could not hook the object_removed event.");

    RegConsoleCmd("sm_inspect", InspectCmd);
    RegAdminCmd("sm_objhp", Command_ObjHealth, ADMFLAG_CHEATS);
    RegAdminCmd("sm_objshell", Command_ObjShells, ADMFLAG_CHEATS);
    RegAdminCmd("sm_objrocket", Command_ObjRockets, ADMFLAG_CHEATS);
}

// Events

public PlayerBuiltObject(Handle event,const char[] name,bool dontBroadcast)
{
    if (GetConVarBool(g_InspectionEnabled))
    {
        int userid = GetEventInt(event,"userid");
        int client = GetClientOfUserId(userid);
        if (client > 0)
        {
            //new objects:type = unknown;
            int objectid = GetEventInt(event,"index");
            new TFObjectType:obj = TFObjectType:GetEventInt(event,"object");

            LogMessage("%s: userid=%d:%d:%N, entity=%d, object=%d:%s",
                       name, userid, client, client, objectid, obj,
                       TF2_ObjectNames[obj]);

            InspectEntity(0, objectid);
        }
    }
}

public ObjectRemoved(Handle event,const char[] name,bool dontBroadcast)
{
    if (GetConVarBool(g_InspectionEnabled))
    {
        int userid = GetEventInt(event,"userid");
        int client = GetClientOfUserId(userid);
        if (client > 0)
        {
            //new objects:type = unknown;
            int objectid = GetEventInt(event,"index");
            new TFObjectType:obj = TFObjectType:GetEventInt(event,"objecttype");

            LogMessage("%s: userid=%d:%d:%N, entity=%d, object=%d:%s",
                       name, userid, client, client, objectid, obj, TF2_ObjectNames[obj]);

            InspectEntity(0, objectid);
        }
    }
}

public Action InspectCmd(client, args)
{
    int target = GetClientAimTarget(client, false);
    if (target > 0)
    {
        LogMessage("Inspected by %d:%N: entity=%d", client, client, target);
        InspectEntity(client, target);
    }
}

InspectEntity(client, entity)
{
    char class[32];
    if (!GetEdictClassname(entity,class,sizeof(class)))
        class[0] = '\0';

    char net_class[32];
    if (!GetEntityNetClass(entity,net_class,sizeof(net_class)))
        net_class[0] = '\0';

    int flags = GetEdictFlags(entity);

    int m_nSkin = GetEntProp(entity, Prop_Send, "m_nSkin");
    int m_nBody = GetEntProp(entity, Prop_Send, "m_nBody");
    int m_iTeamNum = GetEntProp(entity, Prop_Send, "m_iTeamNum");

    float m_vecOrigin[3];
    GetEntPropVector(entity, Prop_Send, "m_vecOrigin", m_vecOrigin);

    float m_angRotation[3];
    GetEntPropVector(entity, Prop_Send, "m_angRotation", m_vecOrigin);

    float m_vecMaxs[3];
    GetEntPropVector(entity, Prop_Send, "m_vecMaxs", m_vecMaxs);

    float m_vecMins[3];
    GetEntPropVector(entity, Prop_Send, "m_vecMins", m_vecMins);

    LogMessage("%s/%s: flags=%x, skin=%d, body=%d, team=%d, maxs={%f,%f,%f}, mins={%f,%f,%f}, origin={%f,%f,%f}, angle={%f,%f,%f}",
               class, net_class, flags, m_nSkin, m_nBody, m_iTeamNum, m_vecMaxs[0], m_vecMaxs[1], m_vecMaxs[2],
               m_vecMins[0], m_vecMins[1], m_vecMins[2], m_vecOrigin[0], m_vecOrigin[1], m_vecOrigin[2],
               m_angRotation[0], m_angRotation[1], m_angRotation[2]);

    if (client > 0)
    {
        PrintToChat(client, "%s/%s: flags=%x, skin=%d, body=%d",
                    class, net_class, flags, m_nSkin, m_nBody);
    }

    if (strncmp(class, "obj_", 4) == 0)
    {
        float m_vecBuildMaxs[3];
        GetEntPropVector(entity, Prop_Send, "m_vecBuildMaxs", m_vecBuildMaxs);

        float m_vecBuildMins[3];
        GetEntPropVector(entity, Prop_Send, "m_vecBuildMins", m_vecBuildMins);

        int m_iHealth = GetEntProp(entity, Prop_Send, "m_iHealth");
        int m_iMaxHealth = GetEntProp(entity, Prop_Send, "m_iMaxHealth");
        int m_bHasSapper = GetEntProp(entity, Prop_Send, "m_bHasSapper");
        int m_iObjectType = GetEntProp(entity, Prop_Send, "m_iObjectType");
        int m_iObjectMode = GetEntProp(entity, Prop_Send, "m_iObjectMode");
        int m_iUpgradeMetal = GetEntProp(entity, Prop_Send, "m_iUpgradeMetal");
        int m_iUpgradeLevel = GetEntProp(entity, Prop_Send, "m_iUpgradeLevel");
        int m_iHighestUpgradeLevel = GetEntProp(entity, Prop_Send, "m_iHighestUpgradeLevel");
        int m_fObjectFlags = GetEntProp(entity, Prop_Send, "m_fObjectFlags");
        int m_bBuilding = GetEntProp(entity, Prop_Send, "m_bBuilding");
        int m_bPlacing = GetEntProp(entity, Prop_Send, "m_bPlacing");
        int m_bCarried = GetEntProp(entity, Prop_Send, "m_bCarried");
        int m_bCarryDeploy = GetEntProp(entity, Prop_Send, "m_bCarryDeploy");
        int m_bMiniBuilding = GetEntProp(entity, Prop_Send, "m_bMiniBuilding");
        int m_hBuiltOnEntity = GetEntProp(entity, Prop_Send, "m_hBuiltOnEntity");
        int m_bDisabled = GetEntProp(entity, Prop_Send, "m_bDisabled");
        int m_iDesiredBuildRotations = GetEntProp(entity, Prop_Send, "m_iDesiredBuildRotations");
        int m_bServerOverridePlacement = GetEntProp(entity, Prop_Send, "m_bServerOverridePlacement");

        int m_hBuilder = GetEntPropEnt(entity, Prop_Send, "m_hBuilder");

        float m_flPercentageConstructed = GetEntPropFloat(entity, Prop_Send, "m_flPercentageConstructed");

        LogMessage("type=%d, mode=%d, mini=%d, oflags=%x, disabled=%d, level=%d, hlevel=%d, BuildMaxs={%f,%f,%f}, BuildMins={%f,%f,%f}",
                   m_iObjectType, m_iObjectMode, m_bMiniBuilding, m_fObjectFlags, m_bDisabled, m_iUpgradeLevel, m_iHighestUpgradeLevel,
                   m_vecBuildMaxs[0], m_vecBuildMaxs[1], m_vecBuildMaxs[2],
                   m_vecBuildMins[0], m_vecBuildMins[1], m_vecBuildMins[2]);

        LogMessage("health=%d, MaxHealth=%d, sapper=%d, metal=%d, building=%d, placing=%d, carried=%d, CarryDeploy=%d, placement=%d, percentage=%f",
                   m_iHealth, m_iMaxHealth, m_bHasSapper, m_iUpgradeMetal,
                   m_bBuilding, m_bPlacing, m_bCarried, m_bCarryDeploy,
                   m_bServerOverridePlacement, m_flPercentageConstructed);

        LogMessage("BuiltOnEntity=%d, DesiredBuildRotations=%d, Builder=%d",
                   m_hBuiltOnEntity, m_iDesiredBuildRotations, m_hBuilder);

        if (client > 0)
        {
            PrintToChat(client, "type=%d, mode=%d, mini=%d, oflags=%x, disabled=%d, metal=%d, level=%d, hlevel=%d, maxs={%f,%f,%f}",
                        m_iObjectType, m_iObjectMode, m_bMiniBuilding, m_fObjectFlags, m_bDisabled, m_iUpgradeMetal, m_iUpgradeLevel,
                        m_iHighestUpgradeLevel, m_vecBuildMaxs[0], m_vecBuildMaxs[1], m_vecBuildMaxs[2]);
        }                        

        if (StrEqual(class, "obj_sentrygun"))
        {
            int m_iAmmoShells = GetEntProp(entity, Prop_Send, "m_iAmmoShells");
            int m_iAmmoRockets = GetEntProp(entity, Prop_Send, "m_iAmmoRockets");
            int m_iState = GetEntProp(entity, Prop_Send, "m_iState");
            int m_bPlayerControlled = GetEntProp(entity, Prop_Send, "m_bPlayerControlled");
            int m_bShielded = GetEntProp(entity, Prop_Send, "m_bShielded");
            int m_hEnemy = GetEntProp(entity, Prop_Send, "m_hEnemy");
            int m_hAutoAimTarget = GetEntProp(entity, Prop_Send, "m_hAutoAimTarget");

            float m_HackedGunPos[3];
            GetEntPropVector(entity, Prop_Data, "m_HackedGunPos", m_HackedGunPos);

            LogMessage("shells=%d, rockets=%d, shielded=%d, state=%d, controlled=%d, enemy=%d, target=%d, GunPos={%f,%f,%f}",
                       m_iAmmoShells, m_iAmmoRockets, m_bShielded, m_iState, m_bPlayerControlled, m_hEnemy, m_hAutoAimTarget, 
                       m_HackedGunPos[0], m_HackedGunPos[1], m_HackedGunPos[2]);

            if (client > 0)
            {
                PrintToChat(client, "shells=%d, rockets=%d, shielded=%d, state=%d, controlled=%d, enemy=%d, target=%d",
                            m_iAmmoShells, m_iAmmoRockets, m_bShielded, m_iState, m_bPlayerControlled, m_hEnemy, m_hAutoAimTarget);

                PrintToChat(client, "GunPos={%f,%f,%f}",
                            m_HackedGunPos[0], m_HackedGunPos[1], m_HackedGunPos[2]);
            }
        }
        LogMessage("------------------------------------------------------------------------------------------------------------------");
    }
}

public Action Command_ObjHealth(client, args)
{
    int target = GetClientAimTarget(client, false);
    if (target > 0)
    {
        char class[32];
        if (GetEdictClassname(target,class,sizeof(class)))
        {
            if (strncmp(class, "obj_", 4) == 0)
            {
                int m_iMaxHealth = GetEntProp(target, Prop_Send, "m_iMaxHealth");
                int m_iHealth = GetEntProp(target, Prop_Send, "m_iHealth");

                if (args >= 1)
                {
                    char arg1[32];
                    GetCmdArg(1, arg1, sizeof(arg1));

                    int num=StringToInt(arg1);
                    if (num != 0)
                    {
                        m_iHealth += num;
                        SetEntProp(target, Prop_Send, "m_iHealth", m_iHealth);
                        PrintToChat(client, "Set Object %d's health=%d/%d", target, m_iHealth, m_iMaxHealth);
                    }
                    else
                        PrintToChat(client, "Object %d's health=%d/%d", target, m_iHealth, m_iMaxHealth);
                }
                else
                    PrintToChat(client, "Object %d's health=%d/%d", target, m_iHealth, m_iMaxHealth);
            }
        }
    }

    return Plugin_Handled;
}

public Action Command_ObjShells(client, args)
{
    int target = GetClientAimTarget(client, false);
    if (target > 0)
    {
        char class[32];
        if (GetEdictClassname(target,class,sizeof(class)))
        {
            if (StrEqual(class, "obj_sentrygun"))
            {
                int m_iAmmoShells = GetEntProp(target, Prop_Send, "m_iAmmoShells");

                if (args >= 1)
                {
                    char arg1[32];
                    GetCmdArg(1, arg1, sizeof(arg1));

                    int num=StringToInt(arg1);
                    if (num != 0)
                    {
                        m_iAmmoShells += num;
                        SetEntProp(target, Prop_Send, "m_iAmmoShells", m_iAmmoShells);
                        PrintToChat(client, "Set Object %d's shells=%d", target, m_iAmmoShells);
                    }
                    else
                        PrintToChat(client, "Object %d's shells=%d", target, m_iAmmoShells);
                }
                else
                    PrintToChat(client, "Object %d's shells=%d", target, m_iAmmoShells);
            }
        }
    }

    return Plugin_Handled;
}

public Action Command_ObjRockets(client, args)
{
    int target = GetClientAimTarget(client, false);
    if (target > 0)
    {
        char class[32];
        if (GetEdictClassname(target,class,sizeof(class)))
        {
            if (StrEqual(class, "obj_sentrygun"))
            {
                int m_iAmmoRockets = GetEntProp(target, Prop_Send, "m_iAmmoRockets");

                if (args >= 1)
                {
                    char arg1[32];
                    GetCmdArg(1, arg1, sizeof(arg1));

                    int num=StringToInt(arg1);
                    if (num != 0)
                    {
                        m_iAmmoRockets += num;
                        SetEntProp(target, Prop_Send, "m_iAmmoRockets", m_iAmmoRockets);
                        PrintToChat(client, "Set Object %d's rockets=%d", target, m_iAmmoRockets);
                    }
                    else
                        PrintToChat(client, "Object %d's rockets=%d", target, m_iAmmoRockets);
                }
                else
                    PrintToChat(client, "Object %d's rockets=%d", target, m_iAmmoRockets);
            }
        }
    }

    return Plugin_Handled;
}


public Action TF2_CalcIsAttackCritical(client, weapon, char weaponname[], bool & result)
{
    if (client > 0 && weapon > 0)
    {
        PrintToChat(client, "Attack with %d:%s", weapon, weaponname);
    }
    return Plugin_Continue;
}
