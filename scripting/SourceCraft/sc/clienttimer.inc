/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: clienttimer.inc
 * Description: ClientTimer functions
 * Author(s): Naris (Murray Wilson)
 */
 
#if defined _clienttimer_included
 #endinput
#endif
#define _clienttimer_included

#include "sc/client"

#define ResetClientTimer(%1) g_ClientTimers[%1] = null

stock Handle g_ClientTimers[MAXPLAYERS+1];

stock CreateClientTimer(client, float int erval, Timer:func,
                        flags=TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE)
{
    if (IsClient(client) && g_ClientTimers[client] == null)
        g_ClientTimers[client] = CreateTimer(int erval,func,GetClientUserId(client),flags);
}

stock KillClientTimer(client)
{
    if (IsClient(client))
    {
        Handle timer=g_ClientTimers[client];
        if (timer != null)
        {
            ResetClientTimer(client);	
            KillTimer(timer);
        }
    }
}

stock KillAllClientTimers()
{
    for (int i = 1; i <= MaxClients; i++)
        KillClientTimer(i);
}

stock ResetAllClientTimers()
{
    for (int i = 1; i <= MaxClients; i++)
        ResetClientTimer(i);
}
