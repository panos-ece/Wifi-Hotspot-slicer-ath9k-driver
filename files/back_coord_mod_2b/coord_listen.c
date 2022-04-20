#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[]){
	char cmd[100];
	FILE *fd;
	int file_length;

	memset(cmd,'\0',strlen(cmd));
	strcat(cmd, "echo 0 > /proc/hello");
	printf("system: %s\n",cmd);
	//system(cmd);

	while(1){
		system("rm test.pcap");
		
		memset(cmd,'\0',strlen(cmd));
		strcat(cmd,"timeout 2 tcpdump -i mon0 src 192.168.2.9  -w test.pcap");
		system(cmd);

		fd = fopen("test.pcap", "r");
		if (fd == NULL){
			printf("Error file missing\n");
			exit(-1);
		}
		fseek(fd, 0L, SEEK_END);
		file_length = ftell(fd);
		printf("\nfile_length = %d\n",file_length);
		fclose(fd);
		memset(cmd,'\0',strlen(cmd));
		strcat(cmd, "echo ");
		if(file_length < 40) {
			strcat(cmd, "0");
		}
		else{
			strcat(cmd, "1");
		}
		strcat(cmd, " > /proc/hello");
		printf("system: %s\n",cmd);
		
		system(cmd);
	}
    return 0;
 } 
 

