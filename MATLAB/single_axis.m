%% single axis example

clc
clear all
close all

%% Parameters

% base
lb = 500; % mm
wb = 25; % mm
tb = 15; % mm

% cursor
t = tb;
w = wb;
l = w;

%% Motor torque curve

% rotational vector
w_m = [0, 1000, 2000, 3000];

% torque of the motor
t_m = [1, 1, 0.95, 0] * 2;

figure
plot(w_m, t_m)
