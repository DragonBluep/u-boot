// SPDX-License-Identifier: GPL-2.0

#include <config.h>
#include <dm.h>
#include <button.h>
#include <env.h>
#include <init.h>
#include <led.h>
#include <mtd.h>
#include <asm/global_data.h>
#include <linux/delay.h>

#ifdef CONFIG_BOARD_LATE_INIT
#if defined(CONFIG_BUTTON) && defined(CONFIG_LED)
/* Get led udevice by dts config property name */
static int mtk_getled(const char *prop_name, struct udevice **devp)
{
	const char *name;
	int ret;

	name = ofnode_conf_read_str(prop_name);
	if (!name)
		return -ENODATA;
	ret = led_get_by_label(name, devp);
	if (ret)
		debug("%s: get=%d\n", __func__, ret);
	return ret;
}

/* Get button udevice by dts config property name */
static int mtk_getbutton(const char *prop_name, struct udevice **devp)
{
	const char *name;
	int ret;

	name = ofnode_conf_read_str(prop_name);
	if (!name)
		return -ENODATA;
	ret = button_get_by_label(name, devp);
	if (ret)
		debug("%s: get=%d\n", __func__, ret);
	return ret;
}

/* Blink LED */
static inline void mtk_blinkled(struct udevice *led1, int flag1, struct udevice *led2, int flag2)
{
	if (flag1)
		led_set_state(led1, LEDST_OFF);
	if (flag2)
		led_set_state(led2, LEDST_ON);
	mdelay(100);
	if (flag1)
		led_set_state(led1, LEDST_ON);
	if (flag2)
		led_set_state(led2, LEDST_OFF);
	mdelay(100);
}

/* Get mtd partition size and save it to u-boot env */
/*
static int mtk_getmtdsz(const char *mtdnm)
{
	struct mtd_info *part;
	char enval[16], envnm[32];

	mtd_probe_devices();
	part = get_mtd_device_nm(mtdnm);
	if (IS_ERR_OR_NULL(part)) {
		printf("Partition %s not found!\n", mtdnm);
		return 1;
	}
	sprintf(enval, "0x%08llx", part->size);
	sprintf(envnm, "mtdsize_%s", mtdnm);
	env_set(envnm, enval);

	return 0;
}
*/
/*
 * Download firmware via TFTP then install it if reset button is pressed
 * Ethernet is not ready here, run_command("tftpboot", 0) will stuck
 */
static void firmware_recovery(void)
{
	struct udevice *led1, *led2, *rst, *wps;
	int flag_led1, flag_led2, flag_rst, flag_wps;
	int cnt;

	flag_led1 = !mtk_getled("status1-led", &led1);
	flag_led2 = !mtk_getled("status2-led", &led2);
	flag_rst = !mtk_getbutton("reset-button", &rst);
	flag_wps = !mtk_getbutton("wps-button", &wps);

	if (flag_rst && button_get_state(rst) == BUTTON_ON) {
		cnt = flag_wps ? 5 : 15;
		for ( ; cnt > 0; cnt--) {
			if (button_get_state(rst) == BUTTON_OFF)
				break;
			mtk_blinkled(led1, flag_led1, led2, flag_led2);
		}
		if ((!flag_wps && cnt != 0 && cnt <= 10) || (flag_wps && cnt == 0)) {
			puts("Reset button is pressed, u-boot will enter into tftp write mode\n");
			env_set("recmode", "1");
			env_set("bootdelay", "0");
		}
		if (!flag_wps && cnt == 0) {
			puts("Reset button is pressed, u-boot will enter into tftp boot mode\n");
			env_set("recmode", "2");
			env_set("bootdelay", "0");
		}
	}

	if (flag_wps && button_get_state(wps) == BUTTON_ON) {
		for (cnt = 5 ; cnt > 0; cnt--) {
			if (button_get_state(wps) == BUTTON_OFF)
				break;
			mtk_blinkled(led1, flag_led1, led2, flag_led2);
		}
		if (cnt == 0) {
			puts("WPS button is pressed, u-boot will enter into tftp boot mode\n");
			env_set("recmode", "2");
			env_set("bootdelay", "0");
		}
	}

	/* Release and press the button again to reset the environment variable */
#ifndef CONFIG_ENV_IS_NOWHERE
	mdelay(2000);
	if (flag_rst && button_get_state(rst) == BUTTON_ON ||
		flag_wps && button_get_state(wps) == BUTTON_ON) {
		for (cnt = 5 ; cnt > 0; cnt--) {
			if (!(button_get_state(rst) == BUTTON_ON ||
				flag_wps && button_get_state(wps) == BUTTON_ON))
				break;
			mtk_blinkled(led1, flag_led1, led2, flag_led2);
		}
		if (cnt == 0) {
			puts("Reset/WPS button pushed, resetting environment\n");
			gd->env_valid = ENV_INVALID;
			env_relocate();
			env_save();
			do_reset(NULL, 0, 0, NULL);
		}
	}
#endif

	// if (!env_get("mtdsize_uboot"))
	// 	mtk_getmtdsz("u-boot");
	// if (!env_get("mtdsize_firmware"))
	// 	mtk_getmtdsz("firmware");
}
#endif

int board_late_init(void)
{
#if defined(CONFIG_BUTTON) && defined(CONFIG_LED)
	firmware_recovery();
#endif

	return 0;
}
#endif // CONFIG_BOARD_LATE_INIT
