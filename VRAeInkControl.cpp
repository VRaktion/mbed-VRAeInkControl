#include "VRAeInkControl.h"    
    
VRAeInkControl::VRAeInkControl(
    UUID *p_uuid, EventQueue *p_eq, StateChain *p_stateChain, 
    SPI *spi, PinName cs, PinName dc, PinName rst, PinName busy):
    BLEService("eInkCtrl", p_uuid, p_eq, p_stateChain),
    eq(p_eq),
    epd(new Epd(spi, cs, dc, rst, busy))
{
}

void VRAeInkControl::init(){
    printf("[eInkCtrl] init\r\n");
    memset(this->frame_black, 0xFF, sizeof(unsigned char) * EPD_HEIGHT * EPD_WIDTH / 8);

    this->testDraw();

    this->eq->call_in(10000, callback(this, &VRAeInkControl::testPartialDraw));
    // if (this->epd->HDirInit() != 0)
    // {
    //     printf("Init EPD not successful\r\n");
    // }
    // this->epd->Display(IMAGE_DATA);
}

void VRAeInkControl::initCharacteristics(){
    printf("[eInkCtrl] init Characteristics\r\n");
    this->setFramebufferCharacteristic = new BLECharacteristic(
            (uint16_t) VRAeInkControl::Characteristics::SetFramebuffer,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ 
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
            CHAR_LEN_SET_FB//size
        );
    this->addCharacteristic(
        this->setFramebufferCharacteristic
        );
    this->setFramebufferCharacteristic->setWriteCallback(
        new Callback<void(void)>(this, &VRAeInkControl::setFramebufferWriteCb));
}

void VRAeInkControl::testDraw(){
    this->epd->LDirInit();
    this->epd->DrawRectangle(frame_black, 10, 60, 50, 110, COLORED);
    this->epd->DrawLine(frame_black, 10, 60, 50, 110, COLORED);
    this->epd->DrawLine(frame_black, 50, 60, 10, 110, COLORED);
    this->epd->DrawCircle(frame_black, 120, 80, 30, COLORED);
    this->epd->DrawFilledRectangle(frame_black, 10, 130, 50, 180, COLORED);
    this->epd->DrawFilledRectangle(frame_black, 0, 6, 200, 26, COLORED);
    this->epd->DrawFilledCircle(frame_black, 120, 150, 30, COLORED);

    /*Write strings to the buffer */
    this->epd->DrawStringAt(frame_black, 30, 30, "e-Paper Demo", &Font16, COLORED);

    this->epd->DrawStringAt(frame_black, 28, 10, "Hello world!", &Font16, UNCOLORED);

    this->epd->Display(frame_black);

    this->eq->call_in(3000, callback(this->epd, &Epd::Sleep));
}

void VRAeInkControl::testPartialDraw(){
    this->epd->LDirInit();
    // memset(this->frame_black, 0xFF, sizeof(unsigned char) * EPD_HEIGHT * EPD_WIDTH / 8);
    this->epd->ClearFramebuffer(frame_black, UNCOLORED);
    this->epd->DrawFilledRectangle(frame_black, 20, 140, 60, 190, COLORED);
    this->epd->DisplayPart(frame_black);//Partial Refresh
    // this->epd->Display(frame_black);//Full Refresh
    this->eq->call_in(5000, callback(this->epd, &Epd::Sleep));
}

void VRAeInkControl::pastBleInit(){
    printf("[eInkCtrl] pastBleInit\r\n");
}

void VRAeInkControl::onStateOff(){
    printf("[eInkCtrl] off\r\n");
}

void VRAeInkControl::onStateStandby(){
    printf("[eInkCtrl] standby\r\n");
}

void VRAeInkControl::onStateOn(){
    printf("[eInkCtrl] on\r\n");
}

void VRAeInkControl::setFramebufferWriteCb(){
        BLE &ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    uint8_t values[CHAR_LEN_SET_FB];
    uint16_t length = CHAR_LEN_SET_FB;
    ble_error_t err = ble.gattServer().read(
            this->setFramebufferCharacteristic->getValueHandle(), values, &length);
}
