/*
 * line.h
 *
 *  Created on: Jan 6, 2019
 *      Author: Cuong
 */

#ifndef LINE_H_
#define LINE_H_

unsigned short line[0x640] ={
0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xA6CA, 0xBF8E, 0xBF6D, 0xBF6D, 0xBF8E, 0xA6CB, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xAEEB, 0xBF8E,   // 0x0010 (16)
0xBF6D, 0xBF8D, 0xBF8E, 0xA6AB, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xAF0C, 0xBF8D, 0xBF6D, 0xBF6D, 0xBF6D, 0xA6AA, 0x9EAA, 0x9EAA,   // 0x0020 (32)
0x9EAA, 0x9E8A, 0xAF2C, 0xBF8D, 0xBF6D, 0xBF8D, 0xBF8E, 0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB72C, 0xBF6D, 0xBF6D, 0xBF8D,   // 0x0030 (48)
0xB74D, 0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB76D, 0xBF6D, 0xB76D, 0xBF8E, 0xB74C, 0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA, 0x9EAA,   // 0x0040 (64)
0xB76D, 0xBF6D, 0xBF6D, 0xBF8E, 0xAF2C, 0x9E8A, 0x9EAA, 0x9EAA, 0x9E8A, 0x9EAA, 0xBF8D, 0xBF6D, 0xBF6D, 0xBF8E, 0xAEEB, 0x9EAA,   // 0x0050 (80)
0x9EAA, 0x9EAA, 0x9E8A, 0xA6CB, 0xBF8E, 0xBF6D, 0xBF6D, 0xBF8E, 0xA6EB, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xA6CB, 0xBF8E, 0xBF6D,   // 0x0060 (96)
0xBF6D, 0xBF8D, 0xA6CB, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xAEEB, 0xBF8D, 0xBF6D, 0xBF8D, 0xBF8E, 0x9EAA, 0x9EAA, 0x9EAA, 0x9EAA,   // 0x0070 (112)
0x9E8A, 0xAF0C, 0xBF8D, 0xBF6D, 0xBF8D, 0xBF6D, 0x9EAA, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB72C, 0xBF6D, 0xBF6D, 0xBF8E, 0xB76D,   // 0x0080 (128)
0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB74D, 0xBF6D, 0xBF6D, 0xBF8E, 0xB74D, 0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB76D,   // 0x0090 (144)
0xBF6D, 0xBF6D, 0xBF8E, 0xAF2C, 0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA, 0x9EAA, 0xBF8E, 0xBF6D, 0xBF6D, 0xBF8E, 0xAF0C, 0x9EAA, 0x9EAA,   // 0x00A0 (160)
0x9EAA, 0x9E8A, 0xA6CB, 0xBF6D, 0xBF6D, 0xBF6D, 0xBF8E, 0xAEEB, 0x9E8A, 0x9EAA, 0x9EAA, 0x9E8A, 0xA6CB, 0xBF8D, 0xBF6D, 0xBF6D,   // 0x00B0 (176)
0xBF8E, 0xA6CB, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xA6EB, 0xBF8E, 0xBF6D, 0xBF6D, 0xBF8E, 0x9EAA, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A,   // 0x00C0 (192)
0xAF0C, 0xBF8D, 0xBF6D, 0xBF8D, 0xB76D, 0xA6AA, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xAF2C, 0xBF8D, 0xBF6D, 0xBF8D, 0xBF8D, 0x9E8A,   // 0x00D0 (208)
0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB72C, 0xBF6D, 0xBF6D, 0xBF8E, 0xB74D, 0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB76D, 0xBF6D,   // 0x00E0 (224)
0xBF6D, 0xBF8E, 0xB72C, 0x9E8A, 0x9EAA, 0x9EAA, 0x9E8A, 0x9EAA, 0xB76D, 0xBF6D, 0xBF6D, 0xBF8E, 0xAF2C, 0x9E8A, 0x9EAA, 0x9EAA,   // 0x00F0 (240)
0x9E8A, 0xA6AA, 0xBF8D, 0xBF6D, 0xBF6D, 0xBF8E, 0xAF0B, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xA6CA, 0xBF8E, 0xBF6D, 0xBF6D, 0xBF8E,   // 0x0100 (256)
0xA6CB, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xAEEB, 0xBF8D, 0xBF6D, 0xBF6D, 0xBF8E, 0xA6CB, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xAF0C,   // 0x0110 (272)
0xBF8D, 0xBF6D, 0xBF8D, 0xBF8D, 0xA6AA, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xAF2C, 0xBF8D, 0xBF6D, 0xBF8D, 0xBF6D, 0x9E8A, 0x9EAA,   // 0x0120 (288)
0x9EAA, 0x9EAA, 0x9E8A, 0xB74D, 0xBF6D, 0xBF6D, 0xBF8D, 0xB74D, 0x9EAA, 0x9EAA, 0x9EAA, 0x9EAA, 0x9E8A, 0xB76D, 0xBF6D, 0xBF6D,   // 0x0130 (304)
0xBF8E, 0xB74D, 0x9E8A, 0x9EAA, 0x9EAA, 0x9E8A, 0x9EAA, 0xB76D, 0xBF6D, 0xBF6D, 0xBF8E, 0xAF2C, 0x9E8A, 0x9EAA, 0x9EAA, 0x9EAA,   // 0x0140 (320)
0x6DE5, 0x6DE5, 0x6DC5, 0x7626, 0x9F2A, 0x9F2A, 0x9F4A, 0x9F4B, 0x7E67, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7E47, 0x972A, 0x9F2A,   // 0x0150 (336)
0x9F4B, 0x972A, 0x7E47, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x8667, 0x9F2A, 0x9F2A, 0x9F4B, 0x972A, 0x7626, 0x6DE5, 0x6DE5, 0x6DC5,   // 0x0160 (352)
0x6DE5, 0x8688, 0x9F4A, 0x9F4A, 0x9F4B, 0x970A, 0x7605, 0x6DC5, 0x6DE5, 0x6DC5, 0x6DE5, 0x8EA8, 0x9F2A, 0x9F2A, 0x9F4B, 0x96E9,   // 0x0170 (368)
0x75E5, 0x6DE5, 0x6DE5, 0x6DC5, 0x6DE5, 0x8EC9, 0x9F4A, 0x9F2A, 0x9F4B, 0x8EE9, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7606, 0x96E9,   // 0x0180 (384)
0x9F2A, 0x9F4A, 0x9F4B, 0x8EA8, 0x6DE5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7606, 0x972A, 0x9F2A, 0x9F2A, 0x9F4B, 0x8688, 0x6DC5, 0x6DE5,   // 0x0190 (400)
0x6DE5, 0x6DC5, 0x7E26, 0x970A, 0x9F4A, 0x9F4A, 0x9F2A, 0x8667, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7E47, 0x9F2A, 0x9F2A, 0x9F4B,   // 0x01A0 (416)
0x9F2A, 0x7E47, 0x6DC5, 0x6DE5, 0x6DC5, 0x6DC5, 0x7E47, 0x9F2A, 0x9F2A, 0x9F4B, 0x972A, 0x7626, 0x6DC5, 0x6DE5, 0x6DC5, 0x6DC5,   // 0x01B0 (432)
0x8688, 0x9F2A, 0x9F2A, 0x9F4B, 0x970A, 0x7606, 0x6DC5, 0x6DE5, 0x6DC5, 0x6DC5, 0x86A8, 0x9F4A, 0x9F2A, 0x9F4B, 0x970A, 0x75E5,   // 0x01C0 (448)
0x6DE5, 0x6DE5, 0x6DC5, 0x75E5, 0x8EC9, 0x9F2A, 0x9F4A, 0x9F4B, 0x8EE9, 0x6DE5, 0x6DE5, 0x6DE5, 0x6DC5, 0x75E5, 0x96E9, 0x9F2A,   // 0x01D0 (464)
0x9F4A, 0x9F4B, 0x8EC8, 0x6DE5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7606, 0x970A, 0x9F4A, 0x9F2A, 0x9F4B, 0x86A8, 0x6DC5, 0x6DE5, 0x6DE5,   // 0x01E0 (480)
0x6DC5, 0x7626, 0x970A, 0x9F2A, 0x9F4B, 0x9F2A, 0x8687, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7E26, 0x9F2A, 0x9F2A, 0x9F4A, 0x9F2A,   // 0x01F0 (496)
0x7E47, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7E67, 0x9F2A, 0x9F2A, 0x9F4B, 0x9F2A, 0x7E26, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x8687,   // 0x0200 (512)
0x9F2A, 0x9F4A, 0x9F4B, 0x970A, 0x7626, 0x6DE5, 0x6DE5, 0x6DC5, 0x6DE5, 0x8688, 0x9F4A, 0x9F4A, 0x9F4B, 0x970A, 0x75E5, 0x6DC5,   // 0x0210 (528)
0x6DE5, 0x6DC5, 0x6DE5, 0x8EC9, 0x9F2A, 0x9F4A, 0x9F4B, 0x8EE9, 0x75E5, 0x6DC5, 0x6DE5, 0x6DC5, 0x75E5, 0x8EE9, 0x9F2A, 0x9F2A,   // 0x0220 (544)
0x9F4B, 0x8EC9, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7606, 0x96E9, 0x9F2A, 0x9F2A, 0x9F4B, 0x8EA8, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5,   // 0x0230 (560)
0x7606, 0x972A, 0x9F2A, 0x9F4A, 0x9F4B, 0x8667, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7E26, 0x972A, 0x9F2A, 0x9F4B, 0x9F4A, 0x7E67,   // 0x0240 (576)
0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7E47, 0x9F2A, 0x9F2A, 0x9F4B, 0x972A, 0x7E47, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x8667, 0x9F2A,   // 0x0250 (592)
0x9F2A, 0x9F4B, 0x972A, 0x7606, 0x6DE5, 0x6DE5, 0x6DC5, 0x6DC5, 0x86A8, 0x9F2A, 0x9F4A, 0x9F4B, 0x970A, 0x7606, 0x6DC5, 0x6DE5,   // 0x0260 (608)
0x6DC5, 0x6DE5, 0x8EA8, 0x9F2A, 0x9F4A, 0x9F4B, 0x96E9, 0x75E5, 0x6DE5, 0x6DE5, 0x6DC5, 0x75E5, 0x8EC9, 0x9F2A, 0x9F4A, 0x9F4B,   // 0x0270 (624)
0x8EE9, 0x6DC5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7605, 0x96E9, 0x9F2A, 0x9F4A, 0x9F4B, 0x8EA8, 0x6DE5, 0x6DE5, 0x6DE5, 0x6DC5, 0x7606,   // 0x0280 (640)
0x75E6, 0x75E5, 0x8667, 0x9F4B, 0x9F4B, 0x9F4B, 0x9F2B, 0x7E47, 0x75C5, 0x75E6, 0x75E5, 0x75C5, 0x8E88, 0x9F4B, 0x9F4B, 0x9F4B,   // 0x0290 (656)
0x9F0A, 0x7E27, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x8EA8, 0xA76B, 0x9F4B, 0xA74B, 0x970A, 0x7E06, 0x75E5, 0x75E6, 0x75E5, 0x75E6,   // 0x02A0 (672)
0x8EC9, 0xA74B, 0x9F4B, 0xA74B, 0x96EA, 0x75E6, 0x75E5, 0x75E6, 0x75E5, 0x75E5, 0x96EA, 0xA74B, 0x9F4B, 0xA74B, 0x96C9, 0x75E6,   // 0x02B0 (688)
0x75E5, 0x75E6, 0x75E5, 0x7E06, 0x96EA, 0xA74B, 0x9F4B, 0xA74B, 0x8EA9, 0x6DC5, 0x75E6, 0x75E6, 0x75C5, 0x7E26, 0x9F0A, 0xA74B,   // 0x02C0 (704)
0x9F4B, 0xA74B, 0x8E88, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x7E27, 0x9F2B, 0x9F4B, 0x9F4B, 0xA74B, 0x8667, 0x75C5, 0x75E6, 0x75E6,   // 0x02D0 (720)
0x75C5, 0x8668, 0x9F2B, 0x9F4B, 0x9F4B, 0x9F2B, 0x8647, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x8668, 0xA74B, 0x9F4B, 0x9F4B, 0x9F2B,   // 0x02E0 (736)
0x7E27, 0x75C5, 0x75E6, 0x75E6, 0x75E5, 0x8E88, 0xA74B, 0x9F4B, 0x9F4B, 0x9F0A, 0x7E06, 0x75C5, 0x75E6, 0x75E5, 0x75C5, 0x8EC9,   // 0x02F0 (752)
0xA74B, 0x9F4B, 0xA74B, 0x96EA, 0x7606, 0x75E5, 0x75E6, 0x75E5, 0x75E6, 0x96C9, 0xA74B, 0x9F4B, 0xA74B, 0x96C9, 0x75E5, 0x75E5,   // 0x0300 (768)
0x75E6, 0x75E5, 0x7606, 0x970A, 0xA74B, 0x9F2B, 0xA74B, 0x8EA9, 0x75E5, 0x75E6, 0x75E6, 0x75E5, 0x7E06, 0x9F0A, 0x9F4B, 0x9F4B,   // 0x0310 (784)
0xA74B, 0x8E88, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x7E27, 0x9F2A, 0xA74B, 0x9F4B, 0xA74B, 0x8688, 0x6DC5, 0x75E6, 0x75E6, 0x75C5,   // 0x0320 (800)
0x8647, 0x9F2B, 0x9F4B, 0x9F4B, 0x9F4B, 0x8647, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x8668, 0xA74B, 0x9F4B, 0x9F4B, 0x9F2B, 0x7E27,   // 0x0330 (816)
0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x8E88, 0x9F4B, 0x9F4B, 0x9F4B, 0x9F0A, 0x7E27, 0x75E5, 0x75E6, 0x75E6, 0x75C5, 0x8EA9, 0xA76B,   // 0x0340 (832)
0x9F4B, 0xA74B, 0x970A, 0x7606, 0x75E5, 0x75E6, 0x75E5, 0x75E6, 0x96C9, 0xA74B, 0x9F4B, 0xA74B, 0x96E9, 0x75E6, 0x75E5, 0x75E6,   // 0x0350 (848)
0x75E5, 0x75E6, 0x96EA, 0xA74B, 0x9F4B, 0xA74B, 0x8EA9, 0x75E6, 0x75E5, 0x75E6, 0x75E5, 0x7E06, 0x970A, 0xA74B, 0x9F4B, 0xA74B,   // 0x0360 (864)
0x8EA9, 0x6DC5, 0x75E6, 0x75E6, 0x75C5, 0x7E27, 0x9F2A, 0xA74B, 0x9F4B, 0x9F4B, 0x8688, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x7E47,   // 0x0370 (880)
0x9F4B, 0x9F4B, 0x9F4B, 0xA76B, 0x8647, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x8667, 0x9F2B, 0x9F4B, 0x9F4B, 0x9F2B, 0x8647, 0x75C5,   // 0x0380 (896)
0x75E6, 0x75E5, 0x75C5, 0x8688, 0xA74B, 0x9F4B, 0x9F4B, 0x9F2A, 0x7E26, 0x75C5, 0x75E6, 0x75E6, 0x75E5, 0x8EA9, 0xA76B, 0x9F2B,   // 0x0390 (912)
0x9F4B, 0x9F0A, 0x7E06, 0x75E5, 0x75E6, 0x75E6, 0x75E5, 0x8EC9, 0xA74B, 0x9F4B, 0xA74B, 0x96E9, 0x7606, 0x75E5, 0x75E6, 0x75E5,   // 0x03A0 (928)
0x75E6, 0x96EA, 0xA74B, 0x9F4B, 0xA74B, 0x96C9, 0x75E6, 0x75E5, 0x75E6, 0x75E5, 0x7E06, 0x96EA, 0xA74B, 0x9F4B, 0xA74B, 0x8EA9,   // 0x03B0 (944)
0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x7E26, 0x9F0A, 0xA74B, 0x9F4B, 0xA74B, 0x8688, 0x75C5, 0x75E6, 0x75E6, 0x75C5, 0x7E27, 0x9F2B,   // 0x03C0 (960)
0x7606, 0x8EC9, 0xA76B, 0xA76B, 0xA76B, 0x9F2A, 0x7E26, 0x75E5, 0x7606, 0x7606, 0x7606, 0x96E9, 0xA78B, 0xA76B, 0xA76B, 0x972A,   // 0x03D0 (976)
0x7606, 0x7605, 0x7606, 0x7606, 0x7626, 0x970A, 0xA78B, 0xA76B, 0x9F6B, 0x970A, 0x7606, 0x7605, 0x7606, 0x7606, 0x7626, 0x9F2A,   // 0x03E0 (992)
0xA76B, 0xA76B, 0xA76B, 0x8EC9, 0x7606, 0x7606, 0x7606, 0x7606, 0x7E46, 0x9F4B, 0xA78B, 0xA76B, 0x9F6B, 0x8EA8, 0x7605, 0x7606,   // 0x03F0 (1008)
0x7606, 0x7606, 0x7E67, 0x9F4B, 0xA76B, 0xA76B, 0x9F6B, 0x86A8, 0x75E5, 0x7606, 0x7606, 0x7606, 0x8667, 0xA76B, 0xA76B, 0xA76B,   // 0x0400 (1024)
0x9F4B, 0x7E67, 0x7605, 0x7606, 0x7606, 0x7606, 0x86A8, 0xA76B, 0xA76B, 0xA76B, 0x9F4B, 0x7E47, 0x75E5, 0x7606, 0x7606, 0x7606,   // 0x0410 (1040)
0x8EC9, 0xA76B, 0xA76B, 0xA76B, 0x9F2A, 0x7E26, 0x75E5, 0x7606, 0x7606, 0x7606, 0x8EE9, 0xA78B, 0xA76B, 0xA76B, 0x9F4A, 0x7606,   // 0x0420 (1056)
0x7605, 0x7606, 0x7606, 0x7626, 0x9709, 0xA76B, 0xA76B, 0xA76B, 0x9709, 0x7606, 0x7605, 0x7606, 0x7606, 0x7626, 0x9F2A, 0xA78B,   // 0x0430 (1072)
0xA76B, 0xA76B, 0x8EE9, 0x7606, 0x7606, 0x7606, 0x7606, 0x7E47, 0x9F2A, 0xA78B, 0xA76B, 0x9F6B, 0x8EC9, 0x7605, 0x7606, 0x7606,   // 0x0440 (1088)
0x7606, 0x7E47, 0x9F6B, 0xA76B, 0xA76B, 0x9F6B, 0x8688, 0x7605, 0x7606, 0x7606, 0x7606, 0x8667, 0xA76B, 0xA76B, 0x9F6B, 0x9F6B,   // 0x0450 (1104)
0x8667, 0x75E5, 0x7606, 0x7606, 0x7606, 0x8688, 0xA76B, 0xA76B, 0x9F6B, 0x9F4B, 0x7E67, 0x75E5, 0x7606, 0x7606, 0x7606, 0x8EA8,   // 0x0460 (1120)
0xA76B, 0xA76B, 0xA76B, 0x9F4B, 0x7E26, 0x7605, 0x7606, 0x7606, 0x7606, 0x8EC9, 0xA78B, 0xA76B, 0xA76B, 0x9F2A, 0x7626, 0x7605,   // 0x0470 (1136)
0x7606, 0x7606, 0x7606, 0x970A, 0xA76B, 0xA76B, 0xA76B, 0x970A, 0x7606, 0x7605, 0x7606, 0x7606, 0x7626, 0x972A, 0xA78C, 0xA76B,   // 0x0480 (1152)
0xA76B, 0x96E9, 0x7606, 0x7606, 0x7606, 0x7606, 0x7E46, 0x9F2A, 0xA76B, 0xA76B, 0x9F6B, 0x8EC9, 0x7606, 0x7606, 0x7606, 0x7606,   // 0x0490 (1168)
0x7E46, 0xA76B, 0xA76B, 0x9F6B, 0x9F6B, 0x86A8, 0x7605, 0x7606, 0x7606, 0x7606, 0x8667, 0x9F4B, 0xA76B, 0xA76B, 0x9F6B, 0x8688,   // 0x04A0 (1184)
0x75E5, 0x7606, 0x7606, 0x7605, 0x8688, 0xA76B, 0xA76B, 0xA76B, 0x9F4B, 0x8667, 0x75E5, 0x7606, 0x7606, 0x7606, 0x86A8, 0xA76B,   // 0x04B0 (1200)
0xA76B, 0xA76B, 0x9F4B, 0x7E26, 0x75E5, 0x7606, 0x7606, 0x7606, 0x8EC9, 0xA76B, 0xA76B, 0xA76B, 0x972A, 0x7E26, 0x75E5, 0x7606,   // 0x04C0 (1216)
0x7606, 0x7606, 0x96E9, 0xA78B, 0xA76B, 0xA76B, 0x972A, 0x7606, 0x7605, 0x7606, 0x7606, 0x7E26, 0x970A, 0xA76B, 0xA76B, 0x9F6B,   // 0x04D0 (1232)
0x96E9, 0x7606, 0x7605, 0x7606, 0x7606, 0x7626, 0x9F4A, 0xA76B, 0xA76B, 0xA76B, 0x8EC9, 0x7606, 0x7606, 0x7606, 0x7606, 0x7E47,   // 0x04E0 (1248)
0x9F4B, 0xA78B, 0xA76B, 0x9F6B, 0x8EA8, 0x75E5, 0x7606, 0x7606, 0x7606, 0x7E67, 0x9F6B, 0xA76B, 0xA76B, 0x9F6B, 0x86A8, 0x75E5,   // 0x04F0 (1264)
0x7606, 0x7606, 0x7606, 0x7E67, 0xA76B, 0xA76B, 0xA76B, 0x9F4B, 0x7E67, 0x7605, 0x7606, 0x7606, 0x7606, 0x8EA8, 0xA76B, 0xA76B,   // 0x0500 (1280)
0x6D25, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x6505, 0x64E4, 0x64E4, 0x64E5, 0x64E4, 0x6D46, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DC8, 0x64E4,   // 0x0510 (1296)
0x64E4, 0x6504, 0x6505, 0x64E4, 0x7566, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x64E4, 0x64E4, 0x64E4, 0x6505, 0x64E4, 0x7D87, 0x7DC8,   // 0x0520 (1312)
0x7DA7, 0x7DA7, 0x7587, 0x64E4, 0x64E4, 0x64E4, 0x6505, 0x64E4, 0x7DA7, 0x7DC7, 0x7DA7, 0x7DA7, 0x7587, 0x64E4, 0x6504, 0x6504,   // 0x0530 (1328)
0x6505, 0x64E4, 0x7DA7, 0x7DA7, 0x7DA7, 0x7DA7, 0x7566, 0x64E4, 0x64E4, 0x6504, 0x6505, 0x6505, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7,   // 0x0540 (1344)
0x6D46, 0x64E4, 0x64E4, 0x6505, 0x6504, 0x6D25, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x6D25, 0x64E4, 0x64E4, 0x6505, 0x64E5, 0x6D25,   // 0x0550 (1360)
0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x6505, 0x64E4, 0x6504, 0x6505, 0x64E4, 0x6D46, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DC8, 0x64E4, 0x64E4,   // 0x0560 (1376)
0x64E4, 0x6505, 0x64E4, 0x7566, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x64E4, 0x64E4, 0x64E4, 0x6505, 0x64E4, 0x7587, 0x7DC7, 0x7DA7,   // 0x0570 (1392)
0x7DA7, 0x7DA7, 0x64C4, 0x64E4, 0x6504, 0x6505, 0x64E4, 0x7587, 0x7DC7, 0x7DA7, 0x7DA7, 0x7587, 0x64E4, 0x6504, 0x64E4, 0x6505,   // 0x0580 (1408)
0x64E4, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x7566, 0x64E4, 0x6505, 0x6504, 0x6505, 0x6505, 0x7DA7, 0x7DA7, 0x7DA7, 0x7DA7, 0x6D46,   // 0x0590 (1424)
0x64E4, 0x6504, 0x64E4, 0x6505, 0x6505, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x6D45, 0x64E4, 0x6504, 0x6504, 0x6505, 0x6D25, 0x7DC8,   // 0x05A0 (1440)
0x7DA7, 0x7DA7, 0x7DC8, 0x6505, 0x64E4, 0x64E4, 0x6505, 0x64E4, 0x6D46, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x6505, 0x64E4, 0x64E4,   // 0x05B0 (1456)
0x6505, 0x64E4, 0x7566, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x64E4, 0x64E4, 0x6504, 0x6505, 0x64E4, 0x7566, 0x7DC8, 0x7DA7, 0x7DA7,   // 0x05C0 (1472)
0x7DA7, 0x64C4, 0x64E4, 0x64E4, 0x6505, 0x64E4, 0x7DA7, 0x7DC7, 0x7DA7, 0x7DA7, 0x7587, 0x64E4, 0x64E4, 0x64E4, 0x6505, 0x64E4,   // 0x05D0 (1488)
0x7DA7, 0x7DA7, 0x7DA7, 0x7DA7, 0x7566, 0x64E4, 0x6504, 0x64E4, 0x6505, 0x6505, 0x7DA7, 0x7DA7, 0x7DA7, 0x7DA7, 0x7566, 0x64E4,   // 0x05E0 (1504)
0x6504, 0x6504, 0x6505, 0x64E4, 0x85C8, 0x7DA7, 0x7DA7, 0x7DC7, 0x6D25, 0x64E4, 0x64E4, 0x6504, 0x6504, 0x6D25, 0x7DC8, 0x7DA7,   // 0x05F0 (1520)
0x7DA7, 0x7DA8, 0x6D25, 0x64E4, 0x64E4, 0x6505, 0x64E4, 0x6D25, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x6505, 0x64E4, 0x6504, 0x6505,   // 0x0600 (1536)
0x64E4, 0x6D46, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DC8, 0x64E4, 0x64E4, 0x6504, 0x6505, 0x64E4, 0x7566, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7,   // 0x0610 (1552)
0x64E4, 0x64E4, 0x64E4, 0x6505, 0x64E4, 0x7587, 0x7DC7, 0x7DA7, 0x7DA7, 0x7DA7, 0x64E4, 0x6504, 0x64E4, 0x6505, 0x64E4, 0x7DA7,   // 0x0620 (1568)
0x7DA7, 0x7DA7, 0x7DA7, 0x7587, 0x64E4, 0x6504, 0x6504, 0x6505, 0x64E4, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x7566, 0x64E4, 0x64E4,   // 0x0630 (1584)
0x6504, 0x6505, 0x6505, 0x7DC8, 0x7DA7, 0x7DA7, 0x7DA7, 0x6D46, 0x64E4, 0x64E4, 0x6504, 0x6505, 0x6D25, 0x7DC8, 0x7DA7, 0x7DA7,   // 0x0640 (1600)
};

#endif /* LINE_H_ */