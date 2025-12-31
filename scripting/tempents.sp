/**
 * vim: set ai et ts=4 sw=4 :
 */

#include <sourcemod>
#include <sdktools>

#define TE_VERSION "$Revision$"

// SDK Handles
Handle hGameConf;
Handle hEyePosition;
Handle hEyeAngles;

public Plugin myinfo = 
{
    name = "TempEnts Tools",
    author = "Naris",
    description = "Add command to add temp ents",
    version = TE_VERSION,
    url = "http://sourcemod.net/"
};       

public OnPluginStart()
{
    CreateConVar("sm_te_tools", TE_VERSION, "Temp Ents tools", FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_REPLICATED|FCVAR_NOTIFY);

    hGameConf=LoadGameConfigFile("plugin.hgrsource"); // Game configuration file

    // EyePosition SDK call
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(hGameConf,SDKConf_Virtual,"EyePosition");
    PrepSDKCall_SetReturnInfo(SDKType_QAngle,SDKPass_ByValue);
    hEyePosition=EndPrepSDKCall();

    // EyeAngles SDK call
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(hGameConf,SDKConf_Virtual,"EyeAngles");
    PrepSDKCall_SetReturnInfo(SDKType_QAngle,SDKPass_ByValue);
    hEyeAngles=EndPrepSDKCall();

}

public bool AskPluginLoad(Handle myself, bool late, char Error[])
{
    //RegConsoleCmd("TE_Effect", Effect);

    RegConsoleCmd("TE_Sparks",          Sparks,         "Sets up a sparks effect.");
    RegConsoleCmd("TE_Smoke",           Smoke,          "Sets up a smoke effect.");
    RegConsoleCmd("TE_Dust",            Dust,           "Sets up a dust cloud effect.");
    RegConsoleCmd("TE_MuzzleFlash",     MuzzleFlash,    "Sets up a muzzle flash effect.");
    RegConsoleCmd("TE_MetalSparks",     MetalSparks,    "Sets up a metal sparks effect");
    RegConsoleCmd("TE_EnergySplash",    EnergySplash,   "Sets up an energy splash effect.");
    RegConsoleCmd("TE_ArmorRicochet",   ArmorRicochet,  "Sets up an armor ricochet effect.");
    RegConsoleCmd("TE_GlowSprite",      GlowSprite,     "Sets up a glowing sprite effect.");
    RegConsoleCmd("TE_Explosion",       Explosion,      "Sets up a explosion effect.");
    RegConsoleCmd("TE_BloodSprite",     BloodSprite,    "Sets up a blood sprite effect.");
    RegConsoleCmd("TE_BeamRingPoint",   BeamRingPoint,  "Sets up a beam ring point effect.");
    RegConsoleCmd("TE_BeamPoints",      BeamPoints,     "Sets up a point to point beam effect.");
    RegConsoleCmd("TE_BeamLaser",       BeamLaser,      "Sets up an entity to entity laser effect.");
    RegConsoleCmd("TE_BeamRing",        BeamRing,       "Sets up a beam ring effect.");
    RegConsoleCmd("TE_BeamFollow",      BeamFollow,     "Sets up a follow beam effect.");

    return true;
}

stock GetPos(client, float pos[3])
{
    float clientloc[3],float clientang[3];
    GetEyePosition(client,clientloc); // Get the position of the player's eyes
    GetAngles(client,clientang); // Get the angle the player is looking
    TR_TraceRayFilter(clientloc,clientang,MASK_SOLID,RayType_Infinite,TraceRayTryToHit); // Create a ray that tells where the player is looking
    TR_GetEndPosition(pos); // Get the end xyz coordinate of where a player is looking
}

public GetAngles(client,float output[3])
{
  SDKCall(hEyeAngles,client,output);
}

public GetEyePosition(client,float output[3])
{
  SDKCall(hEyePosition,client,output);
}

public bool TraceRayTryToHit(entity,mask)
{
  if(entity>0&&entity<=64) // Check if the beam hit a player and tell it to keep tracing if it did
    return false;
  return true;
}

public Action Sparks(client,args)
{
    char buf[256];

    if (args == 5)
    {
        float pos[3];
        GetPos(client, pos);

        float dir[3];
        GetCmdArg(1, buf, 255);
        dir[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        dir[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        dir[2] = StringToFloat(buf);

        GetCmdArg(4, buf, 255);
        int magnitude   = StringToInt(buf);
        GetCmdArg(5, buf, 255);
        int trailLength = StringToInt(buf);

        TE_SetupSparks(pos, dir, magnitude, trailLength);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupSparks(const float pos[3], const float dir[3], Magnitude, TrailLength)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos            Position of the sparks (set automatically).");
        PrintToConsole(client, " dir            Direction of the sparks.");
        PrintToConsole(client, " Magnitude      Sparks size.");
        PrintToConsole(client, " TrailLength    Trail lenght of the sparks.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a sparks effect.");
    }
    return Plugin_Handled;
}

public Action Smoke(client,args)
{
    char buf[256];

    if (args == 3)
    {
        float pos[3];
        GetPos(client, pos);

        GetCmdArg(1, buf, 255); 
        int model       = StringToInt(buf);
        GetCmdArg(2, buf, 255);
        float scale = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        int framerate   = StringToInt(buf);

        TE_SetupSmoke(pos, model, scale, framerate);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupSmoke(const float pos[3], Model, float Scale, FrameRate)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos			Position of the smoke (set automatically).");
        PrintToConsole(client, " Model			Precached model index.");
        PrintToConsole(client, " Scale			Scale of the smoke.");
        PrintToConsole(client, " Framerate		Frame rate of the smoke.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a smoke effect.");
    }
    return Plugin_Handled;
}

public Action Dust(client,args)
{
    char buf[256];

    if (args == 5)
    {
        float pos[3];
        GetPos(client, pos);

        float dir[3];
        GetCmdArg(1, buf, 255);
        dir[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        dir[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        dir[2] = StringToFloat(buf);

        GetCmdArg(4, buf, 255); 
        float size  = StringToFloat(buf);
        GetCmdArg(5, buf, 255);
        float speed = StringToFloat(buf);

        TE_SetupDust(pos, dir, size, speed);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupDust(const float pos[3], const float dir[3], float Size, float Speed)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos			Position of the dust (set automatically).");
        PrintToConsole(client, " dir			Direction of the dust.");
        PrintToConsole(client, " Size			Dust cloud size.");
        PrintToConsole(client, " Speed			Dust cloud speed.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a dust cloud effect."); 

    }
    return Plugin_Handled;
}


public Action MuzzleFlash(client,args)
{
    char buf[256];

    if (args == 5)
    {
        float pos[3];
        GetPos(client, pos);

        float angles[3];
        GetCmdArg(1, buf, 255);
        angles[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        angles[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        angles[2] = StringToFloat(buf);

        GetCmdArg(4, buf, 255); 
        float scale = StringToFloat(buf);
        GetCmdArg(5, buf, 255);
        int type = StringToInt(buf);

        TE_SetupMuzzleFlash(pos, angles, scale, type);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupMuzzleFlash(const float pos[3], const float angles[3], float Scale, Type)"); 
        PrintToConsole(client, ""); 
        PrintToConsole(client, "Usage:"); 
        PrintToConsole(client, ""); 
        PrintToConsole(client, " pos    Position of the muzzle flash (set automatically)."); 
        PrintToConsole(client, " angles Rotation angles of the muzzle flash."); 
        PrintToConsole(client, " Scale  Scale of the muzzle flash."); 
        PrintToConsole(client, " Type   Muzzle flash type to render (Mod specific)."); 
        PrintToConsole(client, ""); 
        PrintToConsole(client, "Notes:"); 
        PrintToConsole(client, " Sets up a muzzle flash effect."); 
    }
    return Plugin_Handled;
}

public Action MetalSparks(client,args)
{
    char buf[256];

    if (args == 3)
    {
        float pos[3];
        GetPos(client, pos);

        float dir[3];
        GetCmdArg(1, buf, 255);
        dir[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        dir[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        dir[2] = StringToFloat(buf);

        TE_SetupMetalSparks(pos, dir);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupMetalSparks(const float pos[3], const float dir[3])");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos    Position of the metal sparks (set automatically).");
        PrintToConsole(client, " dir    Direction of the metal sparks.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a metal sparks effect.");
    }
    return Plugin_Handled;
}

public Action EnergySplash(client,args)
{
    char buf[256];

    if (args == 4)
    {
        float pos[3];
        GetPos(client, pos);

        float dir[3];
        GetCmdArg(1, buf, 255);
        dir[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        dir[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        dir[2] = StringToFloat(buf);

        GetCmdArg(4, buf, 255);
        bool explosive = (StringToInt(buf) != 0);

        TE_SetupEnergySplash(pos, dir, explosive);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupEnergySplash(const float pos[3], const float dir[3], bool Explosive)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos			Position of the energy splash (set automatically).");
        PrintToConsole(client, " dir			Direction of the energy splash.");
        PrintToConsole(client, " Explosive		Makes the effect explosive.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up an energy splash effect.");
    }
    return Plugin_Handled;
}

public Action ArmorRicochet(client,args)
{
    char buf[256];

    if (args == 3)
    {
        float pos[3];
        GetPos(client, pos);

        float dir[3];
        GetCmdArg(1, buf, 255);
        dir[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        dir[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        dir[2] = StringToFloat(buf);

        TE_SetupArmorRicochet(pos, dir);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupArmorRicochet(const float pos[3], const float dir[3])");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos			Position of the armor ricochet (set automatically).");
        PrintToConsole(client, " dir			Directon of the armor ricochet.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up an armor ricochet effect.");
    }
    return Plugin_Handled;
}

public Action GlowSprite(client,args)
{
    char buf[256];

    if (args == 6)
    {
        float pos[3];
        GetPos(client, pos);

        GetCmdArg(3, buf, 255);
        int model = StringToInt(buf);

        GetCmdArg(4, buf, 255); 
        float life  = StringToFloat(buf);

        GetCmdArg(5, buf, 255); 
        float size  = StringToFloat(buf);

        GetCmdArg(6, buf, 255);
        int brightness = StringToInt(buf);

        TE_SetupGlowSprite(pos, model, life, size, brightness);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupGlowSprite(const float pos[3], Model, float Life, float Size, Brightness)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos			Position of the sprite (set automatically).");
        PrintToConsole(client, " Model			Precached model index.");
        PrintToConsole(client, " Life			Time duration of the sprite.");
        PrintToConsole(client, " Size			Sprite size.");
        PrintToConsole(client, " Brightness		Sprite brightness.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a glowing sprite effect.");
    }
    return Plugin_Handled;
}

public Action Explosion(client,args)
{
    char buf[256];

    if (args == 8 || (args >= 11 && args <= 12))
    {
        float pos[3];
        GetPos(client, pos);

        GetCmdArg(3, buf, 255);
        int model = StringToInt(buf);

        GetCmdArg(4, buf, 255); 
        float scale = StringToFloat(buf);

        GetCmdArg(5, buf, 255);
        int framerate = StringToInt(buf);

        GetCmdArg(6, buf, 255);
        int flags = StringToInt(buf);

        GetCmdArg(7, buf, 255);
        int radius = StringToInt(buf);

        GetCmdArg(8, buf, 255);
        int magnitude = StringToInt(buf);

        if (args > 8)
        {
            float normal[3];
            GetCmdArg(9, buf, 255);
            normal[0] = StringToFloat(buf);
            GetCmdArg(10, buf, 255);
            normal[1] = StringToFloat(buf);
            GetCmdArg(11, buf, 255);
            normal[2] = StringToFloat(buf);

            if (args > 11)
            {
                GetCmdArg(12, buf, 255);
                int materialType = buf[0];
                TE_SetupExplosion(pos, model, scale, framerate, flags, radius, magnitude, normal, materialType);
            }
            else
                TE_SetupExplosion(pos, model, scale, framerate, flags, radius, magnitude, normal);

        }
        else
            TE_SetupExplosion(pos, model, scale, framerate, flags, radius, magnitude);

        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupExplosion(const float pos[3], Model, float Scale, Framerate, Flags, Radius, Magnitude, const float normal[3]={0.0, 0.0, 1.0}, MaterialType='C')");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos			Explosion position (set automatically).");
        PrintToConsole(client, " Model			Precached model index.");
        PrintToConsole(client, " Scale			Explosion scale.");
        PrintToConsole(client, " Framerate		Explosion frame rate.");
        PrintToConsole(client, " Flags			Explosion flags.");
        PrintToConsole(client, " Radius		Explosion radius.");
        PrintToConsole(client, " Magnitude		Explosion size.");
        PrintToConsole(client, " normal		Normal vector to the explosion.");
        PrintToConsole(client, " MaterialType		Exploded material type.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a explosion effect.");
    }
    return Plugin_Handled;
}

public Action BloodSprite(client,args)
{
    char buf[256];

    if (args == 10)
    {
        float pos[3];
        GetPos(client, pos);

        float dir[3];
        GetCmdArg(1, buf, 255);
        dir[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        dir[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        dir[2] = StringToFloat(buf);

        int color[4];
        GetCmdArg(4, buf, 255);
        color[0] = StringToInt(buf);
        GetCmdArg(5, buf, 255);
        color[1] = StringToInt(buf);
        GetCmdArg(6, buf, 255);
        color[2] = StringToInt(buf);
        GetCmdArg(7, buf, 255);
        color[3] = StringToInt(buf);

        GetCmdArg(8, buf, 255); 
        int size  = StringToInt(buf);

        GetCmdArg(9, buf, 255);
        int sprayModel = StringToInt(buf);

        GetCmdArg(10, buf, 255);
        int bloodDripModel = StringToInt(buf);

        TE_SetupBloodSprite(pos, dir, color, size, sprayModel, bloodDripModel);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupBloodSprite(const float pos[3], const float dir[3], const color[4], Size, SprayModel, BloodDropModel)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " pos			Position of the sprite (set automatically).");
        PrintToConsole(client, " dir			Sprite direction.");
        PrintToConsole(client, " color			Color array (r, g, b, a).");
        PrintToConsole(client, " Size			Sprite size.");
        PrintToConsole(client, " SprayModel		Precached model index.");
        PrintToConsole(client, " BloodDropModel	Precached model index.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a blood sprite effect.");
    }
    return Plugin_Handled;
}

public Action BeamRingPoint(client,args)
{
    char buf[256];

    if (args == 15)
    {
        float center[3];
        GetPos(client, center);

        GetCmdArg(1, buf, 255);
        float startRadius = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        float endRadius = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        int modelIndex  = StringToInt(buf);
        GetCmdArg(4, buf, 255); 
        int haloIndex  = StringToInt(buf);
        GetCmdArg(5, buf, 255); 
        int startFrame = StringToInt(buf);
        GetCmdArg(6, buf, 255); 
        int frameRate = StringToInt(buf);
        GetCmdArg(7, buf, 255);
        float life = StringToFloat(buf);
        GetCmdArg(8, buf, 255);
        float width = StringToFloat(buf);
        GetCmdArg(9, buf, 255);
        float amplitude = StringToFloat(buf);

        int color[4];
        GetCmdArg(10, buf, 255);
        color[0] = StringToInt(buf);
        GetCmdArg(11, buf, 255);
        color[1] = StringToInt(buf);
        GetCmdArg(12, buf, 255);
        color[2] = StringToInt(buf);
        GetCmdArg(13, buf, 255);
        color[3] = StringToInt(buf);

        GetCmdArg(14, buf, 255); 
        int speed  = StringToInt(buf);

        GetCmdArg(15, buf, 255);
        int flags = StringToInt(buf);

        TE_SetupBeamRingPoint(center, startRadius, endRadius, modelIndex, haloIndex, startFrame, frameRate, life, width, amplitude, color, speed, flags);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupBeamRingPoint(const float center[3], float Start_Radius, float End_Radius, ModelIndex, HaloIndex, StartFrame, ");
        PrintToConsole(client, "                            FrameRate, float Life, float Width, float Amplitude, const Color[4], Speed, Flags)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " center		    Center position of the ring.");
        PrintToConsole(client, " Start_Radius	Initial ring radius.");
        PrintToConsole(client, " End_Radius		Final ring radius.");
        PrintToConsole(client, " ModelIndex		Precached model index.");
        PrintToConsole(client, " HaloIndex		Precached model index.");
        PrintToConsole(client, " StartFrame		Initital frame to render.");
        PrintToConsole(client, " FrameRate		Ring frame rate.");
        PrintToConsole(client, " Life			Time duration of the ring.");
        PrintToConsole(client, " Width			Beam width.");
        PrintToConsole(client, " Amplitude		Beam amplitude.");
        PrintToConsole(client, " color			Color array (r, g, b, a).");
        PrintToConsole(client, " Speed			Speed of the beam.");
        PrintToConsole(client, " Flags			Beam flags.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a beam ring point effect.");
    }
    return Plugin_Handled;
}

public Action BeamPoints(client,args)
{
    char buf[256];

    if (args == 17)
    {
        float start[3];
        GetPos(client, start);

        float end[3];
        GetCmdArg(1, buf, 255);
        end[0] = StringToFloat(buf);
        GetCmdArg(2, buf, 255);
        end[1] = StringToFloat(buf);
        GetCmdArg(3, buf, 255);
        end[2] = StringToFloat(buf);

        GetCmdArg(4, buf, 255);
        int modelIndex  = StringToInt(buf);
        GetCmdArg(5, buf, 255); 
        int haloIndex  = StringToInt(buf);
        GetCmdArg(6, buf, 255); 
        int startFrame = StringToInt(buf);
        GetCmdArg(7, buf, 255); 
        int frameRate = StringToInt(buf);
        GetCmdArg(8, buf, 255);
        float life = StringToFloat(buf);

        GetCmdArg(9, buf, 255);
        float width = StringToFloat(buf);
        GetCmdArg(10, buf, 255);
        float endWidth= StringToFloat(buf);
        GetCmdArg(11, buf, 255); 
        int fadeLength = StringToInt(buf);
        GetCmdArg(12, buf, 255);
        float amplitude = StringToFloat(buf);

        int color[4];
        GetCmdArg(13, buf, 255);
        color[0] = StringToInt(buf);
        GetCmdArg(14, buf, 255);
        color[1] = StringToInt(buf);
        GetCmdArg(15, buf, 255);
        color[2] = StringToInt(buf);
        GetCmdArg(16, buf, 255);
        color[3] = StringToInt(buf);

        GetCmdArg(17, buf, 255);
        int speed = StringToInt(buf);

        TE_SetupBeamPoints(start, end, modelIndex, haloIndex, startFrame, frameRate, life, width, endWidth, fadeLength, amplitude, color, speed);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupBeamPoints(const float start[3], const float end[3], ModelIndex, HaloIndex, StartFrame, FrameRate, float Life,"); 
        PrintToConsole(client, "                         float Width, float EndWidth, FadeLength, float Amplitude, const Color[4], Speed)");

        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " start			Start position of the beam.");
        PrintToConsole(client, " end			End position of the beam.");
        PrintToConsole(client, " ModelIndex		Precached model index.");
        PrintToConsole(client, " HaloIndex		Precached model index.");
        PrintToConsole(client, " StartFrame		Initital frame to render.");
        PrintToConsole(client, " FrameRate		Beam frame rate.");
        PrintToConsole(client, " Life			Time duration of the beam.");
        PrintToConsole(client, " Width			Initial beam width.");
        PrintToConsole(client, " EndWidth		Final beam width.");
        PrintToConsole(client, " FadeLength		Beam fade time duration.");
        PrintToConsole(client, " Amplitude		Beam amplitude.");
        PrintToConsole(client, " color			Color array (r, g, b, a).");
        PrintToConsole(client, " Speed			Speed of the beam.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a point to point beam effect.");
    }
    return Plugin_Handled;
}

public Action BeamLaser(client,args)
{
    char buf[256];

    if (args == 17)
    {
        GetCmdArg(1, buf, 255);
        int endEntity = StringToInt(buf);

        GetCmdArg(2, buf, 255);
        int modelIndex  = StringToInt(buf);
        GetCmdArg(3, buf, 255); 
        int haloIndex  = StringToInt(buf);
        GetCmdArg(4, buf, 255); 
        int startFrame = StringToInt(buf);
        GetCmdArg(5, buf, 255); 
        int frameRate = StringToInt(buf);
        GetCmdArg(6, buf, 255);
        float life = StringToFloat(buf);
        GetCmdArg(9, buf, 255);
        float width = StringToFloat(buf);
        GetCmdArg(10, buf, 255);
        float endWidth= StringToFloat(buf);
        GetCmdArg(11, buf, 255); 
        int fadeLength = StringToInt(buf);
        GetCmdArg(12, buf, 255);
        float amplitude = StringToFloat(buf);

        int color[4];
        GetCmdArg(13, buf, 255);
        color[0] = StringToInt(buf);
        GetCmdArg(14, buf, 255);
        color[1] = StringToInt(buf);
        GetCmdArg(15, buf, 255);
        color[2] = StringToInt(buf);
        GetCmdArg(16, buf, 255);
        color[3] = StringToInt(buf);

        GetCmdArg(17, buf, 255);
        int speed = StringToInt(buf);

        TE_SetupBeamLaser(client, endEntity, modelIndex, haloIndex, startFrame, frameRate, life, width, endWidth, fadeLength, amplitude, color, speed);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupBeamLaser(StartEntity, EndEntity, ModelIndex, HaloIndex, StartFrame, FrameRate, float Life,"); 
        PrintToConsole(client, "                        float Width, float EndWidth, FadeLength, float Amplitude, const Color[4], Speed)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " StartEntity		Entity index from where the beam starts.");
        PrintToConsole(client, " EndEntity		Entity index from where the beam ends.");
        PrintToConsole(client, " ModelIndex		Precached model index.");
        PrintToConsole(client, " HaloIndex		Precached model index.");
        PrintToConsole(client, " StartFrame		Initital frame to render.");
        PrintToConsole(client, " FrameRate		Beam frame rate.");
        PrintToConsole(client, " Life			Time duration of the beam.");
        PrintToConsole(client, " Width			Initial beam width.");
        PrintToConsole(client, " EndWidth		Final beam width.");
        PrintToConsole(client, " FadeLength		Beam fade time duration.");
        PrintToConsole(client, " Amplitude		Beam amplitude.");
        PrintToConsole(client, " color			Color array (r, g, b, a).");
        PrintToConsole(client, " Speed			Speed of the beam.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up an entity to entity laser effect.");
    }
    return Plugin_Handled;
}

public Action BeamRing(client,args)
{
    char buf[256];

    if (args == 14)
    {
        GetCmdArg(1, buf, 255);
        int endEntity = StringToInt(buf);

        GetCmdArg(2, buf, 255);
        int modelIndex  = StringToInt(buf);
        GetCmdArg(3, buf, 255); 
        int haloIndex  = StringToInt(buf);
        GetCmdArg(4, buf, 255); 
        int startFrame = StringToInt(buf);
        GetCmdArg(5, buf, 255); 
        int frameRate = StringToInt(buf);
        GetCmdArg(6, buf, 255);
        float life = StringToFloat(buf);
        GetCmdArg(7, buf, 255);
        float width = StringToFloat(buf);
        GetCmdArg(8, buf, 255);
        float amplitude = StringToFloat(buf);

        int color[4];
        GetCmdArg(9, buf, 255);
        color[0] = StringToInt(buf);
        GetCmdArg(10, buf, 255);
        color[1] = StringToInt(buf);
        GetCmdArg(11, buf, 255);
        color[2] = StringToInt(buf);
        GetCmdArg(12, buf, 255);
        color[3] = StringToInt(buf);

        GetCmdArg(13, buf, 255); 
        int speed  = StringToInt(buf);

        GetCmdArg(14, buf, 255);
        int flags = StringToInt(buf);

        TE_SetupBeamRing(client, endEntity, modelIndex, haloIndex, startFrame, frameRate, life, width, amplitude, color, speed, flags);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupBeamRing(StartEntity, EndEntity, ModelIndex, HaloIndex, StartFrame, FrameRate, float Life, float Width, float Amplitude, const Color[4], Speed, Flags)");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " StartEntity	Entity index from where the ring starts.");
        PrintToConsole(client, " EndEntity		Entity index from where the ring ends.");
        PrintToConsole(client, " ModelIndex		Precached model index.");
        PrintToConsole(client, " HaloIndex		Precached model index.");
        PrintToConsole(client, " StartFrame		Initital frame to render.");
        PrintToConsole(client, " FrameRate		Ring frame rate.");
        PrintToConsole(client, " Life			Time duration of the ring.");
        PrintToConsole(client, " Width			Beam width.");
        PrintToConsole(client, " Amplitude		Beam amplitude.");
        PrintToConsole(client, " color			Color array (r, g, b, a).");
        PrintToConsole(client, " Speed			Speed of the beam.");
        PrintToConsole(client, " Flags			Beam flags.");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a beam ring effect.");
    }
    return Plugin_Handled;
}


public Action BeamFollow(client,args)
{
    char buf[256];

    if (args == 12)
    {
        GetCmdArg(2, buf, 255);
        int modelIndex  = StringToInt(buf);
        GetCmdArg(3, buf, 255); 
        int haloIndex  = StringToInt(buf);
        GetCmdArg(4, buf, 255); 
        float life = StringToFloat(buf);
        GetCmdArg(5, buf, 255);
        float width = StringToFloat(buf);
        GetCmdArg(6, buf, 255);
        float endWidth = StringToFloat(buf);
        GetCmdArg(7, buf, 255); 
        int fadeLength = StringToInt(buf);

        int color[4];
        GetCmdArg(7, buf, 255);
        color[0] = StringToInt(buf);
        GetCmdArg(8, buf, 255);
        color[1] = StringToInt(buf);
        GetCmdArg(9, buf, 255);
        color[2] = StringToInt(buf);
        GetCmdArg(10, buf, 255);
        color[3] = StringToInt(buf);

        TE_SetupBeamFollow(client, modelIndex, haloIndex, life, width, endWidth, fadeLength, color);
        TE_SendToAll();
    }
    else
    {
        PrintToConsole(client, "stock TE_SetupBeamFollow(EntIndex, ModelIndex, HaloIndex, float Life, float Width, float EndWidth, FadeLength, const Color[4])");
        PrintToConsole(client, "");
        PrintToConsole(client, "Usage:");
        PrintToConsole(client, "");
        PrintToConsole(client, " EntIndex		Entity index from where the beam starts.");
        PrintToConsole(client, " ModelIndex		Precached model index.");
        PrintToConsole(client, " HaloIndex		Precached model index.");
        PrintToConsole(client, " Life			Time duration of the beam.");
        PrintToConsole(client, " Width			Initial beam width.");
        PrintToConsole(client, " EndWidth		Final beam width.");
        PrintToConsole(client, " FadeLength		Beam fade time duration.");
        PrintToConsole(client, " color			Color array (r, g, b, a).");
        PrintToConsole(client, "");
        PrintToConsole(client, "Notes:");
        PrintToConsole(client, " Sets up a follow beam effect.");
    }
    return Plugin_Handled;
}

