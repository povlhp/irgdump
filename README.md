My local copy.

# IRG infrared image file dumper

[Terence Eden asked](https://shkspr.mobi/blog/2023/02/reverse-engineering-the-irg-infrared-thermal-imaging-format-help-needed/)
if there was a way to obtain temperature data from his thermal camera without using the closed source Windows app..

## SDK located..

After some research, the following SDK was located that can read IRG file data:
[Infrared thermal imaging irg photo analysis SDK](https://www.autelrobotics.com/download/139.html)

..which turns out to be a nice _small_ windows DLL that is amenable to reversing with Ghidra. Let's go!

## SDK functions

`sdk_get_irg_param(char* file, IRG_Param* irg_param);` which extracts some info from the header:<br/>
```C
struct IRG_Param {
	uint16_t width;
	uint16_t height;
	int32_t emiss;
	int32_t airTemp;
	int32_t distance;
	uint8_t humidity;	// not extracted
	int32_t refTemp;
	int32_t atmTrans;
	int32_t K0;			// not extracted
	int32_t B0;			// not extracted
	int32_t K1;			// not extracted
	int32_t B1;			// not extracted
	int32_t pseudoCol;
	int32_t tempUnit;
	int32_t gainStatus;	// not extracted
}
```

`sdk_get_irg_data(char* file, int colIndex, unsigned short* tempData, unsigned char* imageData);`
which extracts raw temperature values (K x10) to a block of 16 bit values, and pallettized false colour
image to a block of
[YUYV](https://web.archive.org/web/20211011140723/https://www.fourcc.org/pixel-format/yuv-yuy2/)
(aka YUY2) pixels, with the false colour pallette by `colIndex` parameter.

## IRG format

As reversed from the SDK. Everybody loves ASCII art diagrams right?
```text
(all multi-byte values are little-endian - signedness as defined above)

HEADER:
      [ID?]      [repeating pattern to <ee> .......]      [IP addr?]
0000: <b0 0b 80> <00> <c0 a8 00 00> <HH HH>  <WW WW> <00> <c0 a8 00
0010: 00> <HH HH> <WW WW> <00> <c0 a8 00 00> <HH HH> <WW WW> <EE EE
0020: EE EE> <RR RR RR RR> <TT TT TT TT> <DD DD DD DD> <a0 0f
0030: 00 00> <XX XX XX XX> <00 00 00 00> <XX XX XX XX> <00 00
0040: 00 00> <00 00 00 00> <CC> <00 00> <UU> <02> <00 00 00 00> <00>
0050: <00> x 16
0060: <00> * 16
0070: <00> x 14 <ac ca>
                [EOH?]

c0 a8 00 00 => Offset beyond the 0x80 byte header for the 16-bit measurements
HH => height (pixels)
WW => width (pixels)
EE => emissivity (0->1 x 10000)
RR => reflected temperature (K, x 10000)
TT => air temperature (K, x 10000)
DD => distance (m, x 10000)
XX => atmospheric transmittance (0->1, x 10000)
CC => pseudo colour index
UU => temperature units (lookup?)

c0 a8 00 00 is the offset beyond header where the 16-bit image temperature measurements starts)

IMAGE DATA:
Follows directly after the header, offset 0x80. Each 8 bit value is range-adjusted luminence
(lowest temperature => 0, highest temperature => 255).

TEMPERATURE DATA:
Follows directly after the image data, offset 0x80 + width * height. Each 16 bit value is K x10
```

