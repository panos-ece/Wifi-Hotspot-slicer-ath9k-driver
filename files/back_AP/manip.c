#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[]){
    char word[20], ssid[20], file[20],aifs_[4];
    char buffer[200],OUI[20],data[32];
	char cmd[100];
	int isEOF = 0;
	int again = 0;
	int aifs, i, digit;
	size_t index;
	char *pos,*ptr,*temp;
	FILE *fd;

	
	if(argc<3){
		printf("Not enough arguments!\n");
		printf("Write: ./manip SSID(coordinator) SSID(self)\n");
		return 1;
	}

	if(strcmp(argv[2],"HOME")!=0 && strcmp(argv[2],"HOTSPOT")!=0&&strcmp(argv[2],"INTERFIERER")!=0 ){
		printf("Wrong SSID!\n");
		printf("Choose from: HOME-HOTSPOT-INTERFIERER.\n");
		return 1;
	}
	
	memset(file,'\0',strlen(file));
	strcpy(file,"scan_results.txt");
	
	memset(ssid,'\0',strlen(ssid));
	strcpy(ssid,argv[1]);
		
	memset(word,'\0',strlen(word));
	strcpy(word, "111");
	
	while(1){
		again = 0;
		system("rm scan_results.txt");
		system("rm test.pcap");
		
		memset(cmd,'\0',strlen(cmd));
		strcat(cmd,"tcpdump -G 2 -W 1 -i mon0 -n \"broadcast\" -w test.pcap");
		//printf("system: %s\n", cmd);
		system(cmd);

		memset(cmd,'\0',strlen(cmd));
		strcat(cmd,"hexdump -C test.pcap | cut -c 62-77 > ");
		strcat(cmd,file);
		//printf("system: %s\n", cmd);
		system(cmd);

		fd = fopen(file, "r");
		if (fd == NULL){
			printf("Error file missing\n");
			exit(-1);
		}
		fseek(fd,0,SEEK_SET);	
		while(1){
			memset(buffer,'\0',strlen(buffer));
			fread(buffer, strlen(ssid), 1, fd);
			isEOF = getc(fd);
			if(strcmp(buffer, ssid)==0){
				break;
			}
			
			fseek(fd,-(strlen(ssid)),SEEK_CUR);
			
			if(isEOF == EOF){
				printf("There is no SSID: %s\n",ssid);
				again = 1;
				break;
			}
		}
		if(again == 1) {
			continue;
		}
		
		printf("SSID: %s\n",ssid);
		
		while(1){
			memset(buffer,'\0',strlen(buffer));
			fread(buffer, strlen(word), 1, fd);
			isEOF = getc(fd);
			if(strcmp(buffer, word)==0){
				break;
			}
			
			fseek(fd,-(strlen(word)),SEEK_CUR);
			
			if(isEOF == EOF){
				printf("There is no vendor element in this beacon\n");
				again = 1;
				break;
			}
		}
		if(again == 1) {
			continue;
		}
		fseek(fd,-(strlen(word)),SEEK_CUR);
		fseek(fd,-(sizeof(char)),SEEK_CUR);
		
		memset(OUI,'\0',strlen(OUI));
		fread(OUI, 3, 1, fd);
		printf("OUI: %s\n", OUI);
		
		memset(data,'\0',strlen(data));
		fread(data, 30, 1, fd);
		
		fclose(fd);

		memset(temp,'\0',strlen(temp));
		ptr = strchr( data, '\n');
		if(ptr != NULL){
			index = strcspn(data,"\n");
			strncpy(temp,data,sizeof(char)*(index));
			strcat(temp,data+index+1);
		}
		memset(data,'\0',strlen(data));
		strcpy(data,temp);

		printf("Data: %s\n",data);

		if(strcmp(argv[2],"HOME")==0){//SSID: HOME
			pos = strstr(data,"hm");
		}
		else{
			if(strcmp(argv[2],"HOTSPOT")==0){//SSID: HOTSPOT
				pos = strstr(data,"ht");
			}
			else{
				pos = strstr(data,"in");
			}
		}
		if(pos == NULL){
			if(strcmp(argv[2],"INTERFIERER")==0){
				continue;
			}
			printf("Error in data manipulation!\n");
			continue;
		}

		digit = pos[2];
		aifs = digit - '0';
		aifs_[0] = digit;
		i=aifs*100;
		
		digit = pos[3];
		aifs = digit - '0';
		aifs_[1] = digit;
		i = i+aifs*10;
		

		digit = pos[4];
		aifs = digit - '0';
		aifs_[2] = digit;
		aifs = i+aifs;

		aifs_[3] = '\0';

		
		/*if(aifs>=0 && aifs <=9){
			
			i = i+aifs*10;

			digit = pos[4];
			aifs = digit - '0';
			if(aifs>=0 && aifs <=9){
				aifs_[2] = digit;
				aifs = i+aifs;
			}
			else{
				aifs = i/10;
				aifs_[2] = '\0';
			}
		}
		else{
			aifs = i/100;
			aifs_[1] = '\0';
			aifs_[2] = '\0';
		}*/
		
		
		printf("aifs: %d\n",aifs);
		
		memset(cmd,'\0',strlen(cmd));
		strcat(cmd, "echo ");
		strcat(cmd,aifs_);
		strcat(cmd, " > /proc/hello");
		printf("system: %s\n",cmd);
		
		system(cmd);
	}
    return 0;
 } 
 

