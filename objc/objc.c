#include <stdio.h>

struct animal {
    int age;
    void* vptr;
};

struct machine{
    int chargeVolt;
    void* vptr;
};


struct robot{
    struct animal animal_part;
    struct machine machine_part;
};

int main(){
    printf("hello wrold\n");
}
