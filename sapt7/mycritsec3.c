#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

struct record {
    int code;
    float stock;
};

int find_record(int fd, int target_code, off_t *offset, float *stock) {
    struct record rec;
    *offset = 0;
    while (1) {
        if (lseek(fd, *offset, SEEK_SET) == -1) {
            perror("lseek");
            return -1;
        }
        ssize_t n = read(fd, &rec.code, sizeof(int));
        if (n == 0) return 0; // EOF
        if (n != sizeof(int)) {
            perror("read code");
            return -1;
        }
        n = read(fd, &rec.stock, sizeof(float));
        if (n != sizeof(float)) {
            perror("read stock");
            return -1;
        }
        if (rec.code == target_code) {
            *stock = rec.stock;
            return 1;
        }
        *offset += sizeof(struct record);
    }
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "utilizare: %s <database> <instructions>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open database");
        exit(EXIT_FAILURE);
    }

    FILE *inst = fopen(argv[2], "r");
    if (!inst) {
        perror("fopen");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), inst)) {
        int code;
        float delta;
        if (sscanf(line, "%d %f", &code, &delta) != 2) {
            fprintf(stderr, "Invalid instruction: %s", line);
            continue;
        }

        off_t offset;
        float current_stock;
        int found = find_record(fd, code, &offset, &current_stock);
        if (found == -1) {
            exit(EXIT_FAILURE);
        }

        if (found) {
            float new_stock = current_stock + delta;
            if (delta < 0 && new_stock < 0) {
                printf("[%d] Error: stock insuficient  %d\n", getpid(), code);
                continue;
            }
            lseek(fd, offset + sizeof(int), SEEK_SET);
            write(fd, &new_stock, sizeof(float));
            printf("[%d] produs actualizat %d: %.2f -> %.2f\n", getpid(), code, current_stock, new_stock);
        } else {
            if (delta < 0) {
                printf("[%d] Error: produs %d negasit\n", getpid(), code);
                break;
            } else {
                struct record new_rec = {code, delta};
                lseek(fd, 0, SEEK_END);
                write(fd, &new_rec.code, sizeof(int));
                write(fd, &new_rec.stock, sizeof(float));
                printf("[%d] produs adaugat %d: %.2f\n", getpid(), code, delta);
            }
        }
    }

    fclose(inst);
    close(fd);
    return 0;
}
