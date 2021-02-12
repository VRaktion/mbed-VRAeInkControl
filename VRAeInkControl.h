#ifndef VRA_EINK_CONTROL_H
#define VRA_EINK_CONTROL_H

#include "BLEService.h"
#include "BLEBulkWriteCharacteristic.h"

// #include "VRASettings.h"
// #include "VRAStorage.h"
#include "IntervalEvent.h"
#include "epd1in54v2.h"

#define CHAR_LEN_SET_FB 20

class VRAeInkControl : public BLEService
{
public:
    enum class Characteristics: uint16_t
    {
        SetFramebuffer = 0xFF00,
    };

    VRAeInkControl(UUID *p_uuid, EventQueue *p_eq, StateChain *p_stateChain, 
    SPI *spi, PinName cs, PinName dc, PinName rst, PinName busy);

    void init();
    void initCharacteristics();
    void pastBleInit();

    void testDraw();
    void testPartialDraw();

private:
    void recvNewFrabmebufferCb();

    void onStateOff();
    void onStateStandby();
    void onStateOn();

    void setFramebufferCb();

    void updateDisplay();
    void clearFramebuffer();

    EventQueue *eq;
    // VRASettings *settings;
    // VRAStorage *storage;
    Epd *epd;
    BLECharacteristic* setFramebufferCharacteristic;
    unsigned char framebuffer[EPD_HEIGHT * EPD_WIDTH / 8];
    static constexpr int refreshCycle{0};
    int refreshCounter{0};
};

#endif //VRA_EINK_CONTROL_H