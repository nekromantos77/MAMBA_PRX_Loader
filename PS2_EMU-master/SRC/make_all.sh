#! /bin/bash

fw="4_46"
cd $fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make clean
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN
mkdir BIN/446
cp SRC/$fw/BIN/* BIN/446

fw="4_50"
cd SRC/$fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make clean
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN/450
cp SRC/$fw/BIN/* BIN/450

fw="4_65"
cd SRC/$fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN/465
cp SRC/$fw/BIN/* BIN/465

fw="4_66"
cd SRC/$fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make clean
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN/466
cp SRC/$fw/BIN/* BIN/466

fw="4_70"
cd SRC/$fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make clean
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN/470
cp SRC/$fw/BIN/* BIN/470

fw="4_75"
cd SRC/$fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make clean
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN/475
cp SRC/$fw/BIN/* BIN/475

fw="4_76"
cd SRC/$fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make clean
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN/476
cp SRC/$fw/BIN/* BIN/476

fw="4_78"
cd SRC/$fw
cd ps2emu_gen_$fw
make clean
make
cd ../ps2emu_stage1_file
make clean
make all
cd ../ps2emu_stage2/gxemu
make clean
make
cd ../hwemu
make clean
make
cd ../ps2netemu_gen_$fw
make clean
make
cd ../netemu
make clean
make
cd ../../../..
mkdir BIN/478
cp SRC/$fw/BIN/* BIN/478

echo "DONE!"
