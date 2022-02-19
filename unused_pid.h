//
//  pid.h
//  flex
//
//  Created by Joey Tomlinson on 11/10/13.
//
//

#ifndef flex_pid_h
#define flex_pid_h

struct Pid {
    uint16_t t;

    int16_t e;
    
    int32_t p;
    int32_t i;
    int32_t d;
    
    // signed int type to limit range
    int32_t maxi; // should be positive. abs of i is limited to this number
    
    int16_t kp;
    int16_t ki;
    int16_t kd;
    
    int16_t div;
    
    int32_t out;
};

int32_t pid (uint16_t t, int16_t e, struct Pid* pid);


#endif
