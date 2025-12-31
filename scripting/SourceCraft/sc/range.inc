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

stock float DistanceBetween(const float startvec[3],const float endvec[3])
{
    float distance = GetVectorDistance(startvec,endvec);
    if (distance < 0)
        distance *= -1;

    return distance;                                  
}

stock bool IsPointInRange(const float start[3], const float end[3],float maxdistance)
{
    return (DistanceBetween(start,end)<maxdistance);
}

stock bool IsInRange(client,index,float maxdistance)
{
    float start[3];
    float end[3];
    GetClientAbsOrigin(client,start);
    GetClientAbsOrigin(index,end);
    return (DistanceBetween(start,end)<maxdistance);
}

stock PowerOfRange(float location[3],float radius,float check_location[3],maxhp,
                   float factor=0.20,bool limit=true)
{
    if (radius <= 0.0)
        return maxhp;
    else
    {
        float distance=DistanceBetween(location,check_location);
        if (limit && distance > radius)
            return 0;
        else
        {
            float healthtakeaway=1-FloatDiv(distance,radius)+factor;
            return (healthtakeaway > 0.0) ? RoundFloat(float(maxhp)*healthtakeaway) : 0;
        }
    }
}

