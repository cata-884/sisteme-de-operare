#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

volatile sig_atomic_t timeout = 0;
volatile sig_atomic_t intervals_remaining = 5;
volatile sig_atomic_t child_done = 0;
pid_t child_pid;

int N;

void alarm_handler(int sig) {
    if (intervals_remaining > 0) {
        perror("Atentie: nu s-a introdus niciun caracter in ultimele %d secunde!\n", N);
        intervals_remaining--;
        alarm(N);
    } else {
        timeout = 1;
    }
}

void sigchld_handler(int sig) {
    child_done = 1;
}

int stoi(char* str){
    if(str[0] == '-') return -1;
    int rez = 0;
    for(int i=0; str[i]!='\0'; i++){
        rez = rez*10 + (str[i]-'0');
    }
    return rez;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        perror("nu sunt date exact 2 argumente\n");
        return 1;
    }
    N = stoi(argv[2]);
    if (N <= 0) {
        perror("numarul primit ca argument e negativ");
        return 1;
    }

    child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (child_pid == 0) {
        signal(SIGUSR2, SIG_DFL);
        
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR2);
        int sig;
        printf("Fiul: se asteapta sigusr2");
        sigwait(&set, &sig);
        
        printf("Fiul: a fost primit sigusr2");
        
        int fd = open(argv[1], O_RDONLY);
        if (fd >= 0) {
            char buffer[1024];
            ssize_t bytes_read;
            while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
                write(STDOUT_FILENO, buffer, bytes_read);
            }
            close(fd);
        } else {
            perror("eroare la scriere\n");
        }
        printf("fiu: sfarsit de executie\n");
        exit(0);
    } else {
        signal(SIGALRM, alarm_handler);
        signal(SIGCHLD, sigchld_handler);
        
        alarm(N);
        
        int output_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0) {
            perror("nu s-a putut crea fisier output");
            kill(child_pid, SIGKILL);
            return 1;
        }

        char buffer[1024];
        ssize_t bytes_read;

        while (!timeout) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);

            struct timeval tv;
            tv.tv_sec = 1;  
            tv.tv_usec = 0;

            int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
            
            if (retval == -1) {
                if (errno == EINTR) continue; 
                perror("select()");
                break;
            } else if (retval > 0) {
                bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (bytes_read == 0) {
                    break;
                } else if (bytes_read > 0) {
                    write(output_fd, buffer, bytes_read);
                    intervals_remaining = 5;
                    alarm(N);
                }
            }
        }

        close(output_fd);
        
        kill(child_pid, SIGUSR2);
        
        while (!child_done) {
            pause();
        }
        
        const char* parent_end = "Parinte: sfarsit executie!\n";
        write(STDOUT_FILENO, parent_end, strlen(parent_end));
        return timeout ? 1 : 0;
    }
}
