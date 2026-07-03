#!/bin/bash
lch=6
uch=18

for ((ich=${lch}; ich<${uch}; ich++))
do
	hadd L1QTempFit_r1_q${ich}_xy2.root ../result/L1QTempFit_r1_q${ich}_xy2_rig*
done
