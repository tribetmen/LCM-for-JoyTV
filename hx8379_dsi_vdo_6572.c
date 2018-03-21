#if defined(BUILD_LK)
#include <string.h>
#else
#include <linux/string.h>
#endif


#if defined(BUILD_LK)
#include "cust_gpio_usage.h"
#include <platform/mt_gpio.h>
#else
#include "cust_gpio_usage.h"
#include <mach/mt_gpio.h>
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(800)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define UDELAY(n) 									(lcm_util.udelay(n))
#define MDELAY(n) 									(lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   		lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
	unsigned char cmd;
	unsigned char count;
	unsigned char para_list[64];
};      

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dbi.te_edge_polarity                    = LCM_POLARITY_RISING;
	params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;

	params->dsi.mode   = SYNC_PULSE_VDO_MODE;

	params->dsi.LANE_NUM				= LCM_TWO_LANE;

	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size=256;
		
	params->dsi.intermediat_buffer_num = 2;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active				= 4; 
	params->dsi.vertical_backporch					= 7;
	params->dsi.vertical_frontporch					= 7;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 40;
	params->dsi.horizontal_backporch				= 40;
	params->dsi.horizontal_frontporch				= 40;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	params->dsi.pll_div1 = 1;
        params->dsi.pll_div2 = 1;
        params->dsi.fbk_div = 25;
}

static void lcm_init_registers(void)
{
        unsigned int data_array[16];

	// SET password
        data_array[0] = 0x00043902;
        data_array[1] = 0x7983FFB9;
        dsi_set_cmdq(data_array, 2, 1);

	// SET Display 480x800
        data_array[0] = 0x00033902;
	data_array[1] = 0x009351BA;
        dsi_set_cmdq(data_array, 2, 1);

	// SET Display column inversion
        data_array[0] = 0x00143902;
        data_array[1] = 0x245000B1;
        data_array[2] = 0x11088DEA;
        data_array[3] = 0x2F271711;
        data_array[4] = 0x0B421A9A;
        data_array[5] = 0xE600F14A;
        dsi_set_cmdq(data_array, 6, 1);

	// SET GIP
        data_array[0] = 0x000E3902;
        data_array[1] = 0x3C0000B2;
        data_array[2] = 0x22190408;
	data_array[3] = 0x0408FF00;
        data_array[4] = 0x00002019;
        dsi_set_cmdq(data_array, 5, 1);

	// SET Power
	data_array[0] = 0x00203902;
        data_array[1] = 0x000880B4;
        data_array[2] = 0x32031032;
        data_array[3] = 0x10327013;
        data_array[4] = 0x28013708;
        data_array[5] = 0x30003707;
        data_array[6] = 0x083C3814;
        data_array[7] = 0x28084000;
        data_array[8] = 0x04303008;
        dsi_set_cmdq(data_array, 9, 1);

	// SET Panel
        data_array[0] = 0x00303902;
        data_array[1] = 0x0A0000D5;
        data_array[2] = 0x00050100;
        data_array[3] = 0x88880003;
        data_array[4] = 0x01238888;
        data_array[5] = 0x13024567;
	data_array[6] = 0x88888888;
	data_array[7] = 0x88888888;
        data_array[8] = 0x76548888;
        data_array[9] = 0x20313210;
	data_array[10] = 0x88888888;
	data_array[11] = 0x00008888;
        data_array[12] = 0x00000000;
        dsi_set_cmdq(data_array, 13, 1);

	// SET GAMMA
	data_array[0] = 0x00243902;
        data_array[1] = 0x000079E0;
        data_array[2] = 0x3F2F2C02;
        data_array[3] = 0x0E074927;
        data_array[4] = 0x1416140F;
	data_array[5] = 0x00181316;
	data_array[6] = 0x2F2C0200;
        data_array[7] = 0x0749273F;
        data_array[8] = 0x16140F0E;
	data_array[9] = 0x18131614;
        dsi_set_cmdq(data_array, 10, 1);

	// SET pixel format RGB888
        data_array[0] = 0x00033902;
	data_array[1] = 0x000200C6;
        dsi_set_cmdq(data_array, 2, 1);

	// SET MIPI (2 Lane)
        data_array[0] = 0x00033902;
	data_array[1] = 0x000002CC;
        dsi_set_cmdq(data_array, 2, 1);

	// Enable TE
        data_array[1] = 0x00A000B6;
        data_array[2] = 0x000000A0;
        dsi_set_cmdq(data_array, 3, 1);

	// Sleep Out
        data_array[0] = 0x00110500;
        dsi_set_cmdq(data_array, 1, 1);

        MDELAY(200);
	
	// Display On
        data_array[0] = 0x00290500;
        dsi_set_cmdq(data_array, 1, 1);

        MDELAY(30);
	
}

static void lcm_init(void)
{
	printk("[hx8357b] %s\n", "lcm_init");
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	lcm_init_registers();
}

static void lcm_suspend(void)
{
	unsigned int data_array[16];

	// Display Off
	data_array[0] = 0x00280500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);

	// Sleep In
	data_array[0] = 0x00100500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);


	SET_RESET_PIN(0);
	MDELAY(50);

	SET_RESET_PIN(1);
	MDELAY(120);
}

static void lcm_resume(void)
{
	lcm_init();
}	

static void lcm_setbacklight(unsigned int level)
{

	unsigned int data_array[16];

	if (level > 255)
            level = 255;
	printk("%s, %d\n", "lcm_setbacklight", level);
        data_array[0]= 0x00023902;
        data_array[1] =(0x51|(level<<8));
        dsi_set_cmdq(data_array, 2, 1);
}

// ---------------------------------------------------------------------------
//  Get LCM ID Information JUST MOVE OUT SB NOUSE CODE
// ---------------------------------------------------------------------------

static unsigned int lcm_compare_id(void)
{

        unsigned int id = 0;
        unsigned char buffer[2];
        unsigned int array[16];

        SET_RESET_PIN(1);
        MDELAY(1);
        SET_RESET_PIN(0);
        MDELAY(10);
        SET_RESET_PIN(1);
        MDELAY(120);
        array[0] = 0x00043902;
        array[1] = 0x7983FFB9;
        dsi_set_cmdq(&array, 2, 1);
        MDELAY(10);
        array[1] = 0x009351BA;
        array[0] = 0x00033902;
        dsi_set_cmdq(&array, 2, 1);
        array[0] = 0x00013700;
        dsi_set_cmdq(&array, 1, 1);
        MDELAY(10);

        read_reg_v2(0xF4, buffer, 1);
        id = buffer[0];
        printk("lcd id %x %x\n", __func__, id);
#if 0
        return (0x79 == id)?1:0;
#else
        return 1;
#endif
} 

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hx8379_dsi_vdo_6572_lcm_drv = 
{
        .name		= "hx8379_dsi_vdo_6572",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.compare_id     = lcm_compare_id,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.set_backlight  = lcm_setbacklight,
};
