#!/bin/bash

while true
do
	RED='\033[0;31m'
	L_RED='\033[1;31m'
	L_GREEN='\033[1;32m'
	GREEN='\033[0;32m'
	L_BLUE='\033[1;34m'
	BLUE='\033[0;34m'
	WHITE='\033[1;37m'
	ORANGE='\033[0;33m'
	YELLOW='\033[1;33m'
	NC='\033[0m' # No Color

	echo -n "Report of SARACEn at:   "
	echo `date +%d/%m/%y-%H:%M:%S`
	
	for x in $(cat conditions.txt);
	do
       		weather=$(curl -s "https://lesson5-esp32-rtdb-cc25f-default-rtdb.europe-west1.firebasedatabase.app/Sensor/$x.json")
        	echo -n "$x: $weather"
	
	        if [[ $x == "CO2_(ppm)" ]]; then
			if [[ $weather -le 700 ]]; then
				echo -e " ${GREEN}###${NC} ---> Healthy levels."
			elif [[ $weather -le 800 ]]; then
				echo -e " ${L_GREEN}###${NC} ---> Moderate levels."
			elif [[ $weather -le 1100 ]]; then
				echo -e " ${WHITE}###${NC} ---> Warning levels."
			elif [[ $weather -le 1500 ]]; then
				echo -e " ${YELLOW}###${NC} ---> Unhealty levels."
			elif [[ $weather -le 2000 ]]; then
				echo -e " ${ORANGE}###${NC} ---> Very Dangerous levels."
			else
				echo -e " ${RED}###${NC} ---> Extreme levels."
			fi
        	elif [[ $x == "TVOC_levels_(ppb)" ]]; then
                        
                        if [[ $weather -le 220 ]]; then
                                echo -e " ${GREEN}###${NC} ---> Healthy levels."
                        elif [[ $weather -le 660 ]]; then
                                echo -e " ${L_GREEN}###${NC} ---> Moderate levels."
                        elif [[ $weather -le 1430 ]]; then
                                echo -e " ${WHITE}###${NC} ---> Warning levels."
                        elif [[ $weather -le 2200 ]]; then
                                echo -e " ${YELLOW}###${NC} ---> Unhealty levels."
                        elif [[ $weather -le 3300 ]]; then
                                echo -e " ${ORANGE}###${NC} ---> Very Dangerous levels."
                        else
                                echo -e " ${RED}###${NC} ---> Extreme levels."
                        fi




        	elif [[ $x == "Humidity_(percentage)" ]]; then

			if (( $(echo "$weather < 25" | bc -l) )); then
			    echo -e " ${RED}###${NC} ---> Very dry atmosphere."
			elif (( $(echo "$weather < 30" | bc -l) )); then
			    echo -e " ${L_RED}###${NC} ---> Rather dry."
			elif (( $(echo "$weather < 60" | bc -l) )); then
			    echo -e " ${WHITE}###${NC} ---> Healthy atmosphere."
			elif (( $(echo "$weather < 70" | bc -l) )); then
			    echo -e " ${L_BLUE}###${NC} ---> Rather humid."
			else
			    echo -e " ${BLUE}###${NC} ---> Very humid atmosphere."
			fi

		elif [[ $x == "Temperature_(Celcius)" ]]; then
			echo " ºC"

		fi

	
	done
	
	echo ""
	sleep 10
done
