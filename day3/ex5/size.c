#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    const off_t size = lseek(fd, 0, SEEK_END);
    close(fd);

    printf("Size is %ld\n", size);

    return EXIT_SUCCESS;
}
