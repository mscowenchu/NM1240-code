#include "svpwm.h"

//====Iuvw_to_Idq()==================================================
//0. Pre-load Iu/Iv/Iw=Motor->info.i16_Iu/Iv/Iw
//1. Do Clark Transfer: Transfer Iu/v/w to Ialfa/beta
//2. Do Park Transfer: Transfer Ialfa/beta to Id/q
//3. Store the folloiwing variables
//   Motor->info.i16_Ialfa, Motor->info.i16_Ibeta, Motor->info.i16_Id, Motor->info.i16_Iq
//=======================================================================
void Iuvw_to_Idq(AMotor* Motor, int32_t sin, int32_t cos)
{
    int32_t Iu, Iv, Iw, Ialfa, Ibeta, Id, Iq;

    Iu = Motor->info.i16_Iu;
    Iv = Motor->info.i16_Iv;
    Iw = Motor->info.i16_Iw;
  
    /* Clark: (Iu,Iv,Iw) to (Ialfa, Ibeta) */
    Ialfa = Iu;
    Ibeta = ((Iv - Iw) * 18918) >> 15;

    /* Park: (Ialfa, Ibeta) to (Id, Iq) */
    Id = ( (Ialfa * cos) + (Ibeta * sin)) >> 15;
    Iq = (-(Ialfa * sin) + (Ibeta * cos)) >> 15;
  
    //* updat the result */
    Motor->info.i16_Ialfa = Ialfa;
    Motor->info.i16_Ibeta = Ibeta;
    Motor->info.i16_Id = Id;
    Motor->info.i16_Iq = Iq;

}

//void Iuvw_to_Idq(int32_t* Iu, int32_t* Iv, int32_t* Iw, int32_t* Ialfa, int32_t* Ibeta, int32_t* Id, int32_t* Iq, int32_t sin, int32_t cos)
//{
//    /* Clark: (Iu,Iv,Iw) to (Ialfa, Ibeta) */
//    *Ialfa = *Iu;
//    *Ibeta = ((*Iv - *Iw) * 18918) >> 15;

//    /* Park: (Ialfa, Ibeta) to (Id, Iq) */
//    *Id = ( (*Ialfa * cos) + (*Ibeta * sin)) >> 15;
//    *Iq = (-(*Ialfa * sin) + (*Ibeta * cos)) >> 15;
//
//}


//====SVPWM_2R()==================================================
//1. Do Inv(Park)
//2. Do Inv(Clark)
//3. Do SVPWM for output Taoff, Tboff, Tcoff
//4. Store the folloiwing variables
//  Motor->cmd.i16_Valfa, Motor->cmd.i16_Vbeta, Motor->cmd.i16_Va, Motor->cmd.i16_Vb, Motor->cmd.i16_Vc, Motor->cmd.u8_zone      
//================================================================
void SVPWM_2R(int32_t Vd, int32_t Vq, int32_t sin, int32_t cos, int32_t pwm_full_scale, AMotor* Motor, uint16_t* zone, uint16_t* Taoff, uint16_t* Tboff, uint16_t* Tcoff)  
{
    int32_t Valfa, Vbeta, Vrefx, Vrefy, Vrefz;
    int32_t Tx, Ty, Tz, T1, T2, T_total;
    int32_t Taon, Tbon, Tcon;


    /* Inv(Park): Vd/q to Valfa/beta */
    Valfa = ( (Vd * cos) - (Vq * sin)) >> 15;
    Vbeta = ( (Vd * sin) + (Vq * cos)) >> 15;
  
    /* Modified Inv(Clark): Valfa/Vbeta to Vrefx/y/z */
    Vrefx = Vbeta;
    Vrefy = ((-(Vbeta) * 16384) + ((Valfa) * 28377)) >> 15;
    Vrefz = ((-(Vbeta) * 16384) - ((Valfa) * 28377)) >> 15;

    *zone = 0;
    if(Vrefx >= 0)
        *zone += 1;
    if(Vrefy >= 0)
        *zone += 2;
    if(Vrefz >= 0)
        *zone += 4;

    Tx = Vrefx;
    Ty = -Vrefz;
    Tz = -Vrefy;
  
    if(*zone == 1)
    {
        T1 = Tz;
        T2 = Ty;
    }
    else if(*zone == 2)
    {
        T1 = Ty;
        T2 = -Tx;
    }
    else if(*zone == 3)
    {
        T1 = -Tz;
        T2 = Tx;
    }
    else if(*zone == 4)
    {
        T1 = -Tx;
        T2 = Tz;
    }
    else if(*zone == 5)
    {
        T1 = Tx;
        T2 = -Ty;
    }
    else
    {
        T1 = -Ty;
        T2 = -Tz;
    }

    T_total = T1 + T2;

    //%-- Re-scaling T1 and T2 if needs------
    if(T_total > 32767)
    {
        HDIV0->DIVIDEND = (signed int)T1*16384;
        HDIV0->DIVISOR = (T_total >> 1);
        T1 = HDIV0->QUOTIENT;
        HDIV0->DIVIDEND = (signed int)T2*16384;
        HDIV0->DIVISOR = (T_total >> 1);
        T2 = HDIV0->QUOTIENT;
    }

    //-- Re-scaling T1 and T2 as pwm_full_scale ---
    T1 = (T1 * pwm_full_scale) >> 15;  //Rescaling to pwm full scale
    T2 = (T2 * pwm_full_scale) >> 15;  //Rescaling to pwm full scale

    Taon = (pwm_full_scale - T1 - T2)/2;
    Tbon = Taon + T1;
    Tcon = Tbon + T2;

    if(*zone == 1)  // V U W with Large/Middle/Small duty
    {
        *Taoff = Tbon;
        *Tboff = Taon;
        *Tcoff = Tcon;
    }
    else if(*zone == 2)  // U W V
    {
        *Taoff = Taon;
        *Tboff = Tcon;
        *Tcoff = Tbon;
    }
    else if(*zone == 3)  // U V W
    {
        *Taoff = Taon;
        *Tboff = Tbon;
        *Tcoff = Tcon;
    }
    else if(*zone == 4)  // W V U
    {
        *Taoff = Tcon;
        *Tboff = Tbon;
        *Tcoff = Taon;
    }
    else if(*zone == 5)  // V W U
    {
        *Taoff = Tcon;
        *Tboff = Taon;
        *Tcoff = Tbon;
    }
    else if(*zone == 6)  // W U V
    {
        *Taoff = Tbon;
        *Tboff = Tcon;
        *Tcoff = Taon;
    }
    else
    {
        *Taoff = pwm_full_scale / 2;
        *Tboff = pwm_full_scale / 2;
        *Tcoff = pwm_full_scale / 2;
    }


    Motor->cmd.i16_Valfa = Valfa;
    Motor->cmd.i16_Vbeta = Vbeta;
    Motor->cmd.i16_Va = Vrefx;
    Motor->cmd.i16_Vb = Vrefy;
    Motor->cmd.i16_Vc = Vrefz;
    Motor->cmd.u8_zone = *zone;
 }


//====SVPWM_1R()==================================================
//1. Do Inv(Park)
//2. Do Inv(Clark)
//3. Do SVPWM for output Taoff_Up, Tboff_Up, Tcoff_Up, Taoff_Down, Tboff_Down, Tcoff_Down
//4. Store the folloiwing variables
//  Motor->cmd.i16_Valfa, Motor->cmd.i16_Vbeta, Motor->cmd.i16_Va, Motor->cmd.i16_Vb, Motor->cmd.i16_Vc, Motor->cmd.u8_zone      
//================================================================
void SVPWM_1R(int32_t Vd, int32_t Vq, int32_t sin, int32_t cos, int32_t pwm_full_scale, uint16_t pwm_shift_CMP0, uint16_t pwm_shift_CMP1, uint16_t Max_PWM_Duty, uint16_t PWM_Margine,
    AMotor* Motor, uint16_t* zone, uint16_t* Taoff_Up, uint16_t* Tboff_Up, uint16_t* Tcoff_Up, uint16_t* Taoff_Down, uint16_t* Tboff_Down, uint16_t* Tcoff_Down)
{
    int32_t Valfa, Vbeta, Vrefx, Vrefy, Vrefz;
    int32_t Tx, Ty, Tz, T1, T2, T_total;
    int32_t Taon, Tbon, Tcon;

    uint16_t ui16_Error_LeftShift, ui16_Error_RightShift;
    uint16_t Taon_Up, Taon_Down, Tbon_Up, Tbon_Down, Tcon_Up, Tcon_Down;

    /* Inv(Park): Vd/q to Valfa/beta */
    Valfa = ( (Vd * cos) - (Vq * sin)) >> 15;
    Vbeta = ( (Vd * sin) + (Vq * cos)) >> 15;
  
    /* Modified Inv(Clark): Valfa/Vbeta to Vrefx/y/z */
    Vrefx = Vbeta;
    Vrefy = ((-(Vbeta) * 16384) + ((Valfa) * 28377)) >> 15;
    Vrefz = ((-(Vbeta) * 16384) - ((Valfa) * 28377)) >> 15;

    *zone = 0;
    if(Vrefx >= 0) *zone += 1;
    if(Vrefy >= 0) *zone += 2;
    if(Vrefz >= 0) *zone += 4;

    Tx = Vrefx;
    Ty = -Vrefz;
    Tz = -Vrefy;

    if(*zone == 1)
    {
        T1 = Tz;
        T2 = Ty;
    }
    else if(*zone == 2)
    {
        T1 = Ty;
        T2 = -Tx;
    }
    else if(*zone == 3)
    {
        T1 = -Tz;
        T2 = Tx;
    }
    else if(*zone == 4)
    {
        T1 = -Tx;
        T2 = Tz;
    }
    else if(*zone == 5)
    {
        T1 = Tx;
        T2 = -Ty;
    }
    else
    {
        T1 = -Ty;
        T2 = -Tz;
    }

    T_total = T1 + T2;

    //-- Re-scaling T1 and T2 if needs------
    if(T_total > 32767)
    {
        HDIV0->DIVIDEND = (signed int)T1*16384;
        HDIV0->DIVISOR = (T_total >> 1);
        T1 = HDIV0->QUOTIENT;
        HDIV0->DIVIDEND = (signed int)T2*16384;
        HDIV0->DIVISOR = (T_total >> 1);
        T2 = HDIV0->QUOTIENT;
    }

    //-- Re-scaling T1 and T2 as pwm_full_scale ---
    T1 = (T1 * pwm_full_scale) >> 15;  //Rescaling to pwm full scale
    T2 = (T2 * pwm_full_scale) >> 15;  //Rescaling to pwm full scale

    Taon = (pwm_full_scale - T1 - T2)/2;
    Tbon = Taon + T1;
    Tcon = Tbon + T2;


    //---Process to shift PWM-------------------------------------------------------
    /*--Right shit for largest PWM Duty ---*/
    if(T1 < pwm_shift_CMP0)
    {
        ui16_Error_RightShift = pwm_shift_CMP0 - T1;
//      u8_Flag_RightShift = 1;
    }
    else
    {
        ui16_Error_RightShift = 0;
//      u8_Flag_RightShift = 0;
    }
   
    /*--Left shift for smallest PWM Duty ---*/
    if(T2 < pwm_shift_CMP1)
    {
        ui16_Error_LeftShift = pwm_shift_CMP1 - T2;
//      u8_Flag_LeftShift = 1;
    }
    else
    {
        ui16_Error_LeftShift = 0;
//      u8_Flag_LeftShift = 0;
    }

    /*--Largest Duty: Shift right --*/
    Taon_Up = Taon + ui16_Error_RightShift;

    if(Taon_Up > Max_PWM_Duty)
        Taon_Up = Max_PWM_Duty;

    Taon_Down = Taon - ui16_Error_RightShift;

    if(Taon_Down < PWM_Margine)
        Taon_Down = PWM_Margine;
  
    /*--Middle Duty: Not shift --*/
    Tbon_Up = Tbon;
    Tbon_Down = Tbon;

    /*--Smallest Duty: Shift left --*/
    Tcon_Up = Tcon - ui16_Error_LeftShift;

    if(Tcon_Up < PWM_Margine)
        Tcon_Up = PWM_Margine;
  
    Tcon_Down = Tcon + ui16_Error_LeftShift;

    if(Tcon_Down > Max_PWM_Duty)
        Tcon_Down = Max_PWM_Duty;
    //-----------------------------------------

    if(*zone == 1)    // V U W with Large/Middle/Small duty
    {
        *Taoff_Up = Tbon_Up;
        *Tboff_Up = Taon_Up;
        *Tcoff_Up = Tcon_Up;

        *Taoff_Down = Tbon_Down;
        *Tboff_Down = Taon_Down;
        *Tcoff_Down = Tcon_Down;
    }
    else if(*zone == 2)  // U W V
    {
        *Taoff_Up = Taon_Up;
        *Tboff_Up = Tcon_Up;
        *Tcoff_Up = Tbon_Up;

        *Taoff_Down = Taon_Down;
        *Tboff_Down = Tcon_Down;
        *Tcoff_Down = Tbon_Down;
    }
    else if(*zone == 3)  // U V W
    {
        *Taoff_Up = Taon_Up;
        *Tboff_Up = Tbon_Up;
        *Tcoff_Up = Tcon_Up;

        *Taoff_Down = Taon_Down;
        *Tboff_Down = Tbon_Down;
        *Tcoff_Down = Tcon_Down;
    }
    else if(*zone == 4)  // W V U
    {
        *Taoff_Up = Tcon_Up;
        *Tboff_Up = Tbon_Up;
        *Tcoff_Up = Taon_Up;

        *Taoff_Down = Tcon_Down;
        *Tboff_Down = Tbon_Down;
        *Tcoff_Down = Taon_Down;
    }
    else if(*zone == 5)  // V W U
    {
        *Taoff_Up = Tcon_Up;
        *Tboff_Up = Taon_Up;
        *Tcoff_Up = Tbon_Up;

        *Taoff_Down = Tcon_Down;
        *Tboff_Down = Taon_Down;
        *Tcoff_Down = Tbon_Down;
    }
    else if(*zone == 6)  // W U V
    {
        *Taoff_Up = Tbon_Up;
        *Tboff_Up = Tcon_Up;
        *Tcoff_Up = Taon_Up;

        *Taoff_Down = Tbon_Down;
        *Tboff_Down = Tcon_Down;
        *Tcoff_Down = Taon_Down;
    }
    else
    {
        *Taoff_Up = pwm_full_scale / 2;
        *Tboff_Up = pwm_full_scale / 2;
        *Tcoff_Up = pwm_full_scale / 2;
        *Taoff_Down = pwm_full_scale / 2;
        *Tboff_Down = pwm_full_scale / 2;
        *Tcoff_Down = pwm_full_scale / 2;
    }

    Motor->cmd.i16_Valfa = Valfa;
    Motor->cmd.i16_Vbeta = Vbeta;
    Motor->cmd.i16_Va = Vrefx;
    Motor->cmd.i16_Vb = Vrefy;
    Motor->cmd.i16_Vc = Vrefz;
    Motor->cmd.u8_zone = *zone;
}
