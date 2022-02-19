//
//  pid.c
//  flex
//
//  Created by Joey Tomlinson on 11/10/13.
//
//

#include "inttypes.h"
#include "pid.h"

// unused for now

int32_t pid (uint16_t t, int16_t e, struct Pid* pid) {

    int16_t de = e - pid->e;
    uint16_t dt = t - pid->t;
    
    if (dt == 0)
        return pid->out;
    
    pid->e = e;
    pid->t = t;
    
    pid->p = e*pid->kp; // +- 2^30
    
    /*2^31*/ // integer promotion makes this work ok i think?
    if (pid->i /*2^31*/ + e*dt*pid->ki /*2^46*/ <= pid->maxi)
        if (pid->i + e*dt*pid->ki >= -pid->maxi)
            pid->i += e*dt*pid->ki;
        else
            pid->i = -pid->maxi;
    else
        pid->i = pid->maxi;
    
    /*2^31*/
    pid->d = de/dt /*2^16*/ * pid->kd /*2^15*/;
    
                  /*2^28*/   /*2^29*/   /*2^29*/
    int32_t sum = pid->p/4 + pid->i/4 + pid->d/4;
    
    pid->out = sum/pid->div; /*2^31*/
    
    return pid->out; /*2^31*/
}