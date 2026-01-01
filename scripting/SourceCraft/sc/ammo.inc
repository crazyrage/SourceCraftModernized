/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: ammo.inc
 * Description: Ammo functions and variables
 * Author(s): Naris (Murray Wilson)
 */
 
#if defined _ammo_included
 #endinput
#endif
#define _ammo_included

stock GetClip(int weapon)
{
    return GetEntProp(weapon, Prop_Send, "m_iClip1");
}

stock SetClip(int weapon, int amount)
{
    SetEntProp(weapon, Prop_Send, "m_iClip1", amount);
}

stock GetAmmoType(int weapon)
{
    return GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType");
}
