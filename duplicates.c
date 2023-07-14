// Author - Harshawardhan Mane

#include "duplicates.h"
#define COMMANDLIST  "Aaf:h:lq" 
//FLAGS FOR SMOOTH IMPLEMENTATION OF COMMAND LINE OPTIONS AND ARGUMENTS  
int hidden_flag = 0;
int stat_flag = 0;
int l_flag = 0;
int f_flag = 0;
int h_flag = 0;
char f_flag_optarg[MAXPATHLEN];
char h_flag_optarg[66];
//VARIABLES STORING OUR DUPLICATE FILES STATISTICS 
int no_of_files;
long int total_size;
int no_of_unique_files;
long int total_size_unique;

//DECLARE OUR HASHTABLE
HASHTABLE hashtable[HASHTABLE_SIZE];

//FUNCTION TAKES A STRING AND RETURNS AN UNSIGNED LONG INTEGER
unsigned long int hash_string(char *string)
{
    unsigned long int hash = 0;

    while(*string != '\0') {
        hash = hash*64 + *string;
        ++string;
    }
    return hash % HASHTABLE_SIZE;
}

//ADDING ITEMS TO OUR HASH TABLE
void add_items(char * filepath,  int file_size){
	DATA *filedata=malloc(sizeof(DATA));
	filedata->address = malloc(strlen(filepath)+1);
	strcpy(filedata->address, filepath);
	filedata->size = file_size;
	filedata->next = NULL;
	//store the hash key corresponding the sha value of the file
	unsigned long int hash_key = hash_string(strSHA2(filepath));
	//check if the hashtable[hash_key] is empty
	if(hashtable[hash_key] == NULL){
		hashtable[hash_key] = filedata;
		++no_of_unique_files;
		total_size_unique+=filedata->size;
	}
	else{
		//add the DATA node at the end of the hashtable[hash_key]
		DATA *temp = hashtable[hash_key];
		while(temp->next){
			temp = temp->next;
		}
		temp->next = filedata;
	}
	++no_of_files;
	total_size+=filedata->size;
}

//Read the given directory and all its subdirectories recursively.
void read_directory(char* directory_path, int hidden_flag)
{
	DIR* dir = opendir(directory_path);
	if (dir==NULL){exit(EXIT_FAILURE);}
	struct dirent * dp;
	//DIRECTORY READING THAT INCLUDES HIDDEN FILES WITH THE IMPLEMENTATION OF A 'hidden_flag'
	if(hidden_flag==1)
	{
		while((dp=readdir(dir))!=NULL)
		{
			if(strcmp(dp->d_name, ".")!=0 && strcmp(dp->d_name, "..")!=0)
			{
				char filepath[MAXPATHLEN];
				struct stat stat_buffer;
				sprintf(filepath, "%s/%s", directory_path, dp->d_name);
				stat(filepath, &stat_buffer);
				int file_size=stat_buffer.st_size;
				//printf("%s %d\n", filepath, file_size);
				add_items(filepath, file_size);
			}
			//READING THE DIRECTORY RECURSIVELY IF DIRECTORY IS NOT ("." AND "..")
			if(dp->d_type==4 && strcmp(dp->d_name, ".")!=0 && strcmp(dp->d_name, "..")!=0)
			{
				char path[MAXPATHLEN] ={0};
				strcat(path, directory_path);
				strcat(path, "/");
				strcat(path, dp->d_name);
				read_directory(path, hidden_flag);

			}
		}
	}
	//DEFAULT DIRECTORY READING THAT EXCLUDES HIDDEN FILES 
	else
	{
		while((dp=readdir(dir))!=NULL)
		{
			if(dp->d_type!=4 && dp->d_name[0]!='.' && strcmp(dp->d_name, ".")!=0 && strcmp(dp->d_name, "..")!=0)
			{
				char filepath[MAXPATHLEN];
				struct stat stat_buffer;
				sprintf(filepath, "%s/%s", directory_path, dp->d_name);
				stat(filepath, &stat_buffer);
				int file_size=stat_buffer.st_size;
				//printf("%s %d\n", filepath, file_size);
				add_items(filepath, file_size);
			}
			//READING THE DIRECTORY RECURSIVELY IF DIRECTORY IS NOT ("." AND "..")
			else if(dp->d_type==4 && strcmp(dp->d_name, ".")!=0 && strcmp(dp->d_name, "..")!=0)
			{
				char path[MAXPATHLEN] ={0};
				strcat(path, directory_path);
				strcat(path, "/");
				strcat(path, dp->d_name);
				read_directory(path, hidden_flag);

			}
		}
	}
	closedir(dir);
}

//COMMAND LINE OPTION -l IMPLEMENTATION
void command_l(){
	unsigned long int i = 0;
	for(i=0 ; i< HASHTABLE_SIZE; i++){
		DATA *temp = hashtable[i];
		if(temp!=NULL){
			if(temp->next!=NULL){
				while(temp){
					printf("%s\t",temp->address);
					temp = temp->next;
				}
				printf("\n");
			}
		}
	}
}

//COMMAND LINE OPTION -f IMPLEMENTATION
int command_f(char *filename){
	int matcher = 0;
	unsigned long int hash_key=hash_string(strSHA2(filename));
	DATA *temp = hashtable[hash_key];
	while(temp){
		if(strcmp(temp->address, filename)!=0){
			printf("%s\n",temp->address);
			++matcher;
		}
		temp = temp->next;
	}
	return matcher;
}

//COMMAND LINE OPTION -h IMPLEMENTATION
int command_h(char* sha_hash){
	int matcher = 0;
	unsigned long int hash_key = hash_string(sha_hash);
	DATA *temp = hashtable[hash_key];
	while(temp){
		printf("%s\n",temp->address);
		temp = temp->next;
		++matcher;
	}
	return matcher;
}

//PRINT THE REQUIRED STATISTICS FOR THE PROJECT
void stats(){
	printf("%d\n%ld\n%d\n%ld\n",no_of_files, total_size, no_of_unique_files, total_size_unique);
}

//  PROVIDE A 'USAGE' MESSAGE IF AN INVALID SWITCH PROVIDED
void usage(char *argv0)
{
    fprintf(stderr, "Usage: %s [-Aaf<:>h<:>lq] [filepath...]\n", argv0);
    exit(EXIT_FAILURE);
}

//MAIN FUNCTION OF OUR PROGRAM
int main(int argc, char *argv[])
{	
	int opt;
	if (argc<2){usage(argv[0]);}
	else{
//  DETERMINE WHICH, IF ANY, COMMAND-LINE SWITCHES WERE PROVIDED
		while( (opt = getopt(argc, argv, COMMANDLIST)) != -1) {
			switch (opt) {
			
			case 'A' :  exit(EXIT_FAILURE);
						break;

			case 'a' :	hidden_flag = 1;
						break;

			case 'f' :	f_flag=1;
						strcpy(f_flag_optarg, optarg);
						break;

			case 'h' :	h_flag=1;
						strcpy(h_flag_optarg, optarg);
						break;

			case 'l' :	l_flag=1;
						break;
			
			case 'q' :	stat_flag=1;
						break;

			default :	usage(argv[0]);
						break;
			}
		}
		//IF NO OPTIONS ARE PROVIDED - JUST READ THE MENTIONED DIRECTORY AND GIVE THE OUTPUT
		if(stat_flag==0 && l_flag==0 && f_flag==0 && h_flag==0){
				read_directory(argv[argc-1], hidden_flag);
				stats();
			}
		//-q implementation
		else if(stat_flag==1){
				read_directory(argv[argc-1], hidden_flag);
				if(no_of_files - no_of_unique_files==0){exit(EXIT_SUCCESS);}
				else{exit(EXIT_FAILURE);}
			}
		
		//-l implementation
		else if(l_flag==1){
				read_directory(argv[argc-1], hidden_flag);
				command_l();
			}
		
		//-f implementation
		else if(f_flag==1){
				int matcher;
				read_directory(argv[argc-1], hidden_flag);
				matcher = command_f(f_flag_optarg);
				if(matcher>0){
					exit(EXIT_SUCCESS);
				}
				else{
					exit(EXIT_FAILURE);
				}
			}

		//-h implementation
		else if(h_flag==1){
				int matcher;
				read_directory(argv[argc-1], hidden_flag);
				matcher = command_h(h_flag_optarg);
				if(matcher>0){
					exit(EXIT_SUCCESS);
				}
				else{
					exit(EXIT_FAILURE);
				}
			}
    }
}
