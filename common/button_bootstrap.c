// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 Shiji Yang <yangshiji66@outlook.com>
 */

#include <button.h>
#include <command.h>
#include <dm/ofnode.h>
#include <env.h>
#include <init.h>
#include <led.h>
#include <linux/delay.h>
#include <linux/printk.h>
#include <search.h>
#include <u-boot/schedule.h>

#define bootstrap_set_led_state(led, state) \
	({ if (led) led_set_state(led, state); })

#define is_button_pressed(dev) \
	(dev ? button_get_state(dev) == BUTTON_ON : false)

static void bootstrap_period_led(struct udevice **led, u32 period)
{
	u32 interval = period / 2;

	mdelay(interval);
	bootstrap_set_led_state(led[0], LEDST_OFF);
	bootstrap_set_led_state(led[1], LEDST_ON);

	mdelay(interval);
	bootstrap_set_led_state(led[0], LEDST_ON);
	bootstrap_set_led_state(led[1], LEDST_OFF);
}

static void bootstrap_heartbeat_led(struct udevice **led, u32 period)
{
	u32 interval = period / 8;

	for (int cnt = 0; cnt < 2; cnt ++) {
		mdelay(interval);
		bootstrap_set_led_state(led[0], LEDST_OFF);
		bootstrap_set_led_state(led[1], LEDST_OFF);

		mdelay(interval);
		bootstrap_set_led_state(led[0], LEDST_ON);
		bootstrap_set_led_state(led[1], LEDST_ON);
	}
	mdelay(interval * 4);
}

void button_bootstrap(void)
{
	int idx, cnt;
	char bootidx[12];
	const char *bootcmd = NULL;
	struct udevice *btn[2], *led[2];
	const char *btn_label[2], *led_label[2];
	const char *btn_propname[2] = { "boot-btn1", "boot-btn2" };
	const char *led_propname[2] = { "boot-led1", "boot-led2" };

	for (idx = 0; idx < ARRAY_SIZE(btn); idx++) {
		btn_label[idx] = ofnode_conf_read_str(btn_propname[idx]);
		if (!btn_label[idx] || button_get_by_label(btn_label[idx], &btn[idx]))
			btn[idx] = NULL;
	}

	if (!btn[0] && !btn[1]) {
		printf("Bootstrap: Button not defined!\n");
		return;
	}

	for (idx = 0; idx < ARRAY_SIZE(led); idx++) {
		led_label[idx] = ofnode_conf_read_str(led_propname[idx]);
		if (!led_label[idx] || led_get_by_label(led_label[idx], &led[idx]))
			led[idx] = NULL;
	}

	for (idx = 0; idx < ARRAY_SIZE(btn); idx++) {
		for (cnt = 20; cnt > 0 && is_button_pressed(btn[idx]); cnt--){
			if (cnt > 4)
				bootstrap_period_led(led, cnt > 12 ? 128 : 256);
			else
				bootstrap_heartbeat_led(led, 512);
			schedule();
		}

		if (cnt <= 12) {
			snprintf(bootidx, sizeof(bootidx), "bootcmd_%d-%d",
				 idx + 1, cnt > 4 ? 1 : (cnt > 0 ? 2 : 3));
			bootcmd = ofnode_conf_read_str(bootidx);
		}
	}

	if (bootcmd) {
		if (led[0] && led[1]) {
			led_set_state(led[0], LEDST_OFF);
			led_set_state(led[1], LEDST_ON);
		}

		if (!strcmp(bootcmd, "env reset")) {
			env_set_default("Bootstrap: Reset Environment!\n",
					H_FORCE | H_INTERACTIVE);
			env_save();

		} else {
			printf("Bootstrap: run bootcmd '%s'\n", bootcmd);
			run_command(env_get(bootcmd), CMD_FLAG_ENV);
		}
	}
}
