% S6 Projet - BRGH

close all;
clear all;
clc;

Fcy = 16e6;
Tcy = 1/Fcy;
baudrate = 900

UxBRG = (Fcy/(16*baudrate))-1
UxBRG = double(int16(UxBRG))
baudrate_f = (Fcy/(16*(UxBRG+1)))

error = ((baudrate_f - baudrate)/baudrate)*100