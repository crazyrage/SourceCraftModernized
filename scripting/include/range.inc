/**
 * vim: set ai et ts=4 sw=4 syntax=sourcepawn :
 * File: range.inc
 * Description: Range and Distance functions and variables
 * Author(s): Naris (Murray Wilson)
 */
 
#if defined _range_included
 #endinput
#endif
#define _range_included

stock float TargetRange(client,index)
{
    float start[3];
    GetClientAbsOrigin(client,start);

    float end[3];
    if (index <= MaxClients)
        GetClientAbsOrigin(index, end);
    else        
        GetEntPropVector(index, Prop_Send, "m_vecOrigin", end);

    return GetVectorDistance(start,end);
}

stock bool IsInRange(client,index,float maxdistance)
{
    return (TargetRange(client,index)<maxdistance);
}

stock bool IsPointInRange(const float start[3], const float end[3],float maxdistance)
{
    return (GetVectorDistance(start,end)<maxdistance);
}

stock int PowerOfRange(float location[3],float radius,float check_location[3],int amount,
                   float factor=0.20,bool limit=true,float &distance=0.0)
{
    if (radius <= 0.0)
        return amount;
    else
    {
        distance=GetVectorDistance(location,check_location);
        if (limit && distance > radius)
            return 0;
        else
        {
            float percent = 1 - (distance / radius) + factor;
            return (percent > 0.0) ? RoundFloat(float(amount)*percent) : 0;
        }
    }
}

