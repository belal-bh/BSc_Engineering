/*
A program that check correctness of a simple `C file` ( *.c program ), if any error is found then through an exception.
Author: Md. Belal Hossain
Date: 28.07.2018
*/
// INCLUDE SECTION
#include<stdio.h>
#include<ctype.h>

// GLOBAL VARIABLE AND CONSTANT
#define token_len  15 // for every token in C program
#define n_header_file  3
#define n_data_type 4
#define n_return_type 5
char tarr[token_len]; // temporary array tarr
char header_file[n_header_file][token_len]={ "stdio.h", "math.h", "ctype.h" };
char data_type[n_data_type][token_len]={ "int", "char", "float", "double" };
char return_type[n_return_type][token_len]={ "void", "int", "char", "float", "double" };
int nline=0,nerror=0; // line number lineno , number of error noerr

// FUNCTION PROTOTYPE
char next_token(FILE *ptr); // ignore space and return next token as `tarr` and `ch` in a single line
int isidentifier(char arr[]); // check is it identifier or not and return 1 or 0
int isnumber(char arr[]); // check is it number or not and return 1 or 0
void errmsg(char str[]); // show error message
void sucmsg(char str[]); // show success message
int isfound(char word[],int token_row, int token_col, char token[][token_col]); // find word in given token and return 1 or 0

// MAIN FUNCTION
int main(){
    FILE *fp; // file pointer
    char file_path[]="c_file.c"; // path to the `C file`
    fp = fopen(file_path,"r");
    if(fp==NULL){
        printf("FILE DOESN'T EXIST, fp is NULL\n Please give correct path to that `C file`\n");
        return 0;
    }
    char ch;
    // compilation
    int flag = 1;
    while(flag==1){
        ch = next_token(fp);
        if(ch==EOF){
            flag = 0;
            break;
        }
        else if(ch=='\n')continue;
        else if(ch=='#'){
            ch = next_token(fp);
            if(strcmp(tarr,"include")==0){
                if(ch!='<')ch = next_token(fp);
                if(ch=='<'){
                    ch = next_token(fp); // Note: <   header_file> invalid ,but <header_file    > valid . so need to fix it
                    if(isfound(tarr, n_header_file, token_len, header_file)){
                        if(ch!='>')ch = next_token(fp);
                        if(ch=='>'){
                            ch = next_token(fp);
                            if(ch=='\n'){
                                sucmsg("include success");
                                nline++;
                                continue;
                            }
                            else errmsg("error in include section");
                        }
                        else errmsg("syntax error==(>)");
                    }
                    else errmsg("syntax error==(`header-file`)");
                }
                else errmsg("syntax error==(<)");
            }
            else errmsg("syntax error==(include)");
            continue;// for next token
        }
        else{ // main function
            if(isfound(tarr, n_return_type, token_len, return_type)){
                if(ch!=' ')errmsg("syntax error after=( data-type) space required.");
                else{
                    ch = next_token(fp);
                    if(strcmp(tarr,"main")==0){
                        if(ch==' ')ch = next_token(fp);
                        if(ch=='('){
                            ch = next_token(fp);
                            if(ch==')'){
                                // multiline space after `)`
                                ch = next_token(fp);
                                while((ch=='\n'|| ch==' ')&& tarr[0]=='\0')ch = next_token(fp); // for

                                if(ch=='{'){
                                    // {  body scop  }
                                    // multiline space after `{`
                                    ch = next_token(fp);
                                    while((ch=='\n'|| ch==' ')&& tarr[0]=='\0')ch = next_token(fp);

                                    main_body: // goto scop for back to main func-body and to check statement start with data-type
                                    if(isfound(tarr,5,15,data_type)){ // data-type of variable
                                        if(ch==' '){
                                            body_multi_var: // goto scop for multiple variable declaration
                                            ch = next_token(fp);
                                            if(isidentifier(tarr) && isfound(tarr,n_data_type,token_len,data_type)!=1){ // is it an identifier and not a keyword ?
                                                if(ch==' ')ch = next_token(fp);
                                                if(ch==','){
                                                    goto body_multi_var;
                                                }
                                                else if(ch==';'){
                                                    sucmsg("main func-body's statement");
                                                    ch = next_token(fp);
                                                    while((ch=='\n'|| ch==' ')&& tarr[0]=='\0')ch = next_token(fp);
                                                    if(strcmp(tarr,"return")==0)goto main_return_sec;
                                                    else goto main_body;
                                                }
                                                else errmsg("syntax error==(invalid identifier) `,` or `;` required");
                                            }
                                            else errmsg("syntax error==(invalid identifier)");
                                        }
                                        else errmsg("syntax error after ( data-type )");
                                    }
                                    else if(strcmp(tarr,"return")==0){
                                        main_return_sec:
                                        if(ch==' ')ch = next_token(fp);
                                        else errmsg("syntax error==( `return` ) unknown");
                                        if(isnumber(tarr)){
                                            if(ch==' ')ch = next_token(fp);
                                            if(ch==';'){
                                                scop_end:
                                                ch = next_token(fp);
                                                while((ch=='\n'|| ch==' ')&& tarr[0]=='\0')ch = next_token(fp);
                                                if(ch=='}'){
                                                    sucmsg("reached at end of main() function");
                                                    ch = next_token(fp);
                                                    while((ch=='\n'|| ch==' ')&& tarr[0]=='\0')ch = next_token(fp);
                                                    if(ch!=EOF){
                                                        errmsg("syntax error==(unexpected statement) after main-function return");
                                                        break;
                                                    }
                                                }
                                            }
                                            else errmsg("syntax error==( `{}` scop doesn't ended ) `}` expected");
                                        }
                                        else errmsg("syntax error==( return ) unknown");
                                        if(tarr[0]=='\0' && ch==';')goto scop_end;
                                    }
                                    else if(ch=='}')errmsg("syntax error==( `return` ) statement required");
                                    else errmsg("syntax error==(in func-body ) unknown");
                                }
                                else errmsg("syntax error==( `{` )");

                                flag =0;
                            }
                            else errmsg("syntax error==( `)` )");
                        }
                        else errmsg("syntax error==( `(` )");
                    }
                    else errmsg("syntax error==(`main`)");
                }
            }
            else errmsg("syntax error==(int main)");

        }
    }

    fclose(fp);

    printf("\n\n Build finished:  %d  ERROR(s).\n\n",nerror);
    return 0;
}

//  FUNCTION DEFINATION
//
/* Need update: ignore space and return next token as `tarr` and `ch` in a single line if do not pass any `ignor_char` , othewise, ignore given `ignor_char` and return next token as `tarr` and `ch` */
char next_token(FILE *ptr){ //, char ignor_char=' '
    char ch;
    int i=0;
    tarr[0]='\0';
    while((ch=fgetc(ptr))!= EOF){
        if(ch!=' ')break;
    }
    if(ch=='\n'){
        nline++;
    }
    else{
        if(!(isalnum(ch) || ch=='_' || ch=='.')){
            return ch;
        }
        tarr[i++]=ch;
        while((ch=fgetc(ptr))!= EOF){
            if(!(isalnum(ch) || ch=='_' || ch=='.'))break;
            tarr[i++]=ch;
        }
        if(i!=0)tarr[i]='\0';
    }
    return ch;
}

int isidentifier(char arr[]){
    int flag=1,i=1;
    if(isalpha(arr[0])){
        while(arr[i]!='\0'){
            if(!(isalnum(arr[i]) || arr[i]=='_')){
                flag=0;
                break;
            }
            i++;
        }
    }
    else flag=0;
    return flag;
}

int isnumber(char arr[]){
    int flag=1,nd=0;
    if(isdigit(arr[0])){
        int i=1;
        while(arr[i]!='\0'){
            if(arr[i]=='.')nd++;
            if(!(isdigit(arr[i]) || arr[i]=='.') || nd>1){
                flag=0;
                break;
            }
        }
    }
    else flag=0;
    return flag;
}

void errmsg(char str[]){ //  show error message
    nerror++;
    printf("\nERROR! Line - %3d : %s! \n",nline,str);
}

void sucmsg(char str[]){
    printf("\nOK Line - %3d : %s! \n",nline,str);
}

int isfound(char word[],int token_row, int token_col, char token[][token_col]){
    int i=0;
    for(i=0; i<token_col; i++){
        if(strcmp(word,token[i])==0)return 1;
    }
    return 0;
}
