/*
 * CAN.c
 *
 * Created: 13.10.2021 11:24:50
 *  Author: mbmartin
 */

#define F_CPU 4915200

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "SPI.h"
#include "mcp2515.h"
#include "CAN.h"

bool receive_flag;


void CAN_init(){
	mcp2515_init();
}

void CAN_send_message(CAN_message *message){
	/* Sending message ID */
	mcp2515_write(MCP_TXB0SIDH, (message->id) >> 3);
	mcp2515_write(MCP_TXB0SIDL, (message->id) << 5);
	
	/* Sending data length */
	mcp2515_write(MCP_TXB0DLC, message->length);
	
	/* Sending data */
	for(uint8_t i = 0; i < (message->length); i++){
		mcp2515_write(MCP_TXB0D0 + i, message->data[i]);
	}

	/* Using the Transmission Buffer 0 */
	mcp2515_request_to_send('0');
}

void CAN_receive_message(CAN_message* message){

	/* Receive message id */
	message->id = (mcp2515_read(MCP_RXB0SIDH)<<3) + (mcp2515_read(MCP_RXB0SIDL)>>5);
	
	/* Receive message length */
	message->length = mcp2515_read(MCP_RXB0DLC);
	
	/* Receive message data */
	for(uint8_t i = 0; i < message->length; i++){
		message->data[i] = mcp2515_read(MCP_RXB0DM0	+ i);
	}
	
 	//printf("%d %d %d %d \n\r", message->data[0], message->data[1], message->data[2], message->data[3]);
	
	/* Clearing flags */
	mcp2515_bit_modify(MCP_CANINTF, 0x03, 0x00);
}

void send_controls(void){
	transmission_container.id = 10;
	transmission_container.length = 3;
	transmission_container.data[0] = joy.Joy_x;
	transmission_container.data[1] = joy.Slider_right;
	transmission_container.data[2] = joy.joy_push;
	CAN_send_message(&transmission_container);
}

void send_difficulty(void){
	transmission_container.id = 40;
	transmission_container.length = 1;
	transmission_container.data[0] = difficulty_indicator;
	CAN_send_message(&transmission_container);
}
	
void can_debugging_test_send(void){
		CAN_message test;
		test.id = 254;
		test.length = 8;
		test.data[0] = 0x00;
		test.data[1] = 0x01;
		test.data[2] = 0x02;
		test.data[3] = 0x03;
		test.data[4] = 0x04;
		test.data[5] = 0x05;
		test.data[6] = 0x06;
		CAN_send_message(&test);
}

ISR(INT1_vect){
	CAN_receive_message(&reception_container);
	if(reception_container.id == 60){
		score = reception_container.data[0];
		playing_game_flag = false;
		//printf("%d\n\r", score);
	}
	printf("here\n\r");
	//printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n\r", reception_container.data[0], reception_container.data[1], reception_container.data[2], reception_container.data[3], reception_container.data[4], reception_container.data[5], reception_container.data[6], reception_container.data[7]);
}




