#include <stdio.h>
#include <unistd.h>

#define EC_SYSFS "/sys/kernel/debug/ec/ec0/io"
#define EC_REG_SIZE 256

int main(void)
{
	unsigned char reg_data[EC_REG_SIZE];
	FILE *fp;
	int i, ret;

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

	/* Enable hidden registers - set offset 0x81 to 0x60 */
	fseek(fp, 0x81, SEEK_SET);
	ret = fputc(0x60, fp);
	fseek(fp, 0, SEEK_SET);

	/*Read in contents and dump */
	ret = fread(reg_data, 1, EC_REG_SIZE, fp);
	if (ret != EC_REG_SIZE) {
		printf("Failed to read EC registers: err %d\n", ret);
		fclose(fp);
		return 1;
	}

	for (i=0; i<EC_REG_SIZE; i++) {
		if (i % 16 == 0)
			printf("\n%2.2x: ",i);
		printf("%2.2x ",reg_data[i]);
	}
	printf("\n");

	fclose(fp);
	return 0;
}
