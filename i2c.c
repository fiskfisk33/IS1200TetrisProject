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
void i2c_write(uint8_t *data, int length, uint8_t *address){
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

void i2c_read(uint8_t *data, int length, uint8_t *address){
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

/*---------------------- HERE BE DRAGONS ----------------------/
void deprecated_i2c_read(uint8_t *data, int data_length, uint8_t *address){
        read = 1;
        *rw = 1;
        state = 1;
        r_data = data;
        len = data_length;
        mem_address = address;
        
        assert(1==0);
        
       // while(I2C1CON & 0x1F || I2C1STAT & (1 << 14)); //idle wait

        assert(0);
        I2C1CONSET = SEN;
        while(*rw == 1){    //TODO wait till done;

        }
        assert(1==0);
}

void deprecated_i2c_write(uint8_t *data, int data_length, uint8_t *address){
        read = 0;
        *rw = 1;
        state = 1;
        w_data = data;
        mem_address = address;
        len = data_length;
//TODO remove asserts, they are dangerous.
// 1. Turn on the I2C module by setting the ON bit (I2CxCON<15>) to ‘1’.
       //I2C1CONSET = 0x1 << 15;// this is done in setup.c
// 2. Assert a Start condition on SDAx and SCLx.
        while(I2C1CON & 0x1F || I2C1STAT & (1 << 14)); //idle wait

        I2C1CONSET = SEN;
        while(*rw == 1){    //TODO wait till done;

        }
        //assert(1==0);
        return;
} 

// 3. Send the I2C device address byte to the slave with a write indication.
void deprecated_i2c_send_i2c_address(uint8_t read_bit){
        state++;
        read_bit &= 0x1;
        I2C1TRN = ((ADDRESS << 1) | read_bit); //7 bit address + write bit (0)

}

// 4. Wait for and verify an Acknowledge from the slave.
void deprecated_i2c_send_address_lo(){
        state++;
        assert(!(I2C1STAT & ACKSTAT));
        I2C1TRN = mem_address[0];    
}
void deprecated_i2c_send_address_hi(){
        state++;
        assert(!(I2C1STAT & ACKSTAT));
        I2C1TRN = mem_address[1];    
}

void deprecated_i2c_stop(){
        assert(!(I2C1STAT & ACKSTAT));
        state = 0x0FFFFFFF;
        I2C1CON = (I2C1CON & 0x1F)|0b00100;
        int i = 0;
        while( i < 1000){
                i++;
        }
        state = 0;
        *rw=0;
}

void deprecated_i2c_send_data(){
        assert(!(I2C1STAT & ACKSTAT));
        if ((state - 4) >= len){
                deprecated_i2c_stop();
                return;
        }
        
        I2C1TRN = w_data[state-4];

        state++;
}
void deprecated_i2c_recieve_data(){
        if (state-6 >= len){
                deprecated_i2c_stop();
                return;
        }
        r_data[state-6];
        state++;
        I2C1CONSET = 0x10;//generate ACK;
}

void deprecated_i2c_ack(){
        
}

// 5. Send the serial memory address high byte to the slave.
// 6. Wait for and verify an Acknowledge from the slave.
// 7. Send the serial memory address low byte to the slave.
// 8. Wait for and verify an Acknowledge from the slave.
// 9. Assert a Repeated Start condition on SDAx and SCLx.
// 10. Send the device address byte to the slave with a read indication.
// 11. Wait for and verify an Acknowledge from the slave.
// 12. Enable master reception to receive serial memory data.
// 13. Generate an ACK or NACK condition at the end of a received byte of data.
// 14. Generate a Stop condition on SDAx and SCLx
/*
void deprecated_i2c_interrupt(){
        if(state == 0x0FFFFFFF){
                state = 0;
                *rw = 0;

	        assert(0);
                return;
        }
        if(read == 0){
                if (state == 0)
                        return;
                if (state == 1){
                        i2c_send_i2c_address(0);
                        return;
                }if (state == 2){
                        i2c_send_address_hi();
                        return;
                }if (state == 3){
                        i2c_send_address_lo();
                        return;
                }if (state >= 4){
                        i2c_send_data();
                        return;
                }
        }if(read == 1){
                if (state == 0)
                        return;
                if (state == 1){
                        i2c_send_i2c_address(0);
                        return;
                }if (state == 2){
        assert(1==0);
                        i2c_send_address_hi();
                        return;
                }if (state == 3){
        assert(1==0);
                        i2c_send_address_lo();
                        return;
                }if (state == 4){
        assert(1==0);
                        state++;
                        assert(!(I2C1STAT & ACKSTAT));
                        I2C1CONSET = SEN;
                        return;
                }if (state == 5){
        assert(1==0);
                        i2c_send_i2c_address(1);
                }if (state >= 6){
        assert(1==0);
                        i2c_recieve_data();
                }
        }
}*/