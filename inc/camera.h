#ifndef __CAMERA_H__
#define __CAMERA_H__

#ifndef u8
#define u8	unsigned char
#endif

#ifndef u16
#define u16	unsigned short
#endif

#ifndef u32
#define u32	unsigned int
#endif

//#ifndef NULL
//#define NULL	0
//#endif
#define  JZ4740_IPU  1

#ifndef PLATFORM_CETUS


#define VIRT(addr) 	((unsigned long)(addr) + 0x80000000)

 /* Only for test */
#if 1
#define IMG_WIDTH 640
#define IMG_HEIGHT 480
#define IMG_BPP 16
#endif
/*
 * Define the Max Image Size
 */

#define MAX_IMAGE_WIDTH  2048
#define MAX_IMAGE_HEIGHT 2048
#define MAX_IMAGE_BPP    16  
#define MAX_FRAME_SIZE   (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * MAX_IMAGE_BPP / 8)


/* Parameters of camera */
#define CIM_BUF_NUM 3
#define JPEG_BUF_SIZE ( 800*1024)
#undef CIM_UNCACHED
#define CIM_UNCACHED(addr)	((unsigned int)(addr) | 0xa0000000)

typedef struct
{
	u32 in_width;
	u32 in_height;
	u32 in_bpp;
	u32 in_fmt;
	u32 out_width;
	u32 out_height;
	u8* y_buf;
	u8* u_buf;
	u8* v_buf;
} img_param_t;


typedef struct
{
	unsigned int y_buf;
	unsigned int u_buf;
	unsigned int v_buf;
	unsigned int o_fmt;
} ipu_phy_addr_t;

//extern ipu_phy_addr_t ipu_addr;

/*
 * Common interface (For Midware Interface)-----
 */
extern void camera_open(img_param_t *img);		/* Init and start camera */
extern void camera_close(void);		/* Close camera */
extern void camera_pause(void);		/* Disable CIM modules and stop to capture data */
extern void camera_continue(void);	/* Cotinue to capture data from sensor */
extern void docapture(img_param_t *img);		/* Preview one frame to LCD */
extern void enter_night_mode(void);	/* Camera enter night mode */
extern void exit_night_mode(void);	/* Camera exit night mode */
extern void enter_normal_mode(void);	/* Camera enter normal mode */
extern void enter_black_white_mode(void);/* Camera enter black & white mode */
extern void enter_antique_mode(void); 	/* Camera enter antique mode */
extern void enter_negative_mode(void); 	/* Camera enter negative mode */
extern void adjust_bright (unsigned int level); /* Adjust brightness 1 -> 5 */
extern void enter_black_white_negative_mode(void);/* Camera enter black & white negative mode*/
/*
 * CIM interface
 */
extern int cim_init(img_param_t *img);		/* Init cim module*/
extern void cim_start(void);		/* Start cim and dma*/
extern void cim_close(void);		/* Stop cim*/
extern unsigned char * cim_read(void);	/* Read data and do preview*/
extern unsigned char * cim_snapshot(int mode);
extern void cim_snapshot_finish(void);
extern void split_yuv422(unsigned char * yuv_buf,unsigned char * y_buf,unsigned char * u_buf,unsigned char * v_buf);
/*
 * Sensor interface
 */
extern unsigned char sensor_read_reg(unsigned char reg);
extern int sensor_write_reg(unsigned char reg, unsigned char val);
extern unsigned char sensor_read_reg16(unsigned short reg);
extern int sensor_write_reg16(unsigned short reg, unsigned char val);

/*
 * Sensor init function
 */
extern int init_sensor(img_param_t *img);
//extern void enter_night_mode(void);
//extern void exit_night_mode(void);

extern void init_ov7660(void);
extern void init_hv7131(void);
extern void init_ksmov7649(void);
extern void init_ov2640(void);

/*
 * IPU API
 */
extern int jz47_ipu_init (img_param_t *img);
extern int jz47_put_image_with_ipu(img_param_t *img);
extern void ipu_data_transer(void);
extern void ipu_wait_end(void);
#else
#define VIRT(addr) 	((unsigned long)(addr) + 0x80000000)

 /* Only for test */
#if 1
#define IMG_WIDTH 640
#define IMG_HEIGHT 480
#define IMG_BPP 16
#endif
/*
 * Define the Max Image Size
 */

#define MAX_IMAGE_WIDTH  2048
#define MAX_IMAGE_HEIGHT 2048
#define MAX_IMAGE_BPP    16  
#define MAX_FRAME_SIZE   (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * MAX_IMAGE_BPP / 8)


/* Parameters of camera */
#define CIM_BUF_NUM 3
#define JPEG_BUF_SIZE ( 800*1024)
#undef CIM_UNCACHED
#define CIM_UNCACHED(addr)	((unsigned int)(addr) | 0xa0000000)

typedef struct
{
	u32 in_width;
	u32 in_height;
	u32 in_bpp;
	u32 in_fmt;
	u32 out_width;
	u32 out_height;
	u8* y_buf;
	u8* u_buf;
	u8* v_buf;
} img_param_cim;


typedef struct
{
	unsigned int y_buf;
	unsigned int u_buf;
	unsigned int v_buf;
	unsigned int o_fmt;
} ipu_phy_addr_t;

//extern ipu_phy_addr_t ipu_addr;

/*
 * Common interface (For Midware Interface)-----
 */
extern void camera_open(img_param_cim *img);		/* Init and start camera */
extern void camera_close(void);		/* Close camera */
extern void camera_pause(void);		/* Disable CIM modules and stop to capture data */
extern void camera_continue(void);	/* Cotinue to capture data from sensor */
extern void docapture(img_param_cim *img);		/* Preview one frame to LCD */
extern void enter_night_mode(void);	/* Camera enter night mode */
extern void exit_night_mode(void);	/* Camera exit night mode */
extern void enter_normal_mode(void);	/* Camera enter normal mode */
extern void enter_black_white_mode(void);/* Camera enter black & white mode */
extern void enter_antique_mode(void); 	/* Camera enter antique mode */
extern void enter_negative_mode(void); 	/* Camera enter negative mode */
extern void adjust_bright (unsigned int level); /* Adjust brightness 1 -> 5 */
extern void enter_black_white_negative_mode(void);/* Camera enter black & white negative mode*/
/*
 * CIM interface
 */
extern int cim_init(img_param_cim *img);		/* Init cim module*/
extern void cim_start(void);		/* Start cim and dma*/
extern void cim_close(void);		/* Stop cim*/
extern unsigned char * cim_read(void);	/* Read data and do preview*/
extern unsigned char * cim_snapshot(int mode);
extern void cim_snapshot_finish(void);
extern void split_yuv422(unsigned char * yuv_buf,unsigned char * y_buf,unsigned char * u_buf,unsigned char * v_buf);
/*
 * Sensor interface
 */
extern unsigned char sensor_read_reg(unsigned char reg);
extern int sensor_write_reg(unsigned char reg, unsigned char val);
extern unsigned char sensor_read_reg16(unsigned short reg);
extern int sensor_write_reg16(unsigned short reg, unsigned char val);

/*
 * Sensor init function
 */
extern int init_sensor(img_param_cim *img);
//extern void enter_night_mode(void);
//extern void exit_night_mode(void);

extern void init_ov7660(void);
extern void init_hv7131(void);
extern void init_ksmov7649(void);
extern void init_ov2640(void);

/*
 * IPU API
 */
//extern int jz47_ipu_init (img_param_t *img);
//extern int jz47_put_image_with_ipu(img_param_t *img);
//extern void ipu_data_transer(void);
//extern void ipu_wait_end(void);
#endif
#endif /* __CAMERA_H__ */
