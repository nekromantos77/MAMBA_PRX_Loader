#! /bin/bash

rm -r ../BIN/*

fw="4_46"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean
cd ../../../

fw="4_50"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean
cd ../../../

fw="4_65"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean
cd ../../../

fw="4_66"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean
cd ../../../

fw="4_70"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean
cd ../../../

fw="4_75"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean
cd ../../../

fw="4_76"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean
cd ../../../

fw="4_78"
cd $fw
rm BIN/*
cd ps2emu_gen_$fw
make clean
cd ../ps2emu_stage1_file
make clean
cd ../ps2emu_stage2/gxemu
make clean
cd ../hwemu
make clean
cd ../ps2netemu_gen_$fw
make clean
cd ../netemu
make clean

echo "DONE!"
