/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: playertracking.inc
 * Description: More storage for players, temporary and long term.
 * Author(s): -=|JFH|=-Naris (Murray Wilson)
 */

#if defined _player_tracking_included
 #endinput
#endif
#define _player_tracking_included

// Global definition
Handle g_OnUpgradeLevelChangedHandle = null;
Handle g_OnRaceDeselectedHandle = null;
Handle g_OnRaceSelectedHandle = null;

GetOwnsItem(client,item)
{
    TraceInto("PlayerTracking", "GetOwnsItem", "client=%d:%N, item=%d", \
              client, ValidClientIndex(client), item);

    new owns  = 0;
    new count = GetShopItemCount();
    if (count >= 1 && item < count)
    {
        Handle shopitems=GetShopitems(client);
        if (shopitems != null &&
            item < GetArraySize(shopitems))
        {
            owns = GetArrayCell(shopitems,item);
        }
        else
        {
            Trace("Unable to determine if client=%d:%N owns item=%d; shopitems=0x%08x, size=%d", \
                  client, ValidClientIndex(client), item, shopitems, \
                  (shopitems != null) ? GetArraySize(shopitems) : 0);
        }
    }

    TraceReturn("owns=%d", owns);
    return owns;
}

SetOwnsItem(client,item,owns)
{
    TraceInto("PlayerTracking", "SetOwnsItem", "client=%d:%N, item=%d, owns=%d", \
              client, ValidClientIndex(client), item, owns);

    if (GetShopItemCount() >= 1)
    {
        Handle shopitems=GetShopitems(client);
        if (shopitems != null &&
            item < GetArraySize(shopitems))
        {
            SetArrayCell(shopitems,item,owns);
        }
        else
        {
            char itemName[NAME_STRING_LENGTH];
            Handle itemHandle = GetItemIndexHandle(item);
            GetItemName(itemHandle, itemName, sizeof(itemName));
            LogError("Unable to set that client=%d:%N owns %d of item=%d:%s; shopitems=0x%08x, size=%d",
                     client, ValidClientIndex(client), owns, item, itemName, shopitems,
                     (shopitems != null) ? GetArraySize(shopitems) : 0);
        }
    }

    TraceReturn();
}

GetXP(client,race)
{
    TraceInto("PlayerTracking", "GetXP", "client=%d:%N, race=%d", \
              client, ValidClientIndex(client), race);

    new xp = 0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
            xp = GetRaceInfoXP(raceInfo);
    }

    TraceReturn("Get %d:%N's race %d XP=%d", \
                client, ValidClientIndex(client), \
                race, xp);

    return xp;
}

SetXP(client,race,xp)
{
    TraceInto("PlayerTracking", "SetXP", "client=%d:%N, race=%d, xp=%d", \
              client, ValidClientIndex(client), race, xp);

    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            Trace("Set %d:%N's race %d XP=%d", \
                  client, ValidClientIndex(client), \
                  race, xp);

            SetRaceInfoXP(raceInfo,xp);
            SetDatabaseSaved(client, false);
        }
    }

    TraceReturn();
}

ResetXP(client,race,xp)
{
    TraceInto("PlayerTracking", "ResetXP", "client=%d:%N, race=%d, xp=%d", \
              client, ValidClientIndex(client), race, xp);

    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            Trace("Reset %d:%N's race %d XP=%d", \
                  client, ValidClientIndex(client), \
                  race, xp);

            SetRaceInfoXP(raceInfo,xp);
            SetDatabaseSaved(client, false);

            Handle raceHandle=GetRaceHandle(race);
            if (raceHandle != null)
            {
                if (GetRaceInfoLevel(raceInfo) < GetRaceMaxLevel(raceHandle))
                    DoLevelCheck(client);
            }
        }
    }

    TraceReturn();
}

GetLevel(client,race)
{
    TraceInto("PlayerTracking", "GetLevel", "client=%d:%N, race=%d", \
              client, ValidClientIndex(client), race);

    new level = 0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfoHandle(client, race); // Debug
        if (raceInfo != null)
            level = GetRaceInfoLevel(raceInfo);
    }

    TraceReturn("Get %d:%N's race %d level=%d", \
                client, ValidClientIndex(client), \
                race, level);

    return level;
}

SetLevel(client,race,level,bool update=false)
{
    TraceInto("PlayerTracking", "SetLevel", "client=%d:%N, race=%d, level=%d", \
              client, ValidClientIndex(client), race, level);

    new delta = 0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new oldLevel = GetRaceInfoLevel(raceInfo);

            Trace("Set %d:%N's race %d level=%d", \
                  client, ValidClientIndex(client), \
                  race, level);

            delta = level-oldLevel;
            if (delta != 0)
            {
                SetRaceInfoLevel(raceInfo,level);
                SetDatabaseSaved(client, false);

                if (update)
                {
                    new oLevel = GetOverallLevel(client);
                    SetOverallLevel(client,oLevel+delta);

                    Handle raceHandle=GetRaceHandle(race);
                    if (raceHandle != null)
                    {
                        Faction techId = GetRaceFaction(raceHandle);
                        if (techId > Generic)
                        {
                            IncrementTechLevel(client,techId,delta);
                            if (oldLevel <= 0 && level > 0)
                                IncrementTechCount(client,techId,1);
                            else if (oldLevel > 0 && level <= 0)
                                IncrementTechCount(client,techId,-1);
                        }
                    }
                }
            }
        }
    }

    TraceReturn("delta=%d", delta);
    return delta;
}

ResetLevel(client,race,new_level)
{
    TraceInto("PlayerTracking", "ResetLevel", "client=%d:%N, race=%d, new_level=%d", \
              client, ValidClientIndex(client), race, new_level);

    new delta = SetLevel(client,race,new_level,true);
    if (delta < 0)
        ResetUpgradesNow(client, race, true);
    else if (delta > 0 && GetDatabaseLoaded(client) != DataLoading)
        UpgradeMenu(client, UpgradeMode, false);

    TraceReturn();
}

float GetAccumulatedEnergy(client,race)
{
    TraceInto("PlayerTracking", "GetAccumulatedEnergy", "client=%d:%N, race=%d", \
              client, ValidClientIndex(client), race);

    float accumulated_energy=0.0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
            accumulated_energy = GetRaceInfoAccumulatedEnergy(raceInfo);
    }

    TraceReturn("accumulated_energy=%d", accumulated_energy);
    return accumulated_energy;
}

SetAccumulatedEnergy(client,race,float energy)
{
    TraceInto("PlayerTracking", "SetAccumulatedEnergy", "client=%d:%N, race=%d, energy=%d", \
              client, ValidClientIndex(client), race, energy);

    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
            SetRaceInfoAccumulatedEnergy(raceInfo,energy);
    }

    TraceReturn();
}

IncrementAccumulatedEnergy(client,race,float amount)
{
    TraceInto("PlayerTracking", "IncrementAccumulatedEnergy", "client=%d:%N, race=%d, amount=%d", \
              client, ValidClientIndex(client), race, amount);

    if (race >= 0 && GetRaceCount() > 1)
    {
        Handle raceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            float energy = GetRaceInfoAccumulatedEnergy(raceInfo);
            SetRaceInfoAccumulatedEnergy(raceInfo,energy+amount);
        }
    }

    TraceReturn();
}

GetActiveUltimate(client,race,category)
{
    TraceInto("PlayerTracking", "GetActiveUltimate", "client=%d:%N, race=%d, category=%d", \
              client, ValidClientIndex(client), race, category);

    new upgrade=-1;
    if (category >= 1 && category <= MAX_ULTIMATE_CATEGORY &&
        race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new pos; // = RACEINFO_ACTIVE_ULTIMATE_1+category-1;
            switch (category)
            {
                case 1: pos=RACEINFO_ACTIVE_ULTIMATE_1;
                case 2: pos=RACEINFO_ACTIVE_ULTIMATE_2;
                case 3: pos=RACEINFO_ACTIVE_ULTIMATE_3;
                case 4: pos=RACEINFO_ACTIVE_ULTIMATE_4;
            }
            upgrade = GetArrayCell(raceInfo,pos);
        }
    }

    TraceReturn("upgrade=%d", upgrade);
    return upgrade;
}

stock SetActiveUltimate(client,race,category,upgrade)
{
    TraceInto("PlayerTracking", "SetActiveUltimate", "client=%d:%N, race=%d, category=%d, upgrade=%d", \
              client, ValidClientIndex(client), race, category, upgrade);

    if (category >= 1 && category <= MAX_ULTIMATE_CATEGORY &&
        race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new pos; // = RACEINFO_ACTIVE_ULTIMATE_1+category-1;
            switch (category)
            {
                case 1: pos=RACEINFO_ACTIVE_ULTIMATE_1;
                case 2: pos=RACEINFO_ACTIVE_ULTIMATE_2;
                case 3: pos=RACEINFO_ACTIVE_ULTIMATE_3;
                case 4: pos=RACEINFO_ACTIVE_ULTIMATE_4;
            }
            SetArrayCell(raceInfo,pos,upgrade);
        }
    }

    TraceReturn();
}

DataStatus:GetRaceLoaded(client,race)
{
    TraceInto("PlayerTracking", "GetRaceLoaded", "client=%d:%N, race=%d", \
              client, ValidClientIndex(client), race);

    new DataStatus:loaded = DataNotLoaded;
    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
            loaded = GetRaceInfoLoaded(raceInfo);
    }

    TraceReturn("loaded=%d", loaded);
    return loaded;
}

stock SetRaceLoaded(client,race,DataStatus:loaded)
{
    TraceInto("PlayerTracking", "SetRaceLoaded", "client=%d:%N, race=%d, loaded=%d", \
              client, ValidClientIndex(client), race, loaded);

    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
            SetRaceInfoLoaded(raceInfo,loaded);
    }

    TraceReturn();
}

stock GetSavedXP(client,race)
{
    TraceInto("PlayerTracking", "GetSavedXP", "client=%d:%N, race=%d", \
              client, ValidClientIndex(client), race);

    new xp = 0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
            xp = GetRaceInfoSavedXP(raceInfo);
    }

    TraceReturn("Get %d:%N's race %d SavedXP=%d", \
                client, ValidClientIndex(client), \
                race, xp);

    return xp;
}

stock SetSavedXP(client,race,xp)
{
    TraceInto("PlayerTracking", "SetSavedXP", "client=%d:%N, race=%d, xp=%d", \
              client, ValidClientIndex(client), race, xp);

    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            Trace("Set %d:%N's race %d SavedXP=%d", \
                  client, ValidClientIndex(client), \
                  race, xp);

            SetRaceInfoSavedXP(raceInfo,xp);
        }
    }

    TraceReturn();
}

GetUpgradeLevel(client,race,upgrade)
{
    TraceInto("PlayerTracking", "GetUpgradeLevel", "client=%d:%N, race=%d, upgrade=%d", \
              client, ValidClientIndex(client), race, upgrade);

    new level = 0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new upgradePos = GetRaceInfoUpgradePos(upgrade);
            new upgradeSize = GetArraySize(raceInfo);
            if (upgradePos < upgradeSize)
                level = GetArrayCell(raceInfo,upgradePos); // +UPGRADEINFO_LEVEL
            else
            {
                HandleraceHandle=GetRaceHandle(race);
                if (raceHandle != null)
                {
                    new upgradeCount = (upgradeSize-RACEINFO_SIZE)/UPGRADEINFO_SIZE;
                    charrname[64]; GetRaceName(raceHandle,rname,sizeof(rname));
                    LogError("GetUpgradeLevel of Invalid upgrade %d (pos %d) of %d(%d) for race %d(%s)",
                             upgrade, upgradePos, upgradeSize, upgradeCount, race, rname);

                    // For now, Tack on extra upgrade slot
                    PushArrayCell(raceInfo,0); // Upgrade level
                    PushArrayCell(raceInfo,0); // Upgrade level in DB
                }
            }
        }
    }

    TraceReturn("level=%d", level);
    return level;
}

SetUpgradeLevel(client,race,upgrade,level)
{
    TraceInto("PlayerTracking", "SetUpgradeLevel", "client=%d:%N, race=%d, upgrade=%d, level=%d", \
              client, ValidClientIndex(client), race, upgrade, level);

    if (race >= 0 && GetRaceCount() > 1 && IsClientInGame(client))
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new old = 0;
            new upgradePos = GetRaceInfoUpgradePos(upgrade);
            new upgradeSize = GetArraySize(raceInfo);
            if (upgradePos < GetArraySize(raceInfo))
            {
                old=GetArrayCell(raceInfo,upgradePos); // +UPGRADEINFO_LEVEL
                if (level != old)
                {
                    SetArrayCell(raceInfo,upgradePos,level);
                    SetDatabaseSaved(client, false);
                }
            }
            else
            {
                LogError("SetUpgradeLevel of Invalid upgrade %d (pos %d) for race %d to %d. size=%d",
                         upgrade, upgradePos, race, level, upgradeSize);

                while (upgradeSize < upgradePos)
                {
                    PushArrayCell(raceInfo,(upgradeSize >= upgradePos - 2) ? level : 0); // Upgrade level
                    PushArrayCell(raceInfo,0); // Upgrade level in DB
                    upgradeSize = GetArraySize(raceInfo);
                }
            }

            if (level != old)
            {
                new result;
                Call_StartForward(g_OnUpgradeLevelChangedHandle);
                Call_PushCell(client);
                Call_PushCell(race);
                Call_PushCell(upgrade);
                Call_PushCell(level);
                Call_Finish(result);
            }
        }
    }

    TraceReturn();
}

bool IncrementUpgradeLevel(client,race,upgrade,category,max)
{
    TraceInto("PlayerTracking", "IncrementUpgradeLevel", "client=%d:%N, race=%d, upgrade=%d, category=%d", \
              client, ValidClientIndex(client), race, upgrade, category);

    boolsuccess=false;
    if (race >= 0 && GetRaceCount() > 1 && IsClientInGame(client))
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new upgradePos = GetRaceInfoUpgradePos(upgrade);
            new old = GetArrayCell(raceInfo,upgradePos); // +UPGRADEINFO_LEVEL
            if (old < max)
            {
                new level=old+1;
                SetArrayCell(raceInfo,upgradePos,level);
                SetDatabaseSaved(client, false);
                success = true;

                if (category >= 1 && category <= MAX_ULTIMATE_CATEGORY)
                {
                    new pos; // = RACEINFO_ACTIVE_ULTIMATE_1+category-1;
                    switch (category)
                    {
                        case 1: pos=RACEINFO_ACTIVE_ULTIMATE_1;
                        case 2: pos=RACEINFO_ACTIVE_ULTIMATE_2;
                        case 3: pos=RACEINFO_ACTIVE_ULTIMATE_3;
                        case 4: pos=RACEINFO_ACTIVE_ULTIMATE_4;
                    }
                    SetArrayCell(raceInfo,pos,upgrade);
                }

                new result;
                Call_StartForward(g_OnUpgradeLevelChangedHandle);
                Call_PushCell(client);
                Call_PushCell(race);
                Call_PushCell(upgrade);
                Call_PushCell(level);
                Call_Finish(result);
            }
        }
    }

    TraceReturn("success=%d",success);
    return success;
}

LoadUpgradeLevel(client,race,upgrade,level)
{
    TraceInto("PlayerTracking", "LoadUpgradeLevel", "client=%d:%N, race=%d, upgrade=%d, level=%d", \
              client, ValidClientIndex(client), race, upgrade, level);

    if (g_bSaveUpgrades && race >= 0 && GetRaceCount() > 1 && IsClientInGame(client))
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new upgradePos = GetRaceInfoUpgradePos(upgrade);
            if (upgradePos >= GetArraySize(raceInfo))
            {
                HandleraceHandle=GetRaceHandle(race);
                if (raceHandle != null)
                {
                    new ident = GetRaceIdent(raceHandle);
                    charname[NAME_STRING_LENGTH]; GetRaceName(raceHandle,name,sizeof(name));
                    LogError("Unable to Set %d:%N's %d-%d:%s Upgrade %d Level=%d, upgradePos=%d of %d == FUBAR!",
                             client, ValidClientIndex(client), race, ident, name,
                             upgrade, level, upgradePos, GetArraySize(raceInfo));
                }
            }
            else
            {
                new old = GetArrayCell(raceInfo,upgradePos); // +UPGRADEINFO_LEVEL
                if (old != level)
                {
                    SetArrayCell(raceInfo,upgradePos,level); // +UPGRADEINFO_LEVEL
                    SetArrayCell(raceInfo,upgradePos+UPGRADEINFO_SAVED_LEVEL,level);

                    new result;
                    Call_StartForward(g_OnUpgradeLevelChangedHandle);
                    Call_PushCell(client);
                    Call_PushCell(race);
                    Call_PushCell(upgrade);
                    Call_PushCell(level);
                    Call_Finish(result);
                }
            }
        }
    }

    TraceReturn();
}

GetUpgradeLevelCount(client,race)
{
    TraceInto("PlayerTracking", "GetUpgradeLevelCount", "client=%d:%N, race=%d", \
              client, ValidClientIndex(client), race);

    new ret = 0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            HandleraceHandle = GetRaceHandle(race);
            if (raceHandle != null)
            {
                new upgradeCount = GetUpgradeCount(raceHandle);
                new last = GetRaceInfoUpgradePos(upgradeCount);
                for(new x = RACEINFO_SIZE; x < last; x += UPGRADEINFO_SIZE)
                    ret += GetArrayCell(raceInfo,x); // +UPGRADEINFO_LEVEL
            }
        }
    }

    TraceReturn("count=%d",ret);
    return ret;
}

stock GetSavedUpgradeLevel(client,race,upgrade)
{
    TraceInto("PlayerTracking", "GetSavedUpgradeLevel", "client=%d:%N(%d), race=%d, upgrade=%d", \
              ValidClientIndex(client), client, race, upgrade);

    new ret=0;
    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new upgradePos = GetRaceInfoUpgradePos(upgrade);
            ret = GetArrayCell(raceInfo,upgradePos+UPGRADEINFO_SAVED_LEVEL);
        }
    }

    TraceReturn("saved_level=%d",ret);
    return ret;
}

stock SetSavedUpgradeLevel(client,race,upgrade,level)
{
    TraceInto("PlayerTracking", "SetSavedUpgradeLevel", "client=%d:%N, race=%d, upgrade=%d,level=%d", \
              client, ValidClientIndex(client), race, upgrade, level);

    if (race >= 0 && GetRaceCount() > 1)
    {
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            new upgradePos = GetRaceInfoUpgradePos(upgrade);
            SetArrayCell(raceInfo,upgradePos+UPGRADEINFO_SAVED_LEVEL,level);
            SetDatabaseSaved(client, false);
        }
    }

    TraceReturn();
}

DataStatus:GetTechLoaded(client,Faction:techId)
{
    TraceInto("PlayerTracking", "GetTechLoaded", "client=%d:%N, techId=%d", \
              client, ValidClientIndex(client), techId);

    new DataStatus:loaded = DataNotLoaded;
    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
            loaded = GetTechInfoLoaded(techInfo);
    }

    TraceReturn("loaded=%d", loaded);
    return loaded;
}

stock SetTechLoaded(client,Faction:techId,loaded)
{
    TraceInto("PlayerTracking", "SetTechLoaded", "client=%d:%N, techId=%d, loaded=%d", \
              client, ValidClientIndex(client), techId, loaded);

    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
            SetTechInfoLoaded(techInfo,loaded);
    }

    TraceReturn();
}

stock GetTechCount(client,Faction:techId)
{
    TraceInto("PlayerTracking", "GetTechCount", "client=%d:%N, techId=%d", \
              client, ValidClientIndex(client), techId);

    new count = 0;
    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
            count = GetTechInfoCount(techInfo);
    }

    TraceReturn("Get %d:%N's tech %d count=%d", \
                client, ValidClientIndex(client), \
                techId, count);

    return count;
}

stock SetTechCount(client,Faction:techId, count)
{
    TraceInto("PlayerTracking", "GetTechCount", "client=%d:%N, techId=%d", \
              client, ValidClientIndex(client), techId);

    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
            SetTechInfoCount(techInfo, count);
    }

    TraceReturn();
}

IncrementTechCount(client,Faction:techId, amount)
{
    TraceInto("PlayerTracking", "GetTechCount", "client=%d:%N, techId=%d", \
              client, ValidClientIndex(client), techId);

    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
        {
            new count = GetTechInfoCount(techInfo);
            SetTechInfoCount(techInfo, count+amount);
        }
    }

    TraceReturn();
}

GetTechLevel(client,Faction:techId)
{
    TraceInto("PlayerTracking", "GetTechLevel", "client=%d:%N, techId=%d", \
              client, ValidClientIndex(client), techId);

    new level = 0;
    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
            level = GetTechInfoLevel(techInfo);
    }

    TraceReturn("Get %d:%N's tech %d level=%d", \
                client, ValidClientIndex(client), \
                techId, level);

    return level;
}

stock SetTechLevel(client,Faction:techId, level)
{
    TraceInto("PlayerTracking", "GetTechLevel", "client=%d:%N, techId=%d", \
              client, ValidClientIndex(client), techId);

    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
            SetTechInfoLevel(techInfo, level);
    }

    TraceReturn();
}

IncrementTechLevel(client,Faction:techId, amount)
{
    TraceInto("PlayerTracking", "GetTechLevel", "client=%d:%N, techId=%d", \
              client, ValidClientIndex(client), techId);

    if (techId >= Generic)
    {
        HandletechInfo=GetTechInfo(client, techId);
        if (techInfo != null)
        {
            new level = GetTechInfoLevel(techInfo);
            SetTechInfoLevel(techInfo, level+amount);
        }
    }

    TraceReturn();
}

bool SetRace(client, race, bool saveEnergy=false)
{
    TraceInto("PlayerTracking", "SetRace", "client=%d:%N, race=%d, saveEnergy=%d", \
              client, ValidClientIndex(client), race, saveEnergy);

    new oldrace = GetRace(client);
    if (race >= 0 && race != oldrace)
    {
        Trace("Set %d:%N's race=%d", \
              client,ValidClientIndex(client), \
              race);

        boolsaveEnabled = (g_bSaveXP && GetRaceCount() > 1 &&
                                GetPlayerStatus(client) != PlayerDisabled);

        if (oldrace > 0 && saveEnabled && !GetDatabaseSaved(client) &&
            GetDatabaseLoaded(client) >= DataOK &&
            GetRaceLoaded(client, oldrace) >= DataOK)
        {
            Trace("Saving Player Data, g_bDatabaseConnected=%d",g_bDatabaseConnected);
            if (g_bDatabaseConnected)
            {
                Trace("Save %d:%N's race=%d Data", \
                      client,ValidClientIndex(client), \
                      race);

                SavePlayerData(client);
            }
            else
            {
                LogError("Database not available to save %N's levels", client);
                PrintHintText(client, "%t", "NoDatabaseForSave");
            }
        }

        SetRaceValue(client, race);
        SetDatabaseSaved(client, false);

        if (saveEnabled && GetRaceLoaded(client,race) < DataOK)
        {
            if (g_bDatabaseConnected)
            {
                Trace("Load %d:%N's race=%d Data", \
                      client, ValidClientIndex(client), \
                      race);

                LoadPlayerRaceData(client, race);
            }
            else
            {
                LogError("Database not available to load %N's levels", client);
                PrintHintText(client, "%t", "NoDatabaseForLoad");
            }
        }

        if (oldrace > 0)
        {
            Trace("%d:%N (client=%d) Deselected/Changing from %d to %d", \
                  client, ValidClientIndex(client), client, oldrace, race);

            Call_StartForward(g_OnRaceDeselectedHandle);
            Call_PushCell(client);
            Call_PushCell(oldrace);
            Call_PushCell(race);

            Actionresult = Plugin_Continue;
            Call_Finish(result);
            if (result >= Plugin_Stop)
            {
                // Someone rejected the change, set the race back.
                SetRaceValue(client, oldrace);
                TraceReturn("%d:%N Deselect failed, Remaining a %d", \
                            client, ValidClientIndex(client), oldrace);
                return false;
            }
        }

        Trace("%d:%N Selected/Changing from %d to %d", \
              client, ValidClientIndex(client), oldrace, race);

        Call_StartForward(g_OnRaceSelectedHandle);
        Call_PushCell(client);
        Call_PushCell(oldrace);
        Call_PushCell(race);

        Actionresult = Plugin_Continue;
        Call_Finish(result);
        if (result >= Plugin_Stop)
        {
            // Someone rejected the change, set the race back.
            SetRaceValue(client, oldrace);
            TraceReturn("%d:%N Select failed, Remaining a %d", \
                        client, ValidClientIndex(client), oldrace);
            return false;
        }

        // Reset accumulated energy when changing races
        HandleraceInfo = GetRaceInfo(client, race);
        if (raceInfo != null)
        {
            floatenergy = (saveEnergy) ? GetEnergy(client) : 0.0;
            SetRaceInfoAccumulatedEnergy(raceInfo, energy);
        }

        // Set the energy to the initial amount of the new race.
        floatinitial = GetInitialEnergy(client);

        Trace("%d:%N's initial energy is %1.0f", \
              client, ValidClientIndex(client), initial);

        HandleraceHandle=GetRaceHandle(race);
        if (raceHandle != null && initial < 0.0)
        {
            initial = GetRaceInitialEnergy(raceHandle);

            #if defined _TRACE
                charrname[64]; GetRaceName(GetRaceHandle(race),rname,sizeof(rname));
                Trace("Race %d(0x%08x)'s initial energy is %1.0f", race, raceHandle, initial);
            #endif
        }

        if (initial < 0.0)
            initial = 0.0;

        SetEnergy(client, initial);

        Trace("Setting %d:%N's energy to %1.0f", \
              client, ValidClientIndex(client), initial);

        // Setup racial immunities (vulnerabilities)
        new UnitType:type = (raceHandle != null) ? GetRaceType(raceHandle) : GenericUnit;
        SetAttribute(client,Attribute_IsBiological, type == Biological || type == BioMechanical || type == Cybernetic || type == GenericUnit);
        SetAttribute(client,Attribute_IsMechanical, type == Mechanical || type == BioMechanical || type == Cybernetic);
        SetAttribute(client,Attribute_IsRobotic, type == Robotic || type == Cybernetic);
        SetAttribute(client,Attribute_IsEnergy, type == Energy);

        CooldownEvent(client, Cooldown_ResetOnChange);

        #if defined _TRACE
            // Ensure the race data was loaded!
            if (GetRaceLoaded(client,race) < DataOK)
            {
                new ident = GetDatabaseIdent(client);
                if (ident >= 0)
                {
                    Trace("%d:%N was found using ident %d, but their %d race data was either not found or not loaded.", \
                          client, ValidClientIndex(client), race, ident);
                }
                else
                {
                    Trace("%d:%N was not in the Database, so their %d race data was not loaded.", \
                          client, ValidClientIndex(client), race);
                }
            }
        #endif

        TraceReturn("%d:%N Changed from %d to %d", \
                    client, ValidClientIndex(client), oldrace, race);
        return true;
    }

    TraceReturn();
    return false;
}

Handle GetPropInfo(Handle propData, Handle plugin, int id)
{
    new size=GetArraySize(propData);
    for (new x=0;x<size;x++)
    {
        HandlepropInfo=GetArrayCell(propData,x);
        if (GetPropInfoPlugin(propInfo) == plugin &&
            GetPropInfoId(propInfo) == id)
        {
            return propInfo;
        }
    }
    return null;
}

Handle CreateProperties(client, Handle plugin, int id, float forceSpeed=-1.0, float forceGravity=-1.0,
                        int forceVisibility=-1, float speed=-1.0, float gravity=-1.0, int visibility=-1,
                        VisibilityType type=VisibilityType:-1, float factor1=-1.0, float factor2=-1.0,
                        RenderMode mode=RenderMode:-1, RenderFx fx=RenderFx:-1, int r=-1, int g=-1, int b=-1,
                        int weaponAlpha=-1,bool colorWeapons=false, VisibilityPriority priority=VisibilityPriorityNotSpecified,
                        float timestamp=0.0, Immunity immunities=Immunity_None, Restriction restrictions=Restriction_None,
                        MoveType mt=MOVETYPE_DEFAULT, float physicalArmorPercent=0.0, float magicalArmorPercent=0.0,
                        RegenFlags regenHealthFlags=Regen_Default, float regenHealth=0.0,
                        RegenFlags regenShieldFlags=Regen_Default, float regenShield=0.0)
{
    #if defined _TRACE
        SetTraceCategory("Array");
        TraceInto("PlayerTracking", "CreateProperties", "client=%d:%N, plugin=0x%08x,id=%d",
                  client, ValidClientIndex(client), plugin, id);
    #else
        #pragma unused client
    #endif

    HandlepHandle=CreateArray();
    PushArrayCell(pHandle, plugin);                 // plugin handle
    PushArrayCell(pHandle, id);                     // identifier
    PushArrayCell(pHandle, forceSpeed);             // force speed?
    PushArrayCell(pHandle, forceGravity);           // force gravity?
    PushArrayCell(pHandle, forceVisibility);        // force visibility?
    PushArrayCell(pHandle, speed);                  // speed
    PushArrayCell(pHandle, gravity);                // gravity
    PushArrayCell(pHandle, visibility);             // visibility 
    PushArrayCell(pHandle, type);                   // visibility type
    PushArrayCell(pHandle, factor1);                // visibility factor1 (melee/delay)
    PushArrayCell(pHandle, factor2);                // visibility factor2 (movement/duration)
    PushArrayCell(pHandle, mode);                   // visibility render mode
    PushArrayCell(pHandle, fx);                     // visibility render fx
    PushArrayCell(pHandle, r);                      // visibility red component
    PushArrayCell(pHandle, g);                      // visibility green component
    PushArrayCell(pHandle, b);                      // visibility blue component
    PushArrayCell(pHandle, weaponAlpha);            // visibility weapon alpha
    PushArrayCell(pHandle, colorWeapons);           // visibility color weapons flag
    PushArrayCell(pHandle, priority);               // visibility priority
    PushArrayCell(pHandle, timestamp);              // visibility timestamp
    PushArrayCell(pHandle, immunities);             // immunities
    PushArrayCell(pHandle, restrictions);           // restrictions
    PushArrayCell(pHandle, mt);                     // move type
    PushArrayCell(pHandle, regenHealthFlags);       // Health Regeneration Flags
    PushArrayCell(pHandle, regenShieldFlags);       // Shield Regeneration Flags
    PushArrayCell(pHandle, regenHealth);            // Health Regeneration Amount
    PushArrayCell(pHandle, regenShield);           // Shield Regeneration Amount
    PushArrayCell(pHandle, physicalArmorPercent);   // Physical Armor Percent (War3Source attribute)
    PushArrayCell(pHandle, magicalArmorPercent);    // Magical Armor Percent (War3Source attribute)

    #if defined _TRACE
        TraceReturn("CreateArray playerInfo[%d,0x%08x,%d]=0x%08x", \
                    client, plugin, id, pHandle);
    #endif

    return pHandle;
}

ResetPlayer(client, bool clear)
{
    TraceInto("PlayerTracking", "ResetPlayer", "client=%d:%N", \
              client, ValidClientIndex(client));

    new Settings:defaultSettings = Default_Settings;
    
    if (!g_bShowUpgradeInfo)
        defaultSettings |= Show_Upgrade_Info;

    if (g_bShowDisabledRaces)
        defaultSettings |= Show_All_Races;

    SetSettingsBits(client, defaultSettings);
    SetDisplayBits(client, Display_Generic);

    SetChangingClass(client, false);

    SetArmor(client, 0);
    SetArmorFlags(client, Armor_Default);

    SetImmunityBits(client, Immunity_None);         // Player's immunities
    SetRestrictionBits(client, Restriction_None);   // Player's restrictions
    SetAttributeBits(client, Attribute_None);       // Player's attributes
    SetRaceValue(client, -1);                       // Player race (set to pending value)
    SetEnergy(client, 0.0);                         // Energy
    SetEnergyRate(client, -1.0);                    // Energy Rate
    SetEnergyLimit(client, -1.0);                   // Energy Limit
    SetEnergyFlags(client, 0);                      // Energy Flags
    SetInitialEnergy(client, -1.0);                 // Initial Energy
    SetCrystalsValue(client, 0);                    // Crystals
    SetVespeneValue(client, 0);                     // Vespene
    SetPCrystals(client, 0);                        // PCrystals
    SetPendingRace(client, -1);                     // Pending race
    SetPendingUpgradeReset(client, 0);              // Pending upgrade reset
    SetOverallLevelValue(client, 0);                // Overall Level
    SetDatabaseIdent(client, -1);                   // Database Ident
    SetDatabaseLoaded(client, DataNotLoaded);       // Loaded from Database Flag
    SetDatabaseSaved(client, false);                // Saved to Database Flag
    SetDatabaseSaveTime(client, 0.0);               // Time of last Save/Load to/from Database
    SetPlayerStatus(client, PlayerStatus_Unknown);  // Player Status

    // Get The Player's ID (for debugging)
    playerID[client][0] ='\0';
    GetClientAuthId(client, AuthId_Steam2, playerID[client], sizeof(playerID[]));

    // Get The Player's Name (for debugging)
    playerName[client][0] ='\0';
    GetClientName(client,playerName[client],sizeof(playerName[]));

    // Reset The Player's Prev Name
    playerPrevName[client][0] ='\0';

    // Reset the Armor Name
    m_ArmorName[client][0] = '\0';

    if (clear)
        ClearPlayer(client);

    // Player's properties
    HandlepropData=GetPropData(client);
    if (propData == null)
    {
        propData = SetPropData(client, CreateArray()); // Player's properties
        // No need to initialize properties elements.

        TraceCat("Array", "CreateArray playerArray[%d,Properties]=0x%08x", \
                 client, propData);
    }
    else
    {
        new numprops=GetArraySize(propData);
        for (new x=0;x<numprops;x++)
        {
            HandlepropInfo=GetArrayCell(propData,x);
            if (propInfo != null)
            {
                //SetPropInfoPlugin(propInfo,plugin);                   // plugin handle
                //SetPropInfoId(propInfo,id);                           // identifier
                SetPropInfoOverrideSpeed(propInfo,-1.0);                // force speed?
                SetPropInfoOverrideGravity(propInfo,-1.0);              // force gravity?
                SetPropInfoOverrideVisibility(propInfo,-1);             // force visibility?
                SetPropInfoSpeed(propInfo,-1.0);                        // speed
                SetPropInfoGravity(propInfo,-1.0);                      // gravity
                SetPropInfoVisibility(propInfo,-1);                     // visibility 
                SetPropInfoVisibilityType(propInfo,DefaultVisibility);  // visibility type
                SetPropInfoVisibilityFactor1(propInfo,-1.0);            // visibility factor1 (melee/delay)
                SetPropInfoVisibilityFactor2(propInfo,-1.0);            // visibility factor2 (movement/duration)
                SetPropInfoVisibilityMode(propInfo,-1);                 // visibility render mode
                SetPropInfoVisibilityFx(propInfo,-1);                   // visibility render fx
                SetPropInfoVisibilityR(propInfo,-1);                    // visibility red component
                SetPropInfoVisibilityG(propInfo,-1);                    // visibility green component
                SetPropInfoVisibilityB(propInfo,-1);                    // visibility blue component
                SetPropInfoVisibilityWeaponAlpha(propInfo,-1);          // visibility weapons alpha
                SetPropInfoVisibilityWeapons(propInfo,false);           // visibility color weapons flag
                SetPropInfoVisibilityPriority(propInfo,VisibilityPriorityNotSpecified); // visibility priority
                SetPropInfoVisibilityTime(propInfo,0.0);                // visibility timestamp
                SetPropInfoImmunities(propInfo,Immunity_None);          // immunities
                SetPropInfoRestrictions(propInfo,Restriction_None);     // restrictions
                SetPropInfoMoveType(propInfo,MOVETYPE_DEFAULT);         // move type
                SetPropInfoRegenHealthFlags(propInfo,Regen_Default);    // Health Regeneration Flags
                SetPropInfoRegenShieldFlags(propInfo,Regen_Default);    // Shields Regeneration Flags
                SetPropInfoRegenHealth(propInfo,0.0);                   // Health Regeneration Amount
                SetPropInfoRegenShield(propInfo,0.0);                   // Shields Regeneration Amount
                SetPropInfoRegenShield(propInfo,0.0);                   // Shields Regeneration Amount
                SetPropInfoMagicalArmor(propInfo,0.0);                  // Magical Armor Percent (War3Source attribute)
                SetPropInfoPhysicalArmor(propInfo,0.0);                 // Physical Armor Percent (War3Source attribute)
            }
        }
    }

    // Player's shop items
    new shopItemCount;
    new shopVectorCount=GetShopItemCount();
    Handleshopitems=GetShopitems(client);
    if (shopitems == null)
    {
        shopItemCount = shopVectorCount;
        shopitems = SetShopitems(client, CreateArray(1, shopItemCount));

        for (new x=0;x<shopVectorCount;x++)
            SetArrayCell(shopitems,x,0);     // Owns item x

        TraceCat("Array", "CreateArray playerArray[%d,ShopItems]=0x%08x", \
                 client, shopitems);
    }
    else
    {
        shopItemCount = GetArraySize(shopitems);
        if (shopItemCount < shopVectorCount)
        {
            LogError("ERROR: Player %d:%N's shop data got fubared! %d != %d",
                     client, ValidClientIndex(client), shopItemCount, shopVectorCount);

            shopItemCount = shopVectorCount;
            ResizeArray(shopitems, shopVectorCount);
        }
        
        for (new x=0;x<shopItemCount;x++)
            SetArrayCell(shopitems,x,0);     // Owns item x
    }

    // Perform a sanity check.
    if (shopItemCount != shopVectorCount)
    {
        LogError("ERROR: Player %d:%N's shop data got fubared! %d != %d",
                 client, ValidClientIndex(client), shopItemCount, shopVectorCount);

        if (!clear)
        {
            ResetPlayer(client, true);
            TraceReturn();
            return;
        }
    }

    // Player's race data
    new raceDataCount;
    new raceCount = GetRaceCount();
    HandleraceData=GetRaceData(client);
    if (raceData == null)
    {
        raceData = SetRaceData(client, CreateArray()); // Player's race data

        TraceCat("Array", "CreateArray playerArray[%d,RaceData]=0x%08x", \
                 client, raceData);

        for (new race=0;race<raceCount;race++)
        {
            HandleraceInfo=CreateArray(); // Player's races
            PushArrayCell(raceInfo,0); // Race's XP
            PushArrayCell(raceInfo,0); // Race's Level
            PushArrayCell(raceInfo,0.0); // Race's Accumulated Energy
            PushArrayCell(raceInfo,-1); // Race's Active Ultimate 1 Upgrade 
            PushArrayCell(raceInfo,-1); // Race's Active Ultimate 2 Upgrade 
            PushArrayCell(raceInfo,-1); // Race's Active Ultimate 3 Upgrade 
            PushArrayCell(raceInfo,-1); // Race's Active Ultimate 4 Upgrade 
            PushArrayCell(raceInfo,DataNotLoaded); // Race Not Loaded
            PushArrayCell(raceInfo,0); // Race's Saved XP
            PushArrayCell(raceData,raceInfo);

            TraceCat("Array", "CreateArray playerRaceInfo[%d,%d]=0x%08x", \
                     client, race, raceInfo);

            HandleraceHandle=GetRaceHandle(race);
            new upgradeCount = (raceHandle != null)
                               ? GetUpgradeCount(raceHandle) : 0;
            for(new upgrade=0;upgrade<upgradeCount;upgrade++)
            {
                PushArrayCell(raceInfo,0); // Upgrade level
                PushArrayCell(raceInfo,-1); // Saved level in DB
            }

            // Perform a sanity check of the race info.
            if (GetArraySize(raceInfo) != GetRaceInfoUpgradePos(upgradeCount))
            {
                charrname[64]; GetRaceName(GetRaceHandle(race),rname,sizeof(rname));
                LogError("ERROR: Player %d:%N's race %s(%d) data got fubared! %d != %d",
                         client, ValidClientIndex(client), rname, race,
                         GetArraySize(raceInfo), GetRaceInfoUpgradePos(upgradeCount));

                if (!clear)
                {
                    ResetPlayer(client, true);
                    TraceReturn();
                    return;
                }
            }
        }
        raceDataCount=GetArraySize(raceData);
    }
    else
    {
        raceDataCount=GetArraySize(raceData);
        for (new race=0;race<raceDataCount;race++)
        {
            HandleraceInfo=GetArrayCell(raceData,race); // Player's races
            if (raceInfo != null)
            {
                SetRaceInfoXP(raceInfo, 0); // Race's XP
                SetRaceInfoLevel(raceInfo, 0); // Race's Level
                SetRaceInfoAccumulatedEnergy(raceInfo, 0.0); // Race's Accumulated Energy
                SetRaceInfoActiveUltimate1(raceInfo, -1); // Race's Active Ultimate 1 Upgrade 
                SetRaceInfoActiveUltimate2(raceInfo, -1); // Race's Active Ultimate 2 Upgrade 
                SetRaceInfoActiveUltimate3(raceInfo, -1); // Race's Active Ultimate 3 Upgrade 
                SetRaceInfoActiveUltimate4(raceInfo, -1); // Race's Active Ultimate 4 Upgrade 
                SetRaceInfoLoaded(raceInfo, DataNotLoaded); // Race Not Loaded
                SetRaceInfoSavedXP(raceInfo, 0); // Race's Saved XP

                HandleraceHandle=GetRaceHandle(race);
                new upgradeCount = (raceHandle != null) ? GetUpgradeCount(raceHandle) : 0;
                for (new upgrade=0;upgrade<upgradeCount;upgrade++)
                {
                    new upgradePos = GetRaceInfoUpgradePos(upgrade);
                    SetArrayCell(raceInfo,upgradePos,0); // Upgrade level
                    SetArrayCell(raceInfo,upgradePos+_:UPGRADEINFO_SAVED_LEVEL,-1); // Saved level in DB
                }

                // Perform a sanity check of the race info.
                if (GetArraySize(raceInfo)!=GetRaceInfoUpgradePos(upgradeCount))
                {
                    charrname[64]; GetRaceName(GetRaceHandle(race),rname,sizeof(rname));
                    LogError("ERROR: Player %d:%N's race %d:%s data got fubared! %d != %d",
                             client, ValidClientIndex(client), race, rname,
                             GetArraySize(raceInfo), GetRaceInfoUpgradePos(upgradeCount));

                    if (!clear)
                    {
                        ResetPlayer(client, true);
                        TraceReturn();
                        return;
                    }
                }
            }
        }
    }

    // Perform a sanity check of the race data.
    if (raceDataCount != raceCount)
    {
        LogError("ERROR: Player %d:%N's race data got fubared! %d != %d",
                 client, ValidClientIndex(client), raceDataCount, raceCount);

        if (!clear)
        {
            ResetPlayer(client, true);
            TraceReturn();
            return;
        }
    }

    // Player's tech data

    new techDataCount;
    HandletechData=GetTechData(client);
    if (techData == null)
    {
        techData = SetTechData(client, CreateArray()); // Player's tech data

        TraceCat("Array", "CreateArray playerArray[%d,TechData]=0x%08x", \
                 client, techData);

        for (new Faction:techId=Generic;techId<Faction;techId++)
        {
            HandletechInfo=CreateArray(); // Player's tech info
            PushArrayCell(techInfo,0); // Tech Count
            PushArrayCell(techInfo,0); // Tech Level
            PushArrayCell(techInfo,DataNotLoaded); // Tech Not Loaded
            PushArrayCell(techData,techInfo);

            TraceCat("Array", "CreateArray playerTechInfo[%d,%d]=0x%08x", \
                     client, techId, techInfo);

            // Perform a sanity check of the tech info.
            if (GetArraySize(techInfo) != TECHINFO_SIZE)
            {
                chartech[NAME_STRING_LENGTH];
                GetFactionName(techId, tech, sizeof(tech));
                LogError("ERROR: Player %d:%N's tech %d:%s data got fubared! %d != %d",
                         client, ValidClientIndex(client), techId, tech,
                         GetArraySize(techInfo), TECHINFO_SIZE);

                if (!clear)
                {
                    ResetPlayer(client, true);
                    TraceReturn();
                    return;
                }
            }
        }
        techDataCount=GetArraySize(techData);
    }
    else
    {
        techDataCount=GetArraySize(techData);
        for (new Faction:techId=Generic;techId<Faction:techDataCount;techId++)
        {
            HandletechInfo=GetArrayCell(techData,_:techId); // Player's tech data
            if (techInfo != null)
            {
                SetTechInfoCount(techInfo,0); // Tech Count
                SetTechInfoLevel(techInfo,0); // Tech Level
                SetTechInfoLoaded(techInfo,DataNotLoaded); // Tech Not Loaded

                // Perform a sanity check of the tech info.
                if (GetArraySize(techInfo)!=TECHINFO_SIZE)
                {
                    chartech[NAME_STRING_LENGTH];
                    GetFactionName(techId, tech, sizeof(tech));
                    LogError("ERROR: Player %d:%N's tech %d:%s data got fubared! %d != %d",
                             client, ValidClientIndex(client), techId, tech,
                             GetArraySize(techInfo), TECHINFO_SIZE);

                    if (!clear)
                    {
                        ResetPlayer(client, true);
                        TraceReturn();
                        return;
                    }
                }
            }
        }
    }

    // Perform a sanity check of the tech data.
    if (techDataCount != _:Faction)
    {
        LogError("ERROR: Player %d:%N's tech data got fubared! %d != %d",
                 client, ValidClientIndex(client), techDataCount, Faction);

        if (!clear)
            ResetPlayer(client, true);
    }

    TraceReturn();
}

ClearPlayerArray()
{
    TraceInto("PlayerTracking", "ClearPlayerArray");

    for (new client=0;client<=MaxClients;client++)
        ClearPlayer(client);

    TraceReturn();
}

ClearPlayer(client)
{
    TraceInto("PlayerTracking", "ClearPlayer", "client=%d:%N", \
              client, ValidClientIndex(client));

    HandlepropData=Handle:GetPropData(client); // Player's properties
    if (propData != null)
    {
        new numprops=GetArraySize(propData);
        for (new x=0;x<numprops;x++)
        {
            HandlepropInfo=GetArrayCell(propData,x);
            if (propInfo != null)
            {
                #if defined _TRACE
                    new id = GetPropInfoId(propInfo);
                    Handleplugin = GetPropInfoPlugin(propInfo);
                    TraceCat("Array", "CloseArray playerInfo[%d,0x%08x,%d,%d]=0x%08x", \
                             client, plugin, id, x, propInfo);
                #endif

                ClearArray(propInfo);
                CloseHandle(propInfo);
            }
        }
        ClearArray(propData);
        CloseHandle(propData);
        SetPropData(client, null);

        TraceCat("Array", "CloseArray playerArray[%d,INFO_PROPERTIES]=0x%08x", \
                 client, propData);
    }

    Handleshopitems=Handle:GetShopitems(client); // Player's shop items
    if (shopitems != null)
    {
        ClearArray(shopitems);
        CloseHandle(shopitems);
        SetShopitems(client, null);

        TraceCat("Array", "CloseArray playerArray[%d,INFO_SHOPITEMS]=0x%08x", \
                 client, shopitems);
    }

    HandleraceData = GetRaceData(client); // Player's race data
    if (raceData != null)
    {
        new raceDataCount=GetArraySize(raceData);
        for(new race=0;race<raceDataCount;race++)
        {
            HandleraceInfo=GetArrayCell(raceData,race); // Player's races
            if (raceInfo != null)
            {
                ClearArray(raceInfo);
                CloseHandle(raceInfo);

                TraceCat("Array", "CloseArray playerRaceInfo[%d,%d]=0x%08x", \
                         client, race, raceInfo);
            }
        }

        ClearArray(raceData);
        CloseHandle(raceData);
        SetRaceData(client, null);

        TraceCat("Array", "CloseArray playerArray[%d,INFO_RACEDATA]=0x%08x", \
                 client, raceData);
    }

    HandletechData = GetTechData(client); // Player's tech data
    if (techData != null)
    {
        new techDataCount=GetArraySize(techData);
        for(new techId=0;techId<techDataCount;techId++)
        {
            HandletechInfo=GetArrayCell(techData,techId); // Player's tech info
            if (techInfo != null)
            {
                ClearArray(techInfo);
                CloseHandle(techInfo);

                TraceCat("Array", "CloseArray playerTechInfo[%d,%d]=0x%08x", \
                         client, techId, techInfo);
            }
        }

        ClearArray(techData);
        CloseHandle(techData);
        SetTechData(client, null);

        TraceCat("Array", "CloseArray playerArray[%d,INFO_TECHDATA]=0x%08x", \
                 client, techData);
    }

    TraceReturn();
}

float SumFloatClientProperty(client, PropInfo pindex)
{
    float sum = 0.0;
    Handle propData=GetPropData(client);
    if (propData == null)
    {
        new numprops=GetArraySize(propData);
        for (new x=0;x<numprops;x++)
        {
            HandlepropInfo=GetArrayCell(propData,x);
            if (propInfo != null)
                sum += Float:GetArrayCell(propInfo,_:pindex);
        }
    }
    return sum;
}

ResetPlayerProperties(client)
{
    HandlepropData = GetPropData(client);
    if (propData != null)
    {
        new numprops=GetArraySize(propData);
        for (new x=0;x<numprops;x++)
        {
            HandlepropInfo=GetArrayCell(propData,x);
            if (propInfo != null)
            {
                SetPropInfoOverrideSpeed(propInfo,-1.0);
                SetPropInfoSpeed(propInfo,-1.0);

                SetPropInfoOverrideGravity(propInfo,-1.0);
                SetPropInfoGravity(propInfo,-1.0);

                SetPropInfoOverrideVisibility(propInfo,-1);
                SetPropInfoVisibility(propInfo,-1);
                SetPropInfoVisibilityFactor1(propInfo,-1.0);
                SetPropInfoVisibilityFactor2(propInfo,-1.0);
                SetPropInfoVisibilityMode(propInfo,-1);
                SetPropInfoVisibilityFx(propInfo,-1);
                SetPropInfoVisibilityR(propInfo,-1);
                SetPropInfoVisibilityG(propInfo,-1);
                SetPropInfoVisibilityB(propInfo,-1);
                SetPropInfoVisibilityWeaponAlpha(propInfo,-1);
                SetPropInfoVisibilityWeapons(propInfo,false);
                SetPropInfoVisibilityPriority(propInfo,VisibilityPriorityNotSpecified);
                SetPropInfoVisibilityTime(propInfo,0.0);

                SetImmunityBits(client,Immunity_None);
                SetPropInfoImmunities(propInfo,Immunity_None);

                SetRestrictionBits(client,Restriction_None);
                SetPropInfoRestrictions(propInfo,Restriction_None);

                SetPropInfoMoveType(propInfo,MOVETYPE_DEFAULT);

                SetPropInfoRegenHealth(propInfo,0.0);
                SetPropInfoRegenHealthFlags(propInfo,Regen_Default);

                SetPropInfoRegenShield(propInfo,0.0);
                SetPropInfoRegenShieldFlags(propInfo,Regen_Default);

                SetPropInfoPhysicalArmor(propInfo,0.0);
                SetPropInfoMagicalArmor(propInfo,0.0);
            }
        }
    }
}

