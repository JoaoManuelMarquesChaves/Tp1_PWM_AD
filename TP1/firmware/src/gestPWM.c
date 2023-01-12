/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2016-2017
//
//	Auteur 		: 	C. HUBER
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
/*--------------------------------------------------------*/



#include "GestPWM.h"
#include "Mc32DriverLcd.h"
#include "Mc32DriverAdc.h"
#include "app.h"
#include "peripheral/oc/plib_oc.h"

S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    
   // Init état du pont en H
   BSP_EnableHbrige();
   // lance les timers et OC
   DRV_OC0_Start();
   DRV_OC1_Start();
   DRV_TMR0_Start();
   DRV_TMR1_Start();
   DRV_TMR2_Start();
   DRV_TMR3_Start();
    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData )	
{
    static uint16_t Memory_Angle[10]={};
    static uint16_t Memory_Speed[10]={};
    static uint16_t Moyenne_Angle = 0;
    static uint16_t Moyenne_Speed = 0;
    static uint16_t i=0;
    S_ADCResults ReadData;
    // Lecture du convertisseur AD
    ReadData = BSP_ReadAllADC();
    Memory_Speed[i]= ReadData.Chan0;
    Memory_Angle[i]= ReadData.Chan1;
    i++;
  
    if(i>= 10)
    {

        i=0;
    }
    // valeur moyenne glisante 
    Moyenne_Speed =(Memory_Speed[0]+Memory_Speed[1]
            +Memory_Speed[2]+Memory_Speed[3]
            +Memory_Speed[4]+Memory_Speed[5]
            +Memory_Speed[6]+Memory_Speed[7]
            +Memory_Speed[8]+Memory_Speed[9])/TAILLE_FILTRE ;
    
    Moyenne_Angle = (Memory_Angle[0]+Memory_Angle[1]
            +Memory_Angle[2]+Memory_Angle[3]
            +Memory_Angle[4]+Memory_Angle[5]
            +Memory_Angle[6]+Memory_Angle[7]
            +Memory_Angle[8]+Memory_Angle[9])/TAILLE_FILTRE ;
    //calcul pour avoir la valeur brut    
    pData->absSpeed = ((Moyenne_Speed*VITESSE_MAX)/RESOLUTION_ADC );
    pData->absAngle = ((Moyenne_Angle*ANGLE_MAX)/RESOLUTION_ADC );

    // conversion 
    pData->SpeedSetting = pData->absSpeed -99;
    pData->AngleSetting = pData->absAngle -90;
    // conversion pour avoir absSpeed de 99 0 99
    if(pData->SpeedSetting < 0 )
    {
        pData->absSpeed = 0-pData->SpeedSetting; 
    }
    else
    {
        pData->absSpeed = pData->SpeedSetting;
    }
}

// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    lcd_gotoxy(1,1 );
    printf_lcd("Tp1 PWM 2022-23");
    lcd_gotoxy(1,2 );      // ecrire sur la deuxieme ligne
    printf_lcd("SpeedSetting %3d",pData->SpeedSetting);
    lcd_gotoxy(1,3 );      // ecrire sur la deuxieme ligne
    printf_lcd("absSpeed %2d",pData->absSpeed);
    lcd_gotoxy(1,4 );      // ecrire sur la deuxieme ligne
    printf_lcd("Angle %3d",pData->AngleSetting);
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    uint16_t LargeurT2 = 0;
    uint16_t LargeurT3 = 0;
    float Angle = 0;
 // vitesse
 // gestion direction pont H
    // sens horaire
    if(pData->SpeedSetting < 0)
    {
        AIN1_HBRIDGE_W = 1; 
        AIN2_HBRIDGE_W = 0; 
        STBY_HBRIDGE_W = 1; 
    }
    // sens anti horaire
    if (pData->SpeedSetting > 0)
    {
        AIN1_HBRIDGE_W = 0; 
        AIN2_HBRIDGE_W = 1; 
        STBY_HBRIDGE_W = 1; 
    }
    // stop
    if (pData->SpeedSetting == 0)
    {
        STBY_HBRIDGE_W = 0;      
    }

     /* periode du timer */
    LargeurT2 = DRV_TMR1_PeriodValueGet();  
    /* Calcul du rapport */
    LargeurT2 = LargeurT2 * (float)(pData->absSpeed/99.0);
    /* pulse du PWM */
    PLIB_OC_PulseWidth16BitSet(_OCMP2_BASE_ADDRESS, LargeurT2);
    
    /* Calcul du ratio de l'angle */
    Angle = (float)((pData->AngleSetting+90)/180.0);
    /* Calcul de la largeur de la pulse */
    LargeurT3 = (DRV_TMR2_CounterFrequencyGet()/(1/(MAX_SERVO - MIN_SERVO )))*Angle;
    /* Ajout du minimum pour le servo moteur */
    LargeurT3 += DRV_TMR2_CounterFrequencyGet()/(1/(MIN_SERVO ));    
    //génération d'une impulsion dont la largeur est proportionnelle à l'angle
    PLIB_OC_PulseWidth16BitSet(_OCMP3_BASE_ADDRESS, LargeurT3);
 
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    uint8_t pwmCnt;
    
    /* Incrément PWM */
    pwmCnt++;
    
    /* Gestion du temps ON*/
    if(pwmCnt <= pData->absSpeed)
    {
        BSP_LEDOn(BSP_LED_5);
        
    /* Temps OFF */
    }
    else 
    {
        BSP_LEDOff(BSP_LED_5);

    }
    if(pwmCnt >= 100)
    {
        pwmCnt = 0;
    }
}
