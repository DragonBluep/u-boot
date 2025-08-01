// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2018
 * Michalis Pappas <mpappas@fastmail.fm>
 */
#include <asm/psci.h>
#include <command.h>
#include <vsprintf.h>
#include <linux/arm-smccc.h>
#include <linux/compiler.h>
#include <linux/psci.h>
#include <linux/string.h>

static int do_call(struct cmd_tbl *cmdtp, int flag, int argc,
		   char *const argv[])
{
	struct arm_smccc_res res;

	unsigned long fid;

	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
	unsigned long a4;
	unsigned long a5;
	unsigned long a6;
	unsigned long a7;

	if (argc < 2)
		return CMD_RET_USAGE;

	fid = hextoul(argv[1], NULL);

	a1 = argc > 2 ? hextoul(argv[2], NULL) : 0;
	a2 = argc > 3 ? hextoul(argv[3], NULL) : 0;
	a3 = argc > 4 ? hextoul(argv[4], NULL) : 0;
	a4 = argc > 5 ? hextoul(argv[5], NULL) : 0;
	a5 = argc > 6 ? hextoul(argv[6], NULL) : 0;
	a6 = argc > 7 ? hextoul(argv[7], NULL) : 0;
	a7 = argc > 8 ? hextoul(argv[8], NULL) : 0;

	if (!strcmp(argv[0], "smc"))
		arm_smccc_smc(fid, a1, a2, a3, a4, a5, a6, a7, &res);
	else
		arm_smccc_hvc(fid, a1, a2, a3, a4, a5, a6, a7, &res);

	printf("Res:  0x%lx 0x%lx 0x%lx 0x%lx\n", res.a0, res.a1, res.a2, res.a3);

	return 0;
}

#ifdef CONFIG_CMD_SMC
U_BOOT_CMD(
	smc,	9,		2,	do_call,
	"Issue a Secure Monitor Call",
	"<fid> [arg1 ... arg6] [id]\n"
	"  - fid Function ID\n"
	"  - arg SMC arguments, passed to X1-X6 (default to zero)\n"
	"  - id  Secure OS ID / Session ID, passed to W7 (defaults to zero)\n"
);
#endif

#ifdef CONFIG_CMD_HVC
U_BOOT_CMD(
	hvc,	9,		2,	do_call,
	"Issue a Hypervisor Call",
	"<fid> [arg1...arg6] [id]\n"
	"  - fid Function ID\n"
	"  - arg HVC arguments, passed to X1-X6 (default to zero)\n"
	"  - id  Session ID, passed to W7 (defaults to zero)\n"
);
#endif
