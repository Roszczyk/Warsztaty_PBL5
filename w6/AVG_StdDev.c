#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define RANGE 10

struct table_node{
    int value;
    char in_use;
};

void addToTable(struct table_node * table, int value)
{
    struct table_node temp1;
    struct table_node temp2;
    int i=1;
    temp1=table[0];
    while(table[i].in_use == 1 && i<RANGE)
    {
        temp2 = table[i];
        table[i] = temp1;
        temp1=temp2;
        i++;
    }
    table[0].value = value;
    table[0].in_use = 1;
}

uint32_t countAVG(struct table_node * table)
{
    uint32_t i=0;
    int sum=0;
    while(table[i].in_use && i<RANGE)
    {
        sum=sum+table[i].value;
        i++;
    }
    return (uint32_t)(sum/i);
}

uint32_t countStdDev(struct table_node * table)
{
    uint32_t i=0;
    int meter=0;
    uint32_t average = countAVG(table);
    while(table[i].in_use && i<RANGE)
    {
        meter=meter+(table[i].value-average)^2;
        i++;
    }
    return (uint32_t)(sqrt(meter/i));
}

int main()
{
    struct table_node TABLE[RANGE];
    for(int i=0; i<10; i++)
    {
        addToTable(TABLE, i+10);
        printf("avg: %d, StdDev: %d\n", countAVG(TABLE), countStdDev(TABLE));
    }
    return 0;
}