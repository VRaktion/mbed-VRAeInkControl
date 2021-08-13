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
    memset(this->framebuffer, 0xFF, sizeof(unsigned char) * EPD_HEIGHT * EPD_WIDTH / 8);
    this->epd->LDirInit();
    this->testDraw();

    // this->eq->call_in(10000, callback(this, &VRAeInkControl::testPartialDraw));
    // if (this->epd->HDirInit() != 0)
    // {
    //     printf("Init EPD not successful\r\n");
    // }
    // this->epd->Display(IMAGE_DATA);
}

void VRAeInkControl::initCharacteristics(){
    printf("[eInkCtrl] init Characteristics\r\n");
    this->setFramebufferCharacteristic = new BLEBulkWriteCharacteristic(
            (uint16_t) VRAeInkControl::Characteristics::SetFramebuffer,
            this->framebuffer,
            EPD_HEIGHT * EPD_WIDTH / 8,
            this->eq,
            callback(this, &VRAeInkControl::recvNewFrabmebufferCb)
        );
    this->addCharacteristic(
        this->setFramebufferCharacteristic
        );
}

void VRAeInkControl::testDraw(){
    this->epd->LDirInit();
    this->epd->DrawRectangle(framebuffer, 10, 60, 50, 110, COLORED);
    this->epd->DrawLine(framebuffer, 10, 60, 50, 110, COLORED);
    this->epd->DrawLine(framebuffer, 50, 60, 10, 110, COLORED);
    this->epd->DrawCircle(framebuffer, 120, 80, 30, COLORED);
    this->epd->DrawFilledRectangle(framebuffer, 10, 130, 50, 180, COLORED);
    this->epd->DrawFilledRectangle(framebuffer, 0, 6, 200, 26, COLORED);
    this->epd->DrawFilledCircle(framebuffer, 120, 150, 30, COLORED);

    /*Write strings to the buffer */
    this->epd->DrawStringAt(framebuffer, 30, 30, "CHEAL5", &Font16, COLORED);

    this->epd->DrawStringAt(framebuffer, 28, 10, "VRaktion", &Font16, UNCOLORED);
    this->updateDisplay();
}

void VRAeInkControl::testPartialDraw(){
    this->epd->LDirInit();
    this->clearFramebuffer();
    this->epd->DrawFilledRectangle(framebuffer, 20, 140, 60, 190, COLORED);
    this->updateDisplay();
}

void VRAeInkControl::updateDisplay(){
    if(++this->refreshCounter > this->refreshCycle){
        this->refreshCounter = 0;
        this->epd->Display(this->framebuffer);//Full Refresh
    }else{
        this->epd->DisplayPart(this->framebuffer);//Partial Refresh
    }
    // this->eq->call_in(5000, callback(this->epd, &Epd::Sleep));
}

void VRAeInkControl::clearFramebuffer(){
    memset(this->framebuffer, 0xFF, sizeof(unsigned char) * EPD_HEIGHT * EPD_WIDTH / 8);
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

void VRAeInkControl::recvNewFrabmebufferCb(){
    printf("new framebuffer received\r\n");
    this->updateDisplay();
}
