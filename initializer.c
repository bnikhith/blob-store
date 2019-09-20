This file is used to initialize the filepointers..
//#define _CRT_SECURE_NO_WARNINGS
//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
//int main(){
//	FILE* fp;
//	fp = fopen("filedb.bin", "wb");
//	char msg_bit[200];
//	char file_bit[300];
//	char blob_bit[200];
//
//	long int userlastpointer = 716;
//	long int messagelastpointer = 4096;
//	int firstindexofuser = 716;
//	int firstindexofmessage = 4096;
//	long int filelastpointer = 14336;
//	long int filelastmetapointer = 10240;
//	memset(&msg_bit, '0', 200);
//	memset(&file_bit, '0', 300);
//	memset(&blob_bit, '0', 200);
//	fwrite(&userlastpointer, sizeof(long int), 1, fp);
//	fwrite(&messagelastpointer, sizeof(long int), 1, fp);
//	fwrite(&filelastmetapointer, sizeof(long int), 1, fp);
//	fwrite(&filelastpointer, sizeof(long int), 1, fp);
//	fwrite(&msg_bit, sizeof(msg_bit), 1, fp);
//	fwrite(&file_bit, sizeof(file_bit), 1, fp);
//	fwrite(&blob_bit, sizeof(blob_bit), 1, fp);
//	system("pause");
//	return 0;
//}
