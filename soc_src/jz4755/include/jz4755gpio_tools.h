

#define __gpio_as_func0(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXFUNS(p),(1 << o));		\
	OUTREG32(A_GPIO_PXSELC(p),(1 << o));		\
} while (0)

#define __gpio_as_func1(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXFUNS(p),(1 << o));		\
	OUTREG32(A_GPIO_PXSELS(p),(1 << o));		\
} while (0)


//-------------------------------------------
// GPIO or Interrupt Mode

#define __gpio_get_port(p)	(INREG32(A_GPIO_PXPIN(p)))

#define __gpio_port_as_output(p, o)		\
do {						\
	OUTREG32(A_GPIO_PXFUNC(p),(1 << o));		\
	OUTREG32(A_GPIO_PXSELC(p),(1 << o));		\
	OUTREG32(A_GPIO_PXDIRS(p),(1 << o));		\
} while (0)

#define __gpio_port_as_input(p, o)		\
do {						\
	OUTREG32(A_GPIO_PXFUNC(p),(1 << o));		\
	OUTREG32(A_GPIO_PXSELC(p),(1 << o));		\
	OUTREG32(A_GPIO_PXDIRC(p),(1 << o));		\
} while (0)

#define __gpio_port_as_sleep(p, o)		\
do {						\
	OUTREG32(A_GPIO_PXFUNC(p),(o));		\
	OUTREG32(A_GPIO_PXSELC(p),(o));		\
	OUTREG32(A_GPIO_PXDIRC(p),(o));		\
	OUTREG32(A_GPIO_PXPEC(p),(o));		\
} while (0)

#define __gpio_as_output(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_output(p, o);		\
} while (0)

#define __gpio_as_input(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_input(p, o);		\
} while (0)

#define __gpio_set_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXDATS(p),(1 << o));		\
} while (0)

#define __gpio_clear_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXDATC(p),(1 << o));		\
} while (0)

#define __gpio_get_pin(n)			\
({						\
	unsigned int p, o, v;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	if (__gpio_get_port(p) & (1 << o))	\
		v = 1;				\
	else					\
		v = 0;				\
	v;					\
})


#define __gpio_as_irq_high_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXIMS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXTRGC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFUNC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXSELS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXDIRS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFLGC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXIMC(p) , (1 << o));		\
} while (0)

#define __gpio_as_irq_low_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXIMS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXTRGC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFUNC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXSELS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXDIRC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFLGC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXIMC(p) , (1 << o));		\
} while (0)

#define __gpio_as_irq_rise_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXIMS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXTRGS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFUNC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXSELS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXDIRS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFLGC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXIMC(p) , (1 << o));		\
} while (0)

#define __gpio_as_irq_fall_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXIMS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXTRGS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFUNC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXSELS(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXDIRC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXFLGC(p) , (1 << o));		\
	OUTREG32(A_GPIO_PXIMC(p) , (1 << o));		\
} while (0)

#define __gpio_mask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXIMS(p),(1 << o));		\
} while (0)

#define __gpio_unmask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXIMC(p),(1 << o));		\
} while (0)

#define __gpio_ack_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXFLGC(p),(1 << o));		\
} while (0)

#define __gpio_get_irq()			\
({						\
	unsigned int p, i, tmp, v = 0;		\
	for (p = 3; p >= 0; p--) {		\
		tmp = INREG32(A_GPIO_PXFLG(p));	\
		for (i = 0; i < 32; i++)	\
			if (tmp & (1 << i))	\
				v = (32*p + i);	\
	}					\
	v;					\
})

#define __gpio_group_irq(n)			\
({						\
	register int tmp, i;			\
	tmp = INREG32(A_GPIO_PXFLG(n)) & (~INREG32(A_GPIO_PXIM(n)));		\
	for (i=31;i>=0;i--)			\
		if (tmp & (1 << i))		\
			break;			\
	i;					\
})

#define __gpio_enable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXPEC(p),(1 << o));		\
} while (0)

#define __gpio_disable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	OUTREG32(A_GPIO_PXPES(p),(1 << o));		\
} while (0)

////////////////////////////////////////////////////
//#define REG_GPIO_PXPIN(n)	REG32(A_GPIO_PXPIN(n)	)//REG32(GPIO_PXPIN((n)))  /* PIN level */
//#define REG_GPIO_PXDAT(n)	REG32(A_GPIO_PXDAT(n)	)//REG32(GPIO_PXDAT((n)))  /* 1: interrupt pending */
//#define REG_GPIO_PXDATS(n)	REG32(A_GPIO_PXDATS(n))//REG32(GPIO_PXDATS((n)))
//#define REG_GPIO_PXDATC(n)	REG32(A_GPIO_PXDATC(n))//REG32(GPIO_PXDATC((n)))
//#define REG_GPIO_PXIM(n)	REG32(A_GPIO_PXIM(n)	)//REG32(GPIO_PXIM((n)))   /* 1: mask pin interrupt */
//#define REG_GPIO_PXIMS(n)	REG32(A_GPIO_PXIMS(n)	)//REG32(GPIO_PXIMS((n)))
//#define REG_GPIO_PXIMC(n)	REG32(A_GPIO_PXIMC(n)	)//REG32(GPIO_PXIMC((n)))
//#define REG_GPIO_PXPE(n)	REG32(A_GPIO_PXPE(n)	)//REG32(GPIO_PXPE((n)))   /* 1: disable pull up/down */
//#define REG_GPIO_PXPES(n)	REG32(A_GPIO_PXPES(n)	)//REG32(GPIO_PXPES((n)))
//#define REG_GPIO_PXPEC(n)	REG32(A_GPIO_PXPEC(n)	)//REG32(GPIO_PXPEC((n)))
//#define REG_GPIO_PXFUN(n)	REG32(A_GPIO_PXFUN(n)	)//REG32(GPIO_PXFUN((n)))  /* 0:GPIO or intr, 1:FUNC */
//#define REG_GPIO_PXFUNS(n)	REG32(A_GPIO_PXFUNS(n))//REG32(GPIO_PXFUNS((n)))
//#define REG_GPIO_PXFUNC(n)	REG32(A_GPIO_PXFUNC(n))//REG32(GPIO_PXFUNC((n)))
//#define REG_GPIO_PXSEL(n)	REG32(A_GPIO_PXSEL(n)	)//REG32(GPIO_PXSEL((n))) /* 0:GPIO/Fun0,1:intr/fun1*/
//#define REG_GPIO_PXSELS(n)	REG32(A_GPIO_PXSELS(n))//REG32(GPIO_PXSELS((n)))
//#define REG_GPIO_PXSELC(n)	REG32(A_GPIO_PXSELC(n))//REG32(GPIO_PXSELC((n)))
//#define REG_GPIO_PXDIR(n)	REG32(A_GPIO_PXDIR(n)	)//REG32(GPIO_PXDIR((n))) /* 0:input/low-level-trig/falling-edge-trig, 1:output/high-level-trig/rising-edge-trig */
//#define REG_GPIO_PXDIRS(n)	REG32(A_GPIO_PXDIRS(n))//REG32(GPIO_PXDIRS((n)))
//#define REG_GPIO_PXDIRC(n)	REG32(A_GPIO_PXDIRC(n))//REG32(GPIO_PXDIRC((n)))
//#define REG_GPIO_PXTRG(n)	REG32(A_GPIO_PXTRG(n)	)//REG32(GPIO_PXTRG((n))) /* 0:level-trigger, 1:edge-trigger */
//#define REG_GPIO_PXTRGS(n)	REG32(A_GPIO_PXTRGS(n))//REG32(GPIO_PXTRGS((n)))
//#define REG_GPIO_PXTRGC(n)	REG32(A_GPIO_PXTRGC(n))//REG32(GPIO_PXTRGC((n)))
//#define REG_GPIO_PXFLG(n)	REG32(A_GPIO_PXFLG(n)	)//REG32(GPIO_PXFLG((n))) /* interrupt flag */
//#define REG_GPIO_PXFLGC(n)	REG32(A_GPIO_PXFLGC(n))//REG32(GPIO_PXFLGC((n))) /* interrupt flag */
//
/*
#define __gpio_as_func0(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFUNS(p) = (1 << o);		\
	REG_GPIO_PXSELC(p) = (1 << o);		\
} while (0)

#define __gpio_as_func1(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFUNS(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
} while (0)


//-------------------------------------------
// GPIO or Interrupt Mode

#define __gpio_get_port(p)	(REG_GPIO_PXPIN(p))

#define __gpio_port_as_output(p, o)		\
do {						\
    REG_GPIO_PXFUNC(p) = (1 << (o));		\
    REG_GPIO_PXSELC(p) = (1 << (o));		\
    REG_GPIO_PXDIRS(p) = (1 << (o));		\
} while (0)

#define __gpio_port_as_input(p, o)		\
do {						\
    REG_GPIO_PXFUNC(p) = (1 << (o));		\
    REG_GPIO_PXSELC(p) = (1 << (o));		\
    REG_GPIO_PXDIRC(p) = (1 << (o));		\
} while (0)

#define __gpio_as_output(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_output(p, o);		\
} while (0)

#define __gpio_as_input(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_input(p, o);		\
} while (0)

#define __gpio_set_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXDATS(p) = (1 << o);		\
} while (0)

#define __gpio_clear_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXDATC(p) = (1 << o);		\
} while (0)

#define __gpio_get_pin(n)			\
({						\
	unsigned int p, o, v;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	if (__gpio_get_port(p) & (1 << o))	\
		v = 1;				\
	else					\
		v = 0;				\
	v;					\
})

#define __gpio_as_irq_high_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRS(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_low_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRC(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_rise_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRS(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_fall_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRC(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_mask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
} while (0)

#define __gpio_unmask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_ack_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
} while (0)

#define __gpio_get_irq()			\
({						\
	unsigned int p, i, tmp, v = 0;		\
	for (p = 3; p >= 0; p--) {		\
		tmp = REG_GPIO_PXFLG(p);	\
		for (i = 0; i < 32; i++)	\
			if (tmp & (1 << i))	\
				v = (32*p + i);	\
	}					\
	v;					\
})

#define __gpio_group_irq(n)			\
({						\
	register int tmp, i;			\
	tmp = REG_GPIO_PXFLG((n));		\
	for (i=31;i>=0;i--)			\
		if (tmp & (1 << i))		\
			break;			\
	i;					\
})

#define __gpio_enable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPEC(p) = (1 << o);		\
} while (0)

#define __gpio_disable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPES(p) = (1 << o);		\
} while (0)

*/
