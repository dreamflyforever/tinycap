#define __disable_irq()  
#define __enable_irq()

#include <q.h>

/**
 * @biref 环形队列初始化
 * @param  circle_queue_struct队列结构体
 data队列缓冲区
 size队列大小
 * @retval 0成功 1失败
 */
int circle_queue_init(circle_queue_struct *queue,
		uint8_t* data,
		uint32_t size)
{
	if(data==NULL || size<2)
		return -1;
	queue->data=data;
	queue->len=0;
	queue->size=size;
	queue->head=0;
	queue->tail=0;
	return 0;
}

/**
 * @biref 环形队列擦除
 * @param  circle_queue_struct队列结构体
 * @retval 0成功 1失败
 */
int circle_queue_erase(circle_queue_struct *queue)
{
	if(queue==NULL)
		return -1;

	__disable_irq();
	//queue->data=NULL;
	queue->len=0;
	//queue->size=0;
	queue->head=0;
	queue->tail=0;
	__enable_irq();
	return 0;
}

/**
 * @biref 获取环形队列有效数据长度
 * @retval 环形队列长度
 */
unsigned int circle_queue_len(circle_queue_struct *queue)
{
	return queue->len;
}

/**
 * @biref 环形队列入队操作
 * @param  circle_queue_struct队列结构体
 data入队数据数组指针
 len入队数据长度
 * @retval -1错误指针为空 -2入队数据过长非法 -3队列当前空闲大小不足  -4队列满 0成功
 */
int circle_queue_in(circle_queue_struct *queue,
		uint8_t *data,
		uint32_t len)
{
	int ret=0;

	__disable_irq();

	if(queue==NULL || data==NULL)
	{
		ret=-1;
		goto out;
	}
	if(queue->len >= queue->size)
	{
		ret=-2;
		goto out;
	}
	if(len>(queue->size - queue->len))
	{
		ret=-3;
		goto out;
	}

	for(uint32_t i=0;i<len;i++)
	{
		if(queue->len >= queue->size)
		{
			ret=-4;
			goto out;
		}
		queue->data[queue->tail]=*data++;
		queue->tail = (++queue->tail) % (queue->size);
		queue->len++;
	}

out:
	__enable_irq();
	return ret;
}

/**
 * @biref 环形队列出队操作
 * @param  circle_queue_struct队列结构体
 data出队接收数据数组指针
 len需要出队数
 * @retval -1错误指针为空 其他：出队数据字节数
 */
int circle_queue_out(circle_queue_struct *queue,
		uint8_t *data,
		uint32_t len)
{
	int ret=0;

	__disable_irq();

	if(queue==NULL || data==NULL)
	{
		ret=-1;
		goto out;
	}
	if(queue->len==0)
	{
		ret=0;
		goto out;
	}
	if(queue->len<len)
		len=queue->len;

	for(uint32_t i=0;i<len;i++)
	{
		*data = queue->data[queue->head];
		data++;
		ret++;
		queue->head=(++queue->head) % queue->size;
		queue->len--;
	}

out:
	__enable_irq();
	return ret;
}

/**
 * @biref 环形队列预览数据指令
 * @param  circle_queue_struct队列结构体
 data预览接收数据数组指针
 len需要预览的数据字节数
 offset预览起始数据偏移队列头字节数
 * @note 注意此操作不会出队数据
 * @retval -1错误指针为空 -2有效数据长度不够 其他：出队数据字节数
 */
int circle_queue_preview(circle_queue_struct *queue,
		uint8_t *data,
		uint32_t len,
		uint32_t offset)
{
	int ret=0;
	uint32_t queue_head=0;

	__disable_irq();
	if(queue==NULL || data==NULL)
	{
		ret=-1;
		goto out;
	}
	if(queue->len==0)
	{
		ret=-2;
		goto out;
	}
	if(queue->len < (offset+len))
	{
		ret=-3;
		goto out;
	}

	queue_head=(queue->head+offset) % queue->size;
	for(uint32_t i=0;i<len;i++)
	{
		*data = queue->data[queue_head];
		data++;
		ret++;
		queue_head++;
		queue_head %= queue->size;
	}

out:
	__enable_irq();
	return ret;
}

/**
 * @biref 环形队列删除数据指令
 * @param  circle_queue_struct队列结构体
 len需要删除的数据字节数
 * @retval -1错误指针为空 其他：实际删除数据字节数
 */
int circle_queue_delete_data(circle_queue_struct *queue,
		uint32_t len)
{
	int ret=0;

	__disable_irq();
	if(queue==NULL)
	{
		ret=-1;
		goto out;
	}
	if(queue->len<len)
		len=queue->len;
	queue->head=(queue->head+len)%queue->size;
	queue->len-=len;

	ret=len;
out:
	__enable_irq();
	return ret;
}
