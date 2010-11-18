#ifndef _DMA_CHANNEL_H_
#define _DMA_CHANNEL_H_

#define BCH_DMA_CHANNEL                         0
#define NAND_DMA_CHANNEL                        1
#define SSFDC_DMA_CHANNEL 			2
#define CODEC_COPY_CHANNEL			3

#define AUDIO_READ_CHANNEL			6
#define YUV_COPY_CHANNEL            6
#define AUDIO_WRITE_CHANNEL			7
#define MSC_DMA_CHANNEL				8
#define TSSI_DMA_CHANNEL    		9


#define BCH_DMA_CHANNEL                         0
#define NAND_DMA_CHANNEL                        1
#define BCH_DMA_CONTROL                         0
#define NAND_DMA_CONTROL                        0
#define BCH_DMA_CHANNEL_BIT                     (1 << 0)
#define NAND_DMA_CHANNEL_BIT                     (1 << 1)



#define DMA_request(ch,irq_handler,arg) \
do{										\
	if(irq_handler) 					\
		request_irq(EIRQ_DMA_BASE + ch, irq_handler, arg);\
}while(0)

#endif //_DMA_CHANNEL_H_
       
  
