#!/bin/bash

# add board name here
Boards=(
	domywifi_dm203 \
	focusmedia_mt7620a \
	gbcom_n3000-06 \
	haier_hw-l1w \
	hiwifi_hc5661 \
	hiwifi_hc5861 \
	lb-link_bl-w1210 \
	phicomm_k2 \
	phicomm_k2g \
	youku_x2 \
	youku_yk-l1c \
	\
	asus_rt-ac1200gu \
	dlink_dir-878-a1 \
	h3c_tx1801-plus \
	h3c_tx1806 \
	jdcloud_re-sp-01b \
	linksure_sg5 \
	mercury_mac2600r \
	raisecom_msg1500-x-00 \
	sim_simax1800t \
	skspruce_wia3300-10 \
	zte_e8820-v2 \
	\
	360_p2 \
	cmcc_zy-368 \
	hiwifi_hc5661a \
	motorola_mwr03 \
	)

for Board in ${Boards[@]}
do
	echo "Refresh board ${Board}"
	make ${Board}_defconfig
	make savedefconfig
	cat ./defconfig > ./configs/${Board}_defconfig
done
