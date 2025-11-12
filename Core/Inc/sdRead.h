#ifndef sd_Read_H
#define sd_Read_H


void setSpiMode();
void CS_Select();
void CS_UnSelect();
void sendCommand(uint8_t* cmd, int size);




#endif
