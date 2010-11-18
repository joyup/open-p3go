/*
 * JZ4740 Driver Management Routines
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Author: <jgao@ingenic.cn>
 */

//#include <jz4740.h>
#include <dm.h>
#if(DM==1)
static struct dm_jz4740_t* dm = 0;
int jz_dm_init(void)
{
	return 0;
}
#if 0
void dumpdm()
{
	printf("dumpdm:\n");
	struct dm_jz4740_t  *dm_end = dm;
	while(dm_end)
	{
		printf("%s\n",dm_end->name);
		printf("dm->preconvert: 0x%08x\n",dm_end->preconvert);
		printf("dm->convert: 0x%08x",dm_end->convert);
		dm_end = dm_end->next;	
	}
}
#endif
int dm_register(int dev_id, struct dm_jz4740_t  *dm_in)
{
	
	if(dm == 0)
	{
		dm = dm_in;
		dm->type = dev_id;
		dm->result = 0;
		dm->next = 0;
	}else
	{
		if(dev_id == 0)
		{
			dm_in->next = dm;
			dm = dm_in;
			dm->type = dev_id;
			dm->result = 0;
	
		}else
		{
			struct dm_jz4740_t  *dm_end = dm;
			while(dm_end->next)
				dm_end = dm_end->next;
			dm_end->next = dm_in;
			dm_in->next = 0;
			dm_in->type = dev_id;
			dm_in->result = 0;
		}
	}
	return 0;
}

int dm_preconvert_all(PPLL_OPT pllopt)
{
	struct dm_jz4740_t  *dm_end = dm;
	//printf("dm_preconvert_all\n");
		
	while(dm_end)
	{
		if(dm_end->preconvert){
			 //printf("%s->preconvert\n",dm_end->name);
			 dm_end->result = dm_end->preconvert(pllopt); 
			}
		dm_end = dm_end->next;	
	}
}
int dm_preconvert(char *name,PPLL_OPT pllopt)
{
	struct dm_jz4740_t  *dm_end = dm;

	while(dm_end)
	{
		if(dm_end->name && (strcmp(name,dm_end->name) == 0) && (dm_end->preconvert))
				 return dm_end->preconvert(pllopt);
		dm_end = dm_end->next;
	}
}
int dm_convert(char *name,int val)
{
	struct dm_jz4740_t  *dm_end = dm;

	while(dm_end)
	{
		if(dm_end->name && (strcmp(name,dm_end->name) == 0) && (dm_end->convert))
			 return dm_end->convert(val);
		dm_end = dm_end->next;
	}
}
int dm_convert_all()
{
	struct dm_jz4740_t  *dm_end = dm;
	while(dm_end)
	{
		if(dm_end->convert){
			 //printf("%s->convert\n",dm_end->name);
			 dm_end->convert(dm_end->result); 
			}
		dm_end = dm_end->next;
	}
	return 1;
}

#endif
