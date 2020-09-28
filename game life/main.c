#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

union int_4char{
    char ch[4];
};

int to_int(char n1, char n2, char n3, char n4){
    union int_4char to_int;
    to_int.ch[0] = n1;
    to_int.ch[1] = n2;
    to_int.ch[2] = n3;
    to_int.ch[3] = n4;
    return to_int.integ;
}

int** to_matrix(const char* pixels, int width, int height){
    int** matrix = (int**)malloc(height * sizeof(int*));
    int k = 0;
    for (int i = height - 1; i >= 0; --i) {
        matrix[i] = (int*)malloc(width * sizeof(int));
        for (int j = 0; j < width; ++j) {
            if (pixels[k] != 0 || pixels[k + 1] != 0 || pixels[k + 2] != 0){
                matrix[i][j] = 1;
            } else{
                matrix[i][j] = 0;
            }
            k += 4;
        }
    }
    return matrix;
}

char* to_pixels(int** matrix, int width, int height, char* pixels){
    int k = 0;
    for (int i = height - 1; i >= 0; --i) {
        for (int j = 0; j < width; ++j) {
            if (matrix[i][j] == 1){
                pixels[k] = -127;
                pixels[k + 1] = 255;
                pixels[k + 2] = 255;
            } else{
                pixels[k] = 0;
                pixels[k + 1] = 0;
                pixels[k + 2] = 0;
            }
            pixels[k + 3] = 255;
            k += 4;
        }
    }
    return pixels;
}

//void make_pic(char* output, int age, int** matrix, int width, int height, char* buf, int size_buf, char* pixels){
//    char *filename = (char*)malloc(30);
//    char *name = (char*)malloc(30);
//    strcpy(name, output);
//    name = strcat(name, "\\");
//    name = strcat(name, itoa(age, filename, 10));
//    name = strcat(name, ".bmp");
//    FILE* new_file = fopen(name, "wb");
//    pixels = to_pixels(matrix, width, height, pixels);
//    fwrite(buf,size_buf, 1, new_file);
//    fwrite(pixels,1, width * height * 4, new_file);
//}

int game_life(int** matrix, int width, int height){
    int neighbors;
    int new_matrix[height][width];
    int x, y;
    int same_nodes = 0;
    for(y = 0; y < height; y++)
        for(x = 0; x < width; x++)
            new_matrix[y][x] = matrix[y][x];
    for(y = 1; y < height - 1; y++){
        for(x = 1; x < width - 1; x++){
            neighbors = matrix[y + 1][x - 1] + matrix[y + 1][x] + matrix[y + 1][x + 1] + matrix[y][x - 1] + matrix[y][x + 1] + matrix[y - 1][x - 1] + matrix[y - 1][x] + matrix[y - 1][x + 1];
            if(new_matrix[y][x] == 1){
                if(neighbors == 2) new_matrix[y][x] = matrix[y][x];
                if(neighbors == 3) new_matrix[y][x] = matrix[y][x];
                if(neighbors > 3) new_matrix[y][x] = 0;
                if(neighbors < 2) new_matrix[y][x] = 0;
            }
            else{
                if(neighbors == 3) new_matrix[y][x] = 1;
            }
        }
    }
    for(y = 0; y < height; y++)
        for(x = 0; x < width; x++){
            if(matrix[y][x] == new_matrix[y][x])
                same_nodes++;
            matrix[y][x] = new_matrix[y][x];
        }
    if(same_nodes == height * width)
        return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    const struct option options[4] = {
            {"input",     required_argument, NULL, 'I'},
            {"output",    required_argument, NULL, 'O'},
            {"max_iter",  optional_argument, NULL, 'M'},
            {"dump_freq", optional_argument, NULL, 'D'},
    };
    char opt = getopt_long(argc, argv,"", options, NULL);
    char *input = (char *) malloc(20);
    char *output = (char *) malloc(20);
    int max_iter = 0, dump_freq = 0;
    while (opt != -1) {
        switch (opt) {
            case 'I':
                input = optarg;
                break;
            case 'O':
                output = optarg;
                break;
            case 'M':
                max_iter = atoi(optarg);
                break;
            case 'D':
                dump_freq = atoi(optarg);
                break;
        }
        opt = getopt_long(argc, argv,"", options, NULL);
    }

    FILE *file = fopen(input, "rb");
    char* header = (char*)malloc(14);
    fread(header, 14, 1, file);
    int size = to_int(header[2], header[3], header[4], header[5]);
    char* img_header = (char*)malloc(40);
    fread(img_header, 40, 1, file);
    int width = to_int(img_header[4], img_header[5], img_header[6], img_header[7]);
    int height = to_int(img_header[8], img_header[9], img_header[10], img_header[11]);
    int size_img = width * height * 4;
    char* color_table = (char*)malloc(size - size_img - 54);
    fread(color_table, size - size_img - 54, 1, file);
    char* pixels = (char*)malloc(size_img);
    fread(pixels, size_img, 1, file);
    char buf[size - size_img];
    fseek(file, SEEK_SET, 0);
    fread(buf, size - size_img, 1, file);
    int size_buf = size - size_img;
    int **matrix = to_matrix(pixels, width, height);
    char *filename = (char*)malloc(30);
    fclose(file);

    if(dump_freq == 0){
        dump_freq = 1;
    }
    if(max_iter != 0){
        for(int i = 1; i <= max_iter; ++i){
            game_life(matrix, width, height);
            if(i % dump_freq == 0){
                char *name = (char*)malloc(30);
                strcpy(name, output);
                name = strcat(name, "\\");
                name = strcat(name, itoa(i, filename, 10));
                name = strcat(name, ".bmp");
                FILE* new_file = fopen(name, "wb");
                pixels = to_pixels(matrix, width, height, pixels);
                fwrite(buf,size_buf, 1, new_file);
                //fwrite(pixels,1, size_img, new_file);
            }
        }
    }else{
        int i = 1;
        while (game_life(matrix, width, height) == 1){
            if(i % dump_freq == 0){
                char *filename = (char*)malloc(30);
                char *name = (char*)malloc(30);
                strcpy(name, output);
                name = strcat(name, "\\");
                name = strcat(name, itoa(i, filename, 10));
                name = strcat(name, ".bmp");
                FILE* new_file = fopen(name, "wb");
                pixels = to_pixels(matrix, width, height, pixels);
                fwrite(buf,size_buf, 1, new_file);
                fwrite(pixels,1, width * height * 4, new_file);
            }
            i++;
        }
    }
}