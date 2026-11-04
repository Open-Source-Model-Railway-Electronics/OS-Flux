// Single signal decoder

#include "src/macros.h"
#include "src/NmraDcc.h"
#include "src/ST.h"
#include <EEPROM.h>


#define SET_BIT(byte, bit)      ((byte) |= (1U << (bit)))
#define CLEAR_BIT(byte, bit)    ((byte) &= ~(1U << (bit)))
#define TOGGLE_BIT(byte, bit)   ((byte) ^= (1U << (bit)))
#define WRITE_BIT(byte, bit, val) \
    ((byte) = ((byte) & ~(1U << (bit))) | (((val) & 1U) << (bit)))

uint8_t                 aspectMask ;
uint8_t                 blinkMask ;
static const uint8_t    ledBits[]   = {3, 4, 1, 0};
uint16_t                myAddr ;
uint8_t                 getAddress ;
uint8_t                 dccExt ;

Timer       buttonTimer ;

Timer       signalTimer ;
const int   nOutputs = 4 ;

NmraDcc dcc ;

void setup()
{
    dcc.pin(2, 0);
    dcc.init(MAN_ID_DIY, 11, FLAGS_OUTPUT_ADDRESS_MODE | FLAGS_DCC_ACCESSORY_DECODER, 0);

    signalTimer.set( TIMER_BLEEP, 333 ) ;

    pinMode( ledBits[0], OUTPUT ) ;
    pinMode( ledBits[1], OUTPUT ) ;
    pinMode( ledBits[2], OUTPUT ) ;
    pinMode( ledBits[3], OUTPUT ) ;

    buttonTimer.set( TIMER_ON, 1000 ) ;

    uint32_t deadbeef ;
    EEPROM.get( 5, deadbeef ) ;
    if( deadbeef != 0xDEADBEEF )
    {   deadbeef  = 0xDEADBEEF ;

        myAddr = 1 ;
        dccExt = 1 ;
        EEPROM.put( 5, deadbeef ) ;
        EEPROM.put( 0,   myAddr ) ;
        EEPROM.put( 4,   dccExt ) ;
    }

    EEPROM.get( 0,     myAddr ) ;
    EEPROM.get( 2, aspectMask ) ;
    EEPROM.get( 3,  blinkMask ) ;
    EEPROM.get( 4,     dccExt ) ;
}

void loop()
{
    if( buttonTimer.update( !digitalRead(2) ) )
    {
        getAddress = 1 ;
    }
    
    dcc.process() ;

    handleSignal() ;
}
 
void notifyDccAccTurnoutOutput (uint16_t Addr, uint8_t Direction, uint8_t OutputPower)
{
    if( getAddress )
    {
        getAddress  =    0 ;
        myAddr      = Addr ;
        dccExt      =    0 ;
        EEPROM.put( 0, myAddr ) ;
        EEPROM.put( 4, dccExt ) ;
    }
}

void notifyDccSigOutputState( uint16_t Addr, uint8_t State )
{
    if( getAddress )
    {
        getAddress  =    0 ;
        myAddr      = Addr ;
        dccExt      =    1 ;
        EEPROM.put( 0, myAddr ) ;
        EEPROM.put( 4, dccExt ) ;
    }

    if( Addr   != myAddr ) return ;
    if( dccExt ==      0 ) return ;

    aspectMask  =  State       & 0x0F ;
    blinkMask   = (State >> 4) & 0x0F ;
    EEPROM.put( 2, aspectMask ) ;
    EEPROM.put( 3, blinkMask  ) ;
}


// groen wit geel rood
void handleSignal()
{
    static uint8_t blinkState = 0;

    if (!signalTimer.update(1))
        return;

    blinkState ^= 1;

    uint8_t tempBlink  = blinkMask;
    uint8_t tempAspect = aspectMask;

    if (getAddress)
    {
        tempBlink  = 0x0F;
        tempAspect = 0x00;
    }

    for (int i = 0; i < nOutputs; i++)
    {
        uint8_t bit = ledBits[i];

        if (tempBlink & (1U << i))
        {
            uint8_t out;

            if (tempAspect & (1U << i)) out = !blinkState ;
            else                        out =  blinkState ;

            WRITE_BIT(PORTB, bit, out);
        }
        else
        {
            WRITE_BIT(PORTB, bit, (tempAspect >> i) & 1U);
        }
    }
}