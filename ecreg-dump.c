#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EC_SYSFS "/sys/kernel/debug/ec/ec0/io"
#define EC_SIZE 256
#define EC_REGION_SIZE 16

unsigned char regions[] = {0x1, 0x2, 0x3, 0x8, 0x60};

static int ec_set_region(FILE *fp, int region)
{
	int ret;

	fseek(fp, 0x81, SEEK_SET);
	return fputc(region, fp);
}

static int ec_read_and_dump(FILE *fp, int size, int offset)
{
	unsigned char *reg_data;
	int i, ret;

	reg_data = malloc(size);
	if (!reg_data) {
		printf("Memory not allocated (%d bytes)\n", size);
		fclose(fp);
		exit(0);
	}
	fseek(fp, offset, SEEK_SET);
	ret = fread(reg_data, 1, size, fp);
	if (ret != size) {
		printf("Failed to read EC registers: err %d\n", ret);
		free(reg_data);
		fclose(fp);
		exit(0);
	}

	for (i=0; i<size; i++) {
		if (i % 16 == 0)
			printf("%2.2x: ",offset+i);
		printf("%2.2x ",reg_data[i]);
		if ((i+1) % 16 == 0)
			printf("\n");
	}
	printf("\n");
	free(reg_data);
	return 0;	
}

int main(void)
{
	unsigned char reg_data[EC_SIZE];
	FILE *fp;
	int i, ret, reg;

	printf("EC register collector\n");

	if (geteuid() != 0) {
	    fprintf(stderr, "ecreg-dump must be run as root\n");
		return 1;
	}
	
	/* Open EC register access */
	fp = fopen(EC_SYSFS, "r+");
	if (!fp) {
		printf("Unable to open %s\n", EC_SYSFS);
		printf("Ensure that ec_sys module has been loaded with write support enabled\n");
		printf("\tsudo modprobe ec_sys write_support=1\n");
		return 1;
	}

	/* First dump default register set */
	ret = ec_set_region(fp, 0);
	if (ret < 0) {
		printf("Unable to write to EC\n"); 
		printf("Ensure that ec_sys module has been loaded with write support enabled\n");
		printf("\tsudo modprobe ec_sys write_support=1\n");
	}

	/*Read in contents and dump */
	ec_read_and_dump(fp, EC_SIZE, 0);

	/* Now go through the dynamic regions */
	for (reg=0; reg<sizeof(regions);reg++) {
		printf("Dump EC region 0x%x\n", regions[reg]);
		ec_set_region(fp, regions[reg]);
		ec_read_and_dump(fp, EC_REGION_SIZE, 0xA0);
	}

	fclose(fp);
	return 0;
}
