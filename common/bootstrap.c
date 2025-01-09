// SPDX-License-Identifier: GPL-2.0

#include <dm/ofnode.h>
#include <button.h>
#include <env.h>
#include <init.h>
#include <led.h>
#include <linux/delay.h>
#include <linux/printk.h>

/* get button udevice via dts aliases name */
static void
mtk_get_aliases_btn(const char *propname, struct udevice **devp)
{
	int lenp;
	ofnode node;
	const char *label_name;

	node = ofnode_get_aliases_node(propname);
	if (!ofnode_valid(node))
		return;

	label_name = ofnode_get_property(node, "label", &lenp);
	if (!label_name) {
		pr_warn("Failed to get button label from node: %s\n",
			ofnode_get_name(node));
		return;
	}

	if (button_get_by_label(label_name, devp))
		pr_err("Failed to get button udevice for %s\n", label_name);
}

/* get LED udevice via dts aliases name */
static void
mtk_get_aliases_led(const char *propname, struct udevice **devp)
{
	int lenp;
	ofnode node;
	const char *label_name;

	node = ofnode_get_aliases_node(propname);
	if (!ofnode_valid(node))
		return;

	label_name = ofnode_get_property(node, "label", &lenp);
	if (!label_name) {
		pr_warn("Failed to get LED label from node: %s\n",
			ofnode_get_name(node));
		return;
	}

	if (led_get_by_label(label_name, devp))
		pr_err("Failed to get LED udevice for %s\n", label_name);
}

/* blink LED once, period (interval * 2) ms */
static inline void
mtk_blink_leds(struct udevice *led1, struct udevice *led2, u32 interval)
{
	mdelay(interval);
	if (led1)
		led_set_state(led1, LEDST_OFF);
	if (led2)
		led_set_state(led2, LEDST_ON);

	mdelay(interval);
	if (led1)
		led_set_state(led1, LEDST_ON);
	if (led2)
		led_set_state(led2, LEDST_OFF);
}

int mtk_button_bootstrap(void)
{
	struct udevice *btn[] = { NULL, NULL };
	struct udevice *led[] = { NULL, NULL };
	const char *boot_mode[] = { "normal", "reset", "recovery", "netconsole", "initramfs" };
	int index = 0;

	mtk_get_aliases_btn("button-boot1", &btn[0]);
	mtk_get_aliases_btn("button-boot2", &btn[1]);
	if (!btn[0] && !btn[1]) {
		pr_err("Bootstrap button not defined!\n");
		return 0;
	}

	mtk_get_aliases_led("led-status1", &led[0]);
	mtk_get_aliases_led("led-status2", &led[1]);

	for (int n = 1; n >= 0; n--) {
		int m;
		for (m = 16; m > 0 && btn[n]; m--) {
			if (button_get_state(btn[n]) == BUTTON_OFF)
				break;

			mtk_blink_leds(led[0], led[1], m > 8 ? 64 : 128);
		}

		if (m <= 8)
			index = 2 * n + (m == 0 ? 1 : 2);
	}

	/*
	 * Add "button-reset-env;" dts property to config node to enable
	 * reset env by long pressing button-boot1.
	 */
	if (!(CONFIG_IS_ENABLED(ENV_IS_NOWHERE)) &&
	    ofnode_conf_read_bool("button-reset-env") &&
	    (index == 1)) {
		printf("Resetting environment...\n");
		gd->env_valid = ENV_INVALID;
		env_relocate();
		env_set("bootmode", boot_mode[0]);
		env_save();
	}

	if (index) {
		if (!btn[1])
			index *= 2;
		/*
		 * Notice:
		 * The variables 'bootmode' and 'bootdelay' are stored in RAM
		 * here. If you want to run 'saveenv' in the u-boot shell, do
		 * not forget to reset or unset them to make sure your device
		 * can boot into normal mode next time.
		 */
		env_set("bootmode", boot_mode[index]);
		env_set("bootdelay", "0");

		if (led[0] && led[1]) {
			led_set_state(led[0], LEDST_OFF);
			led_set_state(led[1], LEDST_ON);
		}
	}
	printf("Bootstrap: Boot mode \"%s\" selected\n", boot_mode[index]);

	return 0;
}
