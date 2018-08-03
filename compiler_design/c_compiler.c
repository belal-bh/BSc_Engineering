/*
A program that check correctness of a simple `C file` ( *.c program ), if any error is found then through an exception.
Author: Md. Belal Hossain
Date: 28.07.2018
*/
// INCLUDE SECTION
#include<stdio.h>
#include<ctype.h>
#include<string.h>

// GLOBAL VARIABLE AND CONSTANT
#define token_len  15 // for every token in C program
#define n_header_file  3
#define n_data_type 4
#define n_return_type 5
int n_vars=0;
char variables[100][token_len];
int n_funcs=0;
char functions[20][token_len];
char tarr[token_len]; // temporary array tarr
char header_file[n_header_file][token_len]={ "stdio.h", "math.h", "ctype.h" };
char data_type[n_data_type][token_len]={ "int", "char", "float", "double" };
char return_type[n_return_type][token_len]={ "void", "int", "char", "float", "double" };
int nline=1,nerror=0; // line number lineno , number of error noerr

// FUNCTION PROTOTYPE
char next_token(FILE *ptr); // ignore space and return next token as `tarr` and `ch` in a single line
char ignore_gap(FILE *ptr);
int isidentifier(char arr[]); // check is it identifier or not and return 1 or 0
int isnumber(char arr[]); // check is it number or not and return 1 or 0
void errmsg(char str[]); // show error message
void sucmsg(char str[]); // show success message
int isfound(char word[],int token_row, int token_col, char token[][token_col]); // find word in given token and return 1 or 0
int isdirective(FILE *ptr); // check is it valid #directive or not
int isoperator(char op); // is it operator or not
void reg_func(char name[], char type[]); // register function
void reg_var(char name[], char type[]); // register variable

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
    char temp_type[20],temp_var[20]; // temporarily store data-type and identifier name
    while(flag==1){
        ch = next_token(fp);
        if(ch==EOF){
            flag = 0;
            break;
        }
        else if(ch=='\n')continue;
        else if(ch=='#' && tarr[0]=='\0'){
            if(isdirective(fp)){
                sucmsg("Directive success");
                continue;
            }
            else errmsg("invalid directive");
        }
        else if(isfound(tarr, n_return_type, token_len, return_type) || isfound(tarr, n_data_type, token_len, data_type)){
            //printf("tarr=%s, ch=%c, line=%d\n",tarr,ch, nline);
            strcpy(temp_type,tarr);
            if(ch!=' ')errmsg("syntax error after=( data-type) space required.");
            else{
                ch=next_token(fp);
                if(isidentifier(tarr)){
                    strcpy(temp_var,tarr);
                    if(ch==' ')ch = next_token(fp);
                    if(ch=='('){ // that mean it is a function
                        sucmsg("function-defination");
                        ch = next_token(fp);
                        if(ch==')'){
                            ch=ignore_gap(fp);
                            if(ch=='{'){
                                // register this function
                                reg_func(temp_var,temp_type);
                                func_body:
                                ch=ignore_gap(fp);
                                //printf("tarr=%s, ch=%c, line=%d\n",tarr,ch, nline);
                                if(ch=='}'){
                                    sucmsg("func-end `}`");
                                    continue;
                                    //flag=0;
                                    //break; // goto first-while-loop
                                }
                                else if(isfound(tarr, n_data_type, token_len, data_type)){
                                    strcpy(temp_type,tarr);
                                    func_body_vars:
                                    ch=next_token(fp);
                                    if(isidentifier(tarr) && (isfound(tarr, n_data_type, token_len, data_type)!=1)){
                                        strcpy(temp_var,tarr);
                                        // register variable
                                        reg_var(temp_var,temp_type);

                                        if(ch==' ')ch = next_token(fp);
                                        if(ch==','){
                                            goto func_body_vars;
                                        }
                                        else if(ch==';'){
                                            sucmsg("func-body's statement");
                                            goto func_body;
                                        }
                                        else errmsg("syntax error==(invalid identifier) `,` or `;` required");
                                        goto func_body;
                                    }
                                    else{
                                        errmsg("invalid identifier");
                                        goto func_body;
                                    }
                                }
                                else if(strcmp(tarr,"return")==0){ // return statement
                                    //func_return:
                                    if(ch==' ')ch = next_token(fp);
                                    else errmsg("syntax error after `return`");
                                    if(isnumber(tarr)){ // need to update according to func-return-type. now it's for main-func
                                        if(ch==' ')ch = next_token(fp);
                                        if(ch==';'){
                                            goto func_body;
                                        }
                                        else errmsg("invalid `return` statement");
                                    }
                                    else errmsg("invalid `return-value`");
                                    goto func_body;
                                }
                                else if(isfound(tarr,n_vars,token_len,variables)){ // exprations
                                    if(ch==' ')ch=next_token(fp);
                                    if(ch=='='){
                                        ch=next_token(fp);
                                        exp_operand:
                                        if(isnumber(tarr) || isfound(tarr,n_vars,token_len,variables) ){
                                            if(ch==' ')ch=next_token(fp);
                                            if(ch==';')goto func_body;
                                            else if(isoperator(ch)){
                                                ch=next_token(fp);
                                                goto exp_operand;
                                            }
                                            else errmsg("invalid expression");
                                            goto func_body;
                                        }
                                        errmsg("unknown assignment-value");
                                    }
                                    errmsg("unknown expression");
                                }
                                else if(isfound(tarr,n_funcs,token_len,functions)){ // function call
                                    if(ch==' ')ch=next_token(fp);
                                    if(ch=='('){ // let say without arguments
                                        ch=next_token(fp);
                                        if(')'){
                                            ch=next_token(fp);
                                            if(ch==';')sucmsg("function called");
                                            else errmsg("invalid statement");
                                            goto func_body;
                                        }
                                        else errmsg("expected `)`");
                                        goto func_body;
                                    }
                                    else errmsg("invalid func-call");
                                    goto func_body;
                                }
                                else errmsg("expected `}` or unknown scop");
                                goto func_body;
                            }
                            errmsg("expected `{` ");
                            continue;
                        }
                        errmsg("expected `)` ");
                        continue;
                    }
                    else if(ch==','){ // that means more than one identifier
                        global_vars:
                        ch=next_token(fp);
                        if(isidentifier(tarr)){
                            // register variable
                            reg_var(tarr,temp_type);

                            if(ch==' ')ch=next_token(fp);
                            if(ch==';')continue;
                            else if(ch==',')goto global_vars;
                            else errmsg("invalid syntax");
                            continue;
                        }
                        else errmsg("invalid syntax, expected `identifier`");
                    }
                    else if(ch==';')continue; //that means it is global variable
                    else errmsg("invalid syntax or expected `;`");
                    continue;
                }
            }
        }
    }

    fclose(fp);

    int i;
    /*for(i=0; i<n_vars; i++){
        printf("%s\n",variables[i] );
    }
    for(i=0; i<n_funcs; i++){
        printf("%s\n",functions[i] );
    }*/
    if(isfound("main",n_funcs,token_len,functions)==0)errmsg("main() function required");

    //printf("\n nline==%d\n",nline);
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
        if(!(ch==' ' || ch=='\t'))break;
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
// to ignore multiline-space
char ignore_gap(FILE *ptr){
    char ch;
    ch = next_token(ptr);
    while((ch=='\n'|| ch==' ' || ch=='\t')&& tarr[0]=='\0')ch = next_token(ptr);
    return ch;
}

// check is it identifier or not and return 1 or 0
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
// is it a numaric value ?
int isnumber(char arr[]){
    int flag=1,nd=0,i=1;
    if(isdigit(arr[0])){
        while(arr[i]!='\0'){ // careful about incrementing i
            if(arr[i]=='.')nd++;
            if(!(isdigit(arr[i])) || nd>1){
                flag=0;
                break;
            }
            i++;
        }
    }
    else flag=0;
    return flag;
}

//  show error message
void errmsg(char str[]){
    nerror++;
    printf("\nERROR! Line - %3d : %s! \n",nline,str);
}

// show success message
void sucmsg(char str[]){
    printf("\nOK (near)Line - %3d : %s! \n",nline,str);
}

// find word in given token and return 1 or 0
int isfound(char word[],int token_row, int token_col, char token[][token_col]){
    int i=0;
    for(i=0; i<token_col; i++){
        if(strcmp(word,token[i])==0)return 1;
    }
    return 0;
}

// check is it valid #directive or not
int isdirective(FILE *ptr){
    int validity=1;
    char ch;
    ch = next_token(ptr);
    if(strcmp(tarr,"include")==0){
        if(ch==' ')ch = next_token(ptr);
        if(ch=='<'){
            // Note: <   header_file> invalid ,but <header_file    > valid . fixed
            ch=fgetc(ptr);
            if(!isalpha(ch)){
                errmsg("space is not valid after `<` in `include` section");
            }
            else ungetc(ch,ptr); //
            ch = next_token(ptr);
            if(isfound(tarr, n_header_file, token_len, header_file)){
                if(ch==' ')ch = next_token(ptr);
                if(ch=='>'){
                    ch = next_token(ptr);
                    if(ch!='\n') validity = 0;
                }
                else validity = 0;
            }
            else validity = 0;
        }
        else validity = 0;
    }
    else if(strcmp(tarr,"define")==0){
        ch= next_token(ptr);
        if(isidentifier(tarr)){
            while(ch!='\n'){
                ch= next_token(ptr);
                if(ch==';')validity = 0;
            }
        }
        else validity=0;
    }
    else validity=0;
    return validity;
}

// is it operator or not
int isoperator(char op){
    if(op=='+' || op=='-' || op=='*' || op=='/' || op=='%')return 1;
    return 0;
}

// register function
void reg_func(char name[], char type[]){
    // check if the function already exist
    if(isfound(name,n_funcs,token_len,functions)){
        errmsg("function allready exist");
    }
    else{
        // register the function
        strcpy(functions[n_funcs++],name); // function name
        // also store function return type here
    }
}

// register variable
void reg_var(char name[], char type[]){
    // check if the function already exist
    if(isfound(name,n_vars,token_len,variables)){
        errmsg("variable allready exist");
    }
    else{
        // register the variable
        strcpy(variables[n_vars++],name); // function name
        // also store variable data-type here
    }
}
