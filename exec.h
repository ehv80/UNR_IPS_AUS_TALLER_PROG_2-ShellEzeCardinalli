/* Clases para el shell. */
#ifndef __EXEC_H
#define __EXEC_H

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

enum modo { _Return, __Exit};
class Exec {
public:
		virtual int exec(enum modo)=0;
		virtual ~Exec() {}
};
class Exit: public Exec {
public:
	int exec(enum modo m) {
		cout << "--Fin--" << "\n";
		exit(0);
	} 
};
class Secuencia: public Exec {
public:
	class Exec *cond,*secuencia;
	
	Secuencia(Exec *cond,Exec *secuencia):cond(cond),secuencia(secuencia) {}
	int exec(enum modo m){
		int st1, st2,hijo;
		if(fork()!=0) {
			wait(&st1);
			wait(&st2);
		}
		else {
			if(fork()) {
				cond->exec(__Exit);	
			}
			else {
			 	secuencia->exec(__Exit);
			}
		}
		switch(m) {
		case _Return:
			return !(st1==0 && st2==0);
			case __Exit:
			exit(!(st1==0 && st2==0));
		}
	}
};
class Cond_1: public Exec {
public://'&&' AND
        class Exec *redir,*cond;
	
	Cond_1(Exec *redir,Exec *cond):redir(redir),cond(cond) {}
        int exec(enum modo m){
		int st1, st2,test,temp;
		if(fork()!=0) {
			wait(&st1);
			wait(&st2);
		}
		else {
			if(fork()) {
				temp=wait(&test);
				if (temp > 0)
					cond->exec(__Exit);	
			}
			else {
				redir->exec(__Exit);
			}
		}
		switch(m) {
		case _Return:
			return !(st1==0 && st2==0);
		case __Exit:
			exit(!(st1==0 && st2==0));
		}
	}
};
class Cond_2: public Exec {
public:// || OR
	class Exec *redir,*cond;

	Cond_2(Exec *redir,Exec *cond):redir(redir),cond(cond) {}
        int exec(enum modo m){
		int st1, st2,test,temp;
		if(fork()!=0) {
			wait(&st1);
			wait(&st2);
		}
		else {
			if(fork()) {
				temp=wait(&test);
			 	cout<<"temp"<<temp<<"\n";
				if(temp < 0)
					cond->exec(__Exit);
				exit(0);	
			}
			else {
					redir->exec(__Exit);
					exit(0);
			}
		}
		switch(m) {
		case _Return:
			return !(st1==0 && st2==0);
		case __Exit:
			exit(!(st1==0 && st2==0));
		}
	}
        
	

};
class Redir_1: public Exec {
public:
        class Exec *pipe; 
	char *archivo;
	
	Redir_1(Exec *pipe,char *archivo): pipe(pipe), archivo(archivo) {}
	int exec(enum modo m) {
		int status,arch;
		if(fork())
		{
			wait(&status);
		}
		else
		{
			arch=creat(archivo,0666);
			close(1);
			open(archivo,O_RDWR,0);
			dup2(1,arch);
			pipe->exec(__Exit);
		}
	}
};
class Redir_2: public Exec {
public:
	class Exec *pipee;
	char *archivo;
			
	Redir_2(Exec *pipee,char *archivo):pipee(pipee),archivo(archivo) {}
	int exec(enum modo m) {
		int st1, st2,st3;
		int caracter_leido,caracter_escrito,arch,salida;
		int fil,i,caso;
		char buf[2048];
		if(fork()!=0) {
			wait(&st1);
			wait(&st2);
		}
		else {
			int pip[2];
			pipe(pip);
			if(fork()) {
				close(pip[0]);
				dup2(pip[1], 1);
				close(pip[1]);
				// hacer el cat en c
				arch=0;salida=1;;
				arch=open(archivo,O_RDONLY,0);
				while((caracter_leido=read(arch,buf,2048)) > 0){
					caracter_escrito = write(salida,buf,caracter_leido);
					if(caracter_escrito < 0)
						cerr<<"error de escritura \n";
				}
				if(caracter_leido < 0)
					cerr<<"error lectura \n";		
					exit(-3);
			}
			else {
				close(pip[1]);
				dup2(pip[0], 0);
				close(pip[0]);
				pipee->exec(__Exit);	
			}
				exit(-1);
		}
		switch(m) {
		case _Return:
			return !(st1==0 && st2==0);
		case __Exit:
			exit(!(st1==0 && st2==0));
		}
	}
};
class Redir_3: public Exec {
		class Exec *pipe;
		char *archivo;
public:
	Redir_3(Exec *pipe,char *archivo):pipe(pipe),archivo(archivo) {}
        int exec(enum modo m){
		int status,arch,temp;
		
		if(fork())
		{
			wait(&status);
		}
		else
		{
			close(1);
			arch=open(archivo,O_WRONLY|O_APPEND|O_CREAT,0666);
			dup2(1,arch);
			pipe->exec(__Exit);
		}
	}
};
class Pipe: public Exec {
	Exec *prog1,*prog2;               
public:
        Pipe(Exec *prog1,Exec *prog2): prog1(prog1),prog2(prog2) {}
	int exec(enum modo m) {
		int st1, st2;
		if(fork()!=0) {
			wait(&st1);
			wait(&st2);
		}
		else {
			int pip[2];
			pipe(pip);
			if(fork()) {
				close(pip[0]);
				dup2(pip[1], 1);
				close(pip[1]);
				prog1->exec(__Exit);	
			}
			else {
				close(pip[1]);
				dup2(pip[0], 0);
				close(pip[0]);
			 	prog2->exec(__Exit);
			}
		}
		switch(m) {
		case _Return:
			return !(st1==0 && st2==0);
		case __Exit:
			exit(!(st1==0 && st2==0));
		}
	}
};
class Programa: public Exec {
	char *prog, **argvs;
	
	char * retornarPath(char *comando) {
		char path[1024];
		static char path2[1024];
		char *p;
		strcpy(path,getenv("PATH"));
		for(p=strtok(path,":"); p!=0; p=strtok(0,":")) {
			strcpy(path2,p);
			strcat(path2,"/");
			strcat(path2,comando);
			if(access(path2,X_OK)==0) return path2;
		}
		return 0;
	}
public:
	Programa(char **argvs): prog(argvs[0]), argvs(argvs) {}
	~Programa() {
		delete[] prog;
		delete[] argvs;
	} 
	int exec(enum modo m) {
		/* hay que buscar el path */
		char *p;	
		if(strchr(prog,'/'))
			   p=prog;
		else if((p=retornarPath(prog))==0) {
			   cerr<<prog<<":"<<strerror(errno)<<"\n";
			   return -1;
		}
		switch(m) {
		case _Return:
			if(fork()==0) {  /*hijo*/
				execv(p, argvs);
				cerr<<prog<<":"<<strerror(errno)<<"\n";
				return -1;
			}
			else { /*padre*/
				int status;
				wait(&status);
				return status;
			}
			break;
		case __Exit: 
			execv(p, argvs);
			cerr<<prog<<":"<<strerror(errno)<<"\n";
			exit(-1);
		}
	}
};

#endif
