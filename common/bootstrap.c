// SPDX-License-Identifier: GPL-2.0

#include <dm/ofnode.h>
#include <button.h>
#include <env.h>
#include <init.h>
#include <led.h>
#include <linux/delay.h>
#include <linux/printk.h>

/* get key udevice via dts aliases name */
static void
mtk_get_aliases_key(const char *propname, struct udevice **devp)
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

/* blink LED once, period 200 ms */
static inline void
mtk_blink_leds(struct udevice *led1, struct udevice *led2)
{
	if (led1)
		led_set_state(led1, LEDST_OFF);
	if (led2)
		led_set_state(led2, LEDST_ON);
	mdelay(100);

	if (led1)
		led_set_state(led1, LEDST_ON);
	if (led2)
		led_set_state(led2, LEDST_OFF);
	mdelay(100);
}

int mtk_button_bootstrap(void)
{
	struct udevice *key[] = { NULL, NULL };
	struct udevice *led[] = { NULL, NULL };
	const char *boot_mode[] = { "normal", "mode1", "mode2", "mode3", "mode4" };
	int index = 0;
	int n, m;

	mtk_get_aliases_key("button-bootstrap1", &key[0]);
	mtk_get_aliases_key("button-bootstrap2", &key[1]);
	if (!key[0] && !key[1]) {
		pr_err("Bootstrap button not defined!\n");
		goto exit;
	}

	mtk_get_aliases_led("led-status1", &led[0]);
	mtk_get_aliases_led("led-status2", &led[1]);

	for (n = 0; n <= 1 && key[n]; n++) {
		if (button_get_state(key[n]) == BUTTON_OFF)
			continue;

		for (m = 15; m > 0; m--) {
			mtk_blink_leds(led[0], led[1]);

			if (button_get_state(key[n]) == BUTTON_OFF)
				break;
		}

		if (m == 0)
			index = 2 * n + 1;
		else if (m <= 10)
			index = 2 * n + 2;
	}

#ifdef CONFIG_BUTTON_RESET_ENV
	if (index == 1) {
		printf("Resetting environment...\n");
		gd->env_valid = ENV_INVALID;
		env_relocate();
		env_set("bootmode", boot_mode[0]);
		env_save();
	}
#endif

	if (index) {
		printf("Bootstrap mode%d selected by pressing button\n", index);
		/*
		 * Notice:
		 * The variables 'bootmode' and 'bootdelay' are stored in RAM here.
		 * If you want to run 'saveenv' in the u-boot shell, don't forget to
		 * reset or unset them to make sure your device can boot into normal
		 * mode next time.
		 */
		env_set("bootmode", boot_mode[index]);
		env_set("bootdelay", "0");
	}

exit:
	return 0;
}
