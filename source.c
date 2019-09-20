#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct user_meta{
	char name[20];
	long int fisrt_file = -1;
};

typedef struct msg_meta {
	char message[94];
	char author[30];
	long int next = -1;
	long int padding;
};

typedef struct file_meta{
	char filename[30];
	char filetype[30];
	int filesize;
	long int this_file;
	long int next_file = -1;
	long int first_msg = -1;
	long int last_pointer = -1;
};

typedef struct blob{
	char content[1020];
	long int next_block = -1;
};

user_meta searchuser(FILE *fp, char name[30]){
	long int lastuserpointer;
	user_meta ub;

	long int userpointer = 0;
	int i = 1;
	fseek(fp, 0, SEEK_SET);
	fread(&lastuserpointer, sizeof(long int), 1, fp);
	int noofusers = (lastuserpointer - 716) / sizeof(user_meta);
	fseek(fp, 716, SEEK_SET);

	while (noofusers){
		fread(&ub, sizeof(user_meta), 1, fp);
		if (strcmp(name, ub.name) == 0)
			break;
		noofusers--;
	}

	return ub;
}
void display(FILE *fp, char name[30]){
	char filename[49];
	printf("\n enter file name : ");
	scanf("%s", filename);
	long int last_user;
	user_meta ub;
	msg_meta mb;
	long int userpointer = 0;
	int i = 1;

	fseek(fp, 0, SEEK_SET);
	fread(&last_user, sizeof(long int), 1, fp);
	int noofusers = (last_user - 716) / sizeof(user_meta);
	fseek(fp, 716, SEEK_SET);

	while (noofusers){
		fread(&ub, sizeof(user_meta), 1, fp);
		if (strcmp(name, ub.name) == 0)
			break;
		noofusers--;
	}

	long int metapointer = ub.fisrt_file;

	file_meta fd;

	while (metapointer != -1){
		fseek(fp, metapointer, SEEK_SET);
		fread(&fd, sizeof(file_meta), 1, fp);

		if (strcmp(filename, fd.filename) == 0){
			break;
		}
		metapointer = fd.next_file;
	}


	long int messagepointer = fd.first_msg;

	while (messagepointer != -1){
		fseek(fp, messagepointer, SEEK_SET);
		fread(&mb, sizeof(msg_meta), 1, fp);
		printf("\n%d.%s -> %s", i,mb.author, mb.message);
		messagepointer = mb.next;
		i = i + 1;
	}

}
void storemessage(FILE *fp, msg_meta mb, char name[30]){

	char filename[49];
	printf("\n enter file name : ");
	scanf("%s", filename);

	long int lastuserpointer;
	user_meta ub;
	long userpointer = 0;
	fseek(fp, 0, SEEK_SET);
	fread(&lastuserpointer, sizeof(long int), 1, fp);
	int noofusers = (lastuserpointer - 716) / sizeof(user_meta);
	fseek(fp, 716, SEEK_SET);

	while (noofusers){
		fread(&ub, sizeof(user_meta), 1, fp);
		if (strcmp(name, ub.name) == 0)
			break;
		noofusers--;
	}

	long int metapointer = ub.fisrt_file;

	file_meta fd;

	while (metapointer != -1){
		fseek(fp, metapointer, SEEK_SET);
		fread(&fd, sizeof(file_meta), 1, fp);

		if (strcmp(filename, fd.filename) == 0){
			break;
		}
		metapointer = fd.next_file;
	}

	// creating message block;
	int count = -1;
	char ch = '1';
	fseek(fp, 16, SEEK_SET);
	while (ch != '0'){
		fread(&ch, sizeof(ch), 1, fp);
		count++;
	}
	ch = '1';
	int tell1 = ftell(fp);
	int tell = ftell(fp) - sizeof(char);
	fseek(fp, tell, SEEK_SET);
	fwrite(&ch, sizeof(char), 1, fp);
	long int lastmessagepointer = count*sizeof(msg_meta) + 4096;
	fseek(fp, lastmessagepointer, SEEK_SET);
	mb.next = -1;
	fwrite(&mb, sizeof(msg_meta), 1, fp);
	int sizeofmessageblock = sizeof(mb);
	lastmessagepointer = lastmessagepointer + sizeof(mb);

	fseek(fp, 4, SEEK_SET);
	fwrite(&lastmessagepointer, sizeof(long int), 1, fp);

	if (fd.first_msg == -1){ 
		fd.first_msg = lastmessagepointer - sizeof(msg_meta);
		fd.last_pointer = fd.first_msg;

	}
	else{ 
		msg_meta  lastblock;
		fseek(fp, fd.last_pointer, SEEK_SET);
		fread(&lastblock, sizeof(msg_meta), 1, fp);
		lastblock.next = lastmessagepointer - sizeof(msg_meta);
		fseek(fp, fd.last_pointer, SEEK_SET);
		fwrite(&lastblock, sizeof(msg_meta), 1, fp);
		fd.last_pointer = lastmessagepointer - sizeof(msg_meta);
	}
	fseek(fp, metapointer, SEEK_SET);
	fwrite(&fd, sizeof(file_meta), 1, fp);

}
void deletemessage(FILE *fp){
	char name[30];
	int msgno;
	printf("\n enter user name : ");
	scanf("%s", &name);

	display(fp, name);

	char filename[49];
	printf("\n enter file name : ");
	scanf("%s", filename);

	printf("\n enter your message no : ");
	scanf("%d", &msgno);



	user_meta user = searchuser(fp, name);
	msg_meta mb;
	msg_meta dm;

	long int userpointer = ftell(fp) - sizeof(user_meta);

	long int metapointer = user.fisrt_file;

	file_meta ub;
	while (metapointer != -1){
		fseek(fp, metapointer, SEEK_SET);
		fread(&ub, sizeof(file_meta), 1, fp);

		if (strcmp(filename, ub.filename) == 0){
			break;
		}
		metapointer = ub.next_file;
	}


	long int messagepointer = ub.first_msg;
	int i = msgno;
	long int curr = ub.first_msg;
	long int prev;
	char ch = '0';
	int charposition;

	if (msgno == 1){
		fseek(fp, ub.first_msg, SEEK_SET);
		fread(&mb, sizeof(msg_meta), 1, fp);
		ub.first_msg = mb.next;
		fseek(fp, metapointer, SEEK_SET);
		if (ub.first_msg == ub.last_pointer)
			ub.first_msg = ub.last_pointer = -1;
		fwrite(&ub, sizeof(file_meta), 1, fp);

		charposition = (ub.first_msg - 4096) / sizeof(msg_meta);
		fseek(fp, 16 + charposition*sizeof(char), SEEK_SET);
		fwrite(&ch, sizeof(char), 1, fp);

	}
	else{
		i--;
		while (i){
			fseek(fp, curr, SEEK_SET);
			fread(&mb, sizeof(msg_meta), 1, fp);
			prev = curr;
			curr = mb.next;
			i--;
		}

		charposition = (curr - 4096) / sizeof(msg_meta);
		fseek(fp, 16 + charposition*sizeof(char), SEEK_SET);
		fwrite(&ch, sizeof(char), 1, fp);

		fseek(fp, curr, SEEK_SET);
		fread(&dm, sizeof(msg_meta), 1, fp);
		mb.next = dm.next;
		fseek(fp, prev, SEEK_SET);
		fwrite(&mb, sizeof(msg_meta), 1, fp);

		if (ub.last_pointer == curr){
			ub.last_pointer = prev;
			fseek(fp, metapointer, SEEK_SET);
			fwrite(&ub, sizeof(file_meta), 1, fp);
		}

	}

}
char* filetype(char filename[50]){
	char *type = (char*)calloc(1, sizeof(char));

	int i = 0;
	while (filename[i] != '.')
		i++;

	int j = 0;
	i++;

	while (filename[i] != '\0'){
		type[j] = filename[i];
		i++;
		j++;
	}

	type[j] = '\0';

	return type;
}
void upload(FILE *fp){
	char name[30];
	char filename[50];

	printf("\n enter user name : ");
	scanf("%s", name);

	printf("\n enter file name : ");
	scanf("%s", filename);

	long int lastuserpointer;
	user_meta ub;
	long userpointer = 0;
	fseek(fp, 0, SEEK_SET);
	fread(&lastuserpointer, sizeof(long int), 1, fp);
	int noofusers = (lastuserpointer - 716) / sizeof(user_meta);
	fseek(fp, 716, SEEK_SET);

	while (noofusers){
		fread(&ub, sizeof(user_meta), 1, fp);
		if (strcmp(name, ub.name) == 0)
			break;
		noofusers--;
	}

	if (noofusers == 0){
		strcpy(ub.name, name);
		ub.fisrt_file = -1;
		long int lastuserpointer;
		fseek(fp, 0, SEEK_SET);
		fread(&lastuserpointer, sizeof(long int), 1, fp);

		fseek(fp, lastuserpointer, SEEK_SET);
		fwrite(&ub, sizeof(user_meta), 1, fp);
		userpointer = lastuserpointer;
		lastuserpointer = lastuserpointer + sizeof(ub);

		fseek(fp, 0, SEEK_SET);
		fwrite(&lastuserpointer, sizeof(long int), 1, fp);

	}
	else{
		userpointer = ftell(fp) - sizeof(user_meta);
	}


	long int filemetapointer = ub.fisrt_file;

	file_meta fm;

	strcpy(fm.filename, filename);
	strcpy(fm.filetype, filetype(filename));

	FILE* inputfile;
	if (strcmp(fm.filetype, "txt") == 0)
		inputfile = fopen(filename, "r");
	else
		inputfile = fopen(filename, "rb");

	fseek(inputfile, 0, SEEK_END);
	fm.filesize = ftell(inputfile);

	long int filepointer = 0;

	fseek(fp, 12, SEEK_SET);
	fread(&filepointer, sizeof(long int), 1, fp);

	int noofpages = fm.filesize / 1020;
	if (fm.filesize % 1020 != 0)
		noofpages = noofpages + 1;

	blob page;
	long int prev = -1;
	int i = 0;
	char buf = '0';
	long int firstpageoffset = filepointer;

	fseek(inputfile, 0, SEEK_SET);
	while (!feof(inputfile)){
		fread(&buf, 1, 1, inputfile);
		page.content[i] = buf;
		i++;
		if (i == 1020){
			page.next_block = filepointer + sizeof(blob);
			fseek(fp, filepointer, SEEK_SET);
			fwrite(&page, sizeof(blob), 1, fp);
			filepointer = filepointer + sizeof(blob);
			
			fseek(fp, filepointer - sizeof(blob), SEEK_SET);
			blob testingpage;
			fread(&testingpage, sizeof(page), 1, fp);
		
			i = 0;
		}

	}

	page.content[i] = '\0';
	page.next_block = -1;
	fseek(fp, filepointer, SEEK_SET);
	fwrite(&page, sizeof(blob), 1, fp);
	filepointer = filepointer + sizeof(blob);

	
	fseek(fp, 12, SEEK_SET);
	fwrite(&filepointer, sizeof(long int), 1, fp);
	fseek(fp, 8, SEEK_SET);
	long int metapointer = 0;
	fread(&metapointer, sizeof(long int), 1, fp);
	fseek(fp, metapointer, SEEK_SET);
	fm.this_file = firstpageoffset;
	fwrite(&fm, sizeof(file_meta), 1, fp);
	metapointer = metapointer + sizeof(file_meta);
	fseek(fp, 8, SEEK_SET);
	fwrite(&metapointer, sizeof(long int), 1, fp);

	if (ub.fisrt_file == -1){
		ub.fisrt_file = metapointer - sizeof(file_meta);
		fseek(fp, userpointer, SEEK_SET);
		fwrite(&ub, sizeof(user_meta), 1, fp);
	}
	else{
		file_meta fmd;
		fseek(fp, ub.fisrt_file, SEEK_SET);
		fread(&fmd, sizeof(file_meta), 1, fp);
		long int currmetaoffset = fmd.next_file;
		long int prevmetaoffset = ub.fisrt_file;

		while (currmetaoffset != -1){
			fread(&fmd, sizeof(file_meta), 1, fp);
			prevmetaoffset = currmetaoffset;
			currmetaoffset = fmd.next_file;
		}

		fmd.next_file = metapointer - sizeof(file_meta);
		fseek(fp, prevmetaoffset, SEEK_SET);
		fwrite(&fmd, sizeof(file_meta), 1, fp);
	}

}

void download(FILE *fp){

	char name[30];
	char filename[30];

	printf("\n enter username : ");
	scanf("%s", name);

	printf("\n enter filename : ");
	scanf("%s", filename);

	printf("\nDownloading...!!!\n");
	user_meta ub = searchuser(fp, name);

	long int metapointer = ub.fisrt_file;
	file_meta  fd;
	int nooffilepages = 0;
	long int filepointer;

	while (metapointer != -1){
		fseek(fp, metapointer, SEEK_SET);
		fread(&fd, sizeof(file_meta), 1, fp);
		if (strcmp(filename, fd.filename) == 0){
			break;
		}
		metapointer = fd.next_file;
	}

	nooffilepages = (fd.filesize / 1024) + 1;
	filepointer = fd.this_file;

	blob pagefile;

	long int currentpageoffset = filepointer;
	FILE *copy;


	strcat(filename, "(1).");
	strcat(filename, fd.filetype);

	if (strcmp(fd.filetype, "txt") == 0)
		copy = fopen(filename, "w");
	else
		copy = fopen(filename, "wb");

	long int filebytes = fd.filesize;
	long int noofbytes = 0;
	while (nooffilepages > 0){
		fseek(fp, currentpageoffset, SEEK_SET);
		fread(&pagefile, sizeof(blob), 1, fp);

		int i = 0;
		while (i < 1020 && noofbytes < fd.filesize){
			fwrite(&pagefile.content[i], sizeof(char), 1, copy);
			i++;
			noofbytes++;
		}
		nooffilepages--;
		currentpageoffset = pagefile.next_block;
	}

	fclose(copy);
	printf("Download completed.!!");
}

void deletefile(FILE *fp){
	char name[30];
	char filename[30];

	printf("\n enter username : ");
	scanf("%s", name);

	printf("\n enter filename : ");
	scanf("%s", filename);

	user_meta ub = searchuser(fp, name);
	long int userpointer = ftell(fp) - sizeof(user_meta);
	long int metapointer = ub.fisrt_file;
	file_meta  fd;
	file_meta prev;
	long int prevmetapointer;

	fseek(fp, metapointer, SEEK_SET);
	fread(&fd, sizeof(file_meta), 1, fp);

	if (fd.next_file == -1){
		ub.fisrt_file = -1;
		fseek(fp, userpointer, SEEK_SET);
		fwrite(&ub, sizeof(user_meta), 1, fp);
	}
	else{
		while (metapointer != -1){

			if (strcmp(filename, fd.filename) == 0){
				break;
			}
			prevmetapointer = metapointer;
			metapointer = fd.next_file;
		}

		if (metapointer == -1){
			printf("\n file not found");
			return;
		}


		fseek(fp, prevmetapointer, SEEK_SET);
		fread(&prev, sizeof(file_meta), 1, fp);
		prev.next_file = fd.next_file;
		fseek(fp, prevmetapointer, SEEK_SET);
		fwrite(&prev, sizeof(file_meta), 1, fp);
	}
}

void viewfile(FILE* fp){
	char name[30];

	printf("\n enter username : ");
	scanf("%s", name);

	user_meta ub = searchuser(fp, name);
	long int userpointer = ftell(fp) - sizeof(user_meta);
	long int metapointer = ub.fisrt_file;
	file_meta  fd;
	file_meta prev;
	while (metapointer != -1){
		fseek(fp, metapointer, SEEK_SET);
		fread(&fd, sizeof(file_meta), 1, fp);
		printf("\n-->>%s<<--\n", fd.filename);
		if (fd.next_file == -1)
			break;
		metapointer = fd.next_file;
	}

}
int main(void){

	FILE * fp;
	fp = fopen("filedb.bin", "rb+");
	char name[30]; 
	msg_meta mb;

	int choice;
	char ch;
	do{
		printf("\n 1.Upload file");
		printf("\n 2.View file");
		printf("\n 3.Download file");
		printf("\n 4.delete file");
		printf("\n 5.Add comment");
		printf("\n 6.View comment ");
		printf("\n 7.Delete comment");
		printf("\n enter your choice :");
		scanf("%d", &choice);

		switch (choice){
		case 1:
			upload(fp);
			break;
		case 2:
			viewfile(fp);
			break;
		case 3:
			download(fp);
			break;
		case 4:
			deletefile(fp);
			break;
		case 5:
			printf("\n enter user name : ");
			scanf("%s", &name);
			printf("\n enter message : ");
			scanf("%s", mb.message);
			storemessage(fp, mb, name);
			break;

		case 6:
			printf("\n enter user name : ");
			scanf("%s", &name);
			display(fp, name);
			break;

		case 7:
			deletemessage(fp);
			break;
		default :
			printf("\nInvalid option..!!\n");
			break;

		}
		printf("\nIf you want to continue.. press 'y' :");
		scanf(" %c", &ch);
	} while (ch == 'y');
	system("pause");
	return 0;
}
