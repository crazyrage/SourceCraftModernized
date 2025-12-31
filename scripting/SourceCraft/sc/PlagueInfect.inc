/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: PlagueInfect.inc
 * Description: PlagueInfect for the Zerg Plague upgrade for SourceCraft.
 * Author(s): -=|JFH|=-Naris
 */
 
#if defined plague_infect_included
 #endinput
#endif
#define plague_infect_included

#include "sc/PlagueType"

/**
 * Inflicts the plague on someone
 *
 * @param inflicter	   The client doing the inflicting.
 * @param index:       The client getting inflicted.
 * @param duration:    The number of increments to do damage.
 * @param amount:      The amount of damage (* increment) to do per increment.
 * @param type:        The type of plague
 * @param weapon_name: The name of the weapon used to infect the client.
 * @param weapon_desc: The description of the weapon used to infect the client.
 * @noreturn
 */
native PlagueInfect(int inflicter, int index, int duration=1, int amount=1,
                    PlagueType type=NormalPlague,
                    const char[] weapon_name="sc_plague",
                    const char[] weapon_desc="");

/**
 * Explode a Player
 *
 * @param client:      The client to explode
 * @param inflicter:   The client causing the explode (if any)
 * @param team:        The team that is NOT affected by the explosion (if any)
 * @param radius:      Radius of the explosion.
 * @param damage:      Damage caused at the center of the explosion
 * @param building:    Damage caused to buildings at the center of the explosion
 * @param type:        Bits to determine what type of explosion
 * @param xp:          Amount of extra xp for a kill (if any)
 * @param weapon_name: The name of the weapon used for the explosion.
 * @param weapon_desc: The description of the weapon used for the explosion.
 * @noreturn
 */
native ExplodePlayer(int client, int inflicter=0, int team=0, float radius=500.0, int damage=800,
                     int building=800, ExplosionType type=NormalExplosion|UltimateExplosion,
                     int xp=10, const char[] weapon_name="sc_explode", const char[] weapon_desc="");

/**
 * Returns true if the given client has been exploded
 *
 * @param client:      The client to check
 * @return             Returns true if the client has exploded.
 */
native bool HasPlayerExploded(int client);

/* DO NOT EDIT BELOW THIS LINE */

public SharedPlugin:__pl_PlagueInfect = 
{
	name = "PlagueInfect",
	file = "PlagueInfect.smx",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_PLUGIN
public __pl_PlagueInfect_SetNTVOption()//al
{
	MarkNativeAsOptional("PlagueInfect");
	MarkNativeAsOptional("ExplodePlayer");
	MarkNativeAsOptional("HasPlayerExploded");
}
#endif
