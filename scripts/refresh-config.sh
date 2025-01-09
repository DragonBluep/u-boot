#!/bin/bash

# add board name here
Boards=(
	mt7620_domywifi_dm203 \
	mt7620_focusmedia_mt7620a \
	mt7620_gbcom_n3000-06 \
	mt7620_haier_hw-l1w \
	mt7620_hiwifi_hc5661 \
	mt7620_hiwifi_hc5861 \
	mt7620_lb-link_bl-w1210 \
	mt7620_phicomm_k2 \
	mt7620_phicomm_k2g \
	mt7620_youku_x2 \
	mt7620_youku_yk-l1c \
	\
	mt7621_asus_rt-ac1200gu \
	mt7621_dlink_dir-878-a1 \
	mt7621_h3c_tx1801-plus \
	mt7621_h3c_tx1806 \
	mt7621_jdcloud_re-sp-01b \
	mt7621_linksure_sg5 \
	mt7621_mercury_mac2600r \
	mt7621_raisecom_msg1500-x-00 \
	mt7621_sim_simax1800t \
	mt7621_sim_simax1800u \
	mt7621_skspruce_wia3300-10 \
	mt7621_zte_e8820-v2 \
	\
	mt7628_360_p2 \
	mt7628_cmcc_zy-368 \
	mt7628_hiwifi_hc5661a \
	mt7628_motorola_mwr03 \
	\
	mt7981_airopi_ax3 \
	mt7981_e-life_etr631-t \
	mt7981_jcg_q30-pro-256m \
	mt7981_jcg_q30-pro-512m \
	mt7981_siluo_sl-3000 \
	)

for Board in ${Boards[@]}
do
	echo "Refresh board ${Board}"
	make ${Board}_defconfig
	make savedefconfig
	cat ./defconfig > ./configs/${Board}_defconfig
done
