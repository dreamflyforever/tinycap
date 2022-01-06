#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

typedef struct Circle_Queue
{
	uint8_t *data;    //队列数据
	uint32_t size;    //队列大小
	uint32_t head;    //队列头部
	uint32_t tail;    //队列尾部
	uint32_t len;     //队列有效数据长度
}circle_queue_struct;

int circle_queue_init(circle_queue_struct *queue,
                       uint8_t* data,
                       uint32_t size);

int circle_queue_in(circle_queue_struct *queue,
                    uint8_t *data,
                    uint32_t len);

int circle_queue_out(circle_queue_struct *queue,
                     uint8_t *data,
                     uint32_t len);

int circle_queue_erase(circle_queue_struct *queue);
