#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* encode(char* original_msg){
	int len = strlen(original_msg); //len of msg
	int padding = 3 - len%3; // as every 3 characters of msg are encoded into 4 characters, we need to find number of characters last group is short of
	if (padding == 3)
		padding = 0;
	int index = 0;
	char* encoded_msg = (char*) malloc(sizeof(char*)*2*len); //size of encoded msg
	for(int i = 0; i < len; i=i+3){
		char* bits = (char*) malloc(sizeof(char*)*25); //size of char array storing 3 bytes
		int j = 0;
		int flag = 0;
		for(j = 0; j < 24; j++) //intialise all bits to 0
			bits[j] = '0';
		j = 0;
		while(j < 3){
			if (i+j < len){
				int num = (int) original_msg[i+j]; //ascii value of character of original msg
				int k = (j+1)*8 - 1; //index of last bit in bits array
				while(num > 0){ // convert ascii into binary and store into bits array
					if (num%2)
						bits[k]= '1';
					k--;
					num /=2;
				}
			}
			else{
				flag = 1; //means this group has less than 3 characters
			}
			j++;
		}
		//printf("%s ", bits);
		j = 0;
		for(j = 0; j < 4; j++){
			if (flag && j >= 4 - padding) // this group has less than 3 characters so insert a '=' 
				encoded_msg[index++] = '=';
			else{
				int num = 0;
				for(int k = j*6; k < (j+1)*6; k++){ //find decimal number corresponding to next 6 values of bits array
					num = num*2 + (bits[k] - '0');
				}
				//find character corresponding to value num
				 /*For binary values 0 to 25 ASCII
character ‘A’ to ‘Z’ are used followed by lower case letters and the digits for binary values 26 to 51 & 52 to 61
respectively. Character ‘+’ and ‘/’ are used for binary value 62 & 63 respectively*/
				if (num <= 25){
					encoded_msg[index++] = 'A' + num;
				}
				else if (num <= 51){
					encoded_msg[index++] = 'a' + (num - 26);
				}
				else if (num <= 61){
					encoded_msg[index++] = '0' + (num - 52);
				}
				else if (num == 62){
					encoded_msg[index++] = '+';	
				}
				else
					encoded_msg[index++] = '/';
			}
		}
	}
	encoded_msg[index] = '\0';
	return encoded_msg;

}

/*int main (){
	char* inp = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
	char* out = encode(inp);
	printf("%s\n", out);
}*/
