/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: tf2_meter.inc
 * Description: Functions to return information about TF2 cloak, rage and energy drink meters.
 * Author(s): Naris (Murray Wilson)
 */
 
#if defined _tf2_meter_included
 #endinput
#endif
#define _tf2_meter_included

#include <sdktools>

stock float TF2_GetCloakMeter(client)
{
    return GetEntPropFloat(client, Prop_Send, "m_flCloakMeter");
}

stock TF2_SetCloakMeter(client,float cloakMeter)
{
    SetEntPropFloat(client, Prop_Send, "m_flCloakMeter", cloakMeter);
}

stock float TF2_GetChargeMeter(client)
{
    return GetEntPropFloat(client, Prop_Send, "m_flChargeMeter");
}

stock TF2_SetChargeMeter(client,float char geMeter)
{
    SetEntPropFloat(client, Prop_Send, "m_flChargeMeter", char geMeter);
}

stock float TF2_GetRageMeter(client)
{
    return GetEntPropFloat(client, Prop_Send, "m_flRageMeter");
}

stock TF2_SetRageMeter(client,float rageMeter)
{
    SetEntPropFloat(client, Prop_Send, "m_flRageMeter", rageMeter);
}

stock float TF2_GetHypeMeter(client)
{
    return GetEntPropFloat(client, Prop_Send, "m_flHypeMeter");
}

stock TF2_SetHypeMeter(client,float hypeMeter)
{
    SetEntPropFloat(client, Prop_Send, "m_flHypeMeter", hypeMeter);
}

stock float TF2_GetEnergyDrinkMeter(client)
{
    return GetEntPropFloat(client, Prop_Send, "m_flEnergyDrinkMeter");
}

stock TF2_SetEnergyDrinkMeter(client,float energyDrinkMeter)
{
    SetEntPropFloat(client, Prop_Send, "m_flEnergyDrinkMeter", energyDrinkMeter);
}

stock GetRevengeCrits(client)
{
	return GetEntProp(client, Prop_Send, "m_iRevengeCrits");
}

stock SetRevengeCrits(client, iAmount = 99)
{
	SetEntProp(client, Prop_Send, "m_iRevengeCrits", iAmount);
}

stock GetDecapitations(client)
{
	return SetEntProp(client, Prop_Send, "m_iDecapitations");
}

stock SetDecapitations(client, iAmount = 99)
{
	SetEntProp(client, Prop_Send, "m_iDecapitations", iAmount);
}
