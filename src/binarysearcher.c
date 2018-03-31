#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
/*struct File keeps name and path information of the file*/
struct File {
    char name[260];
    char path[500];
};
/*struct Location keeps file name and   number of   repetition of the word in the specific file*/
struct Location {
    char name[260];
    int numberOfWord;
};
/*struct Node element of the binary search tree and nodes generate the binary search tree.*/
/*struct Node keeps the word, total number of repetition in all files,depth in the binary search tree,
 * locations of words and next nodes addresses*/
struct Node {
    char word[260];
    int totalNumber;
    int depth;
    struct Location *wordLocation;
    struct Node *left;
    struct Node *right;
}*root = NULL;/*root pointer is our root of the binary searcher tree*/

/*short function definitions.*/
int is_dir(char* path);
int FileCount(char path1[500]);
int FolderCount(char path1[500]);
struct Node * Find(struct Node *node, char word[260]);
struct Node * Insert(struct Node *node,char word[260],char fileName[260],int fileCount,int depth);
void PrintInorder(struct Node *node,int fileCount,FILE *output);
void ReversePrintInorder(struct Node *node,int fileCount,FILE *output);
void PrintPreorder(struct Node *node,int fileCount,FILE *output);
void Search(char word[260],int fileCount,FILE *output);
struct Node * Remove(struct Node *node, char word[260]);
struct Node* FindMin(struct Node *node);
void AllFiles( char path[500],int totalFileCount);
void AddFiles(struct File *files, int fileCount,int totalFileCount);
void AddFile(struct File file, int totalFileCount);
void quicksort(struct File *files, int left, int right);
void reformat_string(char *src, char *dst);
char* reverseString(char str[260]);
char* toUpperCase(char str[260]);
char* GetFileName(char* path);
int Read(char path[500]);
void PrintNode(struct Node *item,FILE *output,int fileCount);

int main(int argc, char *argv[]) {

    FILE *output;
    output = fopen("output.txt", "w+");/*open output file to write*/
    char path[500] ;
    strcpy(path,argv[1]);/*take the path from comment line arguments*/
    int totalFileCount=FileCount(path);/*Calculate total number of the files*/
    AllFiles(path,totalFileCount);/*Read the files and add the words to tree*/
    FILE* command_file;
    char buff[500];
    command_file = fopen(argv[2], "r");/*open command file to read*/
    if (!command_file) {/*if command file is empty return 0*/
        return 0;
    }
    while (fgets(buff, 500, command_file) != NULL) {/*Take command lines from command file line by line and assign buff array*/
        char *token;
        char *temp=strtok(buff,"\r\n");/*Clean \r\n from end of line*/
        char tempArray[500];
        strcpy(tempArray,temp);/*take copy of the line*/
        if(strcmp(tempArray,"PRINT TREE") == 0){/*if line equal to "PRINT TREE" print all tree in pre-order(write in output.txt and screen)*/
            fprintf(output,"%s\n",tempArray);
            printf("%s\n",tempArray);
            PrintPreorder(root,totalFileCount,output);
        }
        else if(strcmp("PRINT TREE ASC",tempArray) == 0){/*if line equal to "PRINT TREE ASC" print all tree in in-order(write in output.txt and screen)*/
            fprintf(output, "%s\n", tempArray);
            printf("%s\n", tempArray);
            PrintInorder(root, totalFileCount, output);
        }
        else if(strcmp("PRINT TREE DSC",tempArray) == 0){/*if line equal to "PRINT TREE DSC" print all tree in reverse in-order(write in output.txt and screen)*/
            fprintf(output,"%s\n",tempArray);
            printf("%s\n",tempArray);
            ReversePrintInorder(root,totalFileCount,output);
        }
        token = strtok(buff, " \r\n");
        int loop=0;
        while(token!=NULL){

            if(strcmp("REMOVE",token) == 0 && loop == 0){/*if the first word of line is "REMOVE" do removing operations in binary search tree*/
                fprintf(output,"%s\n",tempArray);
                printf("%s\n",tempArray);
                token=strtok(NULL, " \r\n");
                root=Remove(root,token);
            }
            else if(strcmp("SEARCH",token) == 0 && loop == 0){/*if the first word of line is "SEARCH" do search operations in binary search tree*/
                fprintf(output,"SEARCH\n");
                printf("SEARCH\n");
                token=strtok(NULL, " \r\n");
                Search(token,totalFileCount,output);

            }
            else if(strcmp("ADD",token) == 0 && loop == 0){/*if the first word of line is "SEARCH" add the new file in binary search tree*/
                token=strtok(NULL, " \r\n");
                struct File newFile;
                char *fileName=GetFileName(token);
                strcpy(newFile.name,fileName);
                strcpy(newFile.path,token);
                AddFile(newFile,totalFileCount);

            }
            loop++;
            token = strtok(NULL," \r\n");
        }

    }
    fclose(output);/* close the files and return 0 for terminate the program*/
    fclose(command_file);
    return 0;
}
/*PrintPreorder function: Write information of nodes output file and screen in pre-order format(for "PRINT TREE" command)*/
void PrintPreorder(struct Node *node,int fileCount,FILE *output)
{
    if(node==NULL) /*if node points null return previous function*/
    {
        return;
    }
    /*Print node's information*/
    PrintNode(node,output,fileCount);
    /*Call PrintPreorder function for left node of this node*/
    PrintPreorder(node->left,fileCount,output);
    /*Call PrintPreorder function for right node of this node*/
    PrintPreorder(node->right,fileCount,output);
}
/*PrintInorder function: Write information of nodes output file and screen in in-order format(for "PRINT TREE ASC" command)*/
void PrintInorder(struct Node *node,int fileCount,FILE *output)
{
    if(node==NULL)/*if node points null return previous function*/
    {
        return;
    }
    /*Call PrintInorder function for left node of this node*/
    PrintInorder(node->left,fileCount,output);
    /*Print node's information*/
    PrintNode(node,output,fileCount);
    /*Call PrintInorder function for right node of this node*/
    PrintInorder(node->right,fileCount,output);
}
/*ReversePrintInorder function: Write information of nodes output file and screen in  reverse in-order format(for "PRINT TREE DSC" command)*/
void ReversePrintInorder(struct Node *node,int fileCount,FILE *output)
{
    if(node==NULL)/*if node points null return previous function*/
    {
        return;
    }
    /*Call ReversePrintInorder function for right node of this node*/
    ReversePrintInorder(node->right,fileCount,output);
    /*Print node's information*/
    PrintNode(node,output,fileCount);
    /*Call ReversePrintInorder function for left node of this node*/
    ReversePrintInorder(node->left,fileCount,output);
}

/*void Search function traverse the binary search tree and find the node which it's word equal to look for word*/
void Search(char word[260],int fileCount,FILE *output){
    struct Node *item = Find(root, word);/*Find the node and assign it in item */
    if (item != NULL) {/*if we found the node print informations of node*/
        /*Print node's information*/
        PrintNode(item,output,fileCount);
    } else
        return;
}

/*This function finds the node and remove it from binary search tree and return root node*/
struct Node * Remove(struct Node *node, char word[260])
{
    if(node != NULL){/*if node is equal to null then node can not find*/


    char first[260];
    char second[260];
    strcpy(first,word);
    strcpy(second,node->word);
    struct Node *temp;
    if(strcmp(toUpperCase(first),toUpperCase(second)) < 0)
    {
        node->left = Remove(node->left, word);/*if node's word smaller than tree's node's word
                                                call remove function with left element of tree's node*/
    }
    else if(strcmp(toUpperCase(first),toUpperCase(second)) > 0)
    {
        node->right = Remove(node->right, word);/*if node's word smaller than tree's node's word
                                                call remove function with right element of tree's node*/
    }
    else
    {
        /*if looking node not greater and smaller then equal the tree's node.So we find the node in tree*/

        if(node->right && node->left)
        {

            temp = FindMin(node->right);
            strcpy(node -> word, temp->word);
            node->totalNumber=temp->totalNumber;
            node->depth=temp->depth;
            node->wordLocation=temp->wordLocation;
            node -> right = Remove(node->right,temp->word);
        }
        else
        {

            temp = node;
            if(node->left == NULL)
                node = node->right;
            else if(node->right == NULL)
                node = node->left;
            free(temp);
        }
    }
    }
    return node;

}

/*This function finds minimum element of binary search tree  and returns it*/
struct Node* FindMin(struct Node *node)
{
    if(node==NULL)
    {
        return NULL;
    }
    if(node->left)
        return FindMin(node->left);
    else
        return node;
}
/*AllFiles function take path of directory and read all directory recursively.Then first reads all files alphabetically and add the words in
 * binary search tree second open all directories alphabetically and read files in them so it repaits recursively */
void AllFiles( char path[500],int totalFileCount) {
    int fileCount = FileCount(path);
    int index=0;
    struct File *files = (struct File *) calloc(fileCount, sizeof(struct File));/*This array keeps files for sorting operation*/
    char tempPath[500];
    strcpy(tempPath,path);
    struct dirent *de;
    DIR *dr = opendir(path);

    if (dr == NULL) {
        return;
    }
    char isDir[500];
    while ((de = readdir(dr)) != NULL) {
        if(de->d_name[0] == '.') continue;
        strcpy(isDir,tempPath);
        strcat(isDir,"/");
        strcat(isDir,de->d_name);
        if (is_dir(isDir) == 0) {
            continue;
        } else if (de->d_name[0] != '.') {
            struct File newFile;
            char directory[260] = "/";
            strcat(path, directory);
            strcat(path, de->d_name);
            strcpy(newFile.name, de->d_name);
            strcpy(newFile.path, path);
            strcpy(path,tempPath);
            files[index] = newFile;
            index++;

        }
        strcpy(isDir,tempPath);
    }
    closedir(dr);
    if(index>0){
        quicksort(files, 0, index - 1);/* I used quicksort algorithm for sort files */
        AddFiles(files,index,totalFileCount);/*Add all sorted files in binary search tree*/
    }
    dr = opendir(tempPath);
    int folderCount = FolderCount(tempPath);
    if(folderCount>0){
    struct File *folders = (struct File *) calloc(folderCount,sizeof(struct File));/*this array keeps folder for sorting operation*/
    int index2 = 0;
    while ((de = readdir(dr)) != NULL) {
        if(de->d_name[0] == '.') continue;
        strcpy(isDir,tempPath);
        strcat(isDir,"/");
        strcat(isDir,de->d_name);
        if (is_dir(isDir) == 0) {
            struct File newFile;
            char directory[260] = "/";
            strcat(path, directory);
            strcat(path, de->d_name);
            strcpy(newFile.name, de->d_name);
            strcpy(newFile.path, path);
            strcpy(path,tempPath);
            folders[index2] = newFile;
            index2++;
        }
        strcpy(isDir,tempPath);
    }
    quicksort(folders, 0, index2 - 1);/*I used quicksort algorithm for sort folders*/
    int var;
    for (var = 0; var < folderCount; ++var) {
        AllFiles(folders[var].path,totalFileCount);/*Call AllFiles function for all sub folder in ordered*/
    }
    }
    closedir(dr);/*close directory*/
}
/*
 * AddFiles function takes a file list.
 * Read this file one by one and parse the line  then generate the word for tree
 * If the word is already been in tree then increase the word's repetition number
 * else add the new word in binary search tree
 */
void AddFiles(struct File *files, int fileCount,int totalFileCount) {

    int i = 0;
    FILE* input_file;
    char buff[500];

    for (i = 0; i < fileCount; i++) {

        if(Read(files[i].path) == 0) continue;

        input_file = fopen(files[i].path, "r");

        if (!input_file) {
            return;
        }
        while (fgets(buff, 500, input_file) != NULL) {
            char *token;
            reformat_string(buff,buff);
            token = strtok(buff," \t\r\n");
            while( token != NULL && token[0]!='\n' && token[0] != '\r' && token[0] != '\0' )
            {
                struct Node *item=Find(root,token);
                char first[260];
                char second[260];
                if(item==NULL){
                    root=Insert(root,token,files[i].name,totalFileCount,1);
                }
                else{

                    item->totalNumber++;
                    int var;
                    for (var = 0; var < totalFileCount; ++var) {
                        strcpy(first, item->wordLocation[var].name);
                        strcpy(second, files[i].name);
                        if(item->wordLocation[var].numberOfWord == 0 ){
                            strcpy(item->wordLocation[var].name,files[i].name);
                            item->wordLocation[var].numberOfWord++;
                            var=totalFileCount;
                        }
                        else if(strcmp(toUpperCase(first),toUpperCase(second)) == 0){
                            item->wordLocation[var].numberOfWord++;
                            var=totalFileCount;

                        }
                    }
                }
                token = strtok(NULL," \t\r\n");
            }
        }
        fclose(input_file);
    }


}
/*AddFile function works similar with AddFiles function.
 * But AddFile function reads just one file and does same operations
 */
void AddFile(struct File file, int totalFileCount){
    if(Read(file.path) == 0) return;
    FILE* input_file;
    char buff[500];
    input_file = fopen(file.path, "r");
    if (!input_file) {
        return;
    }
    while (fgets(buff, 500, input_file) != NULL) {
        char *token;
        reformat_string(buff,buff);
        token = strtok(buff," \t\r\n");
        while( token != NULL && token[0]!='\n' && token[0] != '\r' && token[0] != '\0')
        {
            struct Node *item=Find(root,token);
            char first[260];
            char second[260];
            if(item==NULL){
                root=Insert(root,token,file.name,totalFileCount,1);
            }
            else{

                item->totalNumber++;
                int var;
                for (var = 0; var < totalFileCount; ++var) {
                    strcpy(first, item->wordLocation[var].name);
                    strcpy(second, file.name);
                    if(item->wordLocation[var].numberOfWord == 0 ){
                        strcpy(item->wordLocation[var].name,file.name);
                        item->wordLocation[var].numberOfWord++;
                        var=totalFileCount;
                    }
                    else if(strcmp(toUpperCase(first),toUpperCase(second)) == 0){
                        item->wordLocation[var].numberOfWord++;
                        var=totalFileCount;

                    }
                }
            }
            token = strtok(NULL," \t\r\n");
        }
    }
    fclose(input_file);
}
/*
 * Find function traverse all binary search tree and looking for specific node
 * If function finds the node then return it.
 * If function can not find the node then return NULL
 */
struct Node * Find(struct Node *node, char word[260])
{
    if(node==NULL)
    {
        return NULL;
    }
    char first[260];
    char second[260];
    strcpy(first,word);
    strcpy(second,node->word);
    /*I compare the node's word with upper case form for ignore case sensitivity*/
    if(strcmp(toUpperCase(first),toUpperCase(second)) > 0)
    {
        return Find(node->right,word);
    }
    else if(strcmp(toUpperCase(first),toUpperCase(second)) < 0)
    {
        return Find(node->left,word);
    }
    else
    {
        return node;
    }
}
/*Insert function determines the new node's location and create the new node and add it to binary search tree*/
struct Node * Insert(struct Node *node,char word[260],char fileName[260],int fileCount,int depth)
{

    if(node==NULL)
    {
        struct Node *temp;
        temp = (struct Node *)malloc(sizeof(struct Node));
        strcpy(temp -> word,word);
        temp->totalNumber=1;
        temp->depth=depth;
        temp->wordLocation=(struct Location *)malloc(fileCount*sizeof(struct Location));
        strcpy(temp->wordLocation[0].name,fileName);
        temp->wordLocation[0].numberOfWord=1;
        int var;
        for (var = 1; var < fileCount; ++var) {
            temp->wordLocation[var].numberOfWord=0;
        }

        temp -> left = temp -> right = NULL;
        return temp;
    }

    else if(strcmp(word,node->word) > 0)
    {
        depth++;
        node->right = Insert(node->right,word,fileName,fileCount,depth);
    }
    else if(strcmp(word,node->word) < 0)
    {
        depth++;
        node->left = Insert(node->left,word,fileName, fileCount,depth);
    }
    return node;

}
/*I used quicksort algorithm for sort the files or folders alphabetically.quicksort function take a struct File array and sorted it with files name.*/
void quicksort(struct File *files, int left, int right) {
    int i, j;
    struct File x;
    struct File temp;

    i = left;
    j = right;
    x = files[(left + right) / 2];

    do {
        char first[260];
        char second[260];
        char third[260];
        strcpy(first,files[i].name);
        strcpy(second,x.name);
        strcpy(third,files[j].name);
        while ((strcmp(toUpperCase(first),toUpperCase(second)) < 0) && (i < right)) {
            i++;
        }
        while ((strcmp(toUpperCase(third),toUpperCase(second)) > 0) && (j > left)) {
            j--;
        }
        if (i <= j) {
            strcpy(temp.name, files[i].name);
            strcpy(temp.path, files[i].path);
            strcpy(files[i].name, files[j].name);
            strcpy(files[i].path, files[j].path);
            strcpy(files[j].name, temp.name);
            strcpy(files[j].path, temp.path);
            i++;
            j--;
        }
    } while (i <= j);

    if (left < j) {
        quicksort(files, left, j);
    }
    if (i < right) {
        quicksort(files, i, right);
    }
}
/*reverseString function take a string and reverse it char by char then returns new string*/
char* reverseString(char str[260]) {
    int count = strlen(str);
    int i;
    char* newStr = (char*) malloc(count*(sizeof(char)));
    for (i = 0; i < count; i++) {
        newStr[i] = str[count-1-i];
    }
    return newStr;
}
/*toUpperCase function takes a string and convert all chars to upper case and returns new string*/
char* toUpperCase(char str[260]) {
    int c = 0;
    char ch;
    while (str[c] != '\0') {
        ch = str[c];
        if (ch >= 'a' && ch <= 'z')
            str[c] = str[c] - 32;
        c++;
    }
    return str;
}
/*GetFileName function takes a path line and determines the file name in the path and returns it as a string*/
char* GetFileName(char* path){
    char *temp=(char *)malloc(strlen(path)*sizeof(char));
    strcpy(temp,path);
    char *ssc;
    char *ssc2;
    int x = 0;
    ssc = strstr(temp, "\\");
    ssc2 = strstr(path, "/");
    if(!ssc && !ssc2){
        return temp;
    }
    while(ssc)
    {
        x = strlen(ssc) + 1;
        temp = &temp[strlen(temp)-x+2];
        ssc = strstr(temp, "\\");
    }
        x = 0;

        while(ssc2)
        {
            x = strlen(ssc2) + 1;
            path = &path[strlen(path)-x+2];
            ssc2 = strstr(path, "/");
        }
    if(strlen(temp) < strlen(path)){
        return temp;
    }
    return path;
}
/*Read function decides to a file is readable or not
 *If file is readable function returns 1
 *else function returns zero
 *(function check only first 100 character)
 */
int Read(char path[500]){
    int result=0;
    FILE *f=fopen(path,"r");
    char x;
    int i=0;
    while((x=(char)fgetc(f))!='\0' && i<100){
        if(isascii(x)){
            result=1;
            break;
        }
        i++;
    }
    fclose(f);
    return result;
}
/*reformat_string function deletes all punctuation marks in the line*/
void reformat_string(char *src, char *dst) {
    for (; *src; ++src){
        if (!ispunct((unsigned char) *src) || (unsigned char) *src == '-' )
            *dst++ = tolower((unsigned char) *src);
    }
    *dst = 0;
}
/*is_dir function takes a path and decides to it is a directory or a file
 * If it is a directory then  returns 0 or if it is a file then return 1
 */
int is_dir(char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}
/*FileCount function takes a path and counts files  in this directory and returns the total number of files*/
int FileCount(char path1[500]) {
    int result = 0;
    char path[500];
    strcpy(path,path1);
    struct dirent *de;
    DIR *dr = opendir(path);

    if (dr == NULL) {
        printf("Could not open current directory");
        return 0;
    }
    char tempPath[500];
    while ((de = readdir(dr)) != NULL) {
        if(de->d_name[0] == '.') continue;
                strcpy(tempPath,path1);
                strcat(tempPath,"/");
                strcat(tempPath,de->d_name);
        if (is_dir(tempPath) == 0) {
            char directory[260] = "/";
            strcat(path, directory);
            strcat(path, de->d_name);
            result += FileCount(path);
        } else {
            if (de->d_name[0] != '.') {
                result++;
            }
        }
        strcpy(tempPath,path1);

    }
    closedir(dr);

    return result;
}
/*FolderCount function takes a path and counts folders  in this directory and returns the total number of folders*/
int FolderCount(char path1[500]) {
    int result = 0;
    char path[500];
    strcpy(path,path1);
    struct dirent *de;
    DIR *dr = opendir(path);

    if (dr == NULL) {
        printf("Could not open current directory");
        return 0;
    }
    char tempPath[500];

    while ((de = readdir(dr)) != NULL) {
        if(de->d_name[0] == '.') continue;
        strcpy(tempPath,path1);
        strcat(tempPath,"/");
        strcat(tempPath,de->d_name);
        if (is_dir(tempPath) == 0) {
            result++;
        }
        strcpy(tempPath,path1);
    }
    closedir(dr);
    return result;
}
/*PrintNode write the output file and screen to node's informations*/
void PrintNode(struct Node *item,FILE *output,int fileCount){
    fprintf(output,"%s\n", item->word);
            printf("%s\n", item->word);
            fprintf(output,"\t%d\n", item->totalNumber);
            printf("\t%d\n", item->totalNumber);
            fprintf(output,"\t%d\n", item->depth);
            printf("\t%d\n", item->depth);
            int var;
            for (var = 0; var < fileCount; ++var) {
                if (item->wordLocation[var].numberOfWord != 0) {
                    fprintf(output,"\t%s %d\n", item->wordLocation[var].name,
                            item->wordLocation[var].numberOfWord);
                    printf("\t%s %d\n", item->wordLocation[var].name,
                                            item->wordLocation[var].numberOfWord);
                }
            }
}
