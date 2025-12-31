#if !defined SOURCECRAFT // Not Implemented Yet!
native War3_SetRaceConfigString(raceID, char[] key, char[] value);
native War3_SetRaceConfigFloat(raceID, char[] key, float value);
native War3_SetRaceConfigInt(raceID, char[] key, value);
native War3_SetRaceConfigBool(raceID, char[] key, bool value);
native War3_SetRaceConfigArray(raceID, char[] key, any:value[], maxlen, KvDataTypes:type = KvData_Float);
// Note! Allows you to define arrays that use strings for keys!
native War3_SetRaceConfigArrayValue(raceID, char[] array, char[] key, char[] value);
native War3_SetRaceGlobalConfigString(char[] key, char[] value);
#endif

#if !defined SOURCECRAFT
native float War3_GetRaceConfigFloat(raceID, char[] key);
native War3_GetRaceConfigInt(raceID, char[] key);
native bool War3_GetRaceConfigBool(raceID, char[] key);
native War3_GetRaceConfigArray(raceID, char[] key, any:array[], maxlen, KvDataTypes:type = KvData_Float);
#else
#define War3_GetRaceConfigFloat(%1,%2) GetConfigFloat(%2,.fileId=%1)
#define War3_GetRaceConfigInt(%1,%2) GetConfigNum(%2,.fileId=%1)
#define War3_GetRaceConfigBool(%1,%2) (bool GetConfigNum(%2,.fileId=%1))
stock War3_GetRaceConfigArray(race, const char key[], any:array[], maxlen, KvDataTypes:type = KvData_Float)
{
    if (type == KvData_Float)
        return GetConfigFloatArray(key[], array[], maxlen, .fileId=race);
    else
        return GetConfigArray(key[], array[], maxlen, .fileId=race);
}
#endif

#if !defined SOURCECRAFT // Not Implemented Yet!
// Note! Allows you to define arrays that use strings for keys!
native War3_GetRaceConfigArrayValue(raceID, char[] array, char[] key, char[] value);
#endif

#if !defined SOURCECRAFT
native War3_GetRaceConfigString(raceID, char[] key, char[] value);
#else
#define War3_GetRaceConfigString(%1,%2,%3) GetConfigString(%2,%3,.fileId=%1)
#endif

#if !defined SOURCECRAFT // Not Implemented Yet!
native War3_GotoRaceConfigArray(raceID, char[] array);
native bool War3_GetRaceConfigArrayNextValue(char[] value);
#endif

#if !defined SOURCECRAFT // Not Implemented Yet!
native War3_SetItemConfigString(ItemID, char[] key, char[] value);
native War3_SetItemConfigFloat(ItemID, char[] key, float value);
native War3_SetItemConfigInt(ItemID, char[] key, value);
native War3_SetItemConfigBool(ItemID, char[] key, bool value);
native War3_SetItemConfigArray(ItemID, char[] key, any:array[], maxlen, KvDataTypes:type = KvData_Float);
// Note! Allows you to define arrays that use strings for keys!
native War3_SetItemConfigArrayValue(ItemID, char[] array, char[] key, char[] value);
native War3_SetItemGlobalConfigString(char[] key, char[] value);
#endif

#if !defined SOURCECRAFT
native float War3_GetItemConfigFloat(ItemID, char[] key);
native War3_GetItemConfigInt(ItemID, char[] key);
native bool War3_GetItemConfigBool(ItemID, char[] key);
native War3_GetItemConfigArray(ItemID, char[] key, any:value[], maxlen, KvDataTypes:type = KvData_Float);
#else
#define War3_GetItemConfigFloat(%1,%2) GetConfigFloat(%2,.fileId=SHOPITEM,.node=%1)
#define War3_GetItemConfigInt(%1,%2) GetConfigNum(%2,.fileId=SHOPITEM,.node=%1)
#define War3_GetItemConfigBool(%1,%2) (bool GetConfigNum(%2,.fileId=SHOPITEM,.node=%1))
stock War3_GetItemConfigArray(ItemID, const char key[], any:array[], maxlen, KvDataTypes:type = KvData_Float)
{
    if (type == KvData_Float)
        return GetConfigFloatArray(key[], array[], maxlen, .fileId=SHOPITEM, .node=ItemID);
    else
        return GetConfigArray(key[], array[], maxlen, .fileId=SHOPITEM,. node=ItemID);
}
#endif

#if !defined SOURCECRAFT // Not Implemented Yet!
// Note! Allows you to define arrays that use strings for keys!
native War3_GetItemConfigArrayValue(ItemID, char[] array, char[] key, char[] value);
#endif

#if !defined SOURCECRAFT
native War3_GetItemConfigString(ItemID, char[] key, char[] value);
#else
#define War3_GetItemConfigString(%1,%2,%3) GetConfigString(%2,%3,.fileId=SHOPITEM,.node=%1)
#endif

#if !defined SOURCECRAFT // Not Implemented Yet!
native War3_GotoItemConfigArray(raceID, char[] array);
native bool War3_GetItemConfigArrayNextValue(char[] value);
#endif
