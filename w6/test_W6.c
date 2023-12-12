#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define RANGE 10

struct table_node{
    int value;
    char in_use=0;
};

void addToTable(struct table_node * table, int value)
{
    struct table_node temp1;
    struct table_node temp2;
    int i=1;
    temp1=table[0];
	do
    {
        temp2 = table[i];
        table[i] = temp1;
        temp1=temp2;
        i++;
    }     while(table[i-1].in_use == 1 && i<RANGE);
    table[0].value = value;
    table[0].in_use = 1;
}

uint32_t countAVG(struct table_node * table)
{
    uint32_t i=0;
    int sum=0;
    while(table[i].in_use==1 && i<RANGE)
    {
        sum=sum+table[i].value;
        i++;
    }
    return (uint32_t)((double)sum/(double)i);
}

uint32_t countStdDev(struct table_node * table)
{
    uint32_t i=0;
    int meter=0;
    uint32_t average = countAVG(table);
    while(table[i].in_use ==1 && i<RANGE)
    {
        meter=meter+(table[i].value-average)^2;
        i++;
    }
    return (uint32_t)(sqrt((double)meter/(double)i));
}

void printTable(struct table_node * table)
{
	int i=0;
	while(i<RANGE)
	{
		printf("%d - value: %d - in_use: %d\n", i, table[i].value, table[i].in_use);
		i++;
	}
}

int main()
{
    struct table_node TABLE[RANGE];
    for(int i=0; i<10; i++)
    {
        addToTable(TABLE, (i+107)%(i+17));
        printf("avg: %d, StdDev: %d\n", countAVG(TABLE), countStdDev(TABLE));
    	printTable(TABLE);
	}
    return 0;
}
