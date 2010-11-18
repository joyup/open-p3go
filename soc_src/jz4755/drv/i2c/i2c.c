/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * http://www.ingenic.cn 
 *
 ********************** END LICENSE BLOCK **************************************
 *
 *  Author:  <dsqiu@ingenic.cn>  <zzhang@ingenic.cn> 
 *
 *  Create:   2008-11-17, by zzhagn
 *            
 *  Maintain: 
 *            
 *
 *******************************************************************************
 */

#include <jz4755.h>

//#define DEVCLK	3686400
#define DEVCLK		DEVICE_CLOCK //12000000

/* I2C protocol */
#define I2C_READ	1
#define I2C_WRITE	0

#define TIMEOUT         1000

/* error code */
#define ETIMEDOUT	1
#define ENODEV		2
#define EREMOTEIO	140


/* I2C protocol */
#define I2C_READ	1
#define I2C_WRITE	0

#define TIMEOUT         1000

/*
 * I2C bus protocol basic routines
 */

static int i2c_put_data(unsigned char data)
{
	unsigned int timeout = TIMEOUT*10;
	int timeout1 = TIMEOUT*10;
	unsigned int timeout2 = TIMEOUT*10;
	
	OUTREG8(A_I2C_I2CDR,data);
	SETREG8(A_I2C_I2CSR,I2CSR_DATA_VALID);

	while((INREG8(A_I2C_I2CSR)&I2CSR_DATA_VALID) != 0 && timeout2--);
	if (timeout2 <= 0)
	{
		os_TimeDelay(1);
//		printf("err while1 \n");
	}
TEND_RETRY:
	while (!(INREG8(A_I2C_I2CSR)&I2CSR_TEND) && timeout1--);
	if (timeout1 <= 0)
	{
		os_TimeDelay(1);
		timeout1 = TIMEOUT*10;
//		printf("err while2\n");
		goto TEND_RETRY;
	}
	while ((INREG8(A_I2C_I2CSR)&I2CSR_ACK_HIGH) && timeout)
		timeout--;
		

	if (timeout)
		return 0;
	else
	{
//		printf("error put data\n");	
		return -ETIMEDOUT;
}
}

//#ifdef CONFIG_JZ_TPANEL_ATA2508
/*
static int i2c_put_data_nack(unsigned char data)
{
	unsigned int timeout = TIMEOUT*10;

	__i2c_write(data);
	__i2c_set_drf();
	while (__i2c_check_drf() != 0);
	while (!__i2c_transmit_ended());
	while (timeout--);
		return 0;
}*/
static int i2c_put_data_nack(unsigned char data)
{
	unsigned int timeout = TIMEOUT*10;
	OUTREG8(A_I2C_I2CDR,data);
	SETREG8(A_I2C_I2CSR,I2CSR_DATA_VALID);
	while((INREG8(A_I2C_I2CSR)&I2CSR_DATA_VALID) != 0 );
	while (!(INREG8(A_I2C_I2CSR)&I2CSR_TEND));
	while (timeout--);
		return 0;
}
//#endif

/*static int i2c_get_data(unsigned char *data, int ack)
{
	int timeout = TIMEOUT*10;

	if (!ack)
		__i2c_send_nack();
	else
		__i2c_send_ack();

	while (__i2c_check_drf() == 0 && timeout)
		timeout--;

	if (timeout) {
		if (!ack)
			__i2c_send_stop();
		*data = __i2c_read();
		__i2c_clear_drf();
		return 0;
	} else
		return -ETIMEDOUT;
}*/

static int i2c_get_data(unsigned char *data, int ack)
{		
	int timeout = TIMEOUT*10;

	if (!ack)
		SETREG8(A_I2C_I2CCR,I2CCR_ACK_HIGH);
	else
		CLRREG8(A_I2C_I2CCR,I2CCR_ACK_HIGH);

	while ((INREG8(A_I2C_I2CSR)&I2CSR_DATA_VALID) == 0 && timeout)
		timeout--;

	if (timeout) {
		if (!ack)
			SETREG8(A_I2C_I2CCR,I2CCR_STOP);
		*data = INREG8(A_I2C_I2CDR);
		CLRREG8(A_I2C_I2CSR,I2CSR_DATA_VALID);
		return 0;
	} else
		return -ETIMEDOUT;		
}


void i2c_open(void)
{
	CLRREG32(A_CPM_CLKGR,CLKGR_STOP_I2C);
	
	OUTREG32(A_GPIO_PXFUNS(4), (3 << 12));		//GPE12,GPE13
	OUTREG32(A_GPIO_PXSELC(4), (3 << 12)); 
	
	OUTREG16(A_I2C_I2CGR, (DEVCLK) / (16*(10000)) - 1 );		

	SETREG8(A_I2C_I2CCR,I2CCR_ENABLE); 
}

void i2c_close(void)
{
	udelay(300); /* wait for STOP goes over. */
	CLRREG8(A_I2C_I2CCR,I2CCR_ENABLE);
}

void i2c_setclk(unsigned int i2cclk)
{
	//__i2c_set_clk(DEVCLK, i2cclk);
	OUTREG16(A_I2C_I2CGR, (DEVCLK) / (16*(i2cclk)) - 1 );
}

int i2c_lseek(unsigned char device, unsigned char offset)
{
	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;
	if (i2c_put_data(offset) < 0)
		goto address_err;
	return 0;
 device_err:
	printf("No I2C device (0x%02x) installed.\n", device);
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return -ENODEV;
 address_err:
	printf("No I2C device (0x%02x) response.\n", device);
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return -EREMOTEIO;
}

int i2c_read(unsigned char device, unsigned char *buf,
	       unsigned char address, int count)
{
	int cnt = count;
	int timeout = 5;

L_try_again:

	if (timeout < 0)
		goto L_timeout;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_werr;
	if (i2c_put_data(address) < 0)
		goto address_err;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
//	udelay(1000);


	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_READ ) < 0)
		goto device_werr;

	//__i2c_send_ack();	/* Master sends ACK for continue reading */
	while (cnt) {
		if (cnt == 1) {
			if (i2c_get_data(buf, 0) < 0)
				break;
		} else {
			if (i2c_get_data(buf, 1) < 0)
				break;
		}
		cnt--;
		buf++;
	}
	
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_rerr:
 device_werr:
 address_err:
	timeout --;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto L_try_again;

L_timeout:
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	printf("Read I2C device 0x%2x failed.\n", device);
	return -ENODEV;
}

int i2c_write(unsigned char device, unsigned char *buf,
		unsigned char address, int count)
{
	int cnt = count;
	int cnt_in_pg;
	int timeout = 5;
	unsigned char *tmpbuf;
	unsigned char tmpaddr;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */

 W_try_again:
	if (timeout < 0)
		goto W_timeout;

	cnt = count;
	tmpbuf = (unsigned char *)buf;
	tmpaddr = address;

 start_write_page:
	cnt_in_pg = 0;
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;
	if (i2c_put_data(tmpaddr) < 0)
		goto address_err;
	while (cnt) {
		if (++cnt_in_pg > 8) {
			SETREG8(A_I2C_I2CCR,I2CCR_STOP);
			mdelay(1);
			tmpaddr += 8;
			goto start_write_page;
		}
		if (i2c_put_data(*tmpbuf) < 0)
			break;
		cnt--;
		tmpbuf++;
	}

	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_err:
 address_err:
	timeout--;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto W_try_again;

 W_timeout:
	printf("Write I2C device 0x%2x failed.\n", device);
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return -ENODEV;
}

int i2c_read_16(unsigned char device, unsigned char *buf,
	       unsigned short address, int count)
{
	int cnt = count;
	int timeout = 5;
	unsigned char tmpaddrh, tmpaddrl;

	tmpaddrh = (unsigned char)((address >> 8) & 0xff);
	tmpaddrl = (unsigned char)(address & 0xff);

L_try_again:

	if (timeout < 0)
		goto L_timeout;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_werr;
	if (i2c_put_data_nack(tmpaddrh) < 0)
		goto address_err;
	if (i2c_put_data(tmpaddrl) < 0)
		goto address_err;

	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_READ ) < 0)
		goto device_rerr;
	//__i2c_send_ack();	/* Master sends ACK for continue reading */
	while (cnt) {
		if (cnt == 1) {
			if (i2c_get_data(buf, 0) < 0)
				break;
		} else {
			if (i2c_get_data(buf, 1) < 0)
				break;
		}
		cnt--;
		buf++;
	}

	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_rerr:
 device_werr:
 address_err:
	timeout --;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto L_try_again;

L_timeout:
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	printf("Read I2C device 0x%2x failed.\n", device);
	return -ENODEV;
}

int i2c_write_16(unsigned char device, unsigned char *buf,
		unsigned short address, int count)
{
	int cnt = count;
	int cnt_in_pg;
	int timeout = 5;
	unsigned char *tmpbuf;
	unsigned char tmpaddrh, tmpaddrl;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */

 W_try_again:
	if (timeout < 0)
		goto W_timeout;

	cnt = count;
	tmpbuf = (unsigned char *)buf;
	tmpaddrh = (unsigned char)((address >> 8) & 0xff);
	tmpaddrl = (unsigned char)(address & 0xff);

 start_write_page:
	cnt_in_pg = 0;
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;
	if (i2c_put_data_nack(tmpaddrh) < 0)
		goto address_err;
	if (i2c_put_data(tmpaddrl) < 0)
		goto address_err;
	while (cnt) {
		if (++cnt_in_pg > 8) {
			SETREG8(A_I2C_I2CCR,I2CCR_STOP);
			mdelay(1);
			tmpaddrh += 8;
			goto start_write_page;
		}
		if (i2c_put_data(*tmpbuf) < 0)
			break;
		cnt--;
		tmpbuf++;
	}

	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_err:
	printf("1:Write I2C device 0x%2x failed.\n", device);
 address_err:
	printf("2:Write I2C address 0x%2x failed.\n", address);
	timeout--;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto W_try_again;

 W_timeout:
	printf("3:Write I2C device 0x%2x failed.\n", device);
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return -ENODEV;
}



/*************************************************************************************
		i2c 16bits address and data read or write( growise allen)
*************************************************************************************/
unsigned char dvbt_DiBcom_i2c_write_sub(int device, unsigned char *i2c_tx_buf, int txcnt)
{
	int cnt = txcnt;
	int timeout = 5;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */

 W_try_again:
	if (timeout < 0)
		goto W_timeout;
 start_write_page:
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;

	while (cnt) 
	{
		if (i2c_put_data(*i2c_tx_buf) < 0)
			break;
		cnt--;
		i2c_tx_buf++;
	}

	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return cnt;			//if cnt=0--success ,or fail
 device_err:
 address_err:
	timeout--;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto W_try_again;

 W_timeout:
	printf("Write I2C device 0x%2x failed.\n", device);
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return ETIMEDOUT;


}

unsigned char dvbt_DiBcom_i2c_read_sub(int device, unsigned char *i2c_tx_buf, int txcnt, unsigned char *i2c_rx_buf, int rxcnt)
{

#if 1
	int cnt = txcnt;
	int timeout = 5;

L_try_again:

	if (timeout < 0)
		goto L_timeout;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_werr;

	while (cnt) 
	{
		if (i2c_put_data(*i2c_tx_buf) < 0)
			break;
		cnt--;
		i2c_tx_buf++;
	}
	if(cnt)
	{
		SETREG8(A_I2C_I2CCR,I2CCR_STOP);
		return cnt;			//if cnt=0--success ,or fail
	}
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_READ ) < 0)
		goto device_werr;

	cnt = rxcnt;
	while (cnt) {
		if (cnt == 1) {
			if (i2c_get_data(i2c_rx_buf, 0) < 0)
				break;
		} else {
			if (i2c_get_data(i2c_rx_buf, 1) < 0)
				break;
		}
		cnt--;
		i2c_rx_buf++;
	}
	
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return cnt;			//if cnt=0--success ,or fail
 device_rerr:
 device_werr:
 address_err:
	timeout --;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto L_try_again;

L_timeout:
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	printf("Read I2C device 0x%2x failed.\n", device);
	return ETIMEDOUT;
#else
return txcnt;	
#endif

 
}

int dvb_i2c_write(unsigned char device, unsigned char *buf, int count);
unsigned char dvbt_DiBcom_i2c_write(int device, unsigned char *i2c_tx_buf, int txcnt)
{ 
	int ret;
//	i2c_open();
//	i2c_setclk(i2c_clk);
//  ret = dvb_i2c_write(device>> 1,i2c_tx_buf,txcnt);
	ret = dvbt_DiBcom_i2c_write_sub(device>> 1,i2c_tx_buf,txcnt);
//	i2c_close();
	return ret;
}

/*
 * Get sensor register through i2c bus
 */
 unsigned char dvbt_DiBcom_i2c_read(int device, unsigned char *i2c_tx_buf, int txcnt, unsigned char *i2c_rx_buf, int rxcnt)
{
	unsigned char ret;
//	i2c_open();
//	i2c_setclk(i2c_clk);
	ret=dvbt_DiBcom_i2c_read_sub(device >> 1,i2c_tx_buf,txcnt,i2c_rx_buf,rxcnt);
//	i2c_close();
	return ret;
}



int i2c_write_address_data_16(unsigned char device, unsigned short *buf,
		unsigned short address, int count)
{
	int cnt = count;
	int cnt_in_pg;
	int timeout = 5;
	unsigned short *tmpbuf;
	unsigned char tmpaddrh, tmpaddrl;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */

 W_try_again:
	if (timeout < 0)
		goto W_timeout;

	cnt = count;
	tmpbuf = (unsigned short *)buf;
	tmpaddrh = (unsigned char)((address >> 8) & 0xff);
	tmpaddrl = (unsigned char)(address & 0xff);

 start_write_page:
	cnt_in_pg = 0;
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;
//	if (i2c_put_data_nack(tmpaddrh) < 0)
	if (i2c_put_data(tmpaddrh) < 0)
		goto address_err;
	if (i2c_put_data(tmpaddrl) < 0)
		goto address_err;
	while (cnt) {
		if (++cnt_in_pg > 8) {
			SETREG8(A_I2C_I2CCR,I2CCR_STOP);
			mdelay(1);
			tmpaddrh += 8;
			goto start_write_page;
		}
		if (i2c_put_data((unsigned char) ((*tmpbuf & 0xff00) >> 8)) < 0)
			break;
		if (i2c_put_data((unsigned char) (*tmpbuf & 0x00ff)) < 0)
			break;
		cnt--;
		tmpbuf++;
	}

	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_err:
	printf("1:Write I2C device 0x%2x failed.\n", device);
 address_err:
	printf("2:Write I2C address 0x%2x failed.\n", address);
	timeout--;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto W_try_again;

 W_timeout:
	printf("3:Write I2C device 0x%2x failed.\n", device);
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return -ENODEV;
}


int i2c_read_address_data_16(unsigned char device, unsigned short *buf,
	       unsigned short address, int count)
{
	int cnt = count;
	int timeout = 5;
	unsigned char tmpaddrh, tmpaddrl;

	unsigned char *tmpbuf;

	tmpbuf = ( unsigned char*)buf;	

	tmpaddrh = (unsigned char)((address >> 8) & 0xff);
	tmpaddrl = (unsigned char)(address & 0xff);

L_try_again:

	if (timeout < 0)
		goto L_timeout;

	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_werr;
//	if (i2c_put_data_nack(tmpaddrh) < 0)
	if (i2c_put_data(tmpaddrh) < 0)
		goto address_err;
	if (i2c_put_data(tmpaddrl) < 0)
		goto address_err;

	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_READ ) < 0)
		goto device_rerr;
	//__i2c_send_ack();	/* Master sends ACK for continue reading */
	while (cnt) {
		if (cnt == 1) {
			if (i2c_get_data(tmpbuf+1, 1) < 0)
				break;
			if (i2c_get_data(tmpbuf, 0) < 0)
				break;
		} else {
			if (i2c_get_data(tmpbuf+1, 1) < 0)
				break;
			if (i2c_get_data(tmpbuf, 1) < 0)
				break;
		}
		cnt--;
		tmpbuf++;
		tmpbuf++;
	}

	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_rerr:
 device_werr:
 address_err:
	timeout --;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto L_try_again;

L_timeout:
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	printf("Read I2C device 0x%2x failed.\n", device);
	return -ENODEV;
}



int dvb_write_address_data16(unsigned short reg, unsigned short val)
{ 
	int ret;
	i2c_open();
//	i2c_setclk(i2c_clk);
	ret = i2c_write_address_data_16(0x12 >> 1, &val, reg, 1);
	i2c_close();
	return ret;
}

/*
 * Get sensor register through i2c bus
 */
unsigned short dvb_read_address_data16(unsigned short reg)
{
	unsigned short val;
	i2c_open();
//	i2c_setclk(i2c_clk);
	i2c_read_address_data_16(0x12 >> 1, &val, reg, 1);
	i2c_close();
	return val;
}



/********************************************
	dvb i2c Read/Write Functions
********************************************/

int dvb_i2c_read(unsigned char device, unsigned char *buf, int count)
{
	int cnt = count;
	int timeout = 5;
	
L_try_again:

	if (timeout < 0)
		goto L_timeout;

	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_READ ) < 0)
		goto device_werr;

	while (cnt) {
		if (cnt == 1) {
			if (i2c_get_data(buf, 0) < 0)
				break;
		} else {
			if (i2c_get_data(buf, 1) < 0)
				break;
		}
		cnt--;
		buf++;
	}
	
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_rerr:
 device_werr:
 address_err:
	timeout --;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto L_try_again;

L_timeout:
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	printf("Read I2C device 0x%2x failed.\n", device);
	return -ENODEV;
}

int dvb_i2c_write(unsigned char device, unsigned char *buf, int count)
{
	int cnt = count;
	int cnt_in_pg;
	int timeout = 5;
	unsigned char *tmpbuf;
	
	//__i2c_send_nack();	/* Master does not send ACK, slave sends it */

 W_try_again:
	if (timeout < 0)
		goto W_timeout;

	cnt = count;
	tmpbuf = (unsigned char *)buf;

 start_write_page:
	cnt_in_pg = 0;
	SETREG8(A_I2C_I2CCR,I2CCR_START);
	if (i2c_put_data( (device << 1) | I2C_WRITE ) < 0)
		goto device_err;

	while (cnt) {
		if (++cnt_in_pg > 8) {
			SETREG8(A_I2C_I2CCR,I2CCR_STOP);
			mdelay(1);
			goto start_write_page;
		}
		if (i2c_put_data(*tmpbuf) < 0)
			break;
		cnt--;
		tmpbuf++;
	}

	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return count - cnt;
 device_err:
 address_err:
	timeout--;
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	goto W_try_again;

 W_timeout:
	printf("Write I2C device 0x%2x failed.\n", device);
	SETREG8(A_I2C_I2CCR,I2CCR_STOP);
	return -ENODEV;
}
