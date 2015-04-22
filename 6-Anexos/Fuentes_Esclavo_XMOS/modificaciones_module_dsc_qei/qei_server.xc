/**
 * Module:  module_dsc_qei
 *
 * The copyrights, all other intellectual and industrial 
 * property rights are retained by XMOS and/or its licensors. 
 * Terms and conditions covering the use of this code can
 * be found in the Xmos End User License Agreement.
 *
 * Copyright XMOS Ltd 2010
 *
 * In the case where this code is a modification of existing code
 * under a separate license, the separate license terms are shown
 * below. The modifications to the code are still covered by the 
 * copyright notice above.
 *
 **/                                   
#include <xs1.h>
#include "qei_server.h"
#include "qei_commands.h"
#include "qei_client.h"
//#include "dsc_config.h"
#include <stdio.h>

#ifndef NUMBER_OF_MOTORS
#define NUMBER_OF_MOTORS 2
#endif

// This is the loop time for 4000RPM on a 1024 count QEI
#pragma xta command "analyze loop qei_main_loop"
#pragma xta command "set required - 14.64 us"

// Order is 00 -> 10 -> 11 -> 01
static const unsigned char lookup[16][4] = {
		{ 5, 4, 6, 5 }, // 00 00
		{ 6, 5, 5, 4 }, // 00 01
		{ 4, 5, 5, 6 }, // 00 10
		{ 5, 6, 4, 5 }, // 00 11
		{ 0, 0, 0, 0 }, // 01 xx
		{ 0, 0, 0, 0 }, // 01 xx
		{ 0, 0, 0, 0 }, // 01 xx
		{ 0, 0, 0, 0 }, // 01 xx

		{ 5, 4, 6, 5 }, // 10 00
		{ 6, 5, 5, 4 }, // 10 01
		{ 4, 5, 5, 6 }, // 10 10
		{ 5, 6, 4, 5 }, // 10 11
		{ 0, 0, 0, 0 }, // 11 xx
		{ 0, 0, 0, 0 }, // 11 xx
		{ 0, 0, 0, 0 }, // 11 xx
		{ 0, 0, 0, 0 }  // 11 xx
};


#pragma unsafe arrays
void do_qei ( streaming chanend c_qei, port in pQEI )
{
	unsigned pos = 0, v, ts1, ts2, ok=0, old_pins=0, new_pins;
	timer t;

	pQEI :> new_pins;
	t :> ts1;

	while (1) {
#pragma xta endpoint "qei_main_loop"
		select {
			case pQEI when pinsneq(new_pins) :> new_pins :
			{
				if ((new_pins & 0x3) != old_pins) {
					ts2 = ts1;
					t :> ts1;
				}
				v = lookup[new_pins][old_pins];
				if (!v) {
					pos = 0;
					ok = 1;
				} else {
					{ v, pos } = lmul(1, pos, v, -5);
				}
				old_pins = new_pins & 0x3;
			}
			break;
			case c_qei :> int :
			{
				c_qei <: pos;
				c_qei <: ts1;
				c_qei <: ts2;
				c_qei <: ok;
			}
			break;
		}
	}
}

#pragma unsafe arrays
void do_multiple_qei ( streaming chanend c_qei[], port in pQEI[] )
{
	unsigned pos[NUMBER_OF_MOTORS], v, ts1[NUMBER_OF_MOTORS], ts2[NUMBER_OF_MOTORS];
	unsigned ok[NUMBER_OF_MOTORS], old_pins[NUMBER_OF_MOTORS], new_pins[NUMBER_OF_MOTORS];
	timer t;

	for (int q=0; q<NUMBER_OF_MOTORS; ++q) {
		old_pins[q] = 0;
		pQEI[q] :> new_pins[q];
		t :> ts1[q];
		pos[q] = 0;
		ok[q] = 0;
	}

	while (1) {
#pragma xta endpoint "qei_main_loop"
		select {
			case (int q=0; q<NUMBER_OF_MOTORS; ++q) pQEI[q] when pinsneq(new_pins[q]) :> new_pins[q] :
			{
				if ((new_pins[q] & 0x3) != old_pins[q]) {
					ts2[q] = ts1[q];
					t :> ts1[q];
				}
				v = lookup[new_pins[q]][old_pins[q]];
				if (!v) {
					pos[q] = 0;
					ok[q] = 1;
				} else {
					{ v, pos[q] } = lmul(1, pos[q], v, -5);
				}
				old_pins[q] = new_pins[q] & 0x3;
			}
			break;
			case (int q=0; q<NUMBER_OF_MOTORS; ++q) c_qei[q] :> int :
			{
				c_qei[q] <: pos[q];
				c_qei[q] <: ts1[q];
				c_qei[q] <: ts2[q];
				c_qei[q] <: ok[q];
			}
			break;
		}
	}
}

// TODO RTnet: ensayar esta función

// *******************************************************************************************************
// Esta rutina inicia un servicio de hasta 4 encoders en cuadratura conectados a un mismo puerto de 8 bits
// o hasta 2 encoders en cuadratura conectados a un mismo puerto de 4 buts.
// Está basada en la rutina do_multiple_qei() pero considerando conjuntos de 2 bits a partir de un puerto
// de entrada de 4 u 8 bits.
// Encoder 0: bits 0,1  Encoder1: bits 2,3  etc...
#define p_qei(uc,q) (uc >> (2*q)) & 0b11
// *******************************************************************************************************
#pragma unsafe arrays
void do_multi4_qei ( streaming chanend c_qei[], port in pQEI )
{
        unsigned pos[NUMBER_OF_MOTORS], v, ts1[NUMBER_OF_MOTORS], ts2[NUMBER_OF_MOTORS];
        unsigned ok[NUMBER_OF_MOTORS], old_pins[NUMBER_OF_MOTORS], new_pins;
        timer t;
        unsigned char uc;

        pQEI :> new_pins;
        for (int q=0; q<NUMBER_OF_MOTORS; ++q) {
            old_pins[q]=0;
            t :> ts1[q];
            pos[q] = 0;
            ok[q] = 0;
        }

        while (1) {
#pragma xta endpoint "qei_main_loop"
                select {
                        case pQEI when pinsneq(new_pins) :> new_pins :
                        {
                          for(int q=0; q<NUMBER_OF_MOTORS; ++q){
                              if ((p_qei(new_pins,q) & 0x3) != old_pins[q]) {
                                      ts2[q] = ts1[q];
                                      t :> ts1[q];
                              }
                              v = lookup[p_qei(new_pins,q)][old_pins[q]];
                              if (!v) {
                                      pos[q] = 0;
                                      ok[q] = 1;
                              } else {
                                      { v, pos[q] } = lmul(1, pos[q], v, -5);
                              }
                              old_pins[q] = p_qei(new_pins,q) & 0x3;
                          }
                        }
                        break;
                        case (int q=0; q<NUMBER_OF_MOTORS; ++q) c_qei[q] :> int :
                        {
                                c_qei[q] <: pos[q];
                                c_qei[q] <: ts1[q];
                                c_qei[q] <: ts2[q];
                                c_qei[q] <: ok[q];
                        }
                        break;
                }
        }
}


