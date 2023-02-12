#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

struct IRGParam {
	uint16_t width;
	uint16_t height;
	int32_t emiss;
	int32_t airTemp;
	int32_t distance;
	uint8_t humidity;
	int32_t refTemp;
	int32_t atmTrans;
	int32_t K0;
	int32_t B0;
	int32_t K1;
	int32_t B1;
	int32_t pseudoCol;
	int32_t tempUnit;
	int32_t gainStatus;
};

int main(int argc, char **argv) {
	FILE *irg = argc>1 ? fopen(argv[1],"rb") : fdopen(0, "rb");
	if (!irg) {
		perror("opening file");
		return 1;
	}
	fseek(irg, 0, SEEK_END);
	off_t len = ftell(irg);
	rewind(irg);
	char *buf = calloc(len,1);
	if (!buf) {
		perror("allocating buffer");
		return 2;
	}
	if (fread(buf, len, 1, irg)!=1) {
		perror("reading file");
		return 3;
	}
	fclose(irg);
	struct IRGParam ip = {0};
	ip.width = *(uint16_t*)(buf+10);
	ip.height= *(uint16_t*)(buf+8);
	ip.emiss = *(int32_t*)(buf+0x1e);
	ip.airTemp = *(int32_t*)(buf+0x26);
	ip.distance = *(int32_t*)(buf+0x2a);
	//ip.humidity?
	ip.refTemp = *(int32_t*)(buf+0x22);
	ip.atmTrans = *(int32_t*)(buf+0x32);
	//ip.K0/B0/K1/B1?
	ip.pseudoCol = *(int32_t*)(buf+0x46);
	ip.tempUnit = *(int32_t*)(buf+0x49);
	//ip.gainStatus?
	fprintf(stderr,"IRG params:\n");
	fprintf(stderr,"WxH:  %ux%u\n", ip.width, ip.height);
	fprintf(stderr,"emiss: %d%%\n", ip.emiss/100);
	fprintf(stderr,"airTemp: %0.1fC\n", (float)ip.airTemp/10000.0-273.15);
	fprintf(stderr,"distance: %0.1fm\n", (float)ip.distance/10000.0);
	fprintf(stderr,"humidity: %0.1f?\n", (float)ip.humidity/10000.0);
	fprintf(stderr,"refTemp: %0.1fC\n", (float)ip.refTemp/10000.0-273.15);
	fprintf(stderr,"atmTrans: %0.1f?\n", (float)ip.atmTrans/10000.0);
	fprintf(stderr,"K0/K1 B0/B1: %0.1f?/%0.1f? %0.1f?/%0.1f?\n",
		(float)ip.K0/10000.0,
		(float)ip.K1/10000.0,
		(float)ip.B0/10000.0,
		(float)ip.B1/10000.0);
	fprintf(stderr,"pseudoCol: %d\n", ip.pseudoCol);
	fprintf(stderr,"tempUnit: %d\n", ip.tempUnit);
	fprintf(stderr,"gainStatus: %d\n", ip.gainStatus);
	return 0;
}
