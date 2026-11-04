// Single signal decoder

#include "src/macros.h"
#include "src/NmraDcc.h"
#include "src/ST.h"
#include <EEPROM.h>

/* TODO
 * fix the lights and think out the remaing options. Double bleep, zwaailicht slave mode etc
 * 

*/

#define SET_BIT(byte, bit)      ((byte) |= (1U << (bit)))
#define CLEAR_BIT(byte, bit)    ((byte) &= ~(1U << (bit)))
#define TOGGLE_BIT(byte, bit)   ((byte) ^= (1U << (bit)))
#define WRITE_BIT(byte, bit, val) \
    ((byte) = ((byte) & ~(1U << (bit))) | (((val) & 1U) << (bit)))

typedef struct // settings
{
    uint16_t  myAddress  ; 
    uint8_t  longAddress ;
    uint8_t        state ;
} ST_Settings ;

typedef struct  // could have been 
{
    uint8_t onDirChange ;               //  30 40 50 60
    uint8_t inverted    ;               //  31 41 51 61
    uint8_t number      ;               //  32 42 52 62
    uint8_t bleeps      ;               //  33 43 53 63
    uint8_t bleepOn     ;               //  34 44 54 64
    uint8_t bleepOff    ;               //  35 45 55 65
    uint8_t pulseTime   ;               //  36 46 56 66
    uint8_t slaved      ;               //  37 47 57 67
    uint8_t reserved1   ;               //  38 48 58 68
    uint8_t     state   ;               //  39 49 59 69 // should not be written really.
} ST_Aux ;

uint8_t         myDirection ;



NmraDcc         dcc ;
const int       CV_OFFSET = 30 ;
const int       nAux = 4 ;

ST_Aux          stAux[nAux] ;
ST_Settings     settings ;

const ST_Settings defaultSettings =
{
    .myAddress   = 3,
    .longAddress = 0,
    .state       = 0   // don't care
};

const uint8_t   pins[nAux] = { 3,4,1,0 };
Trigger         trigger[nAux] ;
Timer           timer[nAux] ;

const int       eeSettings =  0 ;
const int       eeCv       = 10 ;
const int       eeDeadbeef = 80 ;


uint8_t checkMyDeadbeef()
{
    uint32_t        MY_DEADBEEF ;
    const uint8_t   DEADBEEF_TRUE   = 1 ;
    const uint8_t   DEADBEEF_FALSE  = 0 ;
    const uint32_t  DEADBEEF        = 0xDEADBEEF ;

    EEPROM.get( eeDeadbeef, MY_DEADBEEF ) ;
    if( MY_DEADBEEF != DEADBEEF ) {
        MY_DEADBEEF  = DEADBEEF ;
        EEPROM.put( eeDeadbeef, MY_DEADBEEF );

        return DEADBEEF_TRUE ;
    }
    return DEADBEEF_FALSE ;
}

void factoryReset() 
{
    for( int i = 0 ; i < nAux ; i ++ )
    {

        stAux[i].onDirChange = 0 ;
        stAux[i].inverted    = 0 ;
        stAux[i].number      = 0 ;
        stAux[i].bleeps      = 0 ;
        stAux[i].bleepOn     = 0 ;
        stAux[i].bleepOff    = 0 ;
        stAux[i].pulseTime   = 0 ;
        stAux[i].slaved      = 0 ;
        stAux[i].reserved1   = 0 ;
        stAux[i].state       = 0 ;
    }    

    settings = defaultSettings ;
    EEPROM.put( eeSettings, settings ) ;
    // whatever is not assigned should be 0 which is what we want
    stAux[0].onDirChange    = 1 ;
    stAux[0].inverted       = 1 ;

    stAux[1].onDirChange    = 1 ;

    stAux[2].number         = 1 ;
    stAux[3].number         = 2 ;

    EEPROM.put( eeCv, stAux ) ;
}

void loadSettings()
{
    EEPROM.get( eeSettings, settings ) ;
    EEPROM.get( eeCv,       stAux    ) ;
}

void setup()
{
    if( checkMyDeadbeef() ) 
    {
        factoryReset() ;
    }

    loadSettings() ;

    for( int i = 0 ; i < nAux ; i ++ )
    {
        pinMode( pins[i], OUTPUT ) ;
    }

    dcc.pin(2,1) ;
    dcc.init(
        MAN_ID_DIY,       // manufacturer ID
        10,               // version
        CV29_F0_LOCATION, // multifunction decoder flags
        0                 // CV29 value (0 = short address)
    );
    //boot() ;
}

    // uint8_t onDirChange ;               //  30 40 50 60
    // uint8_t inverted    ;               //  31 41 51 61
    // uint8_t number      ;               //  32 42 52 62
    // uint8_t bleeps      ;               //  33 43 53 63
    // uint8_t bleepOn     ;               //  34 44 54 64
    // uint8_t bleepOff    ;               //  35 45 55 65
    // uint8_t pulseTime   ;               //  36 46 56 66
    // uint8_t reserved1   ;               //  37 47 57 67
    // uint8_t    slaved   ;               //  38 48 58 68

void boot()
{
    for( int i = 0 ; i < settings.myAddress*3 ; i ++ )
    {
        delay(100);
        digitalWrite( pins[1], HIGH ) ;
        delay(100);
        digitalWrite( pins[1], LOW ) ;
    }   
    delay(2000) ;
}

void blinkAddress()
{
    for( int i = 0 ; i < settings.myAddress ; i ++ )
    {
        delay(300);
        digitalWrite( pins[1], HIGH ) ;
        delay(300);
        digitalWrite( pins[1], LOW ) ;
    }   
    delay(2000) ;
}

void loop()
{  
    dcc.process() ;

    for( uint8_t i = 0 ; i < nAux ; i ++ )
    {
        uint8_t pin   = pins[i] ;
        uint8_t state = stAux[i].state ;
        uint8_t inv   = stAux[i].inverted ;
        uint8_t dir   = stAux[i].onDirChange & myDirection ;


        if( stAux[i].slaved )
        {
            uint8_t masterPin = stAux[i].slaved - 1 ;
            digitalWrite( pin, digitalRead( masterPin) ^ inv ) ; // aux 1-4 ->  pin 0-3
        }
        else if( stAux[i].pulseTime )
        {
            timer[i].PT   = stAux[i].pulseTime * 100 ;
            timer[i].type = TIMER_ON ;

            trigger[i].update( state ^ inv ^ dir ) ;
            if( trigger[i].rose() ) digitalWrite( pin, HIGH ) ;
            
            timer[i].update( digitalRead( pin )) ;
            if( timer[i].Q ) digitalWrite( pin, LOW ) ;
        }
        else if( stAux[i].bleepOn ) // to be developed
        {
            timer[i].update( state ) ;
            timer[i].type = TIMER_TOGGLE ;
        }
        else if( state == 1 )
        {
            digitalWrite( pin, state ^ inv ^ dir ) ;
        }
        else
        {
            digitalWrite( pin, LOW ) ;
        }
    }
}


uint8_t notifyCVWrite (uint16_t CV, uint8_t Value)
{
    uint16_t temp = Value ;
    switch( CV )
    {
    case  1: 
        settings.longAddress  = 0 ;
        settings.myAddress    = temp ;
        //blinkAddress() ;
        goto storeCv ;

    case 17:
        settings.longAddress  = 1 ;
        settings.myAddress    = (settings.myAddress & 0x00FF) | (temp << 8) ;
        goto storeCv ;

    case 18:
        settings.longAddress  = 1 ;
        settings.myAddress    = (settings.myAddress & 0xFF00) | (temp << 0) ;
        goto storeCv ;

    case 29:
        // needed ? I dont do railcom, I could do dir invert, but my ouputs have it to. Dem just 2 CVs.. And I dont do analog, and DCC only. longaddress is dont care
        
    storeCv:
        EEPROM.put( eeSettings, settings ) ;
        break ;

    case 8:
        if( Value == 8 )
        {
            factoryReset() ;
            loadSettings() ;
        }
    }

    if( CV < CV_OFFSET || CV > 69 ) return Value ;
    
    uint8_t *ptr   = (uint8_t*) stAux ;
    ptr           += (CV - CV_OFFSET);    
    *ptr           = Value ;
    
    EEPROM.put( eeCv, stAux ) ;

    return Value ;
}

void setAux( uint8_t number, uint8_t state )
{
    if( state > 0 ) state = 1 ;
    
    for( int i = 0 ; i < nAux ; i ++ )
    {
        if( number == stAux[i].number && stAux[i].state != state  ) 
        {
            stAux[i].state = state ;
            EEPROM.write( eeCv + 9 + 10*i , stAux[i].state ) ; // base address + 9 (state CV offset) 
        }
    }
}


void notifyDccFunc (uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState)
{
    if( Addr != settings.myAddress ) return ;

    if( FuncGrp == FN_0_4   )
    {
        setAux( 0, FuncState & FN_BIT_00 ) ;
        setAux( 1, FuncState & FN_BIT_01 ) ;
        setAux( 2, FuncState & FN_BIT_02 ) ;
        setAux( 3, FuncState & FN_BIT_03 ) ;
        setAux( 4, FuncState & FN_BIT_04 ) ;
    }
    if( FuncGrp == FN_5_8   )
    {
        setAux( 5, FuncState & FN_BIT_05 ) ;
        setAux( 6, FuncState & FN_BIT_06 ) ;
        setAux( 7, FuncState & FN_BIT_07 ) ;
        setAux( 8, FuncState & FN_BIT_08 ) ;
    }
    if( FuncGrp == FN_9_12  )
    {
        setAux(  9, FuncState & FN_BIT_09 ) ;
        setAux( 10, FuncState & FN_BIT_10 ) ;
        setAux( 11, FuncState & FN_BIT_11 ) ;
        setAux( 12, FuncState & FN_BIT_12 ) ;
    }
    if( FuncGrp == FN_13_20 )
    {
        setAux( 13, FuncState & FN_BIT_13 ) ;
        setAux( 14, FuncState & FN_BIT_14 ) ;
        setAux( 15, FuncState & FN_BIT_15 ) ;
        setAux( 16, FuncState & FN_BIT_16 ) ;
        setAux( 17, FuncState & FN_BIT_17 ) ;
        setAux( 18, FuncState & FN_BIT_18 ) ;
        setAux( 19, FuncState & FN_BIT_19 ) ;
        setAux( 20, FuncState & FN_BIT_20 ) ;
    }
    if( FuncGrp == FN_21_28 )
    {
        setAux( 21, FuncState & FN_BIT_21 ) ;
        setAux( 22, FuncState & FN_BIT_22 ) ;
        setAux( 23, FuncState & FN_BIT_23 ) ;
        setAux( 24, FuncState & FN_BIT_24 ) ;
        setAux( 25, FuncState & FN_BIT_25 ) ;
        setAux( 26, FuncState & FN_BIT_26 ) ;
        setAux( 27, FuncState & FN_BIT_27 ) ;
        setAux( 28, FuncState & FN_BIT_28 ) ;
    }
}
void notifyDccSpeed (uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) 
{
    if( Addr != settings.myAddress ) return ;

    myDirection = Dir ;
}