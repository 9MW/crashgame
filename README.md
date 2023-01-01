data driven iteraion oriented design 
project explain:
input:
assets/configs/unitcfg.csv for unit data configuration

unit property:
hp, Armor, marmor,speed,damage, mdamage,agile, Critical
agile used for percentage miss rate while under attack
speed will accumulate until reach to 1 then gain extra action
normal damage=normal damage*(if trigger Critical 1.5 else be 1.0)-Armor
damage=real damage+normal damage;

files 
src/main.cpp : window & IO setup. UI Graphic initializaton
src/App0:.cpp:main logic control
src/UI0.cpp: UI logic control
src/turnsys.cpp: turn state control

composite
unit is composite by render & logic struct and structed with ECS design patten
a unit is represent by index which used to locate  it's component

system:
at turn begain stage of active player,the command buffer get filled based on the unit attack range
after all command in command buffer executed activer move to next player.

ui:
UI0 contain data pointer and event object array for In & Output data;
event object constitute by function pointer that accept a void pointer.
UI0 draw UI baesd on update flag 
for different update flag UI0 interpret it's void pointer differently 
the reason for usage of void pointer is due to it's offers great flexibility & speed up compile speed
& speed up iteraion process

Where to start read code?
Main logic begin at src/App0.cpp,it contain unit data parse & Render initialization & flow control

How to start game?
run cage.exe under current folder