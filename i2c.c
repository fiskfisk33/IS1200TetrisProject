#include <assert.h>
#include <stdint.h>
#include "mipslab.h"
#include "pic32mx.h"
#include "tetris.h"

/* i2c.c
 * This file written 2023 by Jonathan Johnson
 */
/* CON */
#define SEN  0b01
#define RSEN 0b010
#define PEN  0b0100
#define RCEN 0b01000
#define ACKEN 0b010000
#define ACKDT 0b0100000

/* STAT */
#define RBF 0b010       //recieve buffer full
#define STAT_P 0b010000 //I2C1STAT
#define STAT_S 0b001000 //I2C1STAT
#define TRSTAT 0x4000
#define ACKSTAT 0x08000


#define ADDRESS 0b10100000
volatile uint32_t state;
//uint8_t *w_data;
//uint8_t *r_data;
//uint8_t *mem_address;
//int read;
//int     len;


/*send start signal*/
void i2c_start(){
        I2C1CONSET = SEN;
        while(I2C1CON & SEN);
}

/*send restart*/
void i2c_restart(){
        I2C1CONSET = RSEN;
        while(I2C1CON & RSEN);
}
/*send byte*/
uint8_t i2c_send(uint8_t byte){
        I2C1TRN = byte;
        while(I2C1STAT & TRSTAT); //wait while sending data
        if(I2C1STAT & ACKSTAT){
                //Error, reciever has not ACKed
                //assert(0);
                return 0;
        } 
        return 1;
}
/*recieve byte*/
uint8_t i2c_recieve_byte(){
        while(I2C1CON & 0b011111){
                assert(0);
        }
        
        I2C1CONSET = RCEN; //recieve enable
        while(!(I2C1STAT & RBF)); //wait while recieving
        return I2C1RCV;    //return recieved data
}
/*send ack*/
void i2c_ack(){
        I2C1CONCLR = ACKDT;
        I2C1CONSET = ACKEN; //initiate ack sequence
        while(I2C1CON & ACKEN); //wait for ack sequence to finish
}
/*send nack*/
void i2c_nack(){
        I2C1CONSET = ACKDT;
        I2C1CONSET = ACKEN; //initiate ack sequence
        while(I2C1CON & ACKEN); //wait for ack sequence to finish
}
/*send stop*/
void i2c_stop(){
        I2C1CONSET = PEN;     //initiate stop sequence
        while(I2C1CON & PEN); //wait for stop sequence to finish
}
/*
        TODO this is a test
*/
void i2c_write_eeprom(uint8_t *data, int length, uint8_t *address){
        uint8_t success = 0;
        do{
                i2c_start();
        }while(!i2c_send(ADDRESS)); //retry if no ack
        i2c_send(address[0]);
        i2c_send(address[1]);
        for(int i = 0; i < length; i++){
                i2c_send(data[i]);
        }
        i2c_stop();
}

void i2c_read_eeprom(uint8_t *data, int length, uint8_t *address){
        uint8_t success = 0;
        do{
                i2c_start();
        }while(!i2c_send(ADDRESS)); //retry if no ack
        i2c_send(address[0]);
        i2c_send(address[1]);
        i2c_restart();
        i2c_send(ADDRESS | 0b01);
        for(int i = 0; i < length; i++){
                data[i] = i2c_recieve_byte();
                if(i == length-1){
                        i2c_nack();
                }else{
                        i2c_ack();
                }
        }
        i2c_stop();
}
