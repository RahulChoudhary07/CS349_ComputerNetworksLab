#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* decode(char* encoded_msg){
	int len = strlen(encoded_msg); // len of encoded_msg
	int index = 0;
	char* original_msg = (char*) malloc(sizeof(char*)*len);
	for(int i = 0; i < len; i=i+4){
		char* bits = (char*) malloc(sizeof(char*)*25);
		int j = 0;
		int flag = 0;
		for(j = 0; j < 24; j++) //initialise bits array
			bits[j] = '0';
		j = 0;
		while(j < 4){ // for every 4 characters of encoded msg
			int num = 0; 
			// set num value corresponding to the character
			/*For binary values 0 to 25 ASCII
character ‘A’ to ‘Z’ are used followed by lower case letters and the digits for binary values 26 to 51 & 52 to 61
respectively. Character ‘+’ and ‘/’ are used for binary value 62 & 63 respectively*/
			if (encoded_msg[i+j] >= 'A' && encoded_msg[i+j] <= 'Z'){
				num = encoded_msg[i+j] - 'A';
			}
			else if (encoded_msg[i+j] >= 'a' && encoded_msg[i+j] <= 'z'){
				num = (encoded_msg[i+j] - 'a') + 26;
			}
			else if (encoded_msg[i+j] >= '0' && encoded_msg[i+j] <= '9'){
				num = (encoded_msg[i+j] - '0') + 52;
			}
			else if (encoded_msg[i+j] == '+'){
				num = 62;
			}
			else if (encoded_msg[i+j] == '/'){
				num = 63;
			}
			else if (encoded_msg[i+j] == '='){
				flag++;
				num = 0;
			}
			int k = (j+1)*6 - 1; //index of last bit of this character in bits array
			while(num > 0){ //convert num value to binary and store in bits arrat
				if (num%2)
					bits[k]= '1';
				k--;
				num /=2;
			}
			j++;
		}
		j = 0;
		for(j = 0; j < 3; j++){
			if (flag + j < 3){ //while this condition is true, we are checking valid characters i.e. no '=' which correspond to no value of encoded msg
				int num = 0;
				int k = 0;
				for(k = j*8; k < (j+1)*8; k++){ // find value corresponding to next 8 bits of bits array
					num = num*2 + (bits[k] - '0');
				}
				original_msg[index++] = (char) num; // convert num to character according to ascii value
			}
		}
	}
	original_msg[index] = '\0';
	return original_msg;

}
/*
int main (){
	char* inp = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";
	char* out = decode(inp);
	printf("%s\n", out);
}*/
